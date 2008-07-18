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
INCLUDES	       += $(CLHEPINC)
INCLUDES	       += -Itest/

lib_LTLIBRARIES		= libtpc.la

libtpc_la_LDFLAGS 	= -version-info 0:0:0

libtpc_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base
libtpc_la_LIBADD       += -L$(top_srcdir)/parbase
libtpc_la_LIBADD       += -L$(top_srcdir)/base
libtpc_la_LIBADD       += -L$(top_srcdir)/genfit

cint_sources              	= PndTpcGeo.cxx \
			  PndTpcGeoPar.cxx \
			  PndTpcContFact.cxx \
			  PndTpcDetector.cxx \
			  PndTpcPoint.cxx \
			  PndTpcClusterizerTask.cxx \
			  PndTpcGas.cxx \
			  PndTpcPrimaryCluster.cxx \
			  LinearInterpolPolicy.cxx \
			  PndTpcDriftedElectron.cxx \
			  PndTpcDriftTask.cxx \
			  PndTpcAvalanche.cxx \
		          PndTpcGem.cxx \
			  PndTpcGemTask.cxx \
			  PndTpcSignal.cxx \
			  PndTpcSignalAge.cxx \
			  McId.cxx \
			  PndTpcAbsPadShape.cxx \
			  PndTpcPRLookupTable.cxx \
			  PndTpcPadPlane.cxx \
	                  PndTpcPad.cxx \
			  PndTpcPadShapePool.cxx \
			  PndTpcPadShapeSquare.cxx \
			  PndTpcPadShapePolygon.cxx \
			  PndTpcPadResponseTask.cxx \
			  PndTpcFrontend.cxx \
			  PndTpcAdcTask.cxx \
			  McIdCollection.cxx \
	   		  PndTpcSample.cxx \
			  PndTpcCRRCPulseshape.cxx \
			  PndTpcDigitizationPolicy.cxx \
			  PndTpcDigi.cxx \
			  PndTpcDigiAge.cxx \
			  PndTpcDigiMapper.cxx \
			  PndTpcSimplePSAStrategy.cxx \
	 		  PndTpcPSATask.cxx \
			  PndTpcSpaceChargeTask.cxx \
			  PndTpcAbsField.cxx \
			  PndTpcFieldCylGrid.cxx \
			  PndTpcEFieldCyl.cxx \
			  PndTpcEFieldCylLoader.cxx \
			  PndTpcDevmapCyl.cxx \
			  PndTpcDevmapCylLoader.cxx\
	 		  PndTpcEvtMixTask.cxx \
			  PndTpcElectronicsTask.cxx \
			  PndTpcClusterFinderTask.cxx \
			  PndTpcCluster.cxx \
			  PndTpcClusterRadius.cxx \
			  TORPadProcessor.cxx \
			  TORPPState_Initial.cxx \
			  TORPPState_Compare.cxx \
			  TORPPState_Output.cxx \
			  TORPPState_Send.cxx \
			  TORPPState_Wait.cxx \
			  PndTpcSectorProcessor.cxx \
			  PndTpcRungeKutta.cxx \
			  PndTpcInhFieldDrifter.cxx \
			  test/PndTpcTester.cxx \
			  PndTpcPSAplot.cxx \
			  PndTpcPSA_TOT1.cxx \
			  DEDX.cxx \
			  PndTpcMCDEdxTask.cxx \
			  PndTpcRawDEdxCollection.cxx \
			  PndTpcDXCalculator.cxx

sources 		= $(cint_sources)
sources	                += DebugLogger.cxx \
		          dbgstream.cxx

#PndTpcSimplePatternRecoTask.cxx \
#		  PndTpcXYRecoHit.cxx 
include_HEADERS		= $(patsubst %.cxx,%.h,$(cint_sources))


noinst_HEADERS		= tpcLinkDef.h

libtpc_la_SOURCES	= $(sources) tpcDict.cxx

CLEANFILES		= tpcDict.cxx tpcDict.h *~ libtpc.la

tpcDict.cxx: $(include_HEADERS) tpcLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
