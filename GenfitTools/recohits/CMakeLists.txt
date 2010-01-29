# Create a library called "libRecoHits" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep2
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/RKtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase

${CMAKE_SOURCE_DIR}/pnddata/MdtData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/geane
${GEANT3_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


set(RECOHITS_SRCS 
PndMdtRecoHit.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${RECOHITS_SRCS}" "${INCLUDE_DIRECTORIES}" RECOHITS_RULES)
endif(RULE_CHECKER_FOUND)

add_library(RecoHits SHARED ${RECOHITS_SRCS})
target_link_libraries(RecoHits ${ROOT_LIBRARIES})
set_target_properties(RecoHits PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS RecoHits DESTINATION ${CMAKE_BINARY_DIR}/lib)
