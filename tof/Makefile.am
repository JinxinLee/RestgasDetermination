INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/mcstack
INCLUDES               += -I$(top_srcdir)/passive
INCLUDES	       += $(CLHEPINC)

lib_LTLIBRARIES		= libTof.la

libTof_la_LDFLAGS 	= -version-info 0:0:0

libTof_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS)

include_HEADERS		= PndGeoTof.h PndTof.h PndGeoTofPar.h PndTofPoint.h PndTofHitProducerIdeal.h PndTofHit.h PndTofContFact.h 
                         
noinst_HEADERS		= TofLinkDef.h 

libTof_la_SOURCES	= PndGeoTof.cxx PndTof.cxx PndGeoTofPar.cxx PndTofPoint.cxx PndTofHit.cxx PndTofHitProducerIdeal.cxx PndTofContFact.cxx PndTofDict.cxx 
			   
CLEANFILES		= PndTofDict.cxx PndTofDict.h *~ libTof.la

PndTofDict.cxx: $(include_HEADERS) TofLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^