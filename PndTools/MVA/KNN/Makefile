# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options
INCLUDE   = $(shell root-config --cflags)
LIBS      = $(shell root-config --libs) 
CXX       = g++
CCFLAGS   = -Wall -Os -O3 -funroll-loops

# Define binary file name.
BINS = PndKnnClassify PndKnnTrain 

# Define targets
all: $(BINS)

$(BINS): % : %.cpp #$(HEADS)/*.h
	$(CXX) $(CCFLAGS) $< $(INCLUDE) $(LIBS) -o $@
	@echo "Done"

clean:
	rm -f $(BINS) *~

cleanall:
	rm -f $(BINS) *~ *.root
