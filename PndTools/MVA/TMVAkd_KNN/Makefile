# Setting the linker and compiler flags and options
#INCLUDE  += $(shell root-config --cflags) -I./
#LIBS     = -lTMVA -lMLP -lTreePlayer -lMinuit
#LIBS    += $(shell root-config --libs) 

HDR = -I../

# Define targets
TARGETS  = PndKnnTrain PndKnnClassify PndProjectedKNN 
TARGETS += KnnClassify KnnClassifyModified KnnTrain Prclassify

# all: $(TARGETS)
default: $(TARGETS)

PndKnnTrain: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

PndKnnClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

KnnTrain: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

KnnClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

KnnClassifyModified: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

PndProjectedKNN: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

Prclassify: % : %.cpp PndProjectedKNN
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@
clean:
	rm -f *~ *.o