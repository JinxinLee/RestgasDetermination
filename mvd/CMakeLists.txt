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
${CMAKE_SOURCE_DIR}/PndMCMatch
${CMAKE_SOURCE_DIR}/mvd/MvdMC
${CMAKE_SOURCE_DIR}/mvd/MvdDigi
${CMAKE_SOURCE_DIR}/mvd/MvdReco
${CMAKE_SOURCE_DIR}/mvd/MvdTools
${CMAKE_SOURCE_DIR}/mvd/MvdDAQ
${CMAKE_SOURCE_DIR}/mvd/MvdPid
${CMAKE_SOURCE_DIR}/mvd/MvdTracking
${CMAKE_SOURCE_DIR}/mvd/MvdTracking/LinFit
${CMAKE_SOURCE_DIR}/mvd/transport
${CMAKE_SOURCE_DIR}/mvd
${CMAKE_SOURCE_DIR}/sds/SdsMC
${CMAKE_SOURCE_DIR}/sds/SdsTools
${CMAKE_SOURCE_DIR}/sds/SdsDigi
${CMAKE_SOURCE_DIR}/sds/SdsReco
${CMAKE_SOURCE_DIR}/sds/SdsFEModel
${CMAKE_SOURCE_DIR}/sds
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/recohits
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libMvd ######################################
set(MVD_SRCS


MvdTools/PndMvdContFact.cxx
MvdTools/PndMvdAccessRTDBTask.cxx
#MvdTools/PndMvdEventAna.cxx
#MvdTools/PndMvdMCEventAna.cxx
#MvdTools/PndMvdDigiEventAna.cxx
#MvdTools/PndMvdAllDataEventAna.cxx
MvdTools/PndGeoHitList.cxx
MvdTools/PndEventDisplay.cxx
#MvdTools/PndMvdEventAnaTask.cxx
#MvdTools/PndMvdEventMerger.cxx
#MvdTools/PndMvdEventMergerTask.cxx
MvdTools/PndMvdMSAnaTask.cxx

MvdMC/PndMvdDetector.cxx

#MvdDigi/PndMvdDigiAna.cxx
MvdDigi/PndMvdHybridHitProducer.cxx
MvdDigi/PndMvdStripHitProducer.cxx
MvdDigi/PndMvdHitProducerIdeal.cxx

MvdDigi/PndMvdDigiTask.cxx
MvdDigi/PndMvdNoiseProducer.cxx
#MvdDigi/PndMvdPixelDigiSorterTask.cxx

MvdDAQ/SiHit.cxx
MvdDAQ/TdcData.cxx
MvdDAQ/QdcData.cxx
MvdDAQ/TsEvent.cxx

MvdDAQ/PndMvdConvertApv.cxx
MvdDAQ/PndMvdBoxMap.cxx
MvdDAQ/PndMvdConvertApvTask.cxx
MvdDAQ/PndMvdCreateDefaultApvMap.cxx
)
set (MVD_RULECHECKER_SRCS ${MVD_SRCS})

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h MVD_HEADERS "${MVD_SRCS}")
# SET(MVD_SRCS ${MVD_SRCS} ${DIGI_SRCS})
# SET(MVD_HEADERS ${MVD_HEADERS} ${DIGI_HEADERS})

set(MVD_LINKDEF  MvdLinkDef.h)
set(MVD_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/MvdDict.cxx)

