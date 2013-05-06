# Create a library called "libgenfitAdapter" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/RKTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/geane
${GEANT3_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


set(GFADAPTER_SRCS 
PndGenfitAdapters.cxx
PndGenfitField.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${GFADAPTER_SRCS}" "${INCLUDE_DIRECTORIES}" GFADAPTER_RULES)
endif(RULE_CHECKER_FOUND)

add_library(genfitAdapters SHARED ${GFADAPTER_SRCS})
target_link_libraries(genfitAdapters ${ROOT_LIBRARIES})
set_target_properties(genfitAdapters PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS genfitAdapters DESTINATION ${CMAKE_BINARY_DIR}/lib)
