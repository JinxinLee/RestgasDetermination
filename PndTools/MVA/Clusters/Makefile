# Setting the linker and compiler flags and options

#INCLUDE   = $(shell root-config --cflags) -I./ -I../
#LIBS      = $(shell root-config --libs)

HDR = -I../

# Define targets
TARGETS = exampleCluster somTrain

# all: $(TARGETS)
default: $(TARGETS)

PndMvaCluster:% : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

exampleCluster: % : %.cpp PndMvaCluster
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

PndMvaSOMTrain:% : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

somTrain: % : %.cpp PndMvaSOMTrain
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

clean:
	rm -f *~ *.o