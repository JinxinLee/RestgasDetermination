# Setting the linker and compiler flags and options

#INCLUDE   = $(shell root-config --cflags) -I./ -I../
#LIBS      = $(shell root-config --libs)

HDR = -I../ -I../Clusters/

# Define targets
TARGETS = PndLVQClassify PndLVQTrain LVQtrain LVQclassify

# all: $(TARGETS)
default: $(TARGETS)

PndLVQClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

PndLVQTrain:% : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

LVQclassify: % : %.cpp PndLVQClassify
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

LVQtrain: % : %.cpp PndLVQTrain
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

clean:
	rm -f *~ *.o