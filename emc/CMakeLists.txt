# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/mcstack 
${CMAKE_SOURCE_DIR}/emc/EmcData
${CMAKE_SOURCE_DIR}/emc/EmcDigi
${CMAKE_SOURCE_DIR}/emc/EmcMC
${CMAKE_SOURCE_DIR}/emc/EmcReco
${CMAKE_SOURCE_DIR}/emc/EmcTools
${CMAKE_SOURCE_DIR}/emc

)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(EMC_SRCS
EmcData/PndEmcPoint.cxx
EmcData/PndEmcApdPoint.cxx
EmcData/PndEmcHit.cxx  
EmcData/PndEmcApdHit.cxx
EmcData/PndEmcWaveform.cxx 
EmcData/PndEmcDigi.cxx 
EmcData/PndEmcCluster.cxx 
EmcData/PndEmcSharedDigi.cxx
EmcData/PndEmcBump.cxx
EmcData/PndEmcHeader.cxx
EmcData/PndEmcRecoHit.cxx
EmcData/PndEmcAbsClusterProperty.cxx
EmcData/PndEmcClusterDistances.cxx
EmcData/PndEmcClusterEnergySums.cxx
EmcData/PndEmcClusterMoments.cxx
EmcData/PndEmcXClMoments.cxx
EmcDigi/PndEmcHitProducer.cxx
EmcDigi/PndEmcApdHitProducer.cxx
EmcDigi/PndEmcCRRCPulseshape.cxx 
EmcDigi/PndEmcCR2RCPulseshape.cxx 
EmcDigi/PndEmcHitsToWaveform.cxx 
EmcDigi/PndEmcWaveformToDigi.cxx 
EmcDigi/PndEmcDigiPar.cxx 
EmcDigi/PndEmcMakeDigi.cxx
EmcMC/PndGeoEmc.cxx 
EmcMC/PndEmc.cxx 
EmcMC/PndEmcReader.cxx 
EmcMC/PndGeoEmcApd.cxx 
EmcMC/PndEmcApd.cxx
EmcTools/PndEmcTwoCoordIndex.cxx 
EmcTools/PndEmcStructure.cxx 
EmcTools/PndEmcMapper.cxx 
EmcTools/PndEmcXtal.cxx 
EmcTools/PndEmcHdrFiller.cxx
EmcReco/PndEmcMakeCluster.cxx 
EmcReco/PndEmcClusterLiloPos.cxx
EmcReco/PndEmcClusterLinearPos.cxx  
EmcReco/PndEmcRecoPar.cxx
EmcReco/PndEmcMakeBump.cxx
EmcReco/PndEmc2DLocMaxFinder.cxx
EmcReco/PndEmcExpClusterSplitter.cxx
EmcReco/PndEmcMakeRecoHit.cxx
PndEmcContFact.cxx 
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${EMC_SRCS}" "${INCLUDE_DIRECTORIES}" EMC_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h EMC_HEADERS "${EMC_SRCS}")

set(EMC_LINKDEF  EmcLinkDef.h)
set(EMC_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndEmcDict.cxx) 

ROOT_GENERATE_DICTIONARY("${EMC_HEADERS}" "${EMC_LINKDEF}" "${EMC_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(EMC_SRCS ${EMC_SRCS} ${EMC_DICTIONARY})

add_library(Emc SHARED ${EMC_SRCS})
target_link_libraries(Emc ${ROOT_LIBRARIES})
set_target_properties(Emc PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Emc DESTINATION ${CMAKE_BINARY_DIR}/lib)
