# Create a library called "libEvtGen" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

add_subdirectory(photos)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/pgenerators/EvtGen
${CMAKE_SOURCE_DIR}/pgenerators/EvtGen/EvtGenBase
${CMAKE_SOURCE_DIR}/pgenerators/EvtGen/EvtGenModels
)

include_directories( ${INCLUDE_DIRECTORIES})

add_definitions("-DEVTSTANDALONE -DEVTGEN_NOHOWTO")

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${PYTHIA6_LIBRARY_DIR}
${CLHEP_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(EVTGEN_SRCS
EvtGen.cc
EvtGenBase/Evt3Rank3C.cc
EvtGenBase/EvtAbsLineShape.cc
EvtGenBase/EvtAmp.cc
EvtGenBase/EvtAmpIndex.cc
EvtGenBase/EvtAmpSubIndex.cc
EvtGenBase/EvtBlattWeisskopf.cc
EvtGenBase/EvtBreitWignerPdf.cc
EvtGenBase/EvtCGCoefSingle.cc
EvtGenBase/EvtComplex.cc
EvtGenBase/EvtConst.cc
EvtGenBase/EvtCPUtil.cc
EvtGenBase/EvtCyclic3.cc
EvtGenBase/EvtDalitzCoord.cc
EvtGenBase/EvtDalitzFlatPdf.cc
EvtGenBase/EvtDalitzPlot.cc
EvtGenBase/EvtDalitzPoint.cc
EvtGenBase/EvtDalitzResPdf.cc
EvtGenBase/EvtDecayAmp.cc
EvtGenBase/EvtDecayBase.cc
EvtGenBase/EvtDecayIncoherent.cc
EvtGenBase/EvtDecayMode.cc
EvtGenBase/EvtDecayParm.cc
EvtGenBase/EvtDecayProb.cc
EvtGenBase/EvtDecayTable.cc
EvtGenBase/EvtdFunction.cc
EvtGenBase/EvtdFunctionSingle.cc
EvtGenBase/EvtDiracParticle.cc
EvtGenBase/EvtDiracSpinor.cc
EvtGenBase/EvtEvalHelAmp.cc
EvtGenBase/EvtFlatLineShape.cc
EvtGenBase/EvtGammaMatrix.cc
EvtGenBase/EvtGenKine.cc
EvtGenBase/EvtHighSpinParticle.cc
EvtGenBase/EvtId.cc
EvtGenBase/EvtIdSet.cc
EvtGenBase/EvtIntegPdf1D.cc
EvtGenBase/EvtIntervalFlatPdf.cc
EvtGenBase/EvtKine.cc
EvtGenBase/EvtManyDeltaFuncLineShape.cc
EvtGenBase/EvtMassAmp.cc
EvtGenBase/EvtMBreitWigner.cc
EvtGenBase/EvtMHelAmp.cc
EvtGenBase/EvtMNode.cc
EvtGenBase/EvtModelAlias.cc
EvtGenBase/EvtModel.cc
EvtGenBase/EvtMParticle.cc
EvtGenBase/EvtMTree.cc
EvtGenBase/EvtMTrivialLS.cc
EvtGenBase/EvtMultiChannelParser.cc
EvtGenBase/EvtNeutrinoParticle.cc
EvtGenBase/EvtOrthogVector.cc
EvtGenBase/EvtParser.cc
EvtGenBase/EvtParticle.cc
EvtGenBase/EvtParticleDecay.cc
EvtGenBase/EvtParticleDecayList.cc
EvtGenBase/EvtParticleFactory.cc
EvtGenBase/EvtPartProp.cc
EvtGenBase/EvtPDL.cc
EvtGenBase/EvtPhotonParticle.cc
EvtGenBase/EvtPoint1D.cc
EvtGenBase/EvtPropBreitWigner.cc
EvtGenBase/EvtPropBreitWignerRel.cc
EvtGenBase/EvtPto3PAmp.cc
EvtGenBase/EvtPto3PAmpFactory.cc
EvtGenBase/EvtRadCorr.cc
EvtGenBase/EvtRandom.cc
EvtGenBase/EvtRandomEngine.cc
EvtGenBase/EvtRanFor.cc
EvtGenBase/EvtRaritaSchwinger.cc
EvtGenBase/EvtRaritaSchwingerParticle.cc
EvtGenBase/EvtRelBreitWignerBarrierFact.cc
EvtGenBase/EvtReport.cc
EvtGenBase/EvtResonance2.cc
EvtGenBase/EvtResonance.cc
EvtGenBase/EvtScalarParticle.cc
EvtGenBase/EvtSecondary.cc
EvtGenBase/EvtSemiLeptonicAmp.cc
EvtGenBase/EvtSemiLeptonicBaryonAmp.cc
EvtGenBase/EvtSemiLeptonicScalarAmp.cc
EvtGenBase/EvtSemiLeptonicTensorAmp.cc
EvtGenBase/EvtSemiLeptonicVectorAmp.cc
EvtGenBase/EvtSpinAmp.cc
EvtGenBase/EvtSpinDensity.cc
EvtGenBase/EvtStdHep.cc
EvtGenBase/EvtStringParticle.cc
EvtGenBase/EvtSymTable.cc
EvtGenBase/EvtTensor3C.cc
EvtGenBase/EvtTensor4C.cc
EvtGenBase/EvtTensorParticle.cc
EvtGenBase/EvtTwoBodyKine.cc
EvtGenBase/EvtTwoBodyVertex.cc
EvtGenBase/EvtValError.cc
EvtGenBase/EvtVector3C.cc
EvtGenBase/EvtVector3R.cc
EvtGenBase/EvtVector4C.cc
EvtGenBase/EvtVector4R.cc
EvtGenBase/EvtVectorParticle.cc
EvtGenModels/begevtgenget.F
EvtGenModels/begevtgengetx.F
EvtGenModels/begevtgenstore.F
EvtGenModels/begevtgenstorex.F
EvtGenModels/EvtModelReg.cc
EvtGenModels/EvtBHadronic.cc
EvtGenModels/EvtBsquark.cc
EvtGenModels/EvtBto2piCPiso.cc
EvtGenModels/EvtBTo3piCP.cc
EvtGenModels/EvtBTo3pi.F
EvtGenModels/EvtBTo3piMPP.F
EvtGenModels/EvtBTo3piP00.F
EvtGenModels/EvtBTo4piCP.cc
EvtGenModels/EvtBtoKD3P.cc
EvtGenModels/EvtBtoKpiCPiso.cc
EvtGenModels/EvtBToKpipiCP.cc
EvtGenModels/EvtBToKpipi.F
EvtGenModels/EvtbTosllAli.cc
EvtGenModels/EvtbTosllAliFF.cc
EvtGenModels/EvtbTosllAmp.cc
EvtGenModels/EvtbTosllBall.cc
EvtGenModels/EvtbTosllBallFF.cc
EvtGenModels/EvtbTosllScalarAmp.cc
EvtGenModels/EvtbTosllVectorAmp.cc
EvtGenModels/EvtBtoXsEtap.cc
EvtGenModels/EvtBtoXsgammaAbsModel.cc
EvtGenModels/EvtBtoXsgammaAliGreub.cc
EvtGenModels/EvtBtoXsgamma.cc
EvtGenModels/EvtBtoXsgammaFermiUtil.cc
EvtGenModels/EvtBtoXsgammaFixedMass.cc
EvtGenModels/EvtBtoXsgammaFlatEnergy.cc
EvtGenModels/EvtBtoXsgammaKagan.cc
EvtGenModels/EvtBtoXsgammaRootFinder.cc
EvtGenModels/EvtBtoXsll.cc
EvtGenModels/EvtBtoXsllUtil.cc
EvtGenModels/EvtCBTo3piMPP.cc
EvtGenModels/EvtCBTo3piP00.cc
EvtGenModels/EvtDDalitz.cc
EvtGenModels/EvtElectronPositron.cc
EvtGenModels/EvtEtaDalitz.cc
EvtGenModels/EvtFlatQ2.cc
EvtGenModels/EvtGoityRoberts.cc
EvtGenModels/EvtHelAmp.cc
EvtGenModels/EvtHQET2.cc
EvtGenModels/EvtHQET2FF.cc
EvtGenModels/EvtHQET.cc
EvtGenModels/EvtHQETFF.cc
EvtGenModels/EvtISGW2.cc
EvtGenModels/EvtISGW2FF.cc
EvtGenModels/EvtISGW.cc
EvtGenModels/EvtISGWFF.cc
EvtGenModels/EvtItgAbsFunction.cc
EvtGenModels/EvtItgAbsIntegrator.cc
EvtGenModels/EvtItgFourCoeffFcn.cc
EvtGenModels/EvtItgFunction.cc
EvtGenModels/EvtItgPtrFunction.cc
EvtGenModels/EvtItgSimpsonIntegrator.cc
EvtGenModels/EvtItgThreeCoeffFcn.cc
EvtGenModels/EvtItgTwoCoeffFcn.cc
EvtGenModels/EvtKKLambdaC.cc
EvtGenModels/EvtKKLambdaCFF.cc
EvtGenModels/EvtKstarnunu.cc
EvtGenModels/EvtKstarstargamma.cc
EvtGenModels/EvtLambdaLambdaBar.cc
EvtGenModels/EvtLambdaLambdaBarPol.cc
EvtGenModels/EvtLambdaLambdaBarHE.cc
EvtGenModels/EvtLambdaP_BarGamma.cc
EvtGenModels/EvtLi2Spence.F
EvtGenModels/EvtLNuGamma.cc
EvtGenModels/EvtMelikhov.cc
EvtGenModels/EvtMelikhovFF.cc
EvtGenModels/EvtMultibody.cc
EvtGenModels/EvtOmegaDalitz.cc
EvtGenModels/EvtPartWave.cc
EvtGenModels/EvtPFermi.cc
EvtGenModels/EvtPhiDalitz.cc
EvtGenModels/EvtPHOTOS.cc
EvtGenModels/EvtPhsp.cc
EvtGenModels/EvtPi0Dalitz.cc
EvtGenModels/EvtPpbarGammaGamma.cc
EvtGenModels/EvtPpbarPi0Gamma.cc
EvtGenModels/EvtPto3P.cc
EvtGenModels/EvtPycont.cc
EvtGenModels/EvtPyGaGa.cc
EvtGenModels/EvtPythia.cc
EvtGenModels/EvtSingleParticle.cc
EvtGenModels/EvtSll.cc
EvtGenModels/EvtSLN.cc
EvtGenModels/EvtSLPole.cc
EvtGenModels/EvtSLPoleFF.cc
EvtGenModels/EvtSSDCP.cc
EvtGenModels/EvtSSSCP.cc
EvtGenModels/EvtSSSCPpng.cc
EvtGenModels/EvtSSSCPT.cc
EvtGenModels/EvtSTS.cc
EvtGenModels/EvtSTSCP.cc
EvtGenModels/EvtSVPCP.cc
EvtGenModels/EvtSVPHelAmp.cc
EvtGenModels/EvtSVS.cc
EvtGenModels/EvtSVSCP.cc
EvtGenModels/EvtSVSCPiso.cc
EvtGenModels/EvtSVSCPLH.cc
EvtGenModels/EvtSVSNONCPEIGEN.cc
EvtGenModels/EvtSVVCP.cc
EvtGenModels/EvtSVVCPLH.cc
EvtGenModels/EvtSVVHelAmp.cc
EvtGenModels/EvtSVVNONCPEIGEN.cc
EvtGenModels/EvtTauHadnu.cc
EvtGenModels/EvtTaulnunu.cc
EvtGenModels/EvtTauScalarnu.cc
EvtGenModels/EvtTauVectornu.cc
EvtGenModels/EvtTSS.cc
EvtGenModels/EvtTVSPwave.cc
EvtGenModels/EvtVectorIsr.cc
EvtGenModels/EvtVll.cc
EvtGenModels/EvtVSPPwave.cc
EvtGenModels/EvtVSSBMixCPT.cc
EvtGenModels/EvtVSS.cc
EvtGenModels/EvtVSSMix.cc
EvtGenModels/EvtVub.cc
EvtGenModels/EvtVubdGamma.cc
EvtGenModels/EvtVubHybrid.cc
EvtGenModels/EvtVubNLO.cc
EvtGenModels/EvtVVP.cc
EvtGenModels/EvtVVpipi.cc
EvtGenModels/EvtVVSPwave.cc
EvtGenModels/Pythia.F
EvtGenModels/pykcut.F
EvtGenModels/pytime.F
EvtGenModels/lucomp_beg.F
EvtGenModels/lucomp.F
Cernlib/ddilog.F
Cernlib/ranf.F
)

# Fix a problem with g++ on Mac OS X
set_source_files_properties(EvtGenModels/EvtPythia.cc PROPERTIES COMPILE_FLAGS "-O0")

## not depending on the following files
## EvtGenModels/EvtJetSet.cc
## EvtGenModels/EvtJetSetInit.F
## EvtGenModels/EvtJscont.cc
## EvtGenModels/jetset1.F
## EvtGenModels/jetsetcont.F
## EvtGenModels/continuum.F
## NOT WORKING EvtGenModels/uclacont.F

## Set list of example binaries (actually, thats their source code name)
## binaries in EvtGenBase are not created...
set(ALLEvtGen_exe simpleEvtGen example1 irinaEvtGen pandaEvtGen simpleEvtGenRO cgFinder ggEvtGen testEvtGen)
set(ALLEvtGen_exe simpleEvtGen simpleEvtGenRO)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${EVTGEN_SRCS}" "${INCLUDE_DIRECTORIES}" EVTGEN_RULES)
endif(RULE_CHECKER_FOUND)

set(EVTGEN_HEADERS 
EvtGen.hh
)

set(EVTGEN_LINKDEF EvtGenLinkDef.h)
set(EVTGEN_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/EvtGenDict.cxx) 

#### Does not work... yet?
## ROOT_GENERATE_DICTIONARY("${EVTGEN_HEADERS}" "${EVTGEN_LINKDEF}" "${EVTGEN_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
##
## SET(EVTGEN_SRCS ${EVTGEN_SRCS} ${EVTGEN_DICTIONARY})

add_library(EvtGen SHARED ${EVTGEN_SRCS})
target_link_libraries(EvtGen ${ROOT_LIBRARIES} Pythia6 Photos ${Fortran_UseLib})
set_target_properties(EvtGen PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
set_target_properties(EvtGen PROPERTIES LINK_FLAGS "-fPIC")
set_target_properties(EvtGen PROPERTIES LINKER_LANGUAGE "Fortran")

foreach(program ${ALLEvtGen_exe})
	add_executable(EvtGen_exe_${program} ${program}.cc)
	set_target_properties(EvtGen_exe_${program} PROPERTIES OUTPUT_NAME ${program})
	set_target_properties(EvtGen_exe_${program} PROPERTIES LINK_FLAGS "-fPIC")
        set_target_properties(EvtGen_exe_${program} PROPERTIES LINKER_LANGUAGE "Fortran")
	target_link_libraries(EvtGen_exe_${program} ${ROOT_LIBRARIES} EvtGen Pythia6 Photos ${Fortran_UseLib} CLHEP)
endforeach(program)

################ install ###################
install(TARGETS EvtGen DESTINATION ${CMAKE_BINARY_DIR}/lib)
foreach(program ${ALLEvtGen_exe})
	install(TARGETS EvtGen_exe_${program} DESTINATION ${CMAKE_BINARY_DIR}/bin)
endforeach(program)

