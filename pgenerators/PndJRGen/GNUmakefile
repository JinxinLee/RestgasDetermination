#################################################################
# Package name

PACKAGE = PndJRGen

#################################################################
#++ binary products                             [build it with 'bin'] 

MAIN    = 
PROGRAM =

###############################################################################
### PANDA architecture

ifndef PNDARCH
  PNDARCH       := $(G4SYSTEM)
endif

# Define directory paths

OBJDIR        = ./tmp
TMPDIR        = ./tmp
LIBDIR        = ./lib
SLIBDIR       = ./lib
BINDIR        = ./.


ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)
 
# Linux with egcs

F77	      = g77
CXX           = g++
#CXXFLAGS      = -g -Wall -O0 -DBABAR_COMP_INST -fno-exceptions -fPIC -I..
CXXFLAGS      = -g  -O0 -Wall -DBABAR_COMP_INST -DGNU_GCC -fexceptions -fPIC 
LD            = g++
LDFLAGS       = -g -Wl,-rpath,$(LIBDIR):$(ROOTSYS)/lib:/lib
SOFLAGS       = -shared

include ./arch_spec_GEANT4.mk

CPPFLAGS += -Iinclude
CPPFLAGS += -I$(CLHEP_INCLUDE_DIR)
LDFLAGS  += -L$(CLHEP_LIB_DIR)
LDLIBS   += -l$(CLHEP_LIB)

CPPFLAGS      += $(ROOTCFLAGS) -I$(ROOTSYS)/include -I.. 


LIBS          = $(ROOTLIBS) -lEG -lProof -lTreePlayer -lMinuit
GLIBS         = $(ROOTGLIBS) -lEG -lProof -lTreePlayer -lMinuit

JRPNDLIB          = -L$(LIBDIR) -lPndJRGen

PANDALIBS       = -L$(LIBDIR)

PANDARECOLIBS   = -L$(LIBDIR)

CONDITIONSLIB  = -lPndConditions
HISTOGRAMLIB   = -lPndHistogram
BASELIB        = -lPandaBase
MATHLIB        = -lRhoMath

EXTRALIBS += $(shell \
              G4TMP=$(PANDA)/tmp; export G4TMP; \
              if [ \( -f $(G4INSTALL)/lib/$(G4SYSTEM)/liblist \
              -a -f $(G4INSTALL)/lib/$(G4SYSTEM)/libname.map \) ]; then \
              $(G4INSTALL)/lib/$(G4SYSTEM)/liblist \
              -d $(PANDA)/tmp \
              < $(G4INSTALL)/lib/$(G4SYSTEM)/libname.map; fi )


EXTRALIBS       += -L$(LIBDIR)


LDLIBS  += $(PANDALIBS) $(GLIBS)

# Static pattern rule for object file dependency on sources:
$(OBJDIR)/%.o: %.cc
	@echo Compiling  file $< ...
	@if [ ! -d $(OBJDIR) ] ; then mkdir -p $(OBJDIR)  ;fi
