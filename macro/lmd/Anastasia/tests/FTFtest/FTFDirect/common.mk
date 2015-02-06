#--------------------------------------------------------------------------
# File and Version Information:
#      $Id: common.mk,v 1.20 2004/04/14 14:49:56 asanchez Exp $
#
# Description:
# 	Common part for all standard makefiles
#
# Environment:
#      Software developed for the PANDA Detector at GSI, Darmstadt
#
# Author List:
#      Sergey Ganzhur                Original Author
#
# Copyright Information:
#      Copyright (C) 2002           Ruhr Universitaet Bochum
#
#------------------------------------------------------------------------
# Assumes the environment variable ROOTSYS to exist
#


###############################################################################
### PANDA architecture

ifndef PNDARCH
  PNDARCH	:= $(G4SYSTEM)
endif

# Define directory paths

OBJDIR        = ./tmp
TMPDIR        = ./tmp
LIBDIR        = ./lib
SLIBDIR       = ./lib
BINDIR        = ./

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)
 
# Linux with egcs

F77	      = gfortran
CXX           = g++
CXXFLAGS      = -g -O0 -Wall -DGNU_GCC -fexceptions -fPIC
LD            = g++
LDFLAGS       = -g -Wl,-rpath,$(LIBDIR):$(ROOTSYS)/lib:/lib 
SOFLAGS       = -shared 
F77FLAGS      = -fPIC


CPPFLAGS += -Iinclude
CPPFLAGS += -I$(CLHEP_INCLUDE_DIR)
#LDFLAGS  += -L$(CLHEP_LIB_DIR)
#LDFLAGS  += -L$(CLHEP_LIBRARY_DIR)
#LDLIBS   += -l$(CLHEP_LIB)

CPPFLAGS 	+= $(ROOTCFLAGS) -I$(ROOTSYS)/include -I.. 

LIBS          = $(ROOTLIBS) -lEG  -lTreePlayer -lMinuit
GLIBS         = $(ROOTGLIBS) -lEG -lTreePlayer -lMinuit

FTFLIB		= -L$(LIBDIR) -lFtfEvtGen

PANDALIBS       = -L$(LIBDIR) 

PANDARECOLIBS   = -L$(LIBDIR) 


EXTRALIBS	+= $(shell \
	      G4TMP=$(PANDA)/tmp; export G4TMP; \
              if [ \( -f $(G4INSTALL)/lib/$(G4SYSTEM)/liblist \
              -a -f $(G4INSTALL)/lib/$(G4SYSTEM)/libname.map \) ]; then \
              $(G4INSTALL)/lib/$(G4SYSTEM)/liblist \
              -d $(PANDA)/tmp \
              < $(G4INSTALL)/lib/$(G4SYSTEM)/libname.map; fi )


EXTRALIBS	+= -L$(LIBDIR) 
LDLIBS += $(PANDALIBS) -L$(GEANT4_LIBRARY_DIR) -lG4clhep  -lG4geometry  -lG4materials -lG4particles -lG4processes -lG4track -lG4global -lG4physicslists  $(GLIBS)
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











