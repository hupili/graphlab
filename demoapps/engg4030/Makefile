# Shortcut to the original GraphLab API workflow

# Caculate the limiting factor of CPU or memory
TOTAL_MEM_K=$(shell cat /proc/meminfo | grep MemTotal | awk '{print $$2}')
TOTAL_MEM_G=$(shell expr $(TOTAL_MEM_K) / 1048576)
LAST_CPU=$(shell cat /proc/cpuinfo | grep processor | tail -n1 | awk '{print $$3}')
TOTAL_CPU=$(shell expr $(LAST_CPU) + 1)
NUM_JOBS=$(shell echo "$(TOTAL_MEM_G)\n$(TOTAL_CPU)" | sort -n | head -n1)

all: report compile copy

compile:
	cd ../../debug/demoapps/engg4030/; make -j $(NUM_JOBS)

copy:
	cp ../../debug/demoapps/engg4030/simple_pagerank_annotated output
	cp ../../debug/demoapps/engg4030/hello output

report:
	@echo total number of memory: $(TOTAL_MEM_G)G
	@echo total number of CPUs: $(TOTAL_CPU)
	@echo concurrent jobs: $(NUM_JOBS)

clean:
	rm -rf output
