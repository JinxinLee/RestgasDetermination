# Setting the linker and compiler flags and options

#INCLUDE   = $(shell root-config --cflags) -I./ -I../
#LIBS      = $(shell root-config --libs)

HDR = -I../

# Define binary file name.#
TARGETS = PndStdKnnClassify classifyStdKnn PndPrzWindowClassify classifyPrzWin

# Define targets
# all: $(TARGETS)
default: $(TARGETS)

PndStdKnnClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

classifyStdKnn: % : %.cpp PndStdKnnClassify
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

PndPrzWindowClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

classifyPrzWin: % : %.cpp PndPrzWindowClassify
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@
clean:
	rm -f *~ *.o