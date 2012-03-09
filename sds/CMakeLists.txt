# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/MvdData
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/PndTools/generalTools
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/PndTools/buffers
${CMAKE_SOURCE_DIR}/sds/SdsMC
${CMAKE_SOURCE_DIR}/sds/SdsTools
${CMAKE_SOURCE_DIR}/sds/SdsDigi
${CMAKE_SOURCE_DIR}/sds/SdsFEModel
${CMAKE_SOURCE_DIR}/sds
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libSds ######################################
set(SDS_SRCS

PndSdsTask.cxx

SdsDigi/PndSdsCalcFePixel.cxx
SdsDigi/PndSdsCalcPixel.cxx
SdsDigi/PndSdsCalcPixelDif.cxx
SdsDigi/PndSdsCalcStrip.cxx
#SdsDigi/PndSdsCalcStripDif.cxx
#MvdDigi/PndMvdDigiAna.cxx
SdsDigi/PndSdsHybridHitProducer.cxx
SdsDigi/PndSdsStripHitProducer.cxx
#SdsDigi/PndSdsStripHitProducerDif.cxx
SdsDigi/PndSdsHitProducerIdeal.cxx
#MvdDigi/PndMvdDigiTask.cxx
SdsDigi/PndSdsNoiseProducer.cxx

SdsDigi/PndSdsChargeConversion.cxx
SdsDigi/PndSdsIdealChargeConversion.cxx
SdsDigi/PndSdsTotChargeConversion.cxx
SdsDigi/PndSdsAdcChargeConversion.cxx
SdsDigi/PndSdsDigiPixelWriteoutBuffer.cxx

SdsDigi/PndSdsDigiPixelSorterTask.cxx
SdsDigi/PndSdsDigiPixelRingSorter.cxx

SdsDigi/PndSdsDigiStripWriteoutBuffer.cxx
SdsDigi/PndSdsDigiStripSorterTask.cxx
SdsDigi/PndSdsDigiStripRingSorter.cxx


SdsFEModel/PndSdsFE.cxx
SdsFEModel/PndSdsFESimple.cxx
SdsFEModel/PndSdsFEAmpModelSimple.cxx

SdsMC/PndSdsDetector.cxx
SdsMC/PndSdsGeo.cxx
SdsMC/PndSdsGeoPar.cxx
#SdsMC/PndSdsMaterialDet.cxx

#SdsTools/PndSdsContFact.cxx
SdsTools/PndSdsPixelDigiPar.cxx
SdsTools/PndSdsStripDigiPar.cxx
SdsTools/PndSdsTotDigiPar.cxx
#SdsTools/PndStringSeparator.cxx
#SdsTools/PndGeoHandling.cxx

)
set (SDS_RULECHECKER_SRCS ${SDS_SRCS})

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h SDS_HEADERS "${SDS_SRCS}")
# SET(MVD_SRCS ${MVD_SRCS} ${DIGI_SRCS})
# SET(MVD_HEADERS ${MVD_HEADERS} ${DIGI_HEADERS})

set(SDS_LINKDEF  SdsLinkDef.h)
set(SDS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/SdsDict.cxx)

