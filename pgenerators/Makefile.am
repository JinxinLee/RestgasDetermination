# Copyright (C) 2005 Florian Uhlig <f.uhlig@gsi.de>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INCLUDES		= $(ROOTCFLAGS)
INCLUDES               += -I$(top_srcdir)/base
INCLUDES               += -I$(top_srcdir)/geobase
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/generators

lib_LTLIBRARIES		= libPGen.la

libPGen_la_LDFLAGS 	= -version-info 0:0:0

libPGen_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base/ 

sources                 = PndDpmGenerator.cxx PndUrqmdSmmGenerator.cxx PndBoxGenerator.cxx PndJRGenerator.cxx

include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= PGenLinkDef.h 

libPGen_la_SOURCES	= $(sources) PndPGenDict.cxx

CLEANFILES		= PndPGenDict.cxx PndPGenDict.h *~ lib.la

PndPGenDict.cxx: $(include_HEADERS) PGenLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
