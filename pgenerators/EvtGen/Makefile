
#New EvtGen makefile October 5, 1998
#Authors David Lange and Anders Ryd


SRCSC= $(wildcard *.cc)
SRCSF= $(wildcard *.F)

OBJSC = $(SRCSC:%.cc=%.o)
OBJSF = $(SRCSF:%.F=%.o)

OBJS = $(OBJSC)
OBJS += $(OBJSF)

LIBOBJS=$(filter-out top.o testEvtGen.o ,$(OBJS))

include config.mk

lib: depend comp ar baselib modelslib 

comp:$(OBJS)

clean:
	rm -f *.o libEvtGen.a libEvtGenBase.a libEvtGenModels.a Evt*/*.o testEvtGen


depend:
	@echo "Checking makedepend in EvtGen"
	@makedepend -- $(EVTDEPINC) -- -Y $(SRCSC) $(SRCSF)


.cc.o:
	$(EVTCXX) $(EVTCXXFLAGS) -c $<


.F.o:
	$(EVTF77) $(EVTF77FLAGS) -c $<

ar:
	@if (test -f libEvtGen.a ) then (rm libEvtGen.a) fi
	@ar rv libEvtGen.a $(LIBOBJS) >& /dev/null
	@echo "Done making libEvtGen.a"

baselib:
	@cd EvtGenBase; make lib; cd ..

modelslib:
	@cd EvtGenModels; make lib; cd ..

bin:
	$(EVTCXX) $(EVTLINK) $(EVTLINKOPT) -o testEvtGen testEvtGen.o -lEvtGen -lEvtGenModels -lEvtGenBase  -lCLHEP -l$(EVTPYLIB) -l$(EVTPDLIB) -lpacklib -lmathlib -l$(EVTPHLIB) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -ldl $(EVTSYSLIB) -lm -lMathCore -lThread

panda:
	$(EVTCXX) $(EVTLINK) $(EVTLINKOPT) -o pandaEvtGen pandaEvtGen.o -lEvtGen -lEvtGenModels -lEvtGenBase  -lCLHEP -l$(EVTPYLIB) -l$(EVTPDLIB) -lpacklib -lmathlib -l$(EVTPHLIB) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -ldl $(EVTSYSLIB) -lm -lMathCore -lThread

gg:
	$(EVTCXX) $(EVTLINK) $(EVTLINKOPT) -o ggEvtGen ggEvtGen.o -lEvtGen -lEvtGenModels -lEvtGenBase  -lCLHEP -l$(EVTPYLIB) -l$(EVTPDLIB) -lpacklib -lmathlib -l$(EVTPHLIB) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -ldl $(EVTSYSLIB) -lm -lMathCore -lThread

simple:
	$(EVTCXX) $(EVTLINK) $(EVTLINKOPT) -o simpleEvtGen simpleEvtGen.o -lEvtGen -lEvtGenModels -lEvtGenBase  -lCLHEP -l$(EVTPYLIB) -l$(EVTPDLIB) -lpacklib -lmathlib -l$(EVTPHLIB) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -ldl $(EVTSYSLIB) -lm -lMathCore -lThread

jgm:
	$(EVTCXX) $(EVTLINK) $(EVTLINKOPT) -o jgmEvtGen jgmEvtGen.o -lEvtGen -lEvtGenModels -lEvtGenBase  -lCLHEP -l$(EVTPYLIB) -l$(EVTPDLIB) -lpacklib -lmathlib -l$(EVTPHLIB) -lCore -lCint -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -ldl $(EVTSYSLIB) -lm -lMathCore -lThread
# DO NOT DELETE THIS LINE -- make depend depends on it.

EvtGen.o: EvtGenBase/EvtPatches.hh EvtGenBase/EvtComplex.hh
EvtGen.o: EvtGenBase/EvtConst.hh EvtGen.hh EvtGenBase/EvtPDL.hh
EvtGen.o: EvtGenBase/EvtPartProp.hh EvtGenBase/EvtSpinType.hh
EvtGen.o: EvtGenBase/EvtReport.hh EvtGenBase/EvtId.hh
EvtGen.o: EvtGenBase/EvtAbsLineShape.hh EvtGenBase/EvtStringHash.hh
EvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/LorentzVector.h
EvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/defs.h
EvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ThreeVector.h
EvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ThreeVector.icc
EvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/LorentzVector.icc
EvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ZMxpv.h
EvtGen.o: EvtGenBase/EvtVector4R.hh EvtGenBase/EvtVectorParticle.hh
EvtGen.o: EvtGenBase/EvtVector4C.hh EvtGenBase/EvtVector3C.hh
EvtGen.o: EvtGenBase/EvtVector3R.hh EvtGenBase/EvtParticle.hh
EvtGen.o: EvtGenBase/EvtSpinDensity.hh EvtGenBase/EvtScalarParticle.hh
EvtGen.o: EvtGenBase/EvtDecayTable.hh EvtGenBase/EvtDecayBase.hh
EvtGen.o: EvtGenBase/EvtStdHep.hh EvtGenBase/EvtSecondary.hh
EvtGen.o: EvtGenBase/EvtRandom.hh EvtGenBase/EvtRandomEngine.hh
EvtGen.o: EvtGenBase/EvtParticleFactory.hh EvtGenModels/EvtModelReg.hh
EvtGen.o: EvtGenBase/EvtStatus.hh EvtGenBase/EvtAbsRadCorr.hh
EvtGen.o: EvtGenBase/EvtRadCorr.hh EvtGenModels/EvtPHOTOS.hh
cgFinder.o: EvtGenBase/EvtPatches.hh EvtGenBase/EvtOrthogVector.hh
cgFinder.o: EvtGenBase/EvtCGCoefSingle.hh EvtGenBase/EvtdFunction.hh
cgFinder.o: EvtGenBase/EvtReport.hh
example1.o: EvtGenBase/EvtPatches.hh EvtGenBase/EvtParticleFactory.hh
example1.o: EvtGenBase/EvtSpinType.hh EvtGenBase/EvtReport.hh
example1.o: EvtGenBase/EvtStdHep.hh EvtGenBase/EvtVector4R.hh
example1.o: ../EvtGen/EvtGen.hh EvtGenBase/EvtPDL.hh
example1.o: EvtGenBase/EvtPartProp.hh EvtGenBase/EvtId.hh
example1.o: EvtGenBase/EvtAbsLineShape.hh EvtGenBase/EvtStringHash.hh
example1.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/LorentzVector.h
example1.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/defs.h
example1.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ThreeVector.h
example1.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ThreeVector.icc
example1.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/LorentzVector.icc
example1.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ZMxpv.h
example1.o: EvtGenBase/EvtParticle.hh EvtGenBase/EvtSpinDensity.hh
example1.o: EvtGenBase/EvtComplex.hh EvtGenBase/EvtConst.hh
example1.o: EvtGenBase/EvtRandom.hh
ggEvtGen.o: EvtGenBase/EvtPatches.hh EvtGenBase/EvtParticleFactory.hh
ggEvtGen.o: EvtGenBase/EvtSpinType.hh EvtGenBase/EvtReport.hh
ggEvtGen.o: EvtGenBase/EvtStdHep.hh EvtGenBase/EvtVector4R.hh EvtGen.hh
ggEvtGen.o: EvtGenBase/EvtPDL.hh EvtGenBase/EvtPartProp.hh
ggEvtGen.o: EvtGenBase/EvtId.hh EvtGenBase/EvtAbsLineShape.hh
ggEvtGen.o: EvtGenBase/EvtStringHash.hh
ggEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/LorentzVector.h
ggEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/defs.h
ggEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ThreeVector.h
ggEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ThreeVector.icc
ggEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/LorentzVector.icc
ggEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ZMxpv.h
ggEvtGen.o: EvtGenBase/EvtParticle.hh EvtGenBase/EvtSpinDensity.hh
ggEvtGen.o: EvtGenBase/EvtComplex.hh EvtGenBase/EvtConst.hh
ggEvtGen.o: EvtGenBase/EvtRandom.hh EvtGenBase/EvtRandomEngine.hh
ggEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Random/JamesRandom.h
ggEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Random/defs.h
ggEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Random/RandomEngine.h
ggEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Random/RandomEngine.icc
pandaEvtGen.o: EvtGenBase/EvtPatches.hh EvtGenBase/EvtParticleFactory.hh
pandaEvtGen.o: EvtGenBase/EvtSpinType.hh EvtGenBase/EvtReport.hh
pandaEvtGen.o: EvtGenBase/EvtStdHep.hh EvtGenBase/EvtVector4R.hh
pandaEvtGen.o: ../EvtGen/EvtGen.hh EvtGenBase/EvtPDL.hh
pandaEvtGen.o: EvtGenBase/EvtPartProp.hh EvtGenBase/EvtId.hh
pandaEvtGen.o: EvtGenBase/EvtAbsLineShape.hh EvtGenBase/EvtStringHash.hh
pandaEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/LorentzVector.h
pandaEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/defs.h
pandaEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ThreeVector.h
pandaEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ThreeVector.icc
pandaEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/LorentzVector.icc
pandaEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ZMxpv.h
pandaEvtGen.o: EvtGenBase/EvtParticle.hh EvtGenBase/EvtSpinDensity.hh
pandaEvtGen.o: EvtGenBase/EvtComplex.hh EvtGenBase/EvtConst.hh
pandaEvtGen.o: EvtGenBase/EvtRandom.hh
simpleEvtGen.o: EvtGenBase/EvtPatches.hh EvtGenBase/EvtParticleFactory.hh
simpleEvtGen.o: EvtGenBase/EvtSpinType.hh EvtGenBase/EvtReport.hh
simpleEvtGen.o: EvtGenBase/EvtStdHep.hh EvtGenBase/EvtVector4R.hh
simpleEvtGen.o: ../EvtGen/EvtGen.hh EvtGenBase/EvtPDL.hh
simpleEvtGen.o: EvtGenBase/EvtPartProp.hh EvtGenBase/EvtId.hh
simpleEvtGen.o: EvtGenBase/EvtAbsLineShape.hh EvtGenBase/EvtStringHash.hh
simpleEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/LorentzVector.h
simpleEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/defs.h
simpleEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ThreeVector.h
simpleEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ThreeVector.icc
simpleEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/LorentzVector.icc
simpleEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ZMxpv.h
simpleEvtGen.o: EvtGenBase/EvtParticle.hh EvtGenBase/EvtSpinDensity.hh
simpleEvtGen.o: EvtGenBase/EvtComplex.hh EvtGenBase/EvtConst.hh
simpleEvtGen.o: EvtGenBase/EvtRandom.hh
testEvtGen.o: EvtGenBase/EvtPatches.hh EvtGenBase/EvtComplex.hh
testEvtGen.o: EvtGenBase/EvtConst.hh EvtGenBase/EvtTensor4C.hh
testEvtGen.o: EvtGenBase/EvtVector4C.hh EvtGenBase/EvtVector3C.hh
testEvtGen.o: EvtGenBase/EvtVector3R.hh EvtGenBase/EvtVector4R.hh
testEvtGen.o: EvtGenBase/EvtVectorParticle.hh EvtGenBase/EvtParticle.hh
testEvtGen.o: EvtGenBase/EvtSpinDensity.hh EvtGenBase/EvtId.hh
testEvtGen.o: EvtGenBase/EvtSpinType.hh EvtGenBase/EvtReport.hh
testEvtGen.o: EvtGenBase/EvtDiracParticle.hh EvtGenBase/EvtDiracSpinor.hh
testEvtGen.o: EvtGenBase/EvtScalarParticle.hh EvtGenBase/EvtKine.hh
testEvtGen.o: EvtGenBase/EvtGammaMatrix.hh EvtGenBase/EvtRandom.hh
testEvtGen.o: EvtGenBase/EvtRandomEngine.hh EvtGenBase/EvtDecayTable.hh
testEvtGen.o: EvtGenBase/EvtDecayBase.hh EvtGenBase/EvtPDL.hh
testEvtGen.o: EvtGenBase/EvtPartProp.hh EvtGenBase/EvtAbsLineShape.hh
testEvtGen.o: EvtGenBase/EvtStringHash.hh EvtGenBase/EvtStdHep.hh
testEvtGen.o: EvtGenBase/EvtSecondary.hh ../EvtGen/EvtGen.hh
testEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/LorentzVector.h
testEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/defs.h
testEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ThreeVector.h
testEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ThreeVector.icc
testEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/LorentzVector.icc
testEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Vector/ZMxpv.h
testEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Random/JamesRandom.h
testEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Random/defs.h
testEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Random/RandomEngine.h
testEvtGen.o: /usr/local/cbmsoft/cern/clhep/include/CLHEP/Random/RandomEngine.icc
testEvtGen.o: EvtGenBase/EvtParticleFactory.hh EvtGenBase/EvtIdSet.hh
testEvtGen.o: EvtGenBase/EvtParser.hh
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TH1.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TAxis.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TNamed.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TObject.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/Rtypes.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/RConfig.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/RVersion.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/DllImport.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/Rtypeinfo.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TGenericClassInfo.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TStorage.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TVersionCheck.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/Riosfwd.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TBuffer.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TString.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TRefCnt.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TMathBase.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TAttAxis.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TArrayD.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TArray.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TAttLine.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TAttFill.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TAttMarker.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TArrayC.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TArrayS.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TArrayI.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TArrayF.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/Foption.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TVectorFfwd.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TVectorDfwd.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TH2.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TMatrixFBasefwd.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TMatrixDBasefwd.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TFile.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TDirectoryFile.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TDirectory.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TList.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TSeqCollection.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TCollection.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TIterator.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TDatime.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TUUID.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TUrl.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TApplication.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TQObject.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TApplicationImp.h
testEvtGen.o: /usr/local/cbmsoft/tools/root/include/TROOT.h
