# Setting the linker and compiler flags and options
# points in k-dimensional Euclidean space.

#INCLUDE  += $(shell root-config --cflags) -I./
#LIBS     = -lTMVA -lMLP -lTreePlayer -lMinuit
#LIBS    += $(shell root-config --libs) 

HDR = -I../

# Define targets
TARGETS  = PndMultiClassMlpTrain PndMultiClassMlpClassify
TARGETS += mlpTrain mlpClassify

all: $(TARGETS)

PndMultiClassMlpTrain: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

PndMultiClassMlpClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

mlpTrain: % : %.cpp PndMultiClassMlpTrain
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

mlpClassify: % : %.cpp PndMultiClassMlpClassify
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

clean:
	rm -f *~ *.o