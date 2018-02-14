# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
  ${Boost_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/MvdData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/tracking/sttmvdtracking
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/PndTools/generalTools
  ${CMAKE_SOURCE_DIR}/PndTools/buffers
#  ${CMAKE_SOURCE_DIR}/PndMCMatchNewLinks
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMC
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdDigi
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdReco
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdTools
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdDAQ
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdPid
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdTracking
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdTracking/LinFit
  ${CMAKE_SOURCE_DIR}/detectors/mvd/transport
  ${CMAKE_SOURCE_DIR}/detectors/mvd
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsMC
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsTools
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsDigi
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsReco
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsFEModel
  ${CMAKE_SOURCE_DIR}/detectors/sds
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/geane
  ${CMAKE_SOURCE_DIR}/tracking/riemannfit
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
${Boost_LIBRARY_DIRS}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libMvd ######################################
set(SRCS


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

MvdDAQ/PndMvdSiHit.cxx
MvdDAQ/PndMvdTdcData.cxx
MvdDAQ/PndMvdQdcData.cxx
MvdDAQ/PndMvdTsEvent.cxx

MvdDAQ/PndMvdConvertApv.cxx
MvdDAQ/PndMvdBoxMap.cxx
MvdDAQ/PndMvdConvertApvTask.cxx
MvdDAQ/PndMvdCreateDefaultApvMap.cxx

#MvdOfflineTBAnalysis_Topix4/PndMvdReadInTBData.cxx
#MvdOfflineTBAnalysis_Topix4/PndMvdReadInToPix4TBData.cxx
#MvdOfflineTBAnalysis_Topix4/PndMvdReadInTBDataTask.cxx
#MvdOfflineTBAnalysis_Topix4/PndMapSorterTask.cxx
#MvdOfflineTBAnalysis_Topix4/PndMapSorter.cxx
#MvdOfflineTBAnalysis_Topix4/PndMapSorterTpl.cxx
#MvdOfflineTBAnalysis_Topix4/PndMvdTopix4RingSorterTask.cxx
#MvdOfflineTBAnalysis_Topix4/PndMvdTopix4RingSorter.cxx
#MvdOfflineTBAnalysis_Topix4/mrftools.cxx
#MvdOfflineTBAnalysis_Topix4/mrf_confitem.cxx
#MvdOfflineTBAnalysis_Topix4/mrfdata_8b.cxx
#MvdOfflineTBAnalysis_Topix4/PndTimeOffsetFinderTask.cxx
#MvdOfflineTBAnalysis_Topix4/PndTSCorrectorTask.cxx
#MvdOfflineTBAnalysis_Topix4/PndStraightLineTrackFinderTask.cxx
#MvdOfflineTBAnalysis_Topix4/PndXYResidualTask.cxx
#MvdOfflineTBAnalysis_Topix4/PndPosCorrectorTask.cxx
MvdOfflineTBAnalysis_Topix4/PndHammingDecoder.cxx
MvdOfflineTBAnalysis_Topix4/PndCRCCalculator.cxx
#MvdOfflineTBAnalysis_Topix4/PndTopix4.cxx
)

set(LINKDEF  MvdLinkDef.h)
set(LIBRARY_NAME Mvd)

set(DEPENDENCIES Base GeoBase ParBase PndData generalTools
Geane Sds TrkBase boost_serialization boost_thread MathCore MathMore
  # fairmq_logger
  boost_timer
  boost_system
  boost_filesystem
  boost_program_options
  boost_random
  boost_chrono)

GENERATE_LIBRARY()

########################### create libMvdReco ##################################
set(SRCS
MvdReco/PndMvdClusterTask.cxx
MvdReco/PndMvdPixelClusterTask.cxx
MvdReco/PndMvdIdealClusterTask.cxx
MvdReco/PndMvdStripClusterTask.cxx
MvdReco/PndMvdIdealRecoTask.cxx
MvdReco/PndMvdTimeWalkCorrTask.cxx

MvdOfflineTBAnalysis_Topix4/PndMvdToPix4ClusterTask.cxx


MvdReco/PndMvdChargeWeightedPixelMapping.cxx
MvdReco/PndMvdSimplePixelClusterFinder.cxx
MvdTools/PndMvdRadDamList.cxx
MvdTools/PndMvdRadDamTask.cxx
MvdTools/PndMvdAnaRadDam.cxx
MvdTools/PndMvdRadDamIonizingTask.cxx

)

set(LINKDEF  MvdRecoLinkDef.h)
set(LIBRARY_NAME MvdReco)

set(DEPENDENCIES Base GeoBase ParBase PndData generalTools 
Geane Sds SdsReco TrkBase MathCore MathMore)

GENERATE_LIBRARY()

########################### create libMvdTrk ##################################
set(SRCS

MvdPid/PndMvdPidIdealTask.cxx
MvdPid/PndMvdIdealPidAlgo.cxx
MvdPid/PndMvdSimplePidAlgo.cxx
MvdPid/PndMvdAdvancedPidAlgo.cxx

MvdTracking/PndMvdIdealTrackFinderTask.cxx
MvdTracking/PndMvdRiemannTrackFinderTask.cxx
MvdTracking/PndMvdSttGemRiemannTrackFinderTask.cxx
MvdTracking/PndMvdRiemannVertexFinderTask.cxx
MvdTracking/PndMvdRiemannTrackFinderTaskCutPar.cxx
MvdTracking/PndMvdRiemannTrackFinderTaskEff.cxx

MvdTracking/LinFit/TtFitRes.cxx
MvdTracking/LinFit/TtAliTask.cxx
MvdTracking/LinFit/TtCracowTask.cxx

)


set(LINKDEF  MvdTrkLinkDef.h)
set(LIBRARY_NAME MvdTrk)

set(DEPENDENCIES Base GeoBase ParBase PndData generalTools
riemann Geane Sds TrkBase MathCore MathMore)

GENERATE_LIBRARY()
