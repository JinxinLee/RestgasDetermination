# Create a library called "KF"
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
)

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/pnddata/TrackData/
${CMAKE_SOURCE_DIR}/trackbase/
${CMAKE_SOURCE_DIR}/pnddata/
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/emc/
${CMAKE_SOURCE_DIR}/emc/EmcData/
${CMAKE_SOURCE_DIR}/analysis/kfparticle/
${CMAKE_SOURCE_DIR}/analysis/kfparticle/KFParticle/
${CMAKE_SOURCE_DIR}/analysis/kfparticle/KFParticlePerformance/
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})
   
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-pragmas -DDO_TPCCATRACKER_EFF_PERFORMANCE -DHomogeneousField -DUSE_TIMERS -DPANDA_STT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-pragmas -DDO_TPCCATRACKER_EFF_PERFORMANCE -DHomogeneousField -DUSE_TIMERS -DPANDA_STT")
   
set(SRCS
   KFParticle/KFParticleDatabase.cxx
   KFParticle/KFParticleBase.cxx
   KFParticle/KFParticle.cxx
   KFParticle/KFPTrack.cxx
   KFParticle/KFPVertex.cxx
   KFParticle/KFPTrackVector.cxx
   KFParticle/KFPEmcCluster.cxx
   KFParticle/KFParticleBaseSIMD.cxx
   KFParticle/KFParticleSIMD.cxx
   KFParticle/KFVertex.cxx 
   KFParticle/KFParticlePVReconstructor.cxx
   KFParticle/KFParticleFinder.cxx
   KFParticle/KFParticleTopoReconstructor.cxx
   KFParticlePerformance/KFMCVertex.cxx
   KFParticlePerformance/KFMCParticle.cxx
   KFParticlePerformance/KFParticlePerformanceBase.cxx
   KFParticlePerformance/KFTopoPerformance.cxx
   PndKFParticleFinder.cxx
   PndKFParticleFinderPID.cxx
   PndKFParticleFinderQA.cxx
   KFParticleMatch.cxx
)

set(HEADERS
   KFParticle/KFParticleBase.h
   KFParticle/KFParticle.h
   KFParticle/KFVertex.h
   KFParticlePerformance/KFPartEfficiencies.h
   KFParticlePerformance/KFPVEfficiencies.h
   KFParticlePerformance/KFPartMatch.h
   KFParticlePerformance/KFMCTrack.h
   KFParticlePerformance/KFMCVertex.h
   KFParticlePerformance/KFMCParticle.h
   KFParticlePerformance/KFParticlePerformanceBase.h
   KFParticlePerformance/KFTopoPerformance.h
   PndKFParticleFinder.h
   PndKFParticleFinderPID.h
   PndKFParticleFinderQA.h
   KFParticleMatch.h
)

set(LINKDEF  PndKFLinkDef.h)
set(LIBRARY_NAME KF)

set(DEPENDENCIES Vc.a)

GENERATE_LIBRARY()
