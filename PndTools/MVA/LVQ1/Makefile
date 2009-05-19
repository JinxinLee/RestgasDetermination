# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options

MAKEFLAGS  =  -w #-k -s

# FLAGSMACHINE=-march=athlon64 -mfpmath=sse -msse2 -m64
FLAGSMACHINE = -march=core2 -mfpmath=sse -mtune=core2  -msse3 -m64 -m3dnow

CXX       =  g++
# CCFLAGS   = -Wall -W -g -O0
# CCFLAGS   = -Wall -W -O1 -fopenmp -g -pg #if profiling
CCFLAGS   = -Wall -W -Os -O3 -ffast-math ${FLAGSMACHINE}
CCFLAGS  += -fomit-frame-pointer -funsafe-math-optimizations -fopenmp
CCFLAGS  += -funroll-loops -fvariable-expansion-in-unroller

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

classify: % : %.cpp PndLVQClassify
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o
	$(CXX) $(CCFLAGS) classify.o PndLVQClassify.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done building the classifier."

PndLVQClassify: % : %.cpp
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o

train: % : %.cpp PndLVQTrain
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o
	$(CXX) $(CCFLAGS) train.o PndLVQTrain.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done, building the trainer."

PndLVQTrain:% : %.cpp
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o

clean:
	rm -f $(BINS) *~ *.o

cleanall:
	rm -f $(BINS) *~ *.o *.root
