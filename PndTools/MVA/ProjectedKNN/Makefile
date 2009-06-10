# Setting the linker and compiler flags and options
# points in k-dimensional Euclidean space.

# FLAGSMACHINE=-march=athlon64 -mfpmath=sse -msse2 -m64
FLAGSMACHINE = -march=core2 -mfpmath=sse -mtune=core2  -msse3 -m64 -m3dnow

MAKEFLAGS  =  -w -k #-s 

# CCFLAGS  = -Wall -W -g -ggdb -O0 -pg # Profiling and debug

# Define C++ compiler name
CXX       =  g++

# Define compiler flags
CCFLAGS   = -Wall -W -O3 -fopenmp ${FLAGSMACHINE}
CCFLAGS  += -funroll-loops -fvariable-expansion-in-unroller
# CCFLAGS  += -ffast-math -funsafe-math-optimizations -fomit-frame-pointer

# Path to TMVA headers
HEADS    = /home0/fairroot/fairsoft/tools/root/tmva/inc

INCLUDE   = -I/opt/fairroot/fairsoft/tools/root/tmva/inc
INCLUDE  += $(shell root-config --cflags) -I./

LIBS     = -lTMVA -lMLP -lTreePlayer -lMinuit
LIBS    += $(shell root-config --libs) 

# Define binary file name.
BINS = Prclassify #classify train

# Define targets

all: $(BINS)
	@echo "-> Please check the errors and warnings"
	@echo "-> before running the application."
	@echo ""


PndKnnClassify: % : %.cpp $(HEADS)/*.h
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o

classify: % : %.cpp $(HEADS)/*.h PndKnnClassify
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o
	$(CXX) $(CCFLAGS) classify.o PndKnnClassify.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done building the classifier."
	@echo ""

PndKnnTrain: % : %.cpp $(HEADS)/*.h
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o

train: % : %.cpp $(HEADS)/*.h PndKnnTrain
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o
	$(CXX) $(CCFLAGS) train.o PndKnnTrain.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done, building the trainer."
	@echo ""

PndProjectedKNN: % : %.cpp $(HEADS)/*.h
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o

Prclassify: % : %.cpp $(HEADS)/*.h PndKnnClassify PndProjectedKNN
	$(CXX) $(CCFLAGS) -c $< $(INCLUDE) -o $@.o
	$(CXX) $(CCFLAGS) Prclassify.o PndKnnClassify.o PndProjectedKNN.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done building the projected classifier."
	@echo ""

clean:
	rm -f $(BINS) *~ *.o

cleanall:
	rm -f $(BINS) *~ *.o *.root
