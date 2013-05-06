# Create a library called "Rho" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/PidData 
${CMAKE_SOURCE_DIR}/rho/RhoBase
${CMAKE_SOURCE_DIR}/rho/RhoMath
${CMAKE_SOURCE_DIR}/rho/RhoSelector
${CMAKE_SOURCE_DIR}/rho/RhoConstraints
${CMAKE_SOURCE_DIR}/rho/RhoTools
${CMAKE_SOURCE_DIR}/rho/RhoHistogram
${CMAKE_SOURCE_DIR}/rho
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(Rho_SRCS 
RhoConstraints/RhoMassConstraint.cxx 
RhoConstraints/RhoPointingConstraint.cxx 
RhoConstraints/RhoBeamConstraint.cxx 
RhoBase/RhoEventSelectorBase.cxx 
RhoTools/RhoCalculationTools.cxx
#RhoMath/TAsymGaussConsistency.cxx
#RhoMath/TChisqConsistency.cxx
#RhoMath/TConsistency.cxx
#RhoMath/TConsistencySet.cxx
RhoMath/RhoDoubleErr.cxx
RhoMath/RhoError.cxx
#RhoMath/TGaussConsistency.cxx
RhoMath/RhoLorentzVectorErr.cxx
#RhoMath/TNumRecipes.cxx
#RhoMath/TPointErr.cxx
#RhoMath/TPoissonConsistency.cxx
#RhoMath/TProbab.cxx
RhoMath/RhoVector3Err.cxx

#RhoBase/TFitParams.cxx
RhoBase/RhoCandidate.cxx
RhoBase/RhoCandList.cxx
RhoBase/RhoCandListIterator.cxx
#RhoBase/TConstraint.cxx
RhoBase/RhoEventInfo.cxx
RhoBase/RhoFactory.cxx
#RhoBase/TParm.cxx
RhoTools/RhoPdtLoader.cxx
#RhoBase/TRho.cxx
#RhoBase/TSimpleVertex.cxx
#RhoBase/TSmartPointer.cxx
#RhoBase/VAbsBit.cxx
#RhoBase/VAbsCollection.cxx
RhoBase/RhoFitterBase.cxx
#RhoBase/VAbsMicroCandidate.cxx
#RhoBase/VAbsPidInfo.cxx
RhoBase/RhoParticleSelectorBase.cxx
#RhoBase/VAbsReader.cxx
#RhoBase/VAbsTag.cxx
#RhoBase/VAbsVertexAlgorithm.cxx
#RhoBase/VAbsTruth.cxx
#RhoBase/VAbsVertex.cxx
#RhoBase/VAbsVertexer.cxx
RhoBase/RhoVertexSelectorBase.cxx
#RhoBase/VAbsWriter.cxx


RhoSelector/RhoChargedParticleSelector.cxx
RhoSelector/RhoEnergyParticleSelector.cxx
RhoSelector/RhoMassParticleSelector.cxx
RhoSelector/RhoMinusParticleSelector.cxx
RhoSelector/RhoMomentumParticleSelector.cxx
RhoSelector/RhoNeutralParticleSelector.cxx
RhoSelector/RhoPlusParticleSelector.cxx
RhoSelector/RhoSimpleElectronSelector.cxx
RhoSelector/RhoSimpleKaonSelector.cxx
RhoSelector/RhoSimpleMuonSelector.cxx
RhoSelector/RhoSimplePionSelector.cxx
RhoSelector/RhoSimpleProtonSelector.cxx
RhoSelector/RhoSimpleVertexSelector.cxx
RhoSelector/RhoSimpleVertexSelector.cxx
RhoSelector/RhoGoodPhotonSelector.cxx
RhoSelector/RhoGoodTrackSelector.cxx

#RhoTools/TAdd4Vertexer.cxx
RhoTools/RhoBooster.cxx
#RhoTools/TCandListSort.cxx
#RhoTools/TCandPtr.cxx
#RhoTools/TCandReader.cxx
#RhoTools/TConeMomFlow.cxx
#RhoTools/TCones.cxx
#RhoTools/TDecayMode.cxx
#RhoTools/TDecayTree.cxx
#RhoTools/TDiagVertex.cxx
#RhoTools/TDummyFitter.cxx
RhoBase/RhoEventShape.cxx
#RhoTools/TFastCombiner.cxx
RhoTools/RhoFindOmittedParticle.cxx
#RhoTools/TFoxWolfMom.cxx
#RhoTools/TFWMoments.cxx
#RhoTools/TLgndrMomFlow.cxx
#RhoTools/TMomTensor.cxx
#RhoTools/TOpAdd4.cxx
#RhoTools/TOpClone.cxx
#RhoTools/TOperatorBase.cxx
#RhoTools/TOperator.cxx
#RhoTools/TOpMakeTree.cxx
#RhoTools/TPrintTree.cxx
#RhoTools/TReadTree.cxx
#RhoTools/TSphericity.cxx
#RhoTools/TThrust.cxx
#RhoTools/TThrustVector.cxx
RhoTools/RhoTotalMomentum.cxx
#RhoTools/TTreeNavigator.cxx
#RhoTools/TUnaryOperator.cxx

RhoHistogram/RhoColumn.cxx
RhoHistogram/RhoTuple.cxx

RhoBase/RhoConstraint.cxx 


)
set (Rho_RULECHECKER_SRCS ${Rho_SRCS})

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${Rho_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" RHO_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h Rho_HEADERS "${Rho_SRCS}")

set(Rho_LINKDEF  Rho_LinkDef.h)
set(Rho_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/RhoDict.cxx) 

ROOT_GENERATE_DICTIONARY("${Rho_HEADERS}" "${Rho_LINKDEF}" "${Rho_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(Rho_SRCS ${Rho_SRCS} ${Rho_DICTIONARY})

add_library(Rho SHARED ${Rho_SRCS})
target_link_libraries(Rho ${ROOT_LIBRARIES})
set_target_properties(Rho PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Rho DESTINATION ${CMAKE_BINARY_DIR}/lib)
