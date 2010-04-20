# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options

#INCLUDE   = $(shell root-config --cflags) -I./ -I../
#LIBS      = $(shell root-config --libs)

# Define targets
TARGETS = exampleCluster PndMvaCluster

all: $(TARGETS)

PndMvaCluster:% : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building:" $@

exampleCluster: % : %.cpp PndMvaCluster
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building:" $@

clean:
	rm -f *~ *.o