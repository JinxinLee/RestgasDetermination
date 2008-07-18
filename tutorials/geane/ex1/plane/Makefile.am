INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/mcstack
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base 
INCLUDES               += -I$(top_srcdir)/passive
INCLUDES               += $(GEANT3INC)
INCLUDES	       += $(CLHEPINC)

lib_LTLIBRARIES		= libPlane.la

libPlane_la_LDFLAGS 	= -version-info 0:0:0

libPlane_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base
libPlane_la_LIBADD       += -L$(top_srcdir)/parbase
libPlane_la_LIBADD       += -L$(top_srcdir)/base

sources                 = CbmGeoPlane.cxx \
			  CbmGeoPlanePar.cxx \
	 		  CbmPlane.cxx \
			  CbmPlanePoint.cxx \
			  CbmPlaneContFact.cxx
			  
include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= PlaneLinkDef.h

libPlane_la_SOURCES	= $(sources) CbmPlaneDict.cxx

CLEANFILES		= CbmPlaneDict.cxx CbmPlaneDict.h *~ libPlane.la

CbmPlaneDict.cxx: $(include_HEADERS) PlaneLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
