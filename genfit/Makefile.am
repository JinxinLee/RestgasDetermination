# Makefile.am for module STS
# For use of automake
#
# Volker Friese    21-02-2006


INCLUDES		= $(ROOTCFLAGS)
INCLUDES	       += $(CLHEPINC)
INCLUDES	       += -Itest/
INCLUDES	       += -I$(top_srcdir)/trackbase

lib_LTLIBRARIES		= libgenfit.la

libgenfit_la_LDFLAGS 	= -version-info 0:0:0

libgenfit_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS)
libgenfit_la_LIBADD     += -lGeom -lGeomPainter 
libgenfit_la_LIBADD     +=-L$(top_srcdir)/trackbase

sources              	= AbsRecoHit.cxx \
			  RecoHitFactory.cxx \
			  RecoHitProducer.cxx \
			  AbsTrackRep.cxx \
			  LSLEQM.cxx \
			  LSLTrackRep.cxx \
			  Nystrom.cxx \
			  FitParams.cxx \
			  FitterExceptions.cxx \
			  TrackCand.cxx \
			  DetPlane.cxx \
			  PlanarHitPolicy.cxx \
			  SpacepointHitPolicy.cxx \
			  test/GenfitTester.cxx \
			  test/TestRecoHit.cxx \
			  Track.cxx \
			  Kalman.cxx
#GEANEhelixTrackRep.cxx 
#AliceHelixTrackRep.cxx \
#GEANEhelixTrackRep.cxx \
#SL2DTrackRep.cxx \
#SLTrackRep.cxx \
#SltHit.cxx \
#Slt2DHit.cxx \
#statTree.cxx \
#statTreeAnalyzer.cxx \
#statTreeAnalyzer_auto.C


include_HEADERS		= $(patsubst %.cxx,%.h,$(sources)) statTreeAnalyzer_auto.h

noinst_HEADERS		= 

libgenfit_la_SOURCES	= $(sources) genfitDict.cxx

CLEANFILES		= genfitDict.cxx genfitDict.h *~ libgenfit.la 

genfitDict.cxx: FitParams.h AbsTrackRep.h  LSLTrackRep.h GEANEhelixTrackRep.h Track.h TrackCand.h test/GenfitTester.h genfitLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
#genfitDict.cxx: FitParams.h AbsTrackRep.h  LSLTrackRep.h GEANEhelixTrackRep.h AliceHelixTrackRep.h SLTrackRep.h Track.h TrackCand.h test/GenfitTester.h genfitLinkDef.h 
#$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
#bin_PROGRAMS 	        = localTrkRepTest

#localTrkRepTest_SOURCES 	= localTrkRepTest.cxx

#localTrkRepTest_LDADD 		= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(prefix)/lib /lib/libdl.so.2 -lgenfit
#localTrkRepTest_LDADD           += $(libgenfit_la_LIBADD) $(prefix)/lib/libtpc.so $(prefix)/lib/libBase.so
#TpcDrawPadPlane_SOURCES	= TpcDrawPadPlane.cc

#TpcDrawPadPlane_LDADD	= $(prefix)/lib/libtpc.so -L$(ROOTLIBDIR) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lGeom -lGeomPainter -lVMC -lEG -lEGPythia6 -L$(ROOTSIM)/generators/lib/ -lPythia6 -L$(prefix)/lib -lBase -lGeoBase -lParBase -lPndBase /lib/libdl.so.2 -L$(SYSTEMC)/lib-linux -lsystemc -lm 

#bin_PROGRAMS 	        = manualtest
#manualtest_SOURCES	= test/manualtest.cxx
#manualtest_LDADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(prefix)/lib /lib/libdl.so.2 -lgenfit
#manualtest_LDADD	+= $(libgenfit_la_LIBADD)
