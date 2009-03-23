# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options

# FLAGSMACHINE=-march=athlon64 -mfpmath=sse -msse2 -m64
FLAGSMACHINE = -march=core2 -mfpmath=sse -mtune=core2  -msse3 -m64 -m3dnow

CXX       =  g++
# CCFLAGS   = -Wall -W -g -O0
# CCFLAGS   = -Wall -W -O1 -fopenmp -g -pg #if profiling
CCFLAGS       = -Wall -W -Os -O3 -ffast-math ${FLAGSMACHINE}
CCFLAGS      += -fopenmp -fomit-frame-pointer -funsafe-math-optimizations
CCFLAGS      += -funroll-loops -fvariable-expansion-in-unroller


INCLUDE   = $(shell root-config --cflags) -I./
#INCLUDE  += -I/opt/fairroot/fairsoft/basics/boost/include/
LIBS      = $(shell root-config --libs) 
#LIBS     += -L/opt/fairroot/fairsoft/basics/boost/lib/


# Define binary file name.
BINS = classify train

# Define targets

all: classify train

classify: % : %.cpp
	$(CXX) $(CCFLAGS) -c PndLVQClassify.cpp $< $(INCLUDE)
	$(CXX) $(CCFLAGS) classify.o PndLVQClassify.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done building the classifier."

train: % : %.cpp
	$(CXX) $(CCFLAGS) -c PndLVQTrain.cpp $< $(INCLUDE)
	$(CXX) $(CCFLAGS) train.o PndLVQTrain.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done, building the trainer."

clean:
	rm -f $(BINS) *~ *.o

cleanall:
	rm -f $(BINS) *~ *.o *.root