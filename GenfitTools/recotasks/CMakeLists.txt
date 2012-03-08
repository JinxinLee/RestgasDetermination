# Create a library called "libRecoTasks" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/stt/sttreco
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
)

link_directories( ${LINK_DIRECTORIES})


set(RECOTASKS_SRCS 
PndRecoKalmanFit.cxx
PndRecoDafFit.cxx
PndRecoKalmanTask.cxx
PndRecoMultiKalmanTask.cxx
PrimSelector.cxx
PndGFTrackToPndTrackConvertorTask.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${RECOTASKS_SRCS}" "${INCLUDE_DIRECTORIES}" RECOTASKS_RULES)
endif(RULE_CHECKER_FOUND)

CHANGE_FILE_EXTENSION(*.cxx *.h RECOTASKS_HEADERS "${RECOTASKS_SRCS}")

# fill list of header files from list of source files
# by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h RECOTASKS_HEADERS "${RECOTASKS_SRCS}")


set(RECOTASKS_LINKDEF  RecoTasksLinkDef.h)
set(RECOTASKS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/RecoTasksDict.cxx)


ROOT_GENERATE_DICTIONARY("${RECOTASKS_HEADERS}" "${RECOTASKS_LINKDEF}" "${RECOTASKS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(RECOTASKS_SRCS ${RECOTASKS_SRCS} ${RECOTASKS_DICTIONARY})

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOLDVMC")


add_library(RecoTasks SHARED ${RECOTASKS_SRCS})
target_link_libraries(RecoTasks ${ROOT_LIBRARIES})
set_target_properties(RecoTasks PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS RecoTasks DESTINATION ${CMAKE_BINARY_DIR}/lib)
