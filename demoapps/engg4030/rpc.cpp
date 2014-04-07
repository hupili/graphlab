#include <graphlab.hpp>
int main(int argc, char** argv) {
  graphlab::mpi_tools::init(argc, argv);
  graphlab::distributed_control dc;
  dc.cout() << "GraphLab RPC Example\n";
  graphlab::mpi_tools::finalize();
}
