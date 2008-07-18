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
INCLUDES               += -I$(top_srcdir)/genfit
INCLUDES               += -I$(top_srcdir)/tpc
INCLUDES	       += $(CLHEPINC)

lib_LTLIBRARIES		= libtpcreco.la

libtpcreco_la_LDFLAGS 	= -version-info 0:0:0

libtpcreco_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base
libtpcreco_la_LIBADD       += -L$(top_srcdir)/parbase
libtpcreco_la_LIBADD       += -L$(top_srcdir)/base
libtpcreco_la_LIBADD       += -L$(top_srcdir)/tpc
libtpcreco_la_LIBADD       += -L$(top_srcdir)/genfit


sources              	= PndTpcConfMapRecoHit.cxx \
			  PndTpcConfMapFit.cxx \
			  PndTpcZSFit.cxx \
			  PndTpcPatternRecoTask.cxx \
			  PndTpcConfTrackFinder.cxx \
			  test/PndTpcRecoTester.cxx \
			  PndTpcPlanarRecoHit.cxx \
			  PndTpcTrackVisTask.cxx \
			  PndTpcRecoDEdxTask.cxx

#PndTpcSimplePatternRecoTask.cxx \
#		  PndTpcXYRecoHit.cxx 
include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= tpcrecoLinkDef.h

libtpcreco_la_SOURCES	= $(sources) tpcrecoDict.cxx

CLEANFILES		= tpcrecoDict.cxx tpcrecoDict.h *~ libtpcreco.la

tpcrecoDict.cxx: $(include_HEADERS) tpcrecoLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
