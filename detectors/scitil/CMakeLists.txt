# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${GEANT3_INCLUDE_DIR}
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/PndTools/generalTools
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData
  ${CMAKE_SOURCE_DIR}/detectors/scitil
  ${CMAKE_SOURCE_DIR}/detectors/scitil/SciTMC
  ${CMAKE_SOURCE_DIR}/detectors/scitil/SciTDigi
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
SciTMC/PndGeoSciT.cxx 
SciTMC/PndSciT.cxx 
SciTMC/PndGeoSciTPar.cxx 
PndSciTContFact.cxx
SciTDigi/PndSciTHitProducerIdeal.cxx
SciTDigi/PndSciTDigiTask.cxx
SciTDigi/PndSciTHitWriteoutBuffer.cxx
SciTDigi/PndSciTHitSorterTask.cxx
SciTDigi/PndSciTHitRingSorter.cxx
#PndSciTAnaIdeal.cxx
#PndSciTMassTrig.cxx
)

set(LINKDEF  SciTLinkDef.h)
set(LIBRARY_NAME SciT)

set(DEPENDENCIES Base GeoBase ParBase PndData AnalysisTools generalTools)

GENERATE_LIBRARY()
