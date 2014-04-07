#include <graphlab.hpp>
int main(int argc, char** argv) {
  graphlab::mpi_tools::init(argc, argv);
  graphlab::distributed_control dc;
  dc.cout() << "Hello World!\n";
  dc.cout() << "Hello GraphLab!\n";
  graphlab::mpi_tools::finalize();
}
