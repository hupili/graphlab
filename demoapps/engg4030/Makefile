# Shortcut to the original GraphLab API workflow
# 
# Usage:
#    * Run `make` to configure, compile and copy
#    * To clean the build environment, just `rm _configured`

# Caculate the limiting factor of CPU or memory
TOTAL_MEM_K:=$(shell cat /proc/meminfo | grep MemTotal | awk '{print $$2}')
TOTAL_MEM_G:=$(shell expr $(TOTAL_MEM_K) / 1048576)
LAST_CPU:=$(shell cat /proc/cpuinfo | grep processor | tail -n1 | awk '{print $$3}')
TOTAL_CPU:=$(shell expr $(LAST_CPU) + 1)
NUM_JOBS:=$(shell echo "$(TOTAL_MEM_G)\n$(TOTAL_CPU)" | sort -n | head -n1)

# Whether ./configure was executed
CONFIGURED:=$(shell test -s _configured; echo $$?)

# All executables
ALL_EXECS:=$(patsubst %.cpp,output/%,$(wildcard *.cpp))

all: configure report compile copy

compile:
	cd ../../debug/demoapps/engg4030/; make -j $(NUM_JOBS)

copy: make_output_dir $(ALL_EXECS)

make_output_dir:
	mkdir -p output
	@echo Start to copy files: $(ALL_EXECS)

output/%: %.cpp
	cp ../../debug/demoapps/engg4030/$* output/

report:
	@echo total number of memory: $(TOTAL_MEM_G)G
	@echo total number of CPUs: $(TOTAL_CPU)
	@echo concurrent jobs: $(NUM_JOBS)

configure:
	echo "Configured! `rm _configured` if you want to re-configure" > _configured
	test 1 = $(CONFIGURED) && cd ../../ && ./configure; test 1 = 1

clean:
	rm -rf output
