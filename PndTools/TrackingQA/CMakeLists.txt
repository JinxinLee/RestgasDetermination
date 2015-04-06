# Create a library called "TrackingQA" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/trackrep
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/PndMCMatch 
  ${CMAKE_SOURCE_DIR}/PndTools/TrackingQA
  ${CMAKE_SOURCE_DIR}/rho
  ${CMAKE_SOURCE_DIR}/rho/RhoBase
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS 
PndTrackingQualityData.cxx
PndTrackingQualityAnalysis.cxx
PndTrackingQualityTask.cxx
)

set(LINKDEF  TrackingQualityAnalysis_LinkDef.h)
set(LIBRARY_NAME TrackingQA)

set(DEPENDENCIES Base PndData MCMatch Rho)

GENERATE_LIBRARY()
