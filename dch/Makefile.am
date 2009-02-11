INCLUDES		= $(ROOTCFLAGS) 
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/pndbase
INCLUDES               += $(GEANT3INC)
INCLUDES               += $(CLHEPINC)

lib_LTLIBRARIES		= libDch.la

libDch_la_LDFLAGS 	= -version-info 0:0:0

libDch_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base/ 

sources                 = PndDchGeoPar.cxx PndDchContFact.cxx PndDchGeo.cxx PndDchPoint.cxx \
			  PndDchDetector.cxx PndDchStructure.cxx PndDchDigi.cxx PndDchDigiProducer.cxx \
			  PndDchMapper.cxx PndDchDrifter.cxx PndDchSimpleFitter.cxx \
			  PndDchHit.cxx PndDchIdealHitProducer.cxx PndDchTrack.cxx \
			  PndDchTrackFinder.cxx PndDchTrackFinderIdeal.cxx PndDchTrackFinderIdealDigi.cxx \
			  PndDchFindTracks.cxx PndDchPreFitterTR.cxx PndDchTrackMatch.cxx PndDchMatchTracks.cxx \
			  PndDchRecoHit.cxx PndDchFindTracksQa.cxx
			  
include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= DchLinkDef.h 

libDch_la_SOURCES	= $(sources) DchDict.cxx

CLEANFILES		= DchDict.cxx DchDict.h *~ libDch.la

DchDict.cxx: $(include_HEADERS) DchLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
