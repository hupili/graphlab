project(GraphLab)

add_graphlab_executable(simple_pagerank_annotated simple_pagerank_annotated.cpp)
add_graphlab_executable(hello hello.cpp)
add_graphlab_executable(rpc rpc.cpp)
add_graphlab_executable(pagerank_base pagerank_base.cpp)
add_graphlab_executable(pagerank_scatter pagerank_scatter.cpp)
add_graphlab_executable(pagerank_fixed_iter_vertex pagerank_fixed_iter_vertex.cpp)
add_graphlab_executable(pagerank_fixed_iter_engine pagerank_fixed_iter_engine.cpp)
add_graphlab_executable(pagerank_simulation pagerank_simulation.cpp)
