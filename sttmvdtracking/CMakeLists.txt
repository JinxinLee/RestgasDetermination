# Create a library called "libSttMvdTracking" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

#set(GLPK_INCLUDE_DIR ${SIMPATH}/basics/glpk/include)

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GLPK_INCLUDE_DIR}
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
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/sttmvdtracking
${CMAKE_SOURCE_DIR}/mvd/MvdTracking
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/SciTData
${CMAKE_SOURCE_DIR}/gem 
${CMAKE_SOURCE_DIR}/pnddata/GemData 
${CMAKE_SOURCE_DIR}/geane 
)

include_directories( ${INCLUDE_DIRECTORIES})

#set(GLPK_LIB_DIR ${SIMPATH}/basics/glpk/lib)


set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${GLPK_LIB_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(STTMVDTRACKING_SRCS
PndSttMvdTracking.cxx
PndSttMvdGemTracking.cxx
PndSttMvdGemTrackingIdeal.cxx
#PndSttMvdV0PR.cxx
PndMixBackgroundEvents.cxx
PndSecondaryTrackFinder.cxx
PndLambdaIM.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${STTMVDTRACKING_SRCS}" "${INCLUDE_DIRECTORIES}" STTMVDTRACKING_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h STTMVDTRACKING_HEADERS "${STTMVDTRACKING_SRCS}")

set(STTMVDTRACKING_LINKDEF  SttMvdTrackingLinkDef.h)
set(STTMVDTRACKING_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndSttMvdTrackingDict.cxx) 

ROOT_GENERATE_DICTIONARY("${STTMVDTRACKING_HEADERS}" "${STTMVDTRACKING_LINKDEF}" "${STTMVDTRACKING_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(STTMVDTRACKING_SRCS ${STTMVDTRACKING_SRCS} ${STTMVDTRACKING_DICTIONARY})

add_library(SttMvdTracking SHARED ${STTMVDTRACKING_SRCS})
target_link_libraries(SttMvdTracking ${ROOT_LIBRARIES}  ${GLPK_LIB_DIR}/libglpk.so)
set_target_properties(SttMvdTracking PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS SttMvdTracking DESTINATION ${CMAKE_BINARY_DIR}/lib)
