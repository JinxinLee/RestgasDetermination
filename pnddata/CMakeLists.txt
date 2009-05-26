# Create a library called "libPNDDATA" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/trackbase
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${CMAKE_SOURCE_DIR}/base
)
 
link_directories( ${LINK_DIRECTORIES})

set(PNDDATA_SRCS
PndStack.cxx 
PndMCTrack.cxx

DskData/PndDskCerenkov.cxx
DskData/PndDskParticle.cxx
DskData/PndDskTrackPoint.cxx

DrcData/PndDrcPDPoint.cxx 
DrcData/PndDrcBarPoint.cxx
DrcData/PndDrcHit.cxx

GemData/PndGemMCPoint.cxx
GemData/PndGemDigi.cxx
GemData/PndGemHit.cxx

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

TrackData/PndTrackCand.cxx
TrackData/PndTrack.cxx

)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${PNDDATA_SRCS}" "${INCLUDE_DIRECTORIES}" PNDDATA_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h PNDDATA_HEADERS "${PNDDATA_SRCS}")
SET( PNDDATA_HEADERS ${PNDDATA_HEADERS} HypData/PndHypStrip.h PndDetectorList.h )
SET( PNDDATA_HEADERS ${PNDDATA_HEADERS} MvdData/PndMvdStrip.h MvdData/PndMvdApvHit.h MvdData/PndMvdPixel.h)

set(PNDDATA_LINKDEF  PndDataLinkDef.h)
set(PNDDATA_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndDataDict.cxx) 

ROOT_GENERATE_DICTIONARY("${PNDDATA_HEADERS}" "${PNDDATA_LINKDEF}" "${PNDDATA_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(PNDDATA_SRCS ${PNDDATA_SRCS} ${PNDDATA_DICTIONARY})

add_library(PndData SHARED ${PNDDATA_SRCS})
target_link_libraries(PndData ${ROOT_LIBRARIES})
set_target_properties(PndData PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS PndData DESTINATION ${CMAKE_BINARY_DIR}/lib)
