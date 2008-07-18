# Please see SoftRelTools/HOWTO-GNUmakefile for documentation
# $Id: GNUmakefile,v 1.5 2001-06-07 07:59:40 marcel Exp $
#################################################################
#++ library products				[build it with 'lib']

LIBREMOVEFILES := 
LIBTMPLFILES := 
LIBDDLORDERED := 
LIBCCSTATICFILES := 

CC_SUFFIX = .cxx
LIBEXTRARDLFILES := TEventInfo.h TRho.h VAbsReader.h VAbsVertexSelector.h \
                    TCandList.h TFactory.h TRhoVersion.h VAbsCollection.h VAbsTag.h VAbsVertexer.h \
                    TCandListIterator.h TFitParams.h TSimpleVertex.h VAbsMicroCandidate.h VAbsTruth.h VAbsWriter.h \
                    TCandidate.h TParm.h TSmartPointer.h VAbsPidInfo.h VAbsVertex.h \
                    TConstraint.h TPdt.h VAbsBit.h VAbsPidSelector.h VAbsVertexAlgorithm.h \
                    $(PACKAGE)_LinkDef.h

#################################################################
#++ extra binary products	[not in production, build it with extrabin]

EXTRABINS := 

#################################################################
#++ binary products				[build it with 'bin']

BINS := 
BINCCFILES := 

#++ Binary rules		 [in production, build it with 'bin']

#++ shell script products.. 			[build it with 'bin']
BINSCRIPTS := 

#################################################################
#++ regression test scripts			[build it with 'test']

#################################################################
#++ include standard makefile from SoftRelTools.
include SoftRelTools/standard.mk
override CPPFLAGS += -I$(TOPDIR)/$(PACKAGE)
