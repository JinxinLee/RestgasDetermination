# Create a library called "generalTools" which includes the source files given in
# the array .
# The header extension is already found. Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/PndTools/generalTools
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(GENERALTOOLS_SRCS
PndFileNameCreator.cxx
PndStringSeparator.cxx
PndGeoHandling.cxx
PndSensorNameContFact.cxx
PndSensorNamePar.cxx
PndBlackBoxTask.cxx
PndEventCounterTask.cxx
PndTrackArrayMerger.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h GENERALTOOLS_HEADERS "${GENERALTOOLS_SRCS}")

set(GENERALTOOLS_LINKDEF  generalToolsLinkDef.h)
set(GENERALTOOLS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/generalToolsDict.cxx) 

ROOT_GENERATE_DICTIONARY("${GENERALTOOLS_HEADERS}" "${GENERALTOOLS_LINKDEF}" "${GENERALTOOLS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(GENERALTOOLS_SRCS ${GENERALTOOLS_SRCS} ${GENERALTOOLS_DICTIONARY})

add_library(generalTools SHARED ${GENERALTOOLS_SRCS})
target_link_libraries(generalTools ${ROOT_LIBRARIES})
set_target_properties(generalTools PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS generalTools DESTINATION ${CMAKE_BINARY_DIR}/lib)
