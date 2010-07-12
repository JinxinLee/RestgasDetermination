# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options

#INCLUDE   = $(shell root-config --cflags) -I./ -I../
#LIBS      = $(shell root-config --libs)

HDR = -I../

# Define binary file name.#
TARGETS = PndStdKnnClassify.o classifyStdKnn.o

# Define targets
all: $(TARGETS)

PndStdKnnClassify.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c PndStdKnnClassify.cpp
	@echo "Done building:" $@

classifyStdKnn.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c classifyStdKnn.cpp
	@echo "Done building:" $@

clean:
	rm -f *~ *.o