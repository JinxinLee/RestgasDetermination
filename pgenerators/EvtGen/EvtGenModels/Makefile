
#New EvtGen makefile October 5, 1998
#Authors David Lange and Anders Ryd


SRCSC= $(wildcard *.cc)
SRCSF= $(wildcard *.F)

OBJSC = $(SRCSC:%.cc=%.o)
OBJSF = $(SRCSF:%.F=%.o)

OBJS = $(OBJSC)
OBJS += $(OBJSF)

LIBOBJS=$(OBJS)

include ../config.mk

lib: depend comp ar 

comp:$(OBJS)

clean:
	rm -f *.o ../libEvtGenModels.a


depend:
	@echo "Checking makedepend in EvtGenModels"
	@makedepend -- $(EVTDEPINC) -- $(SRCSC) $(SRCSF)


.cc.o:
	$(EVTCXX) $(EVTCXXFLAGS) -c $<


.F.o:
	$(EVTF77) $(EVTF77FLAGS) -c $<

ar:
	@if (test -f ../libEvtGenModels.a ) then (rm ../libEvtGenModels.a) fi
	@ar rv ../libEvtGenModels.a $(LIBOBJS) >& /dev/null
	@echo "Done making libEvtGenModels.a"


# DO NOT DELETE THIS LINE -- make depend depends on it.

