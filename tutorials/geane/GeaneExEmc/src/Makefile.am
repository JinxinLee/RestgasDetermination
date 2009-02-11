INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/parbase
INCLUDES               += -I$(top_srcdir)/mcstack
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/base 
INCLUDES               += -I$(top_srcdir)/geane
INCLUDES               += -I$(top_srcdir)/trackbase
INCLUDES               += -I$(top_srcdir)/plane
INCLUDES               += -I$(top_srcdir)/genfit
INCLUDES               += -I$(top_srcdir)/stt
INCLUDES               += $(GEANT3INC)


lib_LTLIBRARIES		= libGeaneEx.la

libGeaneEx_la_LDFLAGS 	= -version-info 0:0:0

libGeaneEx_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base
libGeaneEx_la_LIBADD       += -L$(top_srcdir)/parbase
libGeaneEx_la_LIBADD       += -L$(top_srcdir)/base

sources                 = FairGeaneTr.cxx  

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= GeaneExLinkDef.h

libGeaneEx_la_SOURCES	= $(sources) FairGeaneExDict.cxx

CLEANFILES		= FairGeaneExDict.cxx FairGeaneExDict.h *~ libGeaneEx.la

FairGeaneExDict.cxx: $(include_HEADERS) GeaneExLinkDef.h 
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
