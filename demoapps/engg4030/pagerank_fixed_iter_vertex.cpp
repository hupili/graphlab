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

float RESET_PROB = 0.15;
float TOLERANCE = 1.0E-5;
int SCHEDULE_COUNT = 10;

int g_num_vertices = -1;
int g_num_edges = -1;

typedef struct _vertex_data_type{
	float value; // Current PR value
	int count; // # of times the vertex is scheduled
	void save(graphlab::oarchive& oarc) const {
		oarc << count << value;
	}
	void load(graphlab::iarchive& iarc) {
		iarc >> count >> value;
	}
} vertex_data_type;
typedef graphlab::empty edge_data_type;
typedef graphlab::distributed_graph<vertex_data_type, edge_data_type> graph_type;

void init_vertex(graph_type::vertex_type& vertex) {
	vertex.data().value = 1;
	vertex.data().count = 0;
}

class pagerank :
	public graphlab::ivertex_program<graph_type, float>,
	public graphlab::IS_POD_TYPE 
{
	float last_change;
	public:
	edge_dir_type gather_edges(icontext_type& context,
			const vertex_type& vertex) const {
		return graphlab::IN_EDGES;
	}
	float gather(icontext_type& context, const vertex_type& vertex,
			edge_type& edge) const {
		return ((1.0 - RESET_PROB) / edge.source().num_out_edges()) *
			edge.source().data().value;
	}
	void apply(icontext_type& context, vertex_type& vertex,
			const gather_type& total) {
		const double newval = total + RESET_PROB;
		last_change = std::fabs(newval - vertex.data().value);
		vertex.data().value = newval;
		vertex.data().count += 1;
	}
	edge_dir_type scatter_edges(icontext_type& context,
			const vertex_type& vertex) const {
		if (vertex.data().count < SCHEDULE_COUNT) return graphlab::OUT_EDGES;
		else return graphlab::NO_EDGES;
	}
	void scatter(icontext_type& context, const vertex_type& vertex,
			edge_type& edge) const {
		context.signal(edge.target());
	}
}; // end of factorized_pagerank update functor


struct pagerank_writer {
	std::string save_vertex(graph_type::vertex_type v) {
		std::stringstream strm;
		strm << "vertex:" << v.id() << "\t" << v.data().value / g_num_vertices << "\n";
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
	std::string saveprefix = "sample_output/pr_fixed_iter_vertex";

	graph_type graph(dc, clopts);
	dc.cout() << "Loading graph in format: "<< format << std::endl;
	graph.load_format(graph_dir, format);
	graph.finalize();
	g_num_vertices = graph.num_vertices();
	g_num_edges = graph.num_edges();
	dc.cout() << "#vertices: " << graph.num_vertices()
		<< " #edges:" << graph.num_edges() << std::endl;

	graph.transform_vertices(init_vertex);

	graphlab::omni_engine<pagerank> engine(dc, graph, exec_type, clopts);
	engine.signal_all();
	engine.start();
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

