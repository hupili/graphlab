#!/bin/sh
#
# The output may be long and cluttered.
# The best way to explore is to manually issue those commands.
# They are listed here as hints.

# Hello world
./output/hello
mpiexec -n 4 ./output/hello

# RPC
./output/rpc
