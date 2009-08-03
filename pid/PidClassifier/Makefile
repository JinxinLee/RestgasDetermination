####################################################
# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options
#####################################################

# FLAGSMACHINE=-march=athlon64 -mfpmath=sse -msse2 -m64
# -ansi -std=c++98  -Wunsafe-loop-optimizations
# -fprofile-generate -fprofile-use -ftest-coverage

MAKEFLAGS  = -wk #s

FLAGSMACHINE = -march=native -mtune=native -mfpmath=sse -msse3 -m3dnow #-m64

export FLAGSMACHINE

export CXX = g++

# CCFLAGS   = -O0 -g -pg #if profiling
CCFLAGS  = -Wall -W -Wredundant-decls -Wshadow -Werror
CCFLAGS += -O3 -fopenmp ${FLAGSMACHINE} -ftracer
CCFLAGS += -funsafe-loop-optimizations -fvariable-expansion-in-unroller
CCFLAGS += -funroll-loops -fsplit-ivs-in-unroller -fprefetch-loop-arrays

export CCFLAGS

INCLUDE = $(shell root-config --cflags) -I./ -I../

export INCLUDE

LIBS  = $(shell root-config --libs) -lTMVA -lTreePlayer -lMinuit 
LIBS += -lMLP -lXMLIO #-fprofile-generate

export LIBS

# Define targets
TRGTS = PndMvaUtil PndMvaDataSet PndGpidClassifier Classifiers PndMvaTrainer examples 

# .SUFFIXES: .cpp

all: $(TRGTS)
	@echo ""
	@echo "-> Please check the errors and warnings"
	@echo "-> before running the application."
	@echo ""

PndGpidClassifier: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o build/$@.o
	@echo "Done building:" $@

PndMvaDataSet: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o build/$@.o
	@echo "Done building:" $@

PndMvaUtil: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o build/$@.o
	@echo "Done building:" $@

PndMvaTrainer: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o build/$@.o
	@echo "Done building:" $@

Classifiers:
	$(MAKE) -C KNN
	$(MAKE) -C TMVAkd_KNN
	$(MAKE) -C LVQ1

examples: PndMvaUtil PndMvaDataSet PndGpidClassifier Classifiers
	@echo ""
	@echo "       =++++++ Linking example applications. ++++++="
	@echo ""
	$(CXX) $(LIBS) build/classifyStdKnn.o  build/PndStdKnnClassify.o build/PndGpidClassifier.o \
	build/PndMvaUtil.o build/PndMvaDataSet.o -o build/classifyStdKnn

	$(CXX) $(LIBS) build/LVQclassify.o build/PndLVQClassify.o build/PndGpidClassifier.o \
	build/PndMvaUtil.o build/PndMvaDataSet.o -o build/LVQclassify

	$(CXX) $(LIBS) build/KnnClassify.o build/PndKnnClassify.o build/PndGpidClassifier.o \
	build/PndMvaUtil.o build/PndMvaDataSet.o -o build/KNNclassify

	$(CXX) $(LIBS) build/Prclassify.o build/PndProjectedKNN.o build/PndKnnClassify.o \
	build/PndGpidClassifier.o build/PndMvaUtil.o build/PndMvaDataSet.o -o build/PrKNNclassify

	$(CXX) $(LIBS) build/LVQtrain.o build/PndMvaTrainer.o build/PndMvaUtil.o build/PndLVQTrain.o \
	build/PndMvaDataSet.o -o build/LVQtrain

	$(CXX) $(LIBS) build/KnnTrain.o build/PndMvaTrainer.o build/PndMvaUtil.o build/PndKnnTrain.o \
	build/PndMvaDataSet.o -o build/KNNTrain

clean:
	rm -f *~ *.o *.root build/*
	cd KNN ; $(MAKE) clean
	cd LVQ1; $(MAKE) clean
	cd TMVAkd_KNN; $(MAKE) clean
