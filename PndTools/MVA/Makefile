####################################################
# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options
#####################################################

# FLAGSMACHINE=-march=athlon64
# CCFLAGS += -Weffc++ -pedantic -fpic

MAKEFLAGS  = -wkr --no-print-directory #-s

FLAGSMACHINE = -march=native -mtune=native
# FLAGSMACHINE += -m64 -mfpmath=sse -mssse3 
# FLAGSMACHINE += -mpc64 -mfpmath=sse -msse3 -m3dnow

CCFLAGS  = -W -Wall -Wextra -Wredundant-decls -Wshadow -Werror
CCFLAGS += -fdiagnostics-show-option -fdiagnostics-show-location=every-line
CCFLAGS += -Woverlength-strings -Wstack-protector -fopenmp -funroll-loops
CCFLAGS += -O3 ${FLAGSMACHINE}
# CCFLAGS += -malign-double -maccumulate-outgoing-args -ftracer
# CCFLAGS += -fvariable-expansion-in-unroller -fprefetch-loop-arrays
# CCFLAGS += -fstrict-aliasing -Wstrict-aliasing=3

INCLUDE = $(shell root-config --cflags) -I./

LIBS  = $(shell root-config --libs) -lTMVA -lTreePlayer -lMinuit
LIBS += -lMLP -lXMLIO -fopenmp

.EXPORT_ALL_VARIABLES:

# .SUFFIXES: .cpp .o
# .PHONY: all

# Define targets
TRGTS  = PndMvaUtil PndMvaDataSet PndGpidClassifier PndMvaTrainer
TRGTS +=  CreateRandomDataSet Classifiers examples

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

# Build stdKnn Cassifier.
	$(CXX) $(CCFLAGS) $(LIBS) build/classifyStdKnn.o  build/PndStdKnnClassify.o build/PndGpidClassifier.o \
	build/PndMvaUtil.o build/PndMvaDataSet.o -o build/classifyStdKnn
# Build LVQ Cassifier.
	$(CXX) $(CCFLAGS) $(LIBS) build/LVQclassify.o build/PndLVQClassify.o build/PndGpidClassifier.o \
	build/PndMvaUtil.o build/PndMvaDataSet.o -o build/LVQclassify
# Build kd-tree KNN Cassifier.
	$(CXX) $(CCFLAGS) $(LIBS) build/KnnClassify.o build/PndKnnClassify.o build/PndGpidClassifier.o \
	build/PndMvaUtil.o build/PndMvaDataSet.o -o build/KNNclassify
# Build Projected KNN Cassifier.
	$(CXX) $(CCFLAGS) $(LIBS) build/Prclassify.o build/PndProjectedKNN.o build/PndKnnClassify.o \
	build/PndGpidClassifier.o build/PndMvaUtil.o build/PndMvaDataSet.o -o build/PrKNNclassify
# Build LVQ Trainer.
	$(CXX) $(CCFLAGS) $(LIBS) build/LVQtrain.o build/PndMvaTrainer.o build/PndMvaUtil.o build/PndLVQTrain.o \
	build/PndMvaDataSet.o build/PndMvaCluster.o -o build/LVQtrain
# Build KNN Trainer.
	$(CXX) $(CCFLAGS) $(LIBS) build/KnnTrain.o build/PndMvaTrainer.o build/PndMvaUtil.o build/PndKnnTrain.o \
	build/PndMvaDataSet.o -o build/KNNTrain
# Build a random dataset creator.
	$(CXX) $(CCFLAGS) $(LIBS) build/CreateRandomDataSet.o -o build/CreateRandomDataSet
# Build an example to use clustering module.
	$(CXX) $(CCFLAGS) $(LIBS) build/PndMvaCluster.o build/exampleCluster.o build/PndMvaDataSet.o \
	build/PndMvaUtil.o -o build/exampleCluster

# Build an example app using parzenWindow classifier.
	$(CXX) $(CCFLAGS) $(LIBS) build/classifyPrzWin.o  build/PndPrzWindowClassify.o \
	build/PndGpidClassifier.o build/PndMvaUtil.o build/PndMvaDataSet.o -o build/przWinClassify

# Build PDE-Foam trainer and classifier.
#	$(CXX) $(CCFLAGS) $(LIBS) build/FoamTrainTest.o build/PndMvaTrainer.o build/PndMvaUtil.o \
#	build/PndPDEFoamTrain.o build/PndMvaDataSet.o -o build/FoamTrainTest

#	$(CXX) $(CCFLAGS) $(LIBS) build/FoamClassifyTest.o build/PndPDEFoamClassify.o build/PndGpidClassifier.o \
#	build/PndMvaDataSet.o build/PndMvaUtil.o -o build/FoamClassifyTest

clean:
	rm -f *~ *.o *.root build/*
	cd Clusters; $(MAKE) clean
	cd KNN ; $(MAKE) clean
	cd LVQ1; $(MAKE) clean
	cd TMVAkd_KNN; $(MAKE) clean
	cd TMVA_PDE_FOAM; $(MAKE) clean
