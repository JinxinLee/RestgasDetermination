# Create a library called "libRecoHits" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${SIMPATH}/include
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/genfit2
${CMAKE_SOURCE_DIR}/genfit2/core/include
${CMAKE_SOURCE_DIR}/genfit2/measurements/include
${CMAKE_SOURCE_DIR}/genfit2/fitters/include
${CMAKE_SOURCE_DIR}/genfit2/finitePlanes/include
${CMAKE_SOURCE_DIR}/genfit2/trackReps/include
${CMAKE_SOURCE_DIR}/genfit2/fields/include
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/GemData
${CMAKE_SOURCE_DIR}/pnddata/MdtData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/FtsData
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/fts
${CMAKE_SOURCE_DIR}/PndTools/generalTools
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/GenfitTools/recohits2
${GEANT3_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


set(SRCS 
PndGemRecoHit2.cxx
PndMdtRecoHit2.cxx
PndSdsRecoHit2.cxx
PndSttRecoHit2.cxx
PndSttRecoHitProducer2.cxx
PndFtsRecoHit2.cxx
PndFtsRecoHitProducer2.cxx
PndHitRecoHit2.cxx
)

set(LINKDEF  RecoHitsLinkDef.h)
set(LIBRARY_NAME RecoHits2)

set(DEPENDENCIES Base ParBase PndData generalTools Stt Fts genfit2)

GENERATE_LIBRARY()
