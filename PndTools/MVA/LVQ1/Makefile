# HOW TO RUN
#  OMP_NUM_THREADS=4 ./PlotEventData Events03.root
#
# Setting the linker and compiler flags and options
#FLAGSMACHINE=-march=athlon64 -mfpmath=sse -msse2 -m64
CXX       =  g++
#CCFLAGS   = -Wall -Os -O3 -ffast-math ${FLAGSMACHINE} -fomit-frame-pointer
CCFLAGS   = -Wall -g -O0

INCLUDE   = $(shell root-config --cflags) -I./
#INCLUDE  += -I/opt/fairroot/fairsoft/basics/boost/include/
LIBS      = $(shell root-config --libs) 
#LIBS     += -L/opt/fairroot/fairsoft/basics/boost/lib/


# Define binary file name.
BINS = classify train

# Define targets

all: classify train

classify: % : %.cpp
	$(CXX) $(CCFLAGS) -c PndLVQClassify.cpp $< $(INCLUDE)
	$(CXX) $(CCFLAGS) classify.o PndLVQClassify.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done building the classifier."

train: % : %.cpp
	$(CXX) $(CCFLAGS) -c PndLVQTrain.cpp $< $(INCLUDE)
	$(CXX) $(CCFLAGS) train.o PndLVQTrain.o $(INCLUDE) $(LIBS) -o $@
	@echo "Done, building the trainer."

clean:
	rm -f $(BINS) *~ *.o

cleanall:
	rm -f $(BINS) *~ *.o *.root
