#!/bin/sh
#
# The output may be long and cluttered.
# The best way to explore is to manually issue those commands.
# They are listed here as hints.

# Hello world
./output/hello
mpiexec -n 4 ./output/hello

# RPC
mpiexec -n 4 ./output/rpc

./output/simple_pagerank_annotated  --graph sample_tsv/tsv --format tsv --saveprefix sample_output/pr
