# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options

#INCLUDE   = $(shell root-config --cflags) -I./ -I../
#LIBS      = $(shell root-config --libs)

HDR = -I../

# Define targets
TARGETS = exampleCluster

all: $(TARGETS)

PndMvaCluster:% : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

exampleCluster: % : %.cpp PndMvaCluster
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

clean:
	rm -f *~ *.o