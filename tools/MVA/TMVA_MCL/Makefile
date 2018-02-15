# Setting the linker and compiler flags and options
#INCLUDE  += $(shell root-config --cflags) -I./
#LIBS     = -lTMVA -lMLP -lTreePlayer -lMinuit
#LIBS    += $(shell root-config --libs) 

HDR = -I../

# Define targets
TARGETS  = PndMultiClassMlpTrain PndMultiClassMlpClassify
TARGETS += PndMultiClassBdtTrain PndMultiClassBdtClassify
TARGETS += mlpTrain mlpClassify bdtTrain bdtClassify

# all: $(TARGETS)
default: $(TARGETS)
# ############ MLP ############################################
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

# ############ BDT ############################################
PndMultiClassBdtTrain: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

PndMultiClassBdtClassify: % : %.cpp
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

bdtTrain: % : %.cpp PndMultiClassBdtTrain
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@

bdtClassify: % : %.cpp PndMultiClassBdtClassify
	$(CXX) $(CCFLAGS) $(INCLUDE) $(HDR) -c $< -o ../build/$@.o
	@echo "Done building:" $@
###############################################################
clean:
	rm -f *~ *.o