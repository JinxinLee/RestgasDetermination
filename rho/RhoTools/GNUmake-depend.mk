#T standalone makefile
PACKAGE = RhoTools

LIBA	= lib$(PACKAGE).a
LIBSO	= lib$(PACKAGE).so

# Lists of objects to include in library:
NAMELIST = TBVariables \
	   TDiagVertex \
           TCandPtr \
	   TBooster \
	   TCones \
	   TConeMomFlow \
           TDummyFitter \
	   TEventShape \
	   TFindOmittedParticle \
	   TCandReader \
	   TAdd4Vertexer \
	   TFoxWolfMom \
	   TFWMoments \
	   TCandListSort \
	   TMomTensor \
	   TOpAdd4 \
	   TOperator \
	   TOperatorBase \
	   TOpMakeTree \
	   TPrintTree \
	   TReadTree \
	   TSphericity \
	   TThrust \
	   TThrustVector \
	   TTotalMomentum \
	   TTreeNavigator \
	   TUnaryOperator \
	   TOpClone \
	   TDecayMode \
	   TDecayTree \
	   TFastCombiner \
           VAbsFitter

HDRS    = $(addsuffix .h, $(NAMELIST) )
OBJS    = $(addsuffix .o, $(NAMELIST) )
#ROOTCINTTARGETS := $(filter-out $(PACKAGE)_LinkDef.rdl, $(wildcard *.rdl)) $(wildcard $(PACKAGE)_LinkDef.rdl)
#ROOTCINTHDRS :=   $(addsuffix .h, $(basename $(ROOTCINTTARGETS)) )
ROOTCINTHDRS := $(HDRS) $(PACKAGE)_LinkDef.h

# Default action 
all : $(ROOTCINTHDRS) $(LIBA) $(LIBSO)

hdr: $(ROOTCINTHDRS)

shlib: $(LIBSO)

# Static library:
$(LIBA) : $(OBJS)  $(PACKAGE)Cint.o
	-rm -f $@
	$(AR) $(ARFLAGS)  $(LIBDIR)/$@ $^

# Shared library:
$(LIBSO) : $(OBJS)  $(PACKAGE)Cint.o
	$(LD) -g -L$(T)/lib $(SOFLAGS) $(GLIBS) -o $(LIBDIR)/$@ $^

# Rules for Dictionary:
$(PACKAGE)Cint.o : $(PACKAGE)Cint.cxx 
	$(CXX) $(CXXFLAGS) -c $<
	@echo "-----$(PACKAGE)Cint.o---------"

$(PACKAGE)Cint.cxx : $(ROOTCINTHDRS)
	rootcint -f $@ -c  -I.. $^
	@echo "-----$(PACKAGE)Cint.cxx---------"

# Static pattern rule for object file dependency on sources:
$(OBJS) : %.o : %.cxx  %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

#$(ROOTCINTHDRS) : %.h : %.rdl
#	cp $< $@

# Some extra dependecies from #includes (needs to be completed,
# if in doubt do a "gmake clean" first):

#
# Complete cleanup:
#
clean :
	-rm  -f *.o 
	-rm  -f $(PACKAGE)Cint.*
	-rm  -f $(LIBA) $(LIBSO)


