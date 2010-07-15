####################################################
# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options
#####################################################

# FLAGSMACHINE=-march=athlon64
# CCFLAGS += -Weffc++ -pedantic

MAKEFLAGS  = -wkr --no-print-directory #-s

FLAGSMACHINE = -march=native -mtune=native
# FLAGSMACHINE += -m64 -mfpmath=sse -mssse3 
# FLAGSMACHINE += -mpc64 -mfpmath=sse -msse3 -m3dnow

CCFLAGS  = -W -Wall -Wextra -Wredundant-decls -Wshadow -Werror
CCFLAGS += -Woverlength-strings -Wstack-protector
CCFLAGS += -fdiagnostics-show-option -fdiagnostics-show-location=every-line
CCFLAGS += -O3 -funroll-loops -fopenmp
CCFLAGS += ${FLAGSMACHINE}
# CCFLAGS += -fvariable-expansion-in-unroller -fprefetch-loop-arrays
# CCFLAGS += -fstrict-aliasing -Wstrict-aliasing=3
# CCFLAGS += -malign-double -maccumulate-outgoing-args -ftracer


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

	$(CXX) $(LIBS) build/classifyStdKnn.o  build/PndStdKnnClassify.o build/PndGpidClassifier.o \
	build/PndMvaUtil.o build/PndMvaDataSet.o -o build/classifyStdKnn

	$(CXX) $(LIBS) build/LVQclassify.o build/PndLVQClassify.o build/PndGpidClassifier.o \
	build/PndMvaUtil.o build/PndMvaDataSet.o -o build/LVQclassify

	$(CXX) $(LIBS) build/KnnClassify.o build/PndKnnClassify.o build/PndGpidClassifier.o \
	build/PndMvaUtil.o build/PndMvaDataSet.o -o build/KNNclassify

	$(CXX) $(LIBS) build/Prclassify.o build/PndProjectedKNN.o build/PndKnnClassify.o \
	build/PndGpidClassifier.o build/PndMvaUtil.o build/PndMvaDataSet.o -o build/PrKNNclassify

	$(CXX) $(LIBS) build/LVQtrain.o build/PndMvaTrainer.o build/PndMvaUtil.o build/PndLVQTrain.o \
	build/PndMvaDataSet.o build/PndMvaCluster.o -o build/LVQtrain

	$(CXX) $(LIBS) build/KnnTrain.o build/PndMvaTrainer.o build/PndMvaUtil.o build/PndKnnTrain.o \
	build/PndMvaDataSet.o -o build/KNNTrain

#	$(CXX) $(LIBS) build/FoamTrainTest.o build/PndMvaTrainer.o build/PndMvaUtil.o \
#	build/PndPDEFoamTrain.o build/PndMvaDataSet.o -o build/FoamTrainTest

#	$(CXX) $(LIBS) build/FoamClassifyTest.o build/PndPDEFoamClassify.o build/PndGpidClassifier.o \
#	build/PndMvaDataSet.o build/PndMvaUtil.o -o build/FoamClassifyTest

	$(CXX) $(LIBS) build/CreateRandomDataSet.o -o build/CreateRandomDataSet

	$(CXX) $(LIBS) build/PndMvaCluster.o build/exampleCluster.o build/PndMvaDataSet.o \
	build/PndMvaUtil.o -o build/exampleCluster
clean:
	rm -f *~ *.o *.root build/*
	cd KNN ; $(MAKE) clean
	cd LVQ1; $(MAKE) clean
	cd TMVAkd_KNN; $(MAKE) clean
	cd TMVA_PDE_FOAM; $(MAKE) clean
	cd Clusters; $(MAKE) clean
