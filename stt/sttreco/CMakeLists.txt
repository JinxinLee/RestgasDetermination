# Create a library called "libStt" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/recohits
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/stt/sttreco
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(STT_SRCS
#PndSttPatternRecoTask.cxx
#PndSttPatternRecoTask2.cxx
#PndSttKalmanTask.cxx
#PndSttKalmanTask2.cxx
#WirepointHitPolicy.cxx
)
if(RULE_CHECKER_FOUND)
  CHECK_RULES("${STT_SRCS}" "${INCLUDE_DIRECTORIES}" STTRECO_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h STT_HEADERS "${STT_SRCS}")

set(STT_LINKDEF  SttRecoLinkDef.h)
set(STT_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndSttRecoDict.cxx) 

ROOT_GENERATE_DICTIONARY("${STT_HEADERS}" "${STT_LINKDEF}" "${STT_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(STT_SRCS ${STT_SRCS} ${STT_DICTIONARY})

add_library(SttReco SHARED ${STT_SRCS})
target_link_libraries(SttReco ${ROOT_LIBRARIES})
set_target_properties(SttReco PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS SttReco DESTINATION ${CMAKE_BINARY_DIR}/lib)
