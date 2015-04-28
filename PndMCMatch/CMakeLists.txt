# Create a library called "libMCMatch" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
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
  ${CMAKE_SOURCE_DIR}/stt
  ${CMAKE_SOURCE_DIR}/PndMCMatch
  ${CMAKE_SOURCE_DIR}/PndMCMatch/examples
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
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
PndMCMatchPrintFairLinksObjects.cxx
)

set(LINKDEF  PndMCMatchLinkDef.h)
set(LIBRARY_NAME MCMatch)

set(DEPENDENCIES Base riemann Emc ParBase PndData)

GENERATE_LIBRARY()

########################### create libMCMatchExamples ##################################
set(SRCS
examples/PndMCTestMomentumCompare.cxx
examples/PndMCTestHitCompare.cxx
examples/PndMCIdealTrackFinder.cxx
examples/PndMCTestEmcClusterCompare.cxx
examples/PndMCTestPatternRecoQuality.cxx
examples/PndMCTestDataCrawler.cxx
examples/PndMCTimebasedPatternRecoQuality.cxx
)

set(LINKDEF  PndMCMatchExamplesLinkDef.h)
set(LIBRARY_NAME MCMatchExamples)

set(DEPENDENCIES Base MCMatch)

GENERATE_LIBRARY()
