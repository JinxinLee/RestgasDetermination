# Setting the linker and compiler flags and options
# points in k-dimensional Euclidean space.

# FLAGSMACHINE=-march=athlon64 -mfpmath=sse -msse2 -m64
# FLAGSMACHINE = -march=core2 -mfpmath=sse -mtune=core2  -msse3 -m64 -m3dnow

MAKEFLAGS  =  -w -k #-s 

# CCFLAGS  = -Wall -W -g -ggdb -O0 -pg # Profiling and debug

CXX       =  g++

# CCFLAGS   = -Wall -W -Os -O3 -funroll-loops # -fopenmp
CCFLAGS   = -Wall -W -O2 -funroll-loops
CCFLAGS  += -ffast-math ${FLAGSMACHINE} -fomit-frame-pointer
CCFLAGS  += -fvariable-expansion-in-unroller -funsafe-math-optimizations

HEADS    = /home0/fairroot/fairsoft/tools/root/tmva/inc

INCLUDE   = -I/opt/fairroot/fairsoft/tools/root/tmva/inc
INCLUDE  += $(shell root-config --cflags) -I./

LIBS     = -lTMVA -lMLP -lTreePlayer -lMinuit
LIBS    += $(shell root-config --libs) 


# Define binary file name.
BINS = classify train #kdKnnTMVA

# Define targets

all: $(BINS)
	@echo "-> Please check the errors and warnings"
	@echo "-> before running the application."
	@echo ""

PndKnnTrain: % : %.cpp $(HEADS)/*.h
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o

PndKnnClassify: % : %.cpp $(HEADS)/*.h
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o

train: % : %.cpp $(HEADS)/*.h PndKnnTrain
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o
	$(CXX) $(CCFLAGS) train.o PndKnnTrain.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done, building the trainer."
	@echo ""

classify: % : %.cpp $(HEADS)/*.h PndKnnClassify
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o
	$(CXX) $(CCFLAGS) classify.o PndKnnClassify.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done building the classifier."
	@echo ""

kdKnnTMVA: % : %.cpp $(HEADS)/*.h
	$(CXX) $(CCFLAGS) $< $(INCLUDE) $(LIBS) -o $@
	@echo "Done building Sample."
	@echo ""

clean:
	rm -f $(BINS) *~ *.o

cleanall:
	rm -f $(BINS) *~ *.o *.root
