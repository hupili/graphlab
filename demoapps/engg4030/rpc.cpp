/* Reference:
 *    * http://docs.graphlab.org/RPC.html
 *
 *
 */

#include <graphlab.hpp>
#include <iostream>

graphlab::procid_t g_my_id = -1;
graphlab::procid_t g_num_procs = -1;

void print_by_core(const std::string& msg){
	std::cout<< "[proc " << g_my_id << "]: " << msg << std::endl;
}

float rand_u01(){
	return graphlab::random::rand01();
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

	dc.cout() << "Random numbers:" << rand_u01() << "\n" ;
	dc.cout() << "Random numbers:" << rand_u01() << "\n" ;
	dc.cout() << "Random numbers:" << rand_u01() << "\n" ;
	dc.cout() << "Random numbers:" << rand_u01() << "\n" ;

	// Every core will have different seed
	srand(g_my_id + clock());
	//std::string msg = "";
	std::stringstream msg;
	msg << "Greetings from core: " << g_my_id; 
	dc.remote_call(int(rand_u01() * g_num_procs), print_by_core, msg.str());
	dc.barrier();

	graphlab::mpi_tools::finalize();
}
