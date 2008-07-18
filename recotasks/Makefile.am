# Makefile.am for module STS
# For use of automake
#
# Volker Friese    21-02-2006


INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/passive
INCLUDES               += -I$(top_srcdir)/mvd
INCLUDES               += -I$(top_srcdir)/mvd/tst
INCLUDES               += -I$(top_srcdir)/mcstack
INCLUDES	       += -I$(top_srcdir)/genfit
INCLUDES	       += -I$(top_srcdir)/tpc
INCLUDES	       += -I$(top_srcdir)/tpc/tpcreco
INCLUDES	       += -Idemo/
INCLUDES	       += $(CLHEPINC)

lib_LTLIBRARIES		= librecotasks.la 

librecotasks_la_LIBADD        = -L$(ROOTLIBDIR) $(ROOTLIBS) 
librecotasks_la_LIBADD       += -L$(top_srcdir)/base
librecotasks_la_LIBADD       += -L$(top_srcdir)/parbase
librecotasks_la_LIBADD       += -L$(top_srcdir)/base
librecotasks_la_LIBADD       += -L$(top_srcdir)/genfit

librecotasks_la_LDFLAGS 	= -version-info 0:0:0

sources              	= CbmFieldAdaptor.cxx \
			  demo/DemoRecoHit.cxx \
			  demo/DemoPatternRecoTask.cxx \
			  demo/DemoKalmanTask.cxx \
			  TrackVisTask.cxx \
			  KalmanTask.cxx \
			  TrackFitStat.cxx \
			  TrackFitStatTask.cxx

#GenfitTask.cxx \
#TrackVisTask.cxx \
#			  IdealPatternRecTask.cxx \
#			  MCHitGenerator.cxx \
#			  MickeyMouseMCTask.cxx \
#			  TrackReadTask.cxx 

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= recotasksLinkDef.h

librecotasks_la_SOURCES	= $(sources) recotasksDict.cxx

CLEANFILES		= recotasksDict.cxx recotasksDict.h *~ librecotasks.la 

recotasksDict.cxx: $(include_HEADERS) recotasksLinkDef.h 
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
