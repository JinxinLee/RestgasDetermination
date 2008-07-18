# Please see SoftRelTools/HOWTO-GNUmakefile for documentation
# $Id: GNUmakefile,v 1.4 2001-05-27 06:07:42 marcel Exp $
#################################################################
#++ library products				[build it with 'lib']

LIBREMOVEFILES := 
LIBTMPLFILES := 
LIBDDLORDERED := 
LIBCCSTATICFILES := 

CC_SUFFIX = .cxx
CXXFLAGS += -DRHOFRAMEWORK
LIBEXTRARDLFILES := TCandReader.h TDummyFitter.h TLgndrMomFlow.h TOperatorBase.h TTotalMomentum.h \
                    TAdd4Vertexer.h TConeMomFlow.h TEventShape.h TMomTensor.h TPrintTree.h TTreeNavigator.h \
                    TBVariables.h TCones.h TFWMoments.h TOpAdd4.h TReadTree.h TUnaryOperator.h \
                    TBooster.h TDecayMode.h TFastCombiner.h TOpClone.h TSphericity.h VAbsFitter.h \
                    TCandListSort.h TDecayTree.h TFindOmittedParticle.h TOpMakeTree.h TThrust.h \
                    TCandPtr.h TDiagVertex.h TFoxWolfMom.h TOperator.h TThrustVector.h \
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
