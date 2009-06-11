# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options

MAKEFLAGS  =  #-w -k -s

# FLAGSMACHINE=-march=athlon64 -mfpmath=sse -msse2 -m64
FLAGSMACHINE = -march=core2 -mfpmath=sse -mtune=core2  -msse3 -m64 -m3dnow

CXX       =  g++
# CCFLAGS   = -Wall -W -O1 -g -pg -fopenmp #if profiling
CCFLAGS   = -Wall -W -O3  -fopenmp ${FLAGSMACHINE}
CCFLAGS  += -funroll-loops -fvariable-expansion-in-unroller
#CCFLAGS  += -fomit-frame-pointer -ffast-math -funsafe-math-optimizations

INCLUDE   = $(shell root-config --cflags) -I./

LIBS      = $(shell root-config --libs)



# Define binary file name.
BINS = train classify 

# Define targets

all: $(BINS)
	@echo ""
	@echo "-> Please check the errors and warnings"
	@echo "-> before running the application."
	@echo ""

PndLVQClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o $@.o

PndLVQTrain:% : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o $@.o

classify: % : %.cpp PndLVQClassify
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o $@.o
	$(CXX) $(CCFLAGS) $(INCLUDE) $(LIBS) PndLVQClassify.o classify.o -o $@
	@echo "Done building the classifier."

train: % : %.cpp PndLVQTrain
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o $@.o
	$(CXX) $(CCFLAGS) $(INCLUDE) $(LIBS) PndLVQTrain.o train.o  -o $@
	@echo "Done, building the trainer."

clean:
	rm -f $(BINS) *~ *.o

cleanall:
	rm -f $(BINS) *~ *.o *.root
