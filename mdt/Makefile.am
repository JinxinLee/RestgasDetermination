INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/mcstack

lib_LTLIBRARIES		= libMdt.la

libMdt_la_LDFLAGS 	= -version-info 0:0:1

libMdt_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS)

include_HEADERS		= MdtMC/PndMdt.h \
                          MdtMC/PndGeoMdtPar.h \
                          MdtData/PndMdtPoint.h \
			  MdtData/PndMdtHit.h \
			  MdtDigi/PndMdtHitProducerIdeal.h \
			  PndMdtContFact.h

noinst_HEADERS		= MdtLinkDef.h 

libMdt_la_SOURCES	= MdtMC/PndMdt.cxx \
                          MdtMC/PndGeoMdtPar.cxx \
                          MdtData/PndMdtPoint.cxx \
			  MdtData/PndMdtHit.cxx \
                          MdtDigi/PndMdtHitProducerIdeal.cxx \
			  PndMdtContFact.cxx

CLEANFILES		= PndMdtDict.cxx PndMdtDict.h *~ libMdt.la

PndMdtDict.cxx: $(include_HEADERS) MdtLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