ifdef PNDVERBOSE
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
else
	@($(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ )
endif

$(OBJDIR)/%.o: %.c
	@echo Compiling  file $< ...
	@if [ ! -d $(OBJDIR) ] ; then mkdir -p $(OBJDIR)  ;fi
ifdef PNDVERBOSE
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
else
	@($(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ )
endif

$(OBJDIR)/%.o: %.f
	@echo Compiling file $< ...
	@if [ ! -d $(OBJDIR) ] ; then mkdir -p $(OBJDIR)  ;fi
ifdef PNDVERBOSE
	$(F77) $(F77FLAGS) -c $< -o $@
else
	@$(F77) $(F77FLAGS) -c $< -o $@
endif

LIBA    = $(LIBDIR)/lib$(PACKAGE).a
LIBSO   = $(SLIBDIR)/lib$(PACKAGE).so

# Lists of objects to include in library:

SRCS 		:= 	$(filter-out $(addsuffix .cc, $(MAIN) ), $(wildcard *.cc))
SRCS 		:= 	$(filter-out $(PACKAGE)Cint.cc, $(SRCS))
HDRS 		:= 	$(patsubst %.cc,%.hh,$(SRCS))    
OBJS 		:= 	$(patsubst %.hh,$(OBJDIR)/%.o,$(HDRS))
OBJS 		+=	$(shell test -f $(PACKAGE)_LinkDef.hh && echo $(OBJDIR)/$(PACKAGE)Cint.o ) 
dependencies 	:= 	$(patsubst %.hh,$(TMPDIR)/%.d,$(HDRS))
ROOTCINTHDRS 	:= 	$(HDRS) 
ROOTCINTHDRS 	+= 	$(shell test -f $(PACKAGE)_LinkDef.hh && echo $(PACKAGE)_LinkDef.hh ) 


# Default action 

all : 	depend  hdr lib bin 


lib: 	$(LIBA) $(LIBSO)
	@echo ---------------------------------------
	@echo End of $(PACKAGE) package building ...
	@echo ---------------------------------------


depend: $(dependences) 

hdr: 	$(ROOTCINTHDRS)

shlib: 	$(LIBSO)

bin: 	$(PROGRAM)



# Static library:
$(LIBA) : $(OBJS)  
	@rm -f $@
#	@echo Removing $@ library ...
	@if [ ! -d $(LIBDIR) ] ; then mkdir -p $(LIBDIR)  ;fi
	@echo Building  library $@ ...
ifdef PNDVERBOSE
	$(AR) $(ARFLAGS) $@ $^ 
else
	@($(AR) $(ARFLAGS) $@ $^ )
endif

# Shared library:
$(LIBSO) : $(OBJS)  
	@rm -f $@
#	@echo Removing $@ library ...
	@if [ ! -d $(SLIBDIR) ] ; then mkdir -p $(SLIBDIR)  ;fi
	@echo Building  library $@ ...
ifdef PNDVERBOSE
	$(LD) -g  $(SOFLAGS) $(GLIBS) -o $@ $^ 
else
	@($(LD) -g  $(SOFLAGS) $(GLIBS) -o $@ $^ )
endif

# Rules for Dictionary:
$(OBJDIR)/$(PACKAGE)Cint.o : $(PACKAGE)Cint.cc 
	@if [ -f $(PACKAGE)_LinkDef.hh ]; then \
	echo "Compiling Dictionary $(PACKAGE)Cint.cc ..." ; \
	$(CXX)  $(CXXFLAGS) $(CPPFLAGS) $(APPFLAGS) -c $<  -o $@ ; fi 


$(PACKAGE)Cint.cc : $(ROOTCINTHDRS)
	@if [ -f $(PACKAGE)_LinkDef.hh ]; then \
	echo "Building Dictionary $(PACKAGE)Cint.cc ..." ; \
	$(ROOTSYS)/bin/rootcint -f $@ -c  -I.. $^ ; fi 


# Make the .d file(s) and include it(them).
# g++ -MM (or -M) is good at this, except it forgets the subdirectory
# (hence the use of $TMPDIR/).
# Dependencies:

$(TMPDIR)/%.d: %.cc %.hh
	@echo Making dependency for file $< ...
	@if [ ! -d $(TMPDIR) ] ; then mkdir -p $(TMPDIR)  ;fi
	@set -e;\
	$(CXX) -M $(CPPFLAGS) $(CXXFLAGS)  $< |\
	sed 's!$*\.o!$(TMPDIR)/& $@!' >$@;\
	[ -s $@ ] || rm -f $@ 
ifneq ($(dependencies),)
-include $(dependencies)
endif




#
# Complete cleanup:
#

clean : clean_libs
	@echo Removing dependencies for $(PACKAGE) package ...
	@rm  -f $(dependencies) 
	@echo Removing objs for $(PACKAGE) package ...
	@rm  -f $(OBJDIR)/*.o  
	@if [ -f $(PACKAGE)_LinkDef.hh ] ; then \
	echo Removing  dictionary $(PACKAGE)Cint ... ; \
	rm -f $(PACKAGE)Cint.* ; fi
	@if [ -f $(BINDIR)/$(PROGRAM) ] ; then \
	echo Removing  $(PROGRAM) ... ; \
	rm -f $(BINDIR)/$(PROGRAM) ; fi


clean_libs:
	@echo -----------------------------------
	@echo Cleaning up $(PACKAGE) package ...
	@echo -----------------------------------
	@if [  -f $(LIBA) ] ; then \
	echo Removing library $(LIBA) ... ; \
	rm -f $(LIBA) ; fi
	@if [ -f $(LIBSO) ] ; then \
	echo Removing library $(LIBSO) ... ; \
	rm -f $(LIBSO) ; fi


$(PROGRAM) : $(OBJDIR)/$(addsuffix .o, $(MAIN) )
	@if [ ! -d $(BINDIR) ] ; then mkdir -p $(BINDIR)  ;fi
	@echo Linking binary $@ ...
ifdef PNDVERBOSE
	$(LD) -o $(BINDIR)/$@ $< $(LDFLAGS) $(LDLIBS) 
else
	@$(LD) -o $(BINDIR)/$@ $< $(LDFLAGS) $(LDLIBS) 
endif




