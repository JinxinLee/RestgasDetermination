
INCLUDES		= $(ROOTCFLAGS)
INCLUDES	       += -I$(top_srcdir)/base
INCLUDES	       += -I$(top_srcdir)/parbase

lib_LTLIBRARIES		= libField.la

libField_la_LDFLAGS 	= -version-info 0:0:0

libField_la_LIBADD	= -L$(ROOTLIBDIR) $(ROOTLIBS) 

sources	               := PndConstField.cxx       \
			  PndFieldContFact.cxx    \
			  PndFieldMap.cxx         \
			  PndFieldMapData.cxx     \
			  PndSolenoidMap.cxx     \
			  PndFieldPar.cxx         \
			  PndFieldCreator.cxx     \
			  PndDipoleMap.cxx	\
			  PndTransMap.cxx	\
			  PndRegion.cxx         \
			  PndDipolePar.cxx	\
			  PndTransPar.cxx	\
			  PndSolenoidPar.cxx     \
			  PndMapPar.cxx          \
			  PndMultiFieldPar.cxx   \
                          PndConstPar.cxx \
			  PndMultiField.cxx


include_HEADERS		= $(patsubst %.cxx,%.h,$(sources))

noinst_HEADERS		= FieldLinkDef.h 

libField_la_SOURCES	= $(sources) FieldDict.cxx

CLEANFILES		= FieldDict.cxx FieldDict.h *~ libField.la

FieldDict.cxx: $(include_HEADERS) FieldLinkDef.h
	$(ROOTCINT) -f $@ -c -DHAVE_CONFIG_H $(INCLUDES) $^
