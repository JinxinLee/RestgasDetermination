# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options

#INCLUDE   = $(shell root-config --cflags) -I./ -I../
#LIBS      = $(shell root-config --libs)

HDR = -I../

# Define targets
TARGETS = PndPDEFoamTrain FoamTrainTest PndPDEFoamClassify FoamClassifyTest


all: $(TARGETS)

PndPDEFoamTrain: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building "$@.o

PndPDEFoamClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building "$@.o

FoamClassifyTest: % : %.cpp PndPDEFoamClassify
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building "$@

FoamTrainTest: % : %.cpp  PndPDEFoamTrain
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building "$@

clean:
	rm -f $(BINS) *~ *.o
