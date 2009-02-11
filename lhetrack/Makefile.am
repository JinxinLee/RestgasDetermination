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
INCLUDES               += -I$(top_srcdir)/parbase 
INCLUDES               += -I$(top_srcdir)/trackbase 
INCLUDES               += -I$(top_srcdir)/field
INCLUDES               += -I$(top_srcdir)/passive
INCLUDES               += -I$(top_srcdir)/tpc
INCLUDES               += -I$(top_srcdir)/pndbase
INCLUDES               += -I$(top_srcdir)/mvd/tst


lib_LTLIBRARIES		= libLHETrack.la

libLHETrack_la_LDFLAGS 	= -version-info 0:0:0

libLHETrack_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) -L$(top_srcdir)/base/ 

include_HEADERS		=  PndTpcLhePoint.h        PndTpcLheHit.h         \
                           PndTpcLheTrackCuts.h    PndTpcLheSegments.h    \
			   PndTpcLheTrack.h        PndTpcLheCMPoint.h     \
			   PndTpcLheTrackFinder.h  PndTpcLheTrackFitter.h \
			   PndTpcLheHitsMaker.h    PndTpcLheCMTrack.h
			   


noinst_HEADERS		= LHETrackLinkDef.h

libLHETrack_la_SOURCES	= PndTpcLhePoint.cxx       PndTpcLheHit.cxx         \
                          PndTpcLheSegments.cxx    PndTpcLheTrackCuts.cxx   \
			  PndTpcLheCMPoint.cxx     PndTpcLheCMTrack.cxx     \
		          PndTpcLheTrack.cxx       PndTpcLheHitsMaker.cxx   \
			  PndTpcLheTrackFinder.cxx PndTpcLheTrackFitter.cxx \
			  LHETrackDict.cxx

CLEANFILES		= LHETrackDict.cxx LHETrackDict.h *~ libLHETrack.la

LHETrackDict.cxx: $(include_HEADERS) LHETrackLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
