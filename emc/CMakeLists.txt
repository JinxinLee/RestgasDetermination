# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/emc/EmcData
${CMAKE_SOURCE_DIR}/emc/EmcDigi
${CMAKE_SOURCE_DIR}/emc/EmcMC
${CMAKE_SOURCE_DIR}/emc/EmcReco
${CMAKE_SOURCE_DIR}/emc/EmcTools
${CMAKE_SOURCE_DIR}/emc/EmcProtoTools
${CMAKE_SOURCE_DIR}/emc/EmcCorr
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
EmcData/PndEmcMultiWaveform.cxx 
EmcData/PndEmcDigi.cxx 
EmcData/PndEmcCluster.cxx 
EmcData/PndEmcSharedDigi.cxx
EmcData/PndEmcBump.cxx
EmcData/PndEmcHeader.cxx
EmcData/PndEmcRecoHit.cxx
EmcData/PndEmcClusterProperties.cxx
EmcData/PndEmcAbsClusterProperty.cxx
EmcData/PndEmcClusterDistances.cxx
EmcData/PndEmcClusterEnergySums.cxx
EmcData/PndEmcClusterMoments.cxx
EmcData/PndEmcXClMoments.cxx
EmcDigi/PndEmcHitProducer.cxx
EmcDigi/PndEmcApdHitProducer.cxx
EmcDigi/PndEmcCRRCPulseshape.cxx 
EmcDigi/PndEmcCR2RCPulseshape.cxx
EmcDigi/PndEmcExpPulseshape.cxx
EmcDigi/PndEmcAsicPulseshape.cxx 
EmcDigi/PndEmcHitsToWaveform.cxx
#EmcDigi/PndEmcHitsToTmpWaveform.cxx 
EmcDigi/PndEmcWaveformToDigi.cxx
EmcDigi/PndEmcWaveformToCalibratedDigi.cxx
EmcDigi/PndEmcMultiWaveformToCalibratedDigi.cxx
#EmcDigi/PndEmcTmpWaveformToDigi.cxx 
EmcDigi/PndEmcDigiPar.cxx 
EmcDigi/PndEmcDigiNonuniformityPar.cxx 
EmcDigi/PndEmcMakeDigi.cxx
EmcDigi/PndEmcAbsPSA.cxx
EmcDigi/PndEmcPSAParabolic.cxx
EmcDigi/PndEmcPSAParabolicBaseline.cxx
EmcDigi/PndEmcPSATrapDigiFilter.cxx
EmcDigi/PndEmcPSAMatchedDigiFilter.cxx
EmcDigi/PndEmcFadcFilter.cxx
EmcDigi/PndEmcFullDigiTask.cxx
EmcDigi/PndEmcDigiWriteoutBuffer.cxx
EmcDigi/PndEmcDigiRingSorter.cxx
EmcDigi/PndEmcDigiSorterTask.cxx
EmcMC/PndGeoEmc.cxx 
EmcMC/PndEmc.cxx 
EmcMC/PndEmcReader.cxx 
EmcMC/PndGeoEmcApd.cxx 
EmcMC/PndEmcApd.cxx
EmcMC/PndEmcGeoPar.cxx
EmcTools/PndEmcTwoCoordIndex.cxx 
EmcTools/PndEmcStructure.cxx 
EmcTools/PndEmcMapper.cxx 
EmcTools/PndEmcXtal.cxx 
EmcTools/PndEmcHdrFiller.cxx
EmcProtoTools/ReadMainzProto60.cxx
EmcProtoTools/ReadMainzProto60v4.cxx
EmcProtoTools/ReadMainzProto60v6.cxx
EmcProtoTools/PndEmcReadProtoData.cxx
EmcProtoTools/PndEmcReadProto192Data.cxx
EmcProtoTools/TProtoUnpackEvent.cxx
EmcProtoTools/TGo4EventElement.cxx
EmcReco/PndEmcMakeCluster.cxx 
EmcReco/PndEmcRecoPar.cxx
EmcReco/PndEmcMakeBump.cxx
EmcReco/PndEmc2DLocMaxFinder.cxx
EmcReco/PndEmcExpClusterSplitter.cxx
EmcReco/PndEmcMakeRecoHit.cxx
EmcReco/PndEmcErrorMatrix.cxx
EmcReco/PndEmcErrorMatrixPar.cxx
EmcCorr/PndEmcMakeCorr.cxx
EmcCorr/PndEmcCorrection.cxx
EmcCorr/PndEmcClusterCalibrator.cxx
EmcCorr/PndEmcClusterCalibrationPar.cxx
#basetasks/PndGlobalEvtTask.cxx
PndEmcContFact.cxx 
EmcDigi/PndEmcPSAFPGA/PndEmcPSAFPGADigitalFilterAnalyser.cxx
EmcDigi/PndEmcPSAFPGA/PndEmcPSAFPGAFilterCF.cxx
EmcDigi/PndEmcPSAFPGA/PndEmcPSAFPGAFilterDelay.cxx
EmcDigi/PndEmcPSAFPGA/PndEmcPSAFPGAFilterLine.cxx
EmcDigi/PndEmcPSAFPGA/PndEmcPSAFPGAFilterMA.cxx
EmcDigi/PndEmcPSAFPGA/PndEmcPSAFPGAFilterMWD.cxx
EmcDigi/PndEmcPSAFPGA/PndEmcPSAFPGAIntegratingAnalyser.cxx
EmcDigi/PndEmcPSAFPGA/PndEmcPSAFPGALinFitter.cxx
EmcDigi/PndEmcPSAFPGA/PndEmcPSAFPGAMLinFitter.cxx
EmcDigi/PndEmcPSAFPGA/PndEmcPSAFPGASampleAnalyser.cxx
EmcDigi/PndEmcAbsCrystalCalibrator.cxx
EmcDigi/PndEmcDummyCrystalCalibrator.cxx
EmcDigi/PndEmcFileCrystalCalibrator.cxx
EmcDigi/PndEmcSimCrystalCalibrator.cxx
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