ROOT_GENERATE_DICTIONARY("${SDS_HEADERS}" "${SDS_LINKDEF}" "${SDS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(SDS_SRCS ${SDS_SRCS} ${SDS_DICTIONARY})

add_library(Sds SHARED ${SDS_SRCS})
target_link_libraries(Sds ${ROOT_LIBRARIES})
set_target_properties(Sds PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

########################### create libSdsReco ##################################
set(SDSRECO_SRCS

#SdsReco/PndSdsRecoTask.cxx
#SdsReco/PndSdsClusterTask.cxx
SdsReco/PndSdsPixelClusterFinder.cxx
SdsReco/PndSdsSimplePixelClusterFinder.cxx
SdsReco/PndSdsIdealPixelClusterFinder.cxx
SdsReco/PndSdsPixelClusterTask.cxx
SdsReco/PndSdsIdealClusterTask.cxx
SdsReco/PndSdsPixelBackMapping.cxx
SdsReco/PndSdsChargeWeightedPixelMapping.cxx
SdsReco/PndSdsStripClusterer.cxx
SdsReco/PndSdsSimpleStripClusterFinder.cxx
SdsReco/PndSdsStripAdvClusterFinder.cxx
SdsReco/PndSdsStripClusterTask.cxx
SdsReco/PndSdsIdealRecoTask.cxx
SdsReco/PndSdsChargeWeightingAlgorithms.cxx
SdsReco/PndSdsTimeWalkCorr.cxx
SdsReco/PndSdsTimeWalkCorrSimple.cxx
SdsReco/PndSdsTimeWalkCorrTask.cxx

#MvdTools/PndMvdRadDamList.cxx
#MvdTools/PndMvdRadDamTask.cxx
#MvdTools/PndMvdAnaRadDam.cxx

)
set (SDS_RULECHECKER_SRCS ${SDS_RULECHECKER_SRCS} ${SDSRECO_SRCS})

#if(RULE_CHECKER_FOUND)
#  CHECK_RULES("${SDS_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" MVD_RULES)
#endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h SDSRECO_HEADERS "${SDSRECO_SRCS}")
set(SDSRECO_LINKDEF  SdsRecoLinkDef.h)
set(SDSRECO_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/SdsRecoDict.cxx)

ROOT_GENERATE_DICTIONARY("${SDSRECO_HEADERS}" "${SDSRECO_LINKDEF}" "${SDSRECO_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(SDSRECO_SRCS ${SDSRECO_SRCS} ${SDSRECO_DICTIONARY})

add_library(SdsReco SHARED ${SDSRECO_SRCS})
target_link_libraries(SdsReco ${ROOT_LIBRARIES})
set_target_properties(SdsReco PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

########################### create libMvdTrk ##################################
#set(MVDTRK_SRCS
#MvdData/PndSdsRecoHit.cxx

#MvdPid/PndMvdPidIdealTask.cxx
#MvdPid/PndMvdIdealPidAlgo.cxx
#MvdPid/PndMvdSimplePidAlgo.cxx
#MvdPid/PndMvdAdvancedPidAlgo.cxx

#MvdTracking/PndMvdIdealTrackFinderTask.cxx
#MvdTracking/PndMvdKalmanTask.cxx
#MvdTracking/PndMvdTrackFinderAnaTask.cxx

#MvdTracking/PndMvdRiemannTrackFinderTask.cxx
#MvdTracking/PndMvdRiemannTrackFinder.cxx
#MvdTracking/PndMvdRiemannVertexFinderTask.cxx

#MvdTracking/PndMvdTpcRiemannCorrelatorTask.cxx
#MvdTracking/PndMvdTPCRiemannTrackFinderTask.cxx
#MvdTracking/PndMvdTPCRiemannTrackFinderTaskCutPar.cxx
#MvdTracking/PndMvdTPCRiemannTrackFinderTaskEff.cxx

#MvdTracking/PndTpcClustPlusRTFTask.cxx
#MvdTracking/PndTpcClustPlusRTFTaskCutPar.cxx

#MvdTracking/PndMvdRiemannTrackFinderTaskCutPar.cxx
#MvdTracking/PndMvdRiemannTrackFinderTaskEff.cxx

#)
#set (MVD_RULECHECKER_SRCS ${MVD_RULECHECKER_SRCS} ${MVDTRK_SRCS})

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${SDS_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" SDS_RULES)
endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h MVDTRK_HEADERS "${MVDTRK_SRCS}")
#set(MVDTRK_LINKDEF  MvdTrkLinkDef.h)
#set(MVDTRK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/MvdTrkDict.cxx)

#ROOT_GENERATE_DICTIONARY("${MVDTRK_HEADERS}" "${MVDTRK_LINKDEF}" "${MVDTRK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
#SET(MVDTRK_SRCS ${MVDTRK_SRCS} ${MVDTRK_DICTIONARY})

#add_library(MvdTrk SHARED ${MVDTRK_SRCS})
#target_link_libraries(MvdTrk ${ROOT_LIBRARIES})
#set_target_properties(MvdTrk PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################################## install #####################################
install(TARGETS Sds DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS SdsReco DESTINATION ${CMAKE_BINARY_DIR}/lib)
#install(TARGETS MvdTrk DESTINATION ${CMAKE_BINARY_DIR}/lib)

