# Create a library called "libMCMatch" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/pndbase 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/GemData
${CMAKE_SOURCE_DIR}/pnddata/FtsData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/pndbase/PndStdUtils
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackrep
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/emc/EmcData
${CMAKE_SOURCE_DIR}/emc/EmcReco
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/PndMCMatch
${CMAKE_SOURCE_DIR}/PndMCMatch/examples
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(MCMATCH_SRCS
PndMCMatch.cxx
PndMCMatchCreatorTask.cxx
PndMCMatchLoaderTask.cxx
PndMCMatchSelectorTask.cxx
PndMCObject.cxx
PndMCStage.cxx
PndMCResult.cxx
PndMCEntry.cxx
PndMCList.cxx
PndMCTrackAssociator.cxx
PndMCTrackEnumAssociator.cxx
PndMCDataCrawler.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h MCMATCH_HEADERS "${MCMATCH_SRCS}")

set(MCMATCH_LINKDEF  PndMCMatchLinkDef.h)
set(MCMATCH_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndMCMatchDict.cxx) 

ROOT_GENERATE_DICTIONARY("${MCMATCH_HEADERS}" "${MCMATCH_LINKDEF}" "${MCMATCH_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(MCMATCH_SRCS ${MCMATCH_SRCS} ${MCMATCH_DICTIONARY})

add_library(MCMatch SHARED ${MCMATCH_SRCS})
target_link_libraries(MCMatch ${ROOT_LIBRARIES})
set_target_properties(MCMatch PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS MCMatch DESTINATION ${CMAKE_BINARY_DIR}/lib)

########################### create libMCMatchExamples ##################################

set(MCMATCHEXAMPLES_SRCS
examples/PndMCTestMomentumCompare.cxx
examples/PndMCTestHitCompare.cxx
examples/PndMCIdealTrackFinder.cxx
examples/PndMCTestEmcClusterCompare.cxx
examples/PndMCTestPatternRecoQuality.cxx
examples/PndMCTestDataCrawler.cxx
examples/PndMCTimebasedPatternRecoQuality.cxx
)

CHANGE_FILE_EXTENSION(*.cxx *.h MCMATCHEXAMPLES_HEADERS "${MCMATCHEXAMPLES_SRCS}")

set(MCMATCHEXAMPLES_LINKDEF  PndMCMatchExamplesLinkDef.h)
set(MCMATCHEXAMPLES_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndMCMatchExamplesDict.cxx) 

ROOT_GENERATE_DICTIONARY("${MCMATCHEXAMPLES_HEADERS}" "${MCMATCHEXAMPLES_LINKDEF}" "${MCMATCHEXAMPLES_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(MCMATCHEXAMPLES_SRCS ${MCMATCHEXAMPLES_SRCS} ${MCMATCHEXAMPLES_DICTIONARY})

add_library(MCMatchExamples SHARED ${MCMATCHEXAMPLES_SRCS})
target_link_libraries(MCMatchExamples ${ROOT_LIBRARIES})
set_target_properties(MCMatchExamples PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS MCMatchExamples DESTINATION ${CMAKE_BINARY_DIR}/lib)
