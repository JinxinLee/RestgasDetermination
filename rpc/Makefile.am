INCLUDES		= $(ROOTCFLAGS) 
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/pndbase
INCLUDES               += $(GEANT3INC)
INCLUDES               += $(CLHEPINC)

lib_LTLIBRARIES		= libRpc.la

libRpc_la_LDFLAGS 	= -version-info 0:0:0

libRpc_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base/ 

sources                 = PndRpcGeoPar.cxx PndRpcContFact.cxx PndRpcGeo.cxx PndRpcPoint.cxx PndRpcDetector.cxx PndRpcHit.cxx PndRpcHitProducerIdeal.cxx PndRpcHitProducer.cxx PndRpcSimpleReco.cxx

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= PndRpcLinkDef.h 

libRpc_la_SOURCES	= $(sources) PndRpcDict.cxx

CLEANFILES		= PndRpcDict.cxx PndRpcDict.h *~ libPndRpc.la

PndRpcDict.cxx: $(include_HEADERS) PndRpcLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
