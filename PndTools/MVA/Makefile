####################################################
# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options
#####################################################

# FLAGSMACHINE=-march=athlon64 -mfpmath=sse -msse2 -m64
# -ansi -std=c++98  -Wunsafe-loop-optimizations
# -fprofile-generate -fprofile-use -ftest-coverage

# -s
MAKEFLAGS  = -wkr --no-print-directory

FLAGSMACHINE = -march=native -mtune=native -mfpmath=sse
FLAGSMACHINE += -msse3 -m3dnow -m64 -mssse3 -mpc64
FLAGSMACHINE += -m128bit-long-double -mcx16

# -fopenmp -Weffc++
# CCFLAGS = -O0 -g -pg # if profiling

CCFLAGS  = -Wall -W -Wredundant-decls -Wshadow -Wcoverage-mismatch -Werror
CCFLAGS += -ftracer ${FLAGSMACHINE} -O3
CCFLAGS += -funroll-loops -funsafe-loop-optimizations -fvariable-expansion-in-unroller
CCFLAGS += -fsplit-ivs-in-unroller -fprefetch-loop-arrays
#-fprofile-use
CCFLAGS += -malign-double  -momit-leaf-frame-pointer -fomit-frame-pointer
CCFLAGS += -maccumulate-outgoing-args
CCFLAGS += -fdiagnostics-show-option -fdiagnostics-show-location=every-line

INCLUDE = $(shell root-config --cflags) -I./ -I../

LIBS  = $(shell root-config --libs) -lTMVA -lTreePlayer -lMinuit 
LIBS += -lMLP -lXMLIO -fopenmp # -fprofile-generate

.EXPORT_ALL_VARIABLES:

# .SUFFIXES: .cpp .o
# .PHONY: all

# Define targets
TRGTS = PndMvaUtil PndMvaDataSet PndGpidClassifier PndMvaTrainer CreateRandomDataSet Classifiers examples

all: $(TRGTS)
	@echo ""
	@echo "-> Please check the errors and warnings"
	@echo "-> before running the application."
	@echo ""

PndGpidClassifier: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

PndMvaDataSet: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

PndMvaUtil:  % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

PndMvaTrainer: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

CreateRandomDataSet: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

Classifiers:
	$(MAKE) -C KNN;
	$(MAKE) -C LVQ1;
	$(MAKE) -C TMVAkd_KNN;
	$(MAKE) -C Clusters;
#	$(MAKE) -C TMVA_PDE_FOAM;

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

#	$(CXX) $(LIBS) build/FoamTrainTest.o build/PndMvaTrainer.o build/PndMvaUtil.o \
#	build/PndPDEFoamTrain.o build/PndMvaDataSet.o -o build/FoamTrainTest

#	$(CXX) $(LIBS) build/FoamClassifyTest.o build/PndPDEFoamClassify.o build/PndGpidClassifier.o \
#	build/PndMvaDataSet.o build/PndMvaUtil.o -o build/FoamClassifyTest

	$(CXX) $(LIBS) build/CreateRandomDataSet.o -o build/CreateRandomDataSet

	$(CXX) $(LIBS) build/PndMvaCluster.o build/exampleCluster.o -o build/exampleCluster
clean:
	rm -f *~ *.o *.root build/*
	cd KNN ; $(MAKE) clean
	cd LVQ1; $(MAKE) clean
	cd TMVAkd_KNN; $(MAKE) clean
	cd TMVA_PDE_FOAM; $(MAKE) clean
	cd Clusters; $(MAKE) clean