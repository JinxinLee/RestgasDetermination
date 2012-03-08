# Create a library called "libRecoHits" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/RKTrackRep
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/genfit
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
${CMAKE_SOURCE_DIR}/GenfitTools/recohits
${GEANT3_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


set(RECOHITS_SRCS 
PndGemRecoHit.cxx
PndMdtRecoHit.cxx
PndSdsRecoHit.cxx
PndSttRecoHit.cxx
PndSttRecoHitProducer.cxx
PndFtsRecoHit.cxx
PndFtsRecoHitProducer.cxx
PndHitRecoHit.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${RECOHITS_SRCS}" "${INCLUDE_DIRECTORIES}" RECOHITS_RULES)
endif(RULE_CHECKER_FOUND)

CHANGE_FILE_EXTENSION(*.cxx *.h RECOHITS_HEADERS "${RECOHITS_SRCS}")

# fill list of header files from list of source files
# by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h RECOHITS_HEADERS "${RECOHITS_SRCS}")


set(RECOHITS_LINKDEF  RecoHitsLinkDef.h)
set(RECOHITS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/RecoHitsDict.cxx)


ROOT_GENERATE_DICTIONARY("${RECOHITS_HEADERS}" "${RECOHITS_LINKDEF}" "${RECOHITS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(RECOHITS_SRCS ${RECOHITS_SRCS} ${RECOHITS_DICTIONARY})

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOLDVMC")


add_library(RecoHits SHARED ${RECOHITS_SRCS})
target_link_libraries(RecoHits ${ROOT_LIBRARIES})
set_target_properties(RecoHits PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS RecoHits DESTINATION ${CMAKE_BINARY_DIR}/lib)
