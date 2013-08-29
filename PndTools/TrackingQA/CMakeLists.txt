# Create a library called "TrackingQA" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/trackrep
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/PndMCMatch 
${CMAKE_SOURCE_DIR}/PndTools/TrackingQA
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${GLPK_LIB_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS 
PndTrackingQualityData.cxx
PndTrackingQualityAnalysis.cxx
PndTrackingQualityTask.cxx
)

set(LINKDEF  TrackingQualityAnalysis_LinkDef.h)
set(LIBRARY_NAME TrackingQA)

set(DEPENDENCIES Base PndData MCMatch)

GENERATE_LIBRARY()
