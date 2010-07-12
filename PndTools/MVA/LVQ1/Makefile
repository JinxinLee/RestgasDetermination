# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options

#INCLUDE   = $(shell root-config --cflags) -I./ -I../
#LIBS      = $(shell root-config --libs)

HDR = -I../ -I../Clusters/

# Define targets
TARGETS = PndLVQClassify.o PndLVQTrain.o LVQtrain.o LVQclassify.o

all: $(TARGETS)

PndLVQClassify.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c PndLVQClassify.cpp
	@echo "Done building:" $@

PndLVQTrain.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c PndLVQTrain.cpp
	@echo "Done building:" $@

LVQclassify.o: PndLVQClassify.o
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c LVQclassify.cpp
	@echo "Done building:" $@

LVQtrain.o: PndLVQTrain.o
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c LVQtrain.cpp
	@echo "Done building:" $@

clean:
	rm -f *~ *.o