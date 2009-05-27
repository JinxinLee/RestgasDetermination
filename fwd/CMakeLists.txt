# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/GemData
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/dch/DchData
${CMAKE_SOURCE_DIR}/dch/DchTracking
${CMAKE_SOURCE_DIR}/dch/DchFitting
${CMAKE_SOURCE_DIR}/dch
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/trackrep
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/fwd
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libFwd ######################################
set(FWD_SRCS
PndFwdPrepareKalmanTracks.cxx
PndFwdKalmanTask.cxx
)
set (FWD_RULECHECKER_SRCS ${FWD_SRCS})

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h FWD_HEADERS "${FWD_SRCS}")
# SET(FWD_SRCS ${FWD_SRCS} ${DIGI_SRCS})
# SET(FWD_HEADERS ${FWD_HEADERS} ${DIGI_HEADERS})

set(FWD_LINKDEF  FwdLinkDef.h)
set(FWD_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/FwdDict.cxx)

ROOT_GENERATE_DICTIONARY("${FWD_HEADERS}" "${FWD_LINKDEF}" "${FWD_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(FWD_SRCS ${FWD_SRCS} ${FWD_DICTIONARY})

add_library(Fwd SHARED ${FWD_SRCS})
target_link_libraries(Fwd ${ROOT_LIBRARIES})
set_target_properties(Fwd PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################################## install #####################################
install(TARGETS Fwd DESTINATION ${CMAKE_BINARY_DIR}/lib)
# install(TARGETS FwdReco DESTINATION ${CMAKE_BINARY_DIR}/lib)
# install(TARGETS FwdTrk DESTINATION ${CMAKE_BINARY_DIR}/lib)


