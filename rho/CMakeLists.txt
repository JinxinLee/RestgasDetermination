# Create a library called "Rho" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/pnddata # put FairRecoCandidate to fairbase at some point
  ${CMAKE_SOURCE_DIR}/pnddata/PidData # CAUTION we have a PANDA specific access: RhoGoodPhotonSelector auses PndPidCandidate
  ${CMAKE_SOURCE_DIR}/rho/RhoBase
  ${CMAKE_SOURCE_DIR}/rho/RhoMath
  ${CMAKE_SOURCE_DIR}/rho/RhoSelector
  ${CMAKE_SOURCE_DIR}/rho/RhoConstraints
  ${CMAKE_SOURCE_DIR}/rho/RhoTools
  ${CMAKE_SOURCE_DIR}/rho/RhoHistogram
  ${CMAKE_SOURCE_DIR}/rho
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS 
RhoConstraints/RhoMassConstraint.cxx 
RhoConstraints/RhoPointingConstraint.cxx 
RhoConstraints/RhoBeamConstraint.cxx 
RhoBase/RhoEventSelectorBase.cxx 
RhoTools/RhoCalculationTools.cxx
RhoMath/RhoDoubleErr.cxx
RhoMath/RhoError.cxx
RhoMath/RhoLorentzVectorErr.cxx
RhoMath/RhoVector3Err.cxx

RhoBase/RhoCandidate.cxx
RhoBase/RhoCandList.cxx
RhoBase/RhoCandListIterator.cxx
RhoBase/RhoEventInfo.cxx
RhoBase/RhoFactory.cxx
RhoTools/RhoPdtLoader.cxx
RhoBase/RhoFitterBase.cxx
RhoBase/RhoParticleSelectorBase.cxx
RhoBase/RhoVertexSelectorBase.cxx


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

RhoTools/RhoBooster.cxx
RhoBase/RhoEventShape.cxx
RhoTools/RhoEventShapes.cxx
RhoTools/RhoFindOmittedParticle.cxx
RhoTools/RhoTotalMomentum.cxx

RhoHistogram/RhoColumn.cxx
RhoHistogram/RhoTuple.cxx

RhoBase/RhoConstraint.cxx 
)

set(LINKDEF  Rho_LinkDef.h)
set(LIBRARY_NAME Rho)

set(DEPENDENCIES Base PndData)

GENERATE_LIBRARY()
