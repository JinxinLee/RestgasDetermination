INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/mcstack

lib_LTLIBRARIES		= libMuo.la

libMuo_la_LDFLAGS 	= -version-info 0:0:0

libMuo_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS)

include_HEADERS		= PndGeoMuo.h PndMuo.h PndGeoMuoPar.h PndMuoPoint.h PndMuoContFact.h PndMuoDigi.h PndMuoDigiProducer.h

noinst_HEADERS		= MuoLinkDef.h 

libMuo_la_SOURCES	= PndGeoMuo.cxx PndMuo.cxx PndGeoMuoPar.cxx PndMuoPoint.cxx PndMuoContFact.cxx PndMuoDigi.cxx PndMuoDigiProducer.cxx PndMuoDict.cxx

CLEANFILES		= PndMuoDict.cxx PndMuoDict.h *~ libMuo.la

PndMuoDict.cxx: $(include_HEADERS) MuoLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
