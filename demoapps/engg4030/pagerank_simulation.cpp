/*
 * Copyright (c) 2009 Carnegie Mellon University.
 *     All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS
 *  IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *  express or implied.  See the License for the specific language
 *  governing permissions and limitations under the License.
 *
 * For more about this software visit:
 *
 *      http://www.graphlab.ml.cmu.edu
 *
 */

/*
 * This is just original pagerank demo with comments stripped out for
 * easier comparison:
 *
 *     simple_pagerank_annotated.cpp
 */

#include <vector>
#include <string>
#include <fstream>

#include <graphlab.hpp>

#include <time.h>

float RESET_PROB = 0.15;
float TOLERANCE = 1.0E-5;
int MAX_VISIT = 1000;

int g_num_vertices = -1;
int g_num_edges = -1;
int g_total_visits = -1;

typedef int vertex_data_type; // how many times this vertex is visited
typedef graphlab::empty edge_data_type;
typedef graphlab::distributed_graph<vertex_data_type, edge_data_type> graph_type;

float rand_u01(){
	// uniform distribution in [0, 1)
	return graphlab::random::rand01();
}

void init_vertex(graph_type::vertex_type& vertex) {
	vertex.data() = 0; // visited by 0 times initially
}

// Randomly select one vertex for restart
bool p_restarting_vertex(const graph_type::vertex_type& vertex) {
	  return bool(rand_u01() < 1.0 / g_num_vertices);
}

int get_visit_count(const graph_type::vertex_type& vertex) {
	return vertex.data();
}

class pagerank :
	public graphlab::ivertex_program<graph_type, float>,
	public graphlab::IS_POD_TYPE 
{
	public:
	edge_dir_type gather_edges(icontext_type& context,
			const vertex_type& vertex) const {
		// No need to gather anything
		// Once signaled, the current vertex is visited by the RW
		return graphlab::NO_EDGES;
	}
	float gather(icontext_type& context, const vertex_type& vertex,
			edge_type& edge) const {
		// dummy return
		return 0.0;
	}
	void apply(icontext_type& context, vertex_type& vertex,
			const gather_type& total) {
		vertex.data() += 1; // visited by the RW one more time
	}
	edge_dir_type scatter_edges(icontext_type& context,
			const vertex_type& vertex) const {
		if (rand_u01() < RESET_PROB){
			// The random walker gets bored. Reset!
			return graphlab::NO_EDGES;
		} else {
			// Jump to "a" random neighbour.
			return graphlab::OUT_EDGES;
		}
	}
	void scatter(icontext_type& context, const vertex_type& vertex,
			edge_type& edge) const {
		if (rand_u01() < 1.0 / vertex.num_out_edges()){
			context.signal(edge.target());
		}
	}
}; // end of factorized_pagerank update functor


struct pagerank_writer {
	std::string save_vertex(graph_type::vertex_type v) {
		std::stringstream strm;
		strm << "vertex:" << v.id() << "\t" << v.data() / float(g_total_visits) << "\n";
		return strm.str();
	}
	std::string save_edge(graph_type::edge_type e) { return ""; }
}; // end of pagerank writer

int main(int argc, char** argv) {
	graphlab::mpi_tools::init(argc, argv);
	graphlab::distributed_control dc;
	global_logger().set_log_level(LOG_INFO);

	// fixed options
	graphlab::command_line_options clopts("PageRank algorithm.");
	std::string graph_dir = "sample_tsv";
	std::string format = "tsv";
	std::string exec_type = "synchronous";
	std::string saveprefix = "sample_output/pr_simulation";

	graph_type graph(dc, clopts);
	dc.cout() << "Loading graph in format: "<< format << std::endl;
	graph.load_format(graph_dir, format);
	graph.finalize();
	g_num_vertices = graph.num_vertices();
	g_num_edges = graph.num_edges();
	dc.cout() << "#vertices: " << graph.num_vertices()
		<< " #edges:" << graph.num_edges() << std::endl;

	graph.transform_vertices(init_vertex);

	// Simulate the RWs
	graphlab::random::seed(clock());
	graphlab::omni_engine<pagerank> engine(dc, graph, exec_type, clopts);
	while (g_total_visits < MAX_VISIT){
		// Randomly select "a" restarting vertex
		graphlab::vertex_set restarting_vertices = graph.select(p_restarting_vertex);
		engine.signal_vset(restarting_vertices);
		// Simulate the random walk
		engine.start();
		// Caculate current total visits
		g_total_visits = graph.map_reduce_vertices<int>(get_visit_count);
		dc.cout() << "Total visits so far: " << g_total_visits << std::endl;
	}

	const float runtime = engine.elapsed_seconds();
	dc.cout() << "Finished Running engine in " << runtime
		<< " seconds." << std::endl;

	if (saveprefix != "") {
		graph.save(saveprefix, pagerank_writer(),
				false,    // do not gzip
				true,     // save vertices
				false);   // do not save edges
	}

	graphlab::mpi_tools::finalize();
	return EXIT_SUCCESS;
} // End of main

