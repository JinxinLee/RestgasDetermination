# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options
INCLUDE   = $(shell root-config --cflags)
LIBS      = $(shell root-config --libs) 
CXX       = g++
CCFLAGS   = -Wall -g # -O2 # -fopenmp

# Define binary file name.
BINS = PndLVQClassify  PndLVQTrain

# Define targets
all: $(BINS)

$(BINS): % : %.cpp #$(HEADS)/*.h
	@echo "Compiling " $@	
	$(CXX) $(CCFLAGS) $< $(INCLUDE) $(LIBS) -o $@
	@echo "Done"

clean:
	rm -f $(BINS) *~ *.so
