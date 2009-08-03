# Setting the linker and compiler flags and options
# points in k-dimensional Euclidean space.

#INCLUDE   = -I/opt/fairroot/fairsoft/tools/root/tmva/inc
#INCLUDE  += $(shell root-config --cflags) -I./

#LIBS     = -lTMVA -lMLP -lTreePlayer -lMinuit
#LIBS    += $(shell root-config --libs) 

# Define targets
TARGETS  = PndKnnTrain PndKnnClassify PndProjectedKNN 
TARGETS += KnnClassify KnnTrain Prclassify

all: $(TARGETS)

PndKnnTrain: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building:" $@

PndKnnClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building:" $@

KnnTrain: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building:" $@

KnnClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building:" $@

PndProjectedKNN: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building"$@

Prclassify: % : %.cpp PndProjectedKNN
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building"$@
clean:
	rm -f *~