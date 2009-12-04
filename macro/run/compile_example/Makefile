# Makefile for the SimCompleteTpc program, example of a macro compiled with g++.
# Elwin Dijck, December 2009

# Based on the ROOT Makefile by Fons Rademakers.

#------------------------------------------------------------------------------

include Makefile.Panda

#------------------------------------------------------------------------------

SIMCOMPLETETPC := SimCompleteTpc$(ExeSuf)
SIMCOMPLETETPCO := SimCompleteTpc.$(ObjSuf) SimCompleteTpcDict.$(ObjSuf)
SIMCOMPLETETPCSO := libSimCompleteTpc.$(DllSuf)

all:            $(SIMCOMPLETETPC)

lib:            $(SIMCOMPLETETPCSO)

clean:
		@rm -f *.$(ObjSuf) *Dict.h *Dict.cxx *.dep

distclean:      clean
		@rm -f $(SIMCOMPLETETPC) *.$(DllSuf)

$(SIMCOMPLETETPC): SimCompleteTpcMain.$(ObjSuf) $(SIMCOMPLETETPCO)
		@echo "Linking $@..."
		@$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
		@echo "$@ done"

$(SIMCOMPLETETPCSO): $(SIMCOMPLETETPCO)
		@echo "Linking $@..."
		@$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@ $(LIBS)
		@echo "$@ done"

#------------------------------------------------------------------------------

SimCompleteTpcDict.$(SrcSuf): SimCompleteTpc.h SimCompleteTpcLinkDef.h
	@echo "Generating dictionary $@..."
	@rootcint -f $@ -c $(INCLUDE) $^

include $(wildcard *.dep)
