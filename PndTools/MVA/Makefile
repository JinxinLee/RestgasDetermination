####################################################
# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options
#####################################################

# FLAGSMACHINE=-march=athlon64 -mfpmath=sse -msse2 -m64
# FLAGSMACHINE += -m64 -mssse3 -mpc64 -mfpmath=sse -msse3 -m3dnow
# CCFLAGS += -Weffc++ -pedantic
# -Wunreachable-code # -Wunsafe-loop-optimizations #-Winline
MAKEFLAGS  = -wkr --no-print-directory #-s

FLAGSMACHINE = -march=native -mtune=native
# FLAGSMACHINE += -mssse3 -mpc64 -mfpmath=sse -msse3 -m3dnow

CCFLAGS  = -W -Wall -Wextra -Wredundant-decls -Wshadow -Werror
CCFLAGS += -O3 ${FLAGSMACHINE} -funroll-loops -fopenmp
CCFLAGS += -fdiagnostics-show-option -fdiagnostics-show-location=every-line
CCFLAGS += -fvariable-expansion-in-unroller -fprefetch-loop-arrays
# CCFLAGS += -Woverlength-strings -Wstack-protector
# CCFLAGS += -fstrict-aliasing -Wstrict-aliasing=3
# CCFLAGS += -malign-double -maccumulate-outgoing-args -ftracer


INCLUDE = $(shell root-config --cflags) -I./

LIBS  = $(shell root-config --libs) -lTMVA -lTreePlayer -lMinuit
LIBS += -lMLP -lXMLIO -fopenmp

.EXPORT_ALL_VARIABLES:

.SUFFIXES: .cpp .h .hpp .o

# Define targets
TRGTS  = PndMvaUtil.o PndMvaDataSet.o PndGpidClassifier.o PndMvaTrainer.o
TRGTS +=  CreateRandomDataSet.o Classifiers examples

.PHONY: all
all: $(TRGTS)
	@echo ""
	@echo "-> Please check the errors and warnings"
	@echo "-> before running the application."
	@echo ""

.PHONY: PndGpidClassifier
PndGpidClassifier.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) -c PndGpidClassifier.cpp
	@echo "Done building:" $@

.PHONY: PndMvaDataSet
PndMvaDataSet.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) -c PndMvaDataSet.cpp
	@echo "Done building:" $@

.PHONY: PndMvaUtil
PndMvaUtil.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) -c PndMvaUtil.cpp
	@echo "Done building:" $@

.PHONY: PndMvaTrainer
PndMvaTrainer.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) -c PndMvaTrainer.cpp
	@echo "Done building:" $@

.PHONY: CreateRandomDataSet
CreateRandomDataSet.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) -c CreateRandomDataSet.cpp
	@echo "Done building:" $@

.PHONY:Classifiers
Classifiers:
	$(MAKE) -C KNN;
	$(MAKE) -C LVQ1;
	$(MAKE) -C TMVAkd_KNN;
	$(MAKE) -C Clusters;
#	$(MAKE) -C TMVA_PDE_FOAM;

.PHONY: examples
examples: PndMvaUtil PndMvaDataSet PndGpidClassifier Classifiers
	@echo ""
	@echo "       =++++++ Linking example applications. ++++++="
	@echo ""

	$(CXX) $(LIBS) KNN/classifyStdKnn.o  KNN/PndStdKnnClassify.o PndGpidClassifier.o \
	PndMvaUtil.o PndMvaDataSet.o -o build/classifyStdKnn

	$(CXX) $(LIBS) TMVAkd_KNN/KnnClassify.o TMVAkd_KNN/PndKnnClassify.o PndGpidClassifier.o \
	PndMvaUtil.o PndMvaDataSet.o -o build/KNNclassify

	$(CXX) $(LIBS) TMVAkd_KNN/Prclassify.o TMVAkd_KNN/PndProjectedKNN.o TMVAkd_KNN/PndKnnClassify.o \
	PndGpidClassifier.o PndMvaUtil.o PndMvaDataSet.o -o build/PrKNNclassify

	$(CXX) $(LIBS) LVQ1/LVQclassify.o LVQ1/PndLVQClassify.o PndGpidClassifier.o \
	PndMvaUtil.o PndMvaDataSet.o -o build/LVQclassify

	$(CXX) $(LIBS) LVQ1/LVQtrain.o PndMvaTrainer.o PndMvaUtil.o LVQ1/PndLVQTrain.o \
	PndMvaDataSet.o Clusters/PndMvaCluster.o -o build/LVQtrain

	$(CXX) $(LIBS) TMVAkd_KNN/KnnTrain.o PndMvaTrainer.o PndMvaUtil.o TMVAkd_KNN/PndKnnTrain.o \
	PndMvaDataSet.o -o build/KNNTrain

#	$(CXX) $(LIBS) build/FoamTrainTest.o build/PndMvaTrainer.o build/PndMvaUtil.o \
#	build/PndPDEFoamTrain.o build/PndMvaDataSet.o -o build/FoamTrainTest

#	$(CXX) $(LIBS) build/FoamClassifyTest.o build/PndPDEFoamClassify.o build/PndGpidClassifier.o \
#	build/PndMvaDataSet.o build/PndMvaUtil.o -o build/FoamClassifyTest

	$(CXX) $(LIBS) CreateRandomDataSet.o -o build/CreateRandomDataSet

	$(CXX) $(LIBS) Clusters/PndMvaCluster.o Clusters/exampleCluster.o PndMvaDataSet.o \
	PndMvaUtil.o -o build/exampleCluster

.PHONY: clean
clean:
	rm -f *~ *.o *.root build/*
	cd KNN ; $(MAKE) clean
	cd LVQ1; $(MAKE) clean
	cd TMVAkd_KNN; $(MAKE) clean
	cd TMVA_PDE_FOAM; $(MAKE) clean
	cd Clusters; $(MAKE) clean
