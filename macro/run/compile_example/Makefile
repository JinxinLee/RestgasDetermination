# Makefile for the SimCompleteTpc program, example of a macro compiled with g++.
# Elwin Dijck, December 2009

# Based on the ROOT Makefile by Fons Rademakers.

#------------------------------------------------------------------------------

include Makefile.Panda

#------------------------------------------------------------------------------

SIMCOMPLETETPC := SimCompleteTpc$(ExeSuf)
SIMCOMPLETETPCO := SimCompleteTpc.$(ObjSuf) SimCompleteTpcDict.$(ObjSuf)
SIMCOMPLETETPCSO := libSimCompleteTpc.$(DllSuf)

DIGICOMPLETETPC := DigiCompleteTpc$(ExeSuf)
DIGICOMPLETETPCO := DigiCompleteTpc.$(ObjSuf) DigiCompleteTpcDict.$(ObjSuf)
DIGICOMPLETETPCSO := libDigiCompleteTpc.$(DllSuf)

RECOCOMPLETETPC := RecoCompleteTpc$(ExeSuf)
RECOCOMPLETETPCO := RecoCompleteTpc.$(ObjSuf) RecoCompleteTpcDict.$(ObjSuf)
RECOCOMPLETETPCSO := libRecoCompleteTpc.$(DllSuf)

all:            $(SIMCOMPLETETPC) $(DIGICOMPLETETPC) $(RECOCOMPLETETPC)

lib:            $(SIMCOMPLETETPCSO) $(DIGICOMPLETETPCSO) $(RECOCOMPLETETPCSO)

clean:
		@rm -f *.$(ObjSuf) *Dict.h *Dict.cxx *.dep

distclean:      clean
		@rm -f $(SIMCOMPLETETPC) $(DIGICOMPLETETPC) $(RECOCOMPLETETPC) *.$(DllSuf)

$(SIMCOMPLETETPC): SimCompleteTpcMain.$(ObjSuf) $(SIMCOMPLETETPCO)
		@echo "Linking $@..."
		@$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
		@echo "$@ done"

$(DIGICOMPLETETPC): DigiCompleteTpcMain.$(ObjSuf) $(DIGICOMPLETETPCO)
		@echo "Linking $@..."
		@$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
		@echo "$@ done"

$(RECOCOMPLETETPC): RecoCompleteTpcMain.$(ObjSuf) $(RECOCOMPLETETPCO)
		@echo "Linking $@..."
		@$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
		@echo "$@ done"

$(SIMCOMPLETETPCSO): $(SIMCOMPLETETPCO)
		@echo "Linking $@..."
		@$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@ $(LIBS)
		@echo "$@ done"

$(DIGICOMPLETETPCSO): $(DIGICOMPLETETPCO)
		@echo "Linking $@..."
		@$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@ $(LIBS)
		@echo "$@ done"

$(RECOCOMPLETETPCSO): $(RECOCOMPLETETPCO)
		@echo "Linking $@..."
		@$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@ $(LIBS)
		@echo "$@ done"

#------------------------------------------------------------------------------

SimCompleteTpcDict.$(SrcSuf): SimCompleteTpc.h SimCompleteTpcLinkDef.h
	@echo "Generating dictionary $@..."
	@rootcint -f $@ -c $(INCLUDE) $^

DigiCompleteTpcDict.$(SrcSuf): DigiCompleteTpc.h DigiCompleteTpcLinkDef.h
	@echo "Generating dictionary $@..."
	@rootcint -f $@ -c $(INCLUDE) $^

RecoCompleteTpcDict.$(SrcSuf): RecoCompleteTpc.h RecoCompleteTpcLinkDef.h
	@echo "Generating dictionary $@..."
	@rootcint -f $@ -c $(INCLUDE) $^

include $(wildcard *.dep)
