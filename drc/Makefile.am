INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/mcstack
INCLUDES               += -I$(top_srcdir)/drc/drcprop

lib_LTLIBRARIES		= libDrc.la

libDrc_la_LDFLAGS 	= -version-info 0:0:0

libDrc_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS)

include_HEADERS		= PndGeoDrc.h PndDrc.h PndGeoDrcPar.h PndDrcContFact.h PndDrcPDPoint.h PndDrcHitProducerIdeal.h PndDrcHit.h 

noinst_HEADERS		= DrcLinkDef.h 

libDrc_la_SOURCES	= PndGeoDrc.cxx PndDrc.cxx PndGeoDrcPar.cxx PndDrcContFact.cxx PndDrcPoint.cxx PndDrcHitProducerIdeal.cxx PndDrcHit.cxx  PndDrcDict.cxx

CLEANFILES		= PndDrcDict.cxx PndDrcDict.h *~ libDrc.la

PndDrcDict.cxx: $(include_HEADERS) DrcLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
