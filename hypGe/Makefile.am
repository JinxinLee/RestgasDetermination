INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/pndbase

lib_LTLIBRARIES		= libHypGe.la

libHypGe_la_LDFLAGS 	= -version-info 0:0:0

libHypGe_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS)

include_HEADERS		= PndGeoHypGe.h PndHypGe.h PndGeoHypGePar.h PndHypGePoint.h PndHypGeHit.h PndHypGeContFact.h GeCluster.h PndHypGeReader.h PndHypGeHitProducerIdeal.h

noinst_HEADERS		= HypGeLinkDef.h 

libHypGe_la_SOURCES	= PndGeoHypGe.cxx PndHypGe.cxx PndGeoHypGePar.cxx PndHypGePoint.cxx PndHypGeHit.cxx  PndHypGeContFact.cxx PndHypGeDict.cxx GeCluster.cxx PndHypGeReader.cxx PndHypGeHitProducerIdeal.cxx

CLEANFILES		= PndHypGeDict.cxx PndHypGeDict.h *~ libHypGe.la

PndHypGeDict.cxx: $(include_HEADERS) HypGeLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^