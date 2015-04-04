# Create a library called "libRecoTasks" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${SIMPATH}/include
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/RKTrackRep
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/fts
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/GemData
${CMAKE_SOURCE_DIR}/pnddata/MdtData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/FtsData
${CMAKE_SOURCE_DIR}/PndTools/generalTools
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/GenfitTools/adapters
${CMAKE_SOURCE_DIR}/GenfitTools/recohits
${CMAKE_SOURCE_DIR}/GenfitTools/recotasks
${GEANT3_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS 
PndRecoKalmanFit.cxx
PndRecoDafFit.cxx
PndRecoKalmanTask.cxx
PndRecoMultiKalmanTask.cxx
)

set(LINKDEF  RecoTasksLinkDef.h)
set(LIBRARY_NAME RecoTasks)

set(DEPENDENCIES Base ParBase PndData generalTools Stt Fts TrkBase
Geane RecoHits genfitAdapters)

GENERATE_LIBRARY()
