# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options

#INCLUDE   = $(shell root-config --cflags) -I./ -I../

#LIBS      = $(shell root-config --libs)


# Define binary file name.#
TARGETS = PndStdKnnClassify classifyStdKnn

# Define targets
all: $(TARGETS)

PndStdKnnClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building:" $@

classifyStdKnn: % : %.cpp PndStdKnnClassify
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building:" $@

clean:
	rm -f *~