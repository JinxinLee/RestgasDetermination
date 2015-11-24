# Create a library called "ParticleFitters"
Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${BASE_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/rho
  ${CMAKE_SOURCE_DIR}/rho/RhoBase
  ${CMAKE_SOURCE_DIR}/rho/RhoTools
  ${CMAKE_SOURCE_DIR}/rho/RhoSelector
  ${CMAKE_SOURCE_DIR}/rho/RhoMath
  ${CMAKE_SOURCE_DIR}/rho/RhoFitter
  ${CMAKE_SOURCE_DIR}/rho/RhoHistogram
  ${CMAKE_SOURCE_DIR}/rho/DecayTreeFitter
  ${CMAKE_SOURCE_DIR}/PndTools/PndParticleFitters
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
Pnd4CFitter.cxx
PndDecayTreeFitter.cxx
PndKinVtxFitter.cxx
PndKinFitter.cxx
PndVtxPoca.cxx
PndVtxPRG.cxx
PndKalmanVtxFitter.cxx
)

set(LINKDEF PndParticleFittersLinkDef.h)
set(LIBRARY_NAME ParticleFitters)

set(DEPENDENCIES Base PndData Rho RhoFitter TrkBase Geane DecayTreeFitter)

GENERATE_LIBRARY()
