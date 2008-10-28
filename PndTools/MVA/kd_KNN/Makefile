# Setting the linker and compiler flags and options
# points in k-dimensional Euclidean space.

#FLAGSMACHINE=-march=athlon64 -mfpmath=sse -msse2 -m64
CXX       =  g++
CCFLAGS   = -Wall -O3 -ffast-math ${FLAGSMACHINE} -fomit-frame-pointer

INCLUDE   = $(shell root-config --cflags) -I./
INCLUDE  += -I/opt/fairroot/fairsoft/basics/boost/include/
LIBS      = $(shell root-config --libs) 
LIBS     += -L/opt/fairroot/fairsoft/basics/boost/lib/


# Define binary file name.
BINS = classify train

# Define targets

all: kdtree2 classify train

classify: % : %.cpp #$(HEADS)/*.h
	$(CXX) $(CCFLAGS) -c PndKnnClassify.cpp $< $(INCLUDE)
	$(CXX) $(CCFLAGS) classify.o kdtree2.o PndKnnClassify.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done building the classifier."

kdtree2: % : %.cpp #$(HEADS)/*.h
	$(CXX) $(CCFLAGS) -c kdtree2.cpp $< $(INCLUDE)

train: % : %.cpp #$(HEADS)/*.h
	$(CXX) $(CCFLAGS) -c PndKnnTrain.cpp $< $(INCLUDE)
	$(CXX) $(CCFLAGS) train.o PndKnnTrain.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done, building the trainer."

clean:
	rm -f $(BINS) *~ *.o

cleanall:
	rm -f $(BINS) *~ *.o *.root
