
#New EvtGen makefile October 5, 1998
#Authors David Lange and Anders Ryd


SRCSC= $(wildcard *.cc)
SRCSF= $(wildcard *.F)

OBJSC = $(SRCSC:%.cc=%.o)
OBJSF = $(SRCSF:%.F=%.o)

OBJS = $(OBJSC)
OBJS += $(OBJSF)

LIBOBJS=$(filter-out evt*.o, $(OBJS))

include ../config.mk



lib: depend comp ar 

comp:$(OBJS)

clean:
	rm -f *.o ../libEvtGenBase.a

depend:
	@echo "Checking makedepend in EvtGenBase"
	@makedepend -- $(EVTDEPINC) -- -Y $(SRCSC) $(SRCSF)


.cc.o:
	$(EVTCXX) $(EVTCXXFLAGS) -c $<


.F.o:
	$(EVTF77) $(EVTF77FLAGS) -c $<

ar:
	@if (test -f ../libEvtGenBase.a ) then (rm ../libEvtGenBase.a) fi
	@ar rv ../libEvtGenBase.a $(LIBOBJS) > /dev/null
	@echo "Made libEvtGenBase.a"

# DO NOT DELETE THIS LINE -- make depend depends on it.

