# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options

#INCLUDE   = $(shell root-config --cflags) -I./ -I../
#LIBS      = $(shell root-config --libs)

HDR = -I../

# Define targets
TARGETS = exampleCluster.o

all: $(TARGETS)

PndMvaCluster.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c PndMvaCluster.cpp
	@echo "Done building:" $@

exampleCluster.o: PndMvaCluster.o
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c exampleCluster.cpp
	@echo "Done building:" $@

clean:
	rm -f *~ *.o