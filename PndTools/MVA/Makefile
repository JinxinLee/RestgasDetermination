####################################################
# Set openMP env. vars
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options
#####################################################

# FLAGSMACHINE  = -march=athlon64
# FLAGSMACHINE += -m64 -mfpmath=sse -mssse3 
# CCFLAGS += -Weffc++ -pedantic -fpic

MAKEFLAGS  = -wkr --no-print-directory #--debug -s

FLAGSMACHINE  = -march=native -mtune=native
FLAGSMACHINE += -mfpmath=sse -msse3 -mstackrealign

CCFLAGS  = -W -Wall -Wextra -Wshadow -Wredundant-decls
CCFLAGS += -Wnormalized=nfc -Wdisabled-optimization -Woverlength-strings
CCFLAGS += -Winvalid-pch -Wfloat-equal -Werror
CCFLAGS += -fdiagnostics-show-option -fdiagnostics-show-location=every-line
CCFLAGS += -O3 -fopenmp -funroll-loops -fsplit-ivs-in-unroller
CCFLAGS += -fvariable-expansion-in-unroller
CCFLAGS += ${FLAGSMACHINE}

INCLUDE = $(shell root-config --cflags) -I./

LIBS  = $(shell root-config --libs) -lTMVA -lTreePlayer -lMinuit
LIBS += -lMLP -lXMLIO -fopenmp

.EXPORT_ALL_VARIABLES:

# .SUFFIXES: .cpp .o
# .PHONY: all

# Define targets
TRGTS  = PndMvaUtil PndMvaTools PndMvaVarPCATransform PndMvaDataSet
TRGTS += PndMvaClassifier PndMvaTrainer CreateRandomDataSet
TRGTS += examplePCATransform Classifiers examples

all: $(TRGTS)
	@echo ""
	@echo "-> Please check the errors and warnings"
	@echo "-> before running the application."
	@echo ""

PndMvaClassifier: % : %.cpp PndMvaDataSet
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

PndMvaDataSet: % : %.cpp PndMvaVarPCATransform
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

PndMvaUtil:  % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

PndMvaTools: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

PndMvaTrainer: % : %.cpp PndMvaDataSet
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

CreateRandomDataSet: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

PndMvaVarPCATransform: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

examplePCATransform: % : %.cpp PndMvaDataSet
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ./build/$@.o
	@echo "Done building:" $@

Classifiers:
	$(MAKE) -C KNN;
	$(MAKE) -C LVQ;
	$(MAKE) -C TMVAkd_KNN;
	$(MAKE) -C Clusters;
	$(MAKE) -C TMVA_MCL;
#	$(MAKE) -C TMVA_PDE_FOAM;

examples: PndMvaUtil PndMvaDataSet PndMvaClassifier Classifiers
	@echo ""
	@echo "       =++++++ Linking example applications. ++++++="
	@echo ""

# Build stdKnn Cassifier.
	$(CXX) $(CCFLAGS) $(LIBS) build/classifyStdKnn.o  build/PndStdKnnClassify.o build/PndMvaClassifier.o \
	build/PndMvaUtil.o build/PndMvaDataSet.o build/PndMvaVarPCATransform.o build/PndMvaTools.o \
	-o build/classifyStdKnn

# Build LVQ Cassifier.
	$(CXX) $(CCFLAGS) $(LIBS) build/LVQclassify.o build/PndLVQClassify.o build/PndMvaClassifier.o \
	build/PndMvaUtil.o build/PndMvaTools.o build/PndMvaDataSet.o build/PndMvaVarPCATransform.o \
	-o build/LVQclassify

# Build kd-tree KNN Cassifier.
	$(CXX) $(CCFLAGS) $(LIBS) build/KnnClassify.o build/PndKnnClassify.o build/PndMvaClassifier.o \
	build/PndMvaUtil.o build/PndMvaTools.o build/PndMvaDataSet.o build/PndMvaVarPCATransform.o \
	-o build/KNNclassify

# Build Projected KNN Cassifier.
	$(CXX) $(CCFLAGS) $(LIBS) build/Prclassify.o build/PndProjectedKNN.o build/PndKnnClassify.o \
	build/PndMvaClassifier.o build/PndMvaUtil.o build/PndMvaDataSet.o build/PndMvaVarPCATransform.o \
	-o build/PrKNNclassify

