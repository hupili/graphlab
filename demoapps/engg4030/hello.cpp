#include <graphlab.hpp>
#include <iostream>

int main(int argc, char** argv) {
  graphlab::mpi_tools::init(argc, argv);
  graphlab::distributed_control dc;
  dc.cout() << "Hello World! (From distributed control)\n";
  std::cout << "Output per core! (From every core)" << std::endl;
  graphlab::mpi_tools::finalize();
}
