
#New EvtGen makefile October 5, 1998
#Authors David Lange and Anders Ryd


SRCSC= $(wildcard *.cc)
SRCSF= $(wildcard *.F)

OBJSC = $(SRCSC:%.cc=%.o)
OBJSF = $(SRCSF:%.F=%.o)

OBJS = $(OBJSC)
OBJS += $(OBJSF)

LIBOBJS=$(filter-out top.o testEvtGen.o ,$(OBJS))

include config.mk

lib: depend comp ar baselib modelslib 

comp:$(OBJS)

clean:
	rm -f *.o libEvtGen.a libEvtGenBase.a libEvtGenModels.a Evt*/*.o testEvtGen


depend:
	@echo "Checking makedepend in EvtGen"
	@makedepend -- $(EVTDEPINC) -- -Y $(SRCSC) $(SRCSF)


.cc.o:
	$(EVTCXX) $(EVTCXXFLAGS) -c $<


.F.o:
	$(EVTF77) $(EVTF77FLAGS) -c $<

ar:
	@if (test -f libEvtGen.a ) then (rm libEvtGen.a) fi
	@ar rv libEvtGen.a $(LIBOBJS) >& /dev/null
	@echo "Done making libEvtGen.a"

baselib:
	@cd EvtGenBase; make lib; cd ..

modelslib:
	@cd EvtGenModels; make lib; cd ..

bin:
	$(EVTCXX) $(EVTLINK) $(EVTLINKOPT) -o testEvtGen testEvtGen.o -lEvtGen -lEvtGenModels -lEvtGenBase  -lCLHEP -l$(EVTPYLIB) -l$(EVTPDLIB) -lpacklib -lmathlib -l$(EVTPHLIB) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -ldl $(EVTSYSLIB) -lm -lMathCore -lThread

panda:
	$(EVTCXX) $(EVTLINK) $(EVTLINKOPT) -o pandaEvtGen pandaEvtGen.o -lEvtGen -lEvtGenModels -lEvtGenBase  -lCLHEP -l$(EVTPYLIB) -l$(EVTPDLIB) -lpacklib -lmathlib -l$(EVTPHLIB) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -ldl $(EVTSYSLIB) -lm -lMathCore -lThread

simple:
	$(EVTCXX) $(EVTLINK) $(EVTLINKOPT) -o simpleEvtGen simpleEvtGen.o -lEvtGen -lEvtGenModels -lEvtGenBase  -lCLHEP -l$(EVTPYLIB) -l$(EVTPDLIB) -lpacklib -lmathlib -l$(EVTPHLIB) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -ldl $(EVTSYSLIB) -lm -lMathCore -lThread

jgm:
	$(EVTCXX) $(EVTLINK) $(EVTLINKOPT) -o jgmEvtGen jgmEvtGen.o -lEvtGen -lEvtGenModels -lEvtGenBase  -lCLHEP -l$(EVTPYLIB) -l$(EVTPDLIB) -lpacklib -lmathlib -l$(EVTPHLIB) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -ldl $(EVTSYSLIB) -lm -lMathCore -lThread

irina:
	$(EVTCXX) $(EVTLINK) $(EVTLINKOPT) -o irinaEvtGen irinaEvtGen.o -lEvtGen -lEvtGenModels -lEvtGenBase  -lCLHEP -l$(EVTPYLIB) -l$(EVTPDLIB) -lpacklib -lmathlib -l$(EVTPHLIB) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -ldl $(EVTSYSLIB) -lm -lMathCore -lThread

gg:
	$(EVTCXX) $(EVTLINK) $(EVTLINKOPT) -o ggEvtGen ggEvtGen.o -lEvtGen -lEvtGenModels -lEvtGenBase  -lCLHEP -l$(EVTPYLIB) -l$(EVTPDLIB) -lpacklib -lmathlib -l$(EVTPHLIB) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -ldl $(EVTSYSLIB) -lm -lMathCore -lThread
# DO NOT DELETE THIS LINE -- make depend depends on it.

