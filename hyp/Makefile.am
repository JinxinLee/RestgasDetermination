INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/pndbase

lib_LTLIBRARIES		= libHyp.la

libHyp_la_LDFLAGS 	= -version-info 0:0:0

libHyp_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS)

include_HEADERS		= CbmGeoHyp.h CbmHyp.h CbmGeoHypPar.h CbmHypPoint.h CbmHypHit.h CbmHypContFact.h

noinst_HEADERS		= HypLinkDef.h 

libHyp_la_SOURCES	= CbmGeoHyp.cxx CbmHyp.cxx CbmGeoHypPar.cxx CbmHypPoint.cxx CbmHypHit.cxx  CbmHypContFact.cxx CbmHypDict.cxx

CLEANFILES		= CbmHypDict.cxx CbmHypDict.h *~ libHyp.la

CbmHypDict.cxx: $(include_HEADERS) HypLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^