# Build modified version KNN Cassifier (just for evaluation).
	$(CXX) $(CCFLAGS) $(LIBS) build/KnnClassifyModified.o build/PndKnnClassify.o build/PndMvaClassifier.o \
	build/PndMvaUtil.o build/PndMvaDataSet.o build/PndMvaVarPCATransform.o -o build/KnnModifclassify

# Build LVQ Trainer.
	$(CXX) $(CCFLAGS) $(LIBS) build/LVQtrain.o build/PndMvaTrainer.o build/PndMvaUtil.o build/PndLVQTrain.o \
	build/PndMvaDataSet.o build/PndMvaCluster.o build/PndMvaVarPCATransform.o build/PndMvaTools.o \
	-o build/LVQtrain

# Build KNN Trainer.
	$(CXX) $(CCFLAGS) $(LIBS) build/KnnTrain.o build/PndMvaTrainer.o build/PndMvaUtil.o build/PndKnnTrain.o \
	build/PndMvaDataSet.o build/PndMvaVarPCATransform.o -o build/KNNTrain

# Build an example to use clustering module.
	$(CXX) $(CCFLAGS) $(LIBS) build/PndMvaCluster.o build/exampleCluster.o build/PndMvaDataSet.o \
	build/PndMvaUtil.o build/PndMvaVarPCATransform.o -o build/exampleCluster

# Build an example app using parzenWindow classifier.
	$(CXX) $(CCFLAGS) $(LIBS) build/classifyPrzWin.o  build/PndPrzWindowClassify.o \
	build/PndMvaClassifier.o build/PndMvaUtil.o build/PndMvaDataSet.o build/PndMvaVarPCATransform.o \
	build/PndMvaTools.o -o build/przWinClassify

# Build PDE-Foam trainer and classifier.
#	$(CXX) $(CCFLAGS) $(LIBS) build/FoamTrainTest.o build/PndMvaTrainer.o build/PndMvaUtil.o \
#	build/PndPDEFoamTrain.o build/PndMvaDataSet.o -o build/FoamTrainTest

#	$(CXX) $(CCFLAGS) $(LIBS) build/FoamClassifyTest.o build/PndPDEFoamClassify.o build/PndMvaClassifier.o \
#	build/PndMvaDataSet.o build/PndMvaUtil.o -o build/FoamClassifyTest

# Build a random dataset creator.
	$(CXX) $(CCFLAGS) $(LIBS) build/CreateRandomDataSet.o build/PndMvaVarPCATransform.o \
	-o build/CreateRandomDataSet

# Build PCA example
	$(CXX) $(CCFLAGS) $(LIBS) build/examplePCATransform.o build/PndMvaVarPCATransform.o \
	build/PndMvaDataSet.o -o build/pcaExample

# Build MLP trainer.
	$(CXX) $(CCFLAGS) $(LIBS) build/mlpTrain.o build/PndMvaTrainer.o build/PndMvaUtil.o \
	build/PndMultiClassMlpTrain.o build/PndMvaDataSet.o build/PndMvaVarPCATransform.o \
	-o build/mlpTrain

# Build MLP Classifier.
	$(CXX) $(CCFLAGS) $(LIBS) build/mlpClassify.o build/PndMvaClassifier.o build/PndMvaUtil.o \
	build/PndMultiClassMlpClassify.o build/PndMvaDataSet.o build/PndMvaVarPCATransform.o \
	build/PndMvaTools.o -o build/mlpClassify

# Build BDT Trainer
	$(CXX) $(CCFLAGS) $(LIBS) build/bdtTrain.o build/PndMvaTrainer.o build/PndMvaUtil.o \
	build/PndMultiClassBdtTrain.o build/PndMvaDataSet.o build/PndMvaVarPCATransform.o \
	-o build/bdtTrain

# Build BDT Classifier
	$(CXX) $(CCFLAGS) $(LIBS) build/bdtClassify.o build/PndMvaClassifier.o build/PndMvaUtil.o \
	build/PndMultiClassBdtClassify.o build/PndMvaDataSet.o build/PndMvaVarPCATransform.o \
	build/PndMvaTools.o -o build/bdtClassify

clean:
	rm -f *~ *.o build/*
	cd Clusters; $(MAKE) clean
	cd KNN ; $(MAKE) clean
	cd LVQ; $(MAKE) clean
	cd TMVAkd_KNN; $(MAKE) clean
	cd TMVA_PDE_FOAM; $(MAKE) clean
	cd TMVA_MCL; $(MAKE) clean

distclean: clean
	rm -f  *.root
	rm -fr PndMVAWeights/* plots
	cd macro; $(MAKE) cleanall
