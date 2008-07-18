# Makefile.am for module PASSIVE
# For use of automake
#
# Volker Friese    20-02-2006


INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/base

lib_LTLIBRARIES		= libPassive.la

libPassive_la_LDFLAGS 	= -version-info 0:0:0

libPassive_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS)

sources                 = PndCave.cxx            \
	       		  PndGeoCave.cxx         \
			  PndGeoMagnet.cxx       \
			  PndGeoPassivePar.cxx   \
			  PndGeoPipe.cxx         \
			  PndGeoTarget.cxx       \
			  PndMagnet.cxx          \
			  PndPassiveContFact.cxx \
			  PndPipe.cxx            \
			  PndTarget.cxx 

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= PassiveLinkDef.h 

libPassive_la_SOURCES	= $(sources) PndPassiveDict.cxx

CLEANFILES		= PndPassiveDict.cxx PndPassiveDict.h *~ libPassive.la

PndPassiveDict.cxx: $(include_HEADERS) PassiveLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
