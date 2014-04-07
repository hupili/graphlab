/* Reference:
 *    * http://docs.graphlab.org/RPC.html
 *
 *
 */

#include <graphlab.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>

graphlab::procid_t g_my_id = -1;
graphlab::procid_t g_num_procs = -1;

void print_by_core(const std::string& msg){
	std::cout<< "[proc " << g_my_id << "]: " << msg << std::endl;
}

int main(int argc, char** argv) {
	graphlab::mpi_tools::init(argc, argv);
	graphlab::distributed_control dc;
	dc.cout() << "GraphLab RPC Example\n";

	g_my_id = dc.procid();
	g_num_procs = dc.numprocs();
	dc.barrier();

	print_by_core("I'm online!");
	dc.barrier();

	// Every core will have different seed
	srand(g_my_id + clock());
	//std::string msg = "";
	std::stringstream msg;
	msg << "Greetings from core: " << g_my_id; 
	dc.remote_call(rand() % g_num_procs, print_by_core, msg.str());
	dc.barrier();

	graphlab::mpi_tools::finalize();
}
