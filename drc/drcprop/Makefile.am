INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/pndbase

lib_LTLIBRARIES		= libDrcProp.la

libDrcProp_la_LDFLAGS 	= -version-info 0:0:0

libDrcProp_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS)

include_HEADERS		= 	PndDrcOptDev.h \
				PndDrcOptDevManager.h \
				PndDrcOptDevSys.h \
				PndDrcOptMatAbs.h \
				PndDrcOptMatLithotecQ0.h \
				PndDrcOptMatNLAK33A.h \
				PndDrcOptMatLLF1.h \
				PndDrcOptMatVacuum.h \
				PndDrcOptReflAbs.h \
				PndDrcOptReflSilver.h \
				PndDrcOptReflSurf.h \
				PndDrcOptReflPerfect.h \
				PndDrcOptVol.h \
				PndDrcPhoton.h \
				PndDrcSurfAbs.h \
				PndDrcSurfCyl.h \
				PndDrcSurfQuadFlatDiff.h \
				PndDrcSurfPolyFlat.h \
				PndDrcSurfPolySphere.h \
				PndDrcSurfPolyPara.h \
				PndDrcSurfPolyAsphere.h \
				PndDrcUtil.h

noinst_HEADERS		= DrcPropLinkDef.h 

libDrcProp_la_SOURCES	= 	PndDrcOptDev.cxx \
				PndDrcOptDevManager.cxx \
				PndDrcOptDevSys.cxx \
				PndDrcOptMatAbs.cxx  \
				PndDrcOptMatLithotecQ0.cxx \
				PndDrcOptMatNLAK33A.cxx \
				PndDrcOptMatLLF1.cxx \
				PndDrcOptMatVacuum.cxx \
				PndDrcOptReflSilver.cxx \
				PndDrcOptReflSurf.cxx \
				PndDrcOptReflPerfect.cxx \
				PndDrcOptVol.cxx PndDrcPhoton.cxx \
				PndDrcSurfAbs.cxx PndDrcSurfCyl.cxx \
				PndDrcSurfQuadFlatDiff.cxx \
				PndDrcSurfPolyFlat.cxx \
				PndDrcSurfPolySphere.cxx \
				PndDrcSurfPolyPara.cxx \
				PndDrcSurfPolyAsphere.cxx \
				PndDrcUtil.cxx

CLEANFILES		= PndDrcPropDict.cxx PndDrcPropDict.h *~ libPndDrcProp.la

PndDrcPropDict.cxx: $(include_HEADERS) DrcPropLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
