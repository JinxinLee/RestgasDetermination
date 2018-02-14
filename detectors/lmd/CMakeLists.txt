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
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/passive
  ${CMAKE_SOURCE_DIR}/PndTools/
  ${CMAKE_SOURCE_DIR}/PndTools/buffers
  ${CMAKE_SOURCE_DIR}/tracking/riemannfit
  ${CMAKE_SOURCE_DIR}/PndTools/generalTools
  ${CMAKE_SOURCE_DIR}/sds/SdsMC
  ${CMAKE_SOURCE_DIR}/sds/SdsDigi
  ${CMAKE_SOURCE_DIR}/sds/SdsReco
  ${CMAKE_SOURCE_DIR}/sds/SdsTools
  ${CMAKE_SOURCE_DIR}/sds/SdsFEModel
  ${CMAKE_SOURCE_DIR}/sds
  ${CMAKE_SOURCE_DIR}/lmd/LmdMC
  ${CMAKE_SOURCE_DIR}/lmd/LmdDigi
  ${CMAKE_SOURCE_DIR}/lmd/LmdSensorAlignment
  ${CMAKE_SOURCE_DIR}/lmd/LmdSensorAlignment/icp
  ${CMAKE_SOURCE_DIR}/lmd/LmdSensorAlignment/SensorAligner
  ${CMAKE_SOURCE_DIR}/lmd/LmdReco
  ${CMAKE_SOURCE_DIR}/lmd/LmdTrack
  ${CMAKE_SOURCE_DIR}/lmd/LmdTools
  ${CMAKE_SOURCE_DIR}/lmd/LmdQA
  ${CMAKE_SOURCE_DIR}/lmd
#  ${CMAKE_SOURCE_DIR}/mvd/MvdDigi # to do: get rid of that dependency!
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/geane
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


########################### create libLmd ######################################
set(SRCS

LmdTools/PndLmdContFact.cxx
LmdTools/PndLmdAlignPar.cxx
LmdDigi/PndLmdPixelHitProducerFast.cxx
LmdDigi/PndLmdHybridHitProducer.cxx
LmdDigi/PndLmdStripHitProducer.cxx
LmdDigi/PndLmdHitProducerIdeal.cxx
LmdDigi/PndLmdDigiTask.cxx 
#LmdDigi/PndLmdNoiseProducer.cxx
LmdMC/PndLmdDetector.cxx
LmdMC/PndLmdGeo.cxx
LmdMC/PndLmdGeometryFactory.cxx
LmdMC/PndLmdGeometryHelper.cxx
)

set(LINKDEF  LmdLinkDef.h)
set(LIBRARY_NAME Lmd)

set(DEPENDENCIES Base ParBase GeoBase TrkBase Sds PndData) # to do: Get rid of the Mvd dependency which is only due to the noise producer. It is not derived from SDS!

GENERATE_LIBRARY()

########################### create libLmdReco ##################################
set(SRCS

LmdReco/PndLmdStripClusterTask.cxx
LmdReco/PndLmdPixelClusterTask.cxx
LmdReco/PndLmdHitMergeTask.cxx
LmdReco/PndLmdSimplePixelClusterFinder.cxx
LmdReco/PndLmdChargeWeightedPixelMapping.cxx
LmdReco/PndSdsMergedHit.cxx
LmdReco/PndSdsCell.cxx
#LmdReco/PndLmdIdealRecoTask.cxx
)

set(LINKDEF  LmdRecoLinkDef.h)
set(LIBRARY_NAME LmdReco)

set(DEPENDENCIES SdsReco Lmd)

GENERATE_LIBRARY()

