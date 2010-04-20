# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options

#INCLUDE   = $(shell root-config --cflags) -I./ -I../
#LIBS      = $(shell root-config --libs)

# Define targets
TARGETS = PndLVQClassify PndLVQTrain LVQtrain LVQclassify

all: $(TARGETS)

PndLVQClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building:" $@

PndLVQTrain:% : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building:" $@

LVQclassify: % : %.cpp PndLVQClassify
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building:" $@

LVQtrain: % : %.cpp PndLVQTrain
	$(CXX) $(CCFLAGS) $(INCLUDE) -c $< -o ../build/$@.o
	@echo "Done building:" $@

clean:
	rm -f *~ *.o