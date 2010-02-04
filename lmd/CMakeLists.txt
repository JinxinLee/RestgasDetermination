# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/sds
${CMAKE_SOURCE_DIR}/sds/SdsMC
${CMAKE_SOURCE_DIR}/sds/SdsDigi
${CMAKE_SOURCE_DIR}/sds/SdsReco
${CMAKE_SOURCE_DIR}/sds/SdsTools
${CMAKE_SOURCE_DIR}/lmd/LmdMC
${CMAKE_SOURCE_DIR}/lmd
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


########################### create libLmd ######################################
set(LMD_SRCS

#MvdDigi/PndMvdCalcFePixel.cxx
#MvdDigi/PndMvdCalcPixel.cxx
#MvdDigi/PndMvdCalcStrip.cxx
#MvdDigi/PndMvdDigiAna.cxx
#MvdDigi/PndMvdHybridHitProducer.cxx
#MvdDigi/PndMvdStripHitProducer.cxx
#MvdDigi/PndMvdHitProducerIdeal.cxx
#MvdDigi/PndMvdDigiTask.cxx
#MvdDigi/PndMvdNoiseProducer.cxx

LmdMC/PndLmdDetector.cxx

#MvdTools/PndMvdContFact.cxx
#MvdTools/PndMvdPixelDigiPar.cxx
#MvdTools/PndMvdStripDigiPar.cxx
#MvdTools/PndMvdAccessRTDBTask.cxx
#MvdTools/PndStringVector.cxx
#MvdTools/PndMvdGeoHandling.cxx
#MvdTools/PndMvdEventAna.cxx
#MvdTools/PndMvdMCEventAna.cxx
#MvdTools/PndMvdDigiEventAna.cxx
#MvdTools/PndMvdAllDataEventAna.cxx
#MvdTools/PndMvdFileNameCreator.cxx
#MvdTools/PndGeoHitList.cxx
#MvdTools/PndEventDisplay.cxx
#MvdTools/PndMvdEventAnaTask.cxx
#MvdTools/PndMvdEventMerger.cxx
#MvdTools/PndMvdEventMergerTask.cxx
#MvdTools/PndMvdMSAnaTask.cxx

#MvdDAQ/PndMvdConvertApv.cxx
#MvdDAQ/PndMvdConvertApvTask.cxx

)
set (LMD_RULECHECKER_SRCS ${LMD_SRCS})

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h LMD_HEADERS "${LMD_SRCS}")
# SET(MVD_SRCS ${MVD_SRCS} ${DIGI_SRCS})
# SET(MVD_HEADERS ${MVD_HEADERS} ${DIGI_HEADERS})

set(LMD_LINKDEF  LmdLinkDef.h)
set(LMD_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/LmdDict.cxx)

ROOT_GENERATE_DICTIONARY("${LMD_HEADERS}" "${LMD_LINKDEF}" "${LMD_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(LMD_SRCS ${LMD_SRCS} ${LMD_DICTIONARY})

add_library(Lmd SHARED ${LMD_SRCS})
target_link_libraries(Lmd ${ROOT_LIBRARIES})
set_target_properties(Lmd PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

########################### create libMvdReco ##################################
#set(MVDRECO_SRCS


##MvdReco/PndMvdRecoTask.cxx
#MvdReco/PndMvdClusterTask.cxx
##MvdReco/PndMvdPixelClusterFinder.cxx
##MvdReco/PndMvdSimplePixelClusterFinder.cxx
##MvdReco/PndMvdIdealPixelClusterFinder.cxx
#MvdReco/PndMvdPixelClusterTask.cxx
#MvdReco/PndMvdIdealClusterTask.cxx
##MvdReco/PndMvdPixelBackMapping.cxx
##MvdReco/PndMvdChargeWeightedPixelMapping.cxx
##MvdReco/PndMvdStripClusterBuilder.cxx
##MvdReco/PndMvdSimpleStripClusterFinder.cxx
##MvdReco/PndMvdStripClusterFinder.cxx
#MvdReco/PndMvdStripClusterTask.cxx
#MvdReco/PndMvdIdealRecoTask.cxx
##MvdReco/PndMvdChargeWeightingAlgorithms.cxx

#MvdTools/PndMvdRadDamList.cxx
#MvdTools/PndMvdRadDamTask.cxx
#MvdTools/PndMvdAnaRadDam.cxx

#)
#set (MVD_RULECHECKER_SRCS ${MVD_RULECHECKER_SRCS} ${MVDRECO_SRCS})

##if(RULE_CHECKER_FOUND)
##  CHECK_RULES("${MVD_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" MVD_RULES)
##endif(RULE_CHECKER_FOUND)


## fill list of header files from list of source files
## by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h MVDRECO_HEADERS "${MVDRECO_SRCS}")
#set(MVDRECO_LINKDEF  MvdRecoLinkDef.h)
#set(MVDRECO_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/MvdRecoDict.cxx)

#ROOT_GENERATE_DICTIONARY("${MVDRECO_HEADERS}" "${MVDRECO_LINKDEF}" "${MVDRECO_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
#SET(MVDRECO_SRCS ${MVDRECO_SRCS} ${MVDRECO_DICTIONARY})

#add_library(MvdReco SHARED ${MVDRECO_SRCS})
#target_link_libraries(MvdReco ${ROOT_LIBRARIES})
#set_target_properties(MvdReco PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

########################### create libMvdTrk ##################################
#set(MVDTRK_SRCS
#MvdData/PndMvdRecoHit.cxx

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

#if(RULE_CHECKER_FOUND)
#  CHECK_RULES("${MVD_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" MVD_RULES)
#endif(RULE_CHECKER_FOUND)


## fill list of header files from list of source files
## by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h MVDTRK_HEADERS "${MVDTRK_SRCS}")
#set(MVDTRK_LINKDEF  MvdTrkLinkDef.h)
#set(MVDTRK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/MvdTrkDict.cxx)

#ROOT_GENERATE_DICTIONARY("${MVDTRK_HEADERS}" "${MVDTRK_LINKDEF}" "${MVDTRK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
#SET(MVDTRK_SRCS ${MVDTRK_SRCS} ${MVDTRK_DICTIONARY})

#add_library(MvdTrk SHARED ${MVDTRK_SRCS})
#target_link_libraries(MvdTrk ${ROOT_LIBRARIES})
#set_target_properties(MvdTrk PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################################## install #####################################
install(TARGETS Lmd DESTINATION ${CMAKE_BINARY_DIR}/lib)
#install(TARGETS MvdReco DESTINATION ${CMAKE_BINARY_DIR}/lib)
#install(TARGETS MvdTrk DESTINATION ${CMAKE_BINARY_DIR}/lib)