########################### create PndLmdPairFinderTask ######################################
#set(SRCS
#LmdSensorAlignment/LmdPairFinderTask.cxx
#LmdSensorAlignment/PndLmdHitPair.cxx
#)
#
#set(HEADERS
#LmdSensorAlignment/LmdPairFinderTask.h
#LmdSensorAlignment/PndLmdHitPair.h
#)
#
#set(LINKDEF  LmdPairFinderLinkDef.h)
#set(LIBRARY_NAME LmdPairFinder)
#
#set(DEPENDENCIES Base Sds Lmd boost_system boost_filesystem)#
#
#GENERATE_LIBRARY()
#
########################### create PndLmdSensorAligner ######################################
set(SRCS
LmdSensorAlignment/PndLmdAlignManager.cxx
LmdSensorAlignment/PndLmdSensorAligner.cxx
#LmdSensorAlignment/PndLmdAlignQA.cxx
LmdSensorAlignment/LmdPairFinderTask.cxx
LmdSensorAlignment/PndLmdHitPair.cxx
)

set(HEADERS
LmdSensorAlignment/PndLmdAlignManager.h
LmdSensorAlignment/PndLmdSensorAligner.h
#LmdSensorAlignment/PndLmdAlignQA.h 
LmdSensorAlignment/LmdPairFinderTask.h
LmdSensorAlignment/PndLmdHitPair.h
)

set(LINKDEF  LmdSensorAlignerLinkDef.h)
set(LIBRARY_NAME LmdSensorAligner)

set(DEPENDENCIES Base Sds Lmd boost_system boost_thread boost_regex boost_filesystem LmdSensorAlignerTools)

GENERATE_LIBRARY()

########################### create PndLmdSensorAlignerTools ######################################

set(SRCS
LmdSensorAlignment/icp/matrix.cpp
LmdSensorAlignment/icp/kdtree.cpp
LmdSensorAlignment/icp/icpPointToPoint.cpp
LmdSensorAlignment/icp/icp.cpp
#LmdSensorAlignment/RunningStats.cpp
)

set(HEADERS
LmdSensorAlignment/icp/matrix.h
LmdSensorAlignment/icp/kdtree.h
LmdSensorAlignment/icp/icpPointToPoint.h
LmdSensorAlignment/icp/icp.h
#LmdSensorAlignment/RunningStats.h
LmdSensorAlignment/PndLmdAlignStructs.h
)

set(LIBRARY_NAME LmdSensorAlignerTools)

set(DEPENDENCIES boost_system boost_filesystem)

GENERATE_LIBRARY()

########################### create libLmdTrk ##################################
set(SRCS

LmdTrack/PndLmdTrackFinderTask.cxx
LmdTrack/PndLmdTrackFinderCATask.cxx
LmdTrack/PndLmdLinFitTask.cxx
LmdTrack/PndLmdLineTask.cxx
LmdTrack/PndLmdIdealFitTask.cxx
LmdTrack/PndLinTrack.cxx
#LmdTrack/PndLmdKalmanTask.cxx
LmdTrack/PndLmdGeaneTask.cxx
LmdTrack/PndLmdBPtestTask.cxx
#LmdTrack/PndLmdBPRungeKuttaTask.cxx	
LmdTrack/PndLmdTrksFilterTask.cxx
LmdTrack/PndLmdSigCleanTask.cxx
LmdTrack/PndLmdNoiseTrkSuppressionTask.cxx
)

set(LINKDEF  LmdTrkLinkDef.h)
set(LIBRARY_NAME LmdTrk)

#set(DEPENDENCIES genfitAdapters RecoHits TrkBase Geane LmdReco TMVA)
set(DEPENDENCIES TrkBase Geane LmdReco TMVA)

GENERATE_LIBRARY()

########################### create libLmdTools ##################################
set(SRCS

LmdQA/PndLmdTrackQ.cxx
LmdQA/PndLmdTrkQTask.cxx
LmdQA/PndLmdDigiQ.cxx
LmdQA/PndLmdDigiQTask.cxx
LmdQA/PndLmdQATask.cxx
)

set(LINKDEF  LmdToolLinkDef.h)
set(LIBRARY_NAME LmdTool)

set(DEPENDENCIES Sds LmdTrk)

GENERATE_LIBRARY()
