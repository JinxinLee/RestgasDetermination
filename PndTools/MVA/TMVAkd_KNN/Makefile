# Setting the linker and compiler flags and options
# points in k-dimensional Euclidean space.

#INCLUDE  += $(shell root-config --cflags) -I./
#LIBS     = -lTMVA -lMLP -lTreePlayer -lMinuit
#LIBS    += $(shell root-config --libs) 

HDR = -I../

# Define targets
TARGETS  = PndKnnTrain.o PndKnnClassify.o PndProjectedKNN.o
TARGETS += KnnClassify.o KnnTrain.o Prclassify.o

all: $(TARGETS)

PndKnnTrain.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c PndKnnTrain.cpp
	@echo "Done building:" $@

PndKnnClassify.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c PndKnnClassify.cpp
	@echo "Done building:" $@

KnnTrain.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c KnnTrain.cpp
	@echo "Done building:" $@

KnnClassify.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c KnnClassify.cpp
	@echo "Done building:" $@

PndProjectedKNN.o:
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c PndProjectedKNN.cpp
	@echo "Done building"$@

Prclassify.o: PndProjectedKNN.o
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c Prclassify.cpp
	@echo "Done building"$@
clean:
	rm -f *~ *.o