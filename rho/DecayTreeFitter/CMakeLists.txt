# Create a library called DecayTreeFitter

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES}
${SIMPATH}/include
#${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/rho
${CMAKE_SOURCE_DIR}/rho/RhoBase
${CMAKE_SOURCE_DIR}/rho/RhoMath
${CMAKE_SOURCE_DIR}/rho/RhoTools
${CMAKE_SOURCE_DIR}/rho/RhoSelector
${CMAKE_SOURCE_DIR}/rho/RhoHistogram
${CMAKE_SOURCE_DIR}/rho/DecayTreeFitter
#${GEANT3_INCLUDE_DIR}
#${CMAKE_SOURCE_DIR}/geane
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
ErrCode.cxx
ChiSquare.cxx
Configuration.cxx
Constraint.cxx
DecayChain.cxx
LineTool.cxx
SortTool.cxx
FitParams.cxx
Fitter.cxx
#HelixUtils.cxx
InteractionPoint.cxx
InternalParticle.cxx
KalmanCalculator.cxx
MergedConstraint.cxx
MissingParticle.cxx
ParticleBase.cxx
Projection.cxx
RecoComposite.cxx
RecoParticle.cxx
RecoPhoton.cxx
RecoResonance.cxx
RecoTrack.cxx
RecoTrackStateProvider.cxx
Resonance.cxx
State.cxx
)

#CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")
#set(HEADERS ${HEADERS})

set(LINKDEF DecayTreeFitterLinkDef.h)
set(LIBRARY_NAME DecayTreeFitter)

set(DEPENDENCIES Base PndData Rho)

GENERATE_LIBRARY()
