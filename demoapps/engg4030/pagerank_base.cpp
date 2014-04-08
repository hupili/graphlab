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

typedef float vertex_data_type;
typedef graphlab::empty edge_data_type;
typedef graphlab::distributed_graph<vertex_data_type, edge_data_type> graph_type;

void init_vertex(graph_type::vertex_type& vertex) { vertex.data() = 1; }

class pagerank :
	public graphlab::ivertex_program<graph_type, float>,
	public graphlab::IS_POD_TYPE {
		float last_change;
		public:
		float gather(icontext_type& context, const vertex_type& vertex,
				edge_type& edge) const {
			return ((1.0 - RESET_PROB) / edge.source().num_out_edges()) *
				edge.source().data();
		}

		void apply(icontext_type& context, vertex_type& vertex,
				const gather_type& total) {
			const double newval = total + RESET_PROB;
			last_change = std::fabs(newval - vertex.data());
			vertex.data() = newval;
		}

		edge_dir_type scatter_edges(icontext_type& context,
				const vertex_type& vertex) const {
			if (last_change > TOLERANCE) return graphlab::OUT_EDGES;
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
		strm << "vertex:" << v.id() << "\t" << v.data() << "\n";
		return strm.str();
	}
	std::string save_edge(graph_type::edge_type e) { return ""; }
}; // end of pagerank writer

int main(int argc, char** argv) {
	graphlab::mpi_tools::init(argc, argv);
	graphlab::distributed_control dc;
	global_logger().set_log_level(LOG_INFO);

	// Parse command line options -----------------------------------------------
	graphlab::command_line_options clopts("PageRank algorithm.");
	std::string graph_dir;
	std::string format = "adj";
	std::string exec_type = "synchronous";
	clopts.attach_option("graph", graph_dir,
			"The graph file. Required ");
	clopts.add_positional("graph");
	clopts.attach_option("format", format,
			"The graph file format");
	clopts.attach_option("engine", exec_type, 
			"The engine type synchronous or asynchronous");
	clopts.attach_option("tol", TOLERANCE,
			"The permissible change at convergence.");
	std::string saveprefix;
	clopts.attach_option("saveprefix", saveprefix,
			"If set, will save the resultant pagerank to a "
			"sequence of files with prefix saveprefix");

	if(!clopts.parse(argc, argv)) {
		dc.cout() << "Error in parsing command line arguments." << std::endl;
		return EXIT_FAILURE;
	}

	if (graph_dir == "") {
		dc.cout() << "Graph not specified. Cannot continue";
		return EXIT_FAILURE;
	}

	graph_type graph(dc, clopts);
	dc.cout() << "Loading graph in format: "<< format << std::endl;
	graph.load_format(graph_dir, format);

	graph.finalize();
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