ROOT_GENERATE_DICTIONARY("${MVD_HEADERS}" "${MVD_LINKDEF}" "${MVD_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(MVD_SRCS ${MVD_SRCS} ${MVD_DICTIONARY})

add_library(Mvd SHARED ${MVD_SRCS})
target_link_libraries(Mvd ${ROOT_LIBRARIES})
set_target_properties(Mvd PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

########################### create libMvdReco ##################################
set(MVDRECO_SRCS
MvdReco/PndMvdClusterTask.cxx
MvdReco/PndMvdPixelClusterTask.cxx
MvdReco/PndMvdIdealClusterTask.cxx
MvdReco/PndMvdStripClusterTask.cxx
MvdReco/PndMvdIdealRecoTask.cxx
MvdReco/PndMvdTimeWalkCorrTask.cxx


MvdReco/PndMvdChargeWeightedPixelMapping.cxx
MvdReco/PndMvdSimplePixelClusterFinder.cxx
MvdTools/PndMvdRadDamList.cxx
MvdTools/PndMvdRadDamTask.cxx
MvdTools/PndMvdAnaRadDam.cxx
MvdTools/PndMvdRadDamIonizingTask.cxx

)
set (MVD_RULECHECKER_SRCS ${MVD_RULECHECKER_SRCS} ${MVDRECO_SRCS})

#if(RULE_CHECKER_FOUND)
#  CHECK_RULES("${MVD_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" MVD_RULES)
#endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h MVDRECO_HEADERS "${MVDRECO_SRCS}")
set(MVDRECO_LINKDEF  MvdRecoLinkDef.h)
set(MVDRECO_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/MvdRecoDict.cxx)

ROOT_GENERATE_DICTIONARY("${MVDRECO_HEADERS}" "${MVDRECO_LINKDEF}" "${MVDRECO_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(MVDRECO_SRCS ${MVDRECO_SRCS} ${MVDRECO_DICTIONARY})

add_library(MvdReco SHARED ${MVDRECO_SRCS})
target_link_libraries(MvdReco ${ROOT_LIBRARIES})
set_target_properties(MvdReco PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

########################### create libMvdTrk ##################################
set(MVDTRK_SRCS

MvdPid/PndMvdPidIdealTask.cxx
MvdPid/PndMvdIdealPidAlgo.cxx
MvdPid/PndMvdSimplePidAlgo.cxx
MvdPid/PndMvdAdvancedPidAlgo.cxx

MvdTracking/PndMvdIdealTrackFinderTask.cxx
MvdTracking/PndMvdKalmanTask.cxx
MvdTracking/PndMvdTrackFinderAnaTask.cxx

MvdTracking/PndMvdRiemannTrackFinderTask.cxx
MvdTracking/PndMvdRiemannTrackFinder.cxx
MvdTracking/PndMvdRiemannVertexFinderTask.cxx

#MvdTracking/PndMvdTpcRiemannCorrelatorTask.cxx
#MvdTracking/PndMvdTPCRiemannTrackFinderTask.cxx
#MvdTracking/PndMvdTPCRiemannTrackFinderTaskCutPar.cxx
#MvdTracking/PndMvdTPCRiemannTrackFinderTaskEff.cxx

#MvdTracking/PndTpcClustPlusRTFTask.cxx
#MvdTracking/PndTpcClustPlusRTFTaskCutPar.cxx

MvdTracking/PndMvdRiemannTrackFinderTaskCutPar.cxx
MvdTracking/PndMvdRiemannTrackFinderTaskEff.cxx

MvdTracking/LinFit/TtFitRes.cxx
MvdTracking/LinFit/TtAliTask.cxx
MvdTracking/LinFit/TtCracowTask.cxx

)
Set (MVD_RULECHECKER_SRCS ${MVD_RULECHECKER_SRCS} ${MVDTRK_SRCS})

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${MVD_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" MVD_RULES)
endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h MVDTRK_HEADERS "${MVDTRK_SRCS}")
set(MVDTRK_LINKDEF  MvdTrkLinkDef.h)
set(MVDTRK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/MvdTrkDict.cxx)

ROOT_GENERATE_DICTIONARY("${MVDTRK_HEADERS}" "${MVDTRK_LINKDEF}" "${MVDTRK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(MVDTRK_SRCS ${MVDTRK_SRCS} ${MVDTRK_DICTIONARY})

add_library(MvdTrk SHARED ${MVDTRK_SRCS})
target_link_libraries(MvdTrk ${ROOT_LIBRARIES})
set_target_properties(MvdTrk PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################################## install #####################################
install(TARGETS Mvd DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS MvdReco DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS MvdTrk DESTINATION ${CMAKE_BINARY_DIR}/lib)

