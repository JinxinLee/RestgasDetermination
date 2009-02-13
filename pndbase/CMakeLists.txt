# Create a library called "libPndBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/pndbase 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${CMAKE_SOURCE_DIR}/base
)
 
link_directories( ${LINK_DIRECTORIES})

set(PNDBASE_SRCS
PndStack.cxx 
PndMCTrack.cxx
DskData/PndDskCerenkov.cxx

DrcData/PndDrcPDPoint.cxx 
DrcData/PndDrcBarPoint.cxx
DrcData/PndDrcHit.cxx

GemData/PndGemHit.cxx
GemData/PndGemMCPoint.cxx

MdtData/PndMdtPoint.cxx
MdtData/PndMdtHit.cxx
MdtData/PndMdtTrk.cxx

SttData/PndSttGeomPoint.cxx
SttData/PndSttHelixHit.cxx
SttData/PndSttHit.cxx  
SttData/PndSttHitInfo.cxx  
SttData/PndSttPoint.cxx  
SttData/PndSttTrack.cxx

TofData/PndTofHit.cxx
TofData/PndTofPoint.cxx


RpcData/PndRpcPoint.cxx
RpcData/PndRpcHit.cxx


HypData/PndHypCluster.cxx
HypData/PndHypDigi.cxx
HypData/PndHypDigiStrip.cxx
HypData/PndHypHit.cxx
HypData/PndHypHitInfo.cxx
HypData/PndHypPidCand.cxx
HypData/PndHypPoint.cxx

MvdData/PndMvdCluster.cxx
MvdData/PndMvdDigi.cxx
MvdData/PndMvdDigiPixel.cxx
MvdData/PndMvdDigiStrip.cxx
MvdData/PndMvdHit.cxx
MvdData/PndMvdHitInfo.cxx
MvdData/PndMvdMCPoint.cxx
MvdData/PndMvdPidCand.cxx
MvdData/PndMvdRadDamHit.cxx

)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${PNDBASE_SRCS}" "${INCLUDE_DIRECTORIES}" PNDBASE_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h PNDBASE_HEADERS "${PNDBASE_SRCS}")
SET( PNDBASE_HEADERS ${PNDBASE_HEADERS} HypData/PndHypStrip.h PndDetectorList.h )
SET( PNDBASE_HEADERS ${PNDBASE_HEADERS} MvdData/PndMvdStrip.h MvdData/PndMvdApvHit.h MvdData/PndMvdPixel.h)

set(PNDBASE_LINKDEF  PndBaseLinkDef.h)
set(PNDBASE_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndBaseDict.cxx) 

ROOT_GENERATE_DICTIONARY("${PNDBASE_HEADERS}" "${PNDBASE_LINKDEF}" "${PNDBASE_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(PNDBASE_SRCS ${PNDBASE_SRCS} ${PNDBASE_DICTIONARY})

add_library(PndBase SHARED ${PNDBASE_SRCS})
target_link_libraries(PndBase ${ROOT_LIBRARIES})
set_target_properties(PndBase PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS PndBase DESTINATION ${CMAKE_BINARY_DIR}/lib)
