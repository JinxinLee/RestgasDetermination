# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${SIMPATH}/include
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/PndTools/
${CMAKE_SOURCE_DIR}/PndTools/buffers
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/PndTools/generalTools
${CMAKE_SOURCE_DIR}/sds/SdsMC
${CMAKE_SOURCE_DIR}/sds/SdsDigi
${CMAKE_SOURCE_DIR}/sds/SdsReco
${CMAKE_SOURCE_DIR}/sds/SdsTools
${CMAKE_SOURCE_DIR}/sds/SdsFEModel
${CMAKE_SOURCE_DIR}/sds
${CMAKE_SOURCE_DIR}/lmd/LmdMC
${CMAKE_SOURCE_DIR}/lmd/LmdDigi
${CMAKE_SOURCE_DIR}/lmd/LmdReco
${CMAKE_SOURCE_DIR}/lmd/LmdTrack
${CMAKE_SOURCE_DIR}/lmd/LmdTools
${CMAKE_SOURCE_DIR}/lmd/LmdQA
${CMAKE_SOURCE_DIR}/lmd/LmdFit/LumiImpl
${CMAKE_SOURCE_DIR}/lmd/LmdFit/ModelFramework
${CMAKE_SOURCE_DIR}/lmd
#${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
#${CMAKE_SOURCE_DIR}/recotasks
#${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
#${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
#${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/RKTrackRep
#${CMAKE_SOURCE_DIR}/GenfitTools/recohits
#${CMAKE_SOURCE_DIR}/GenfitTools/adapters
#${CMAKE_SOURCE_DIR}/GenfitTools
${CMAKE_SOURCE_DIR}/geane
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
)

link_directories( ${LINK_DIRECTORIES})


########################### create libLmd ######################################
set(SRCS

LmdTools/PndLmdContFact.cxx
LmdTools/PndLmdAlignPar.cxx
LmdMC/PndLmdDim.cxx
LmdDigi/PndLmdPixelHitProducerFast.cxx
LmdDigi/PndLmdHybridHitProducer.cxx
LmdDigi/PndLmdStripHitProducer.cxx
LmdDigi/PndLmdHitProducerIdeal.cxx
LmdDigi/PndLmdDigiTask.cxx 
LmdDigi/PndLmdNoiseProducer.cxx
LmdMC/PndLmdDetector.cxx
LmdMC/PndLmdGeo.cxx

)

set(LINKDEF  LmdLinkDef.h)
set(LIBRARY_NAME Lmd)

set(DEPENDENCIES Base ParBase GeoBase TrkBase Sds PndData)

GENERATE_LIBRARY()

########################### create libLmdReco ##################################
set(SRCS

LmdReco/PndLmdStripClusterTask.cxx
LmdReco/PndLmdPixelClusterTask.cxx
LmdReco/PndLmdHitMergeTask.cxx
LmdReco/PndLmdSimplePixelClusterFinder.cxx
LmdReco/PndLmdChargeWeightedPixelMapping.cxx
LmdReco/PndSdsMergedHit.cxx
#LmdReco/PndLmdIdealRecoTask.cxx
#LmdMC/PndLmdDim.cxx
)

set(LINKDEF  LmdRecoLinkDef.h)
set(LIBRARY_NAME LmdReco)

set(DEPENDENCIES SdsReco Lmd)

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


########################### create libModel  ##################################
# modeling framework (completely independent of ROOT)

include_directories(/usr/X11R6/include/)
include_directories(/opt/X11/include/)
link_directories(/usr/X11R6/lib)
SET(EXTRA_LIBS GL X11 GLU gsl gslcblas)

FILE(GLOB_RECURSE MODEL_SRCS LmdFit/ModelFramework/*.cxx)

add_definitions(${CXX11_COMPILER_FLAGS})
add_library(Model SHARED ${MODEL_SRCS})

TARGET_LINK_LIBRARIES(Model ${EXTRA_LIBS} boost_filesystem boost_system)


########################### create libLmdFit ##################################

FILE(GLOB_RECURSE LMDFIT_SRCS  LmdFit/LumiImpl/*.cxx)

set(SRCS ${LMDFIT_SRCS})

set(HEADERS
LmdFit/ModelFramework/fit/ModelFitResult.h
LmdFit/ModelFramework/fit/EstimatorOptions.h
LmdFit/LumiImpl/fit/PndLmdLumiFitResult.h
LmdFit/LumiImpl/fit/PndLmdLumiFitOptions.h 
LmdFit/LumiImpl/data/PndLmdFitStorage.h
LmdFit/LumiImpl/data/PndLmdAbstractData.h
LmdFit/LumiImpl/data/PndLmdHistogramData.h
LmdFit/LumiImpl/data/PndLmdAngularData.h
LmdFit/LumiImpl/data/PndLmdVertexData.h
LmdFit/LumiImpl/data/PndLmdAcceptance.h
LmdFit/LumiImpl/PndLmdLumiHelper.h 
LmdFit/LumiImpl/PndLmdPlotter.h
LmdFit/LumiImpl/ROOTPlotHelper.hpp
LmdFit/LumiImpl/LumiFitStructs.h
LmdFit/LumiImpl/LumiModelOptions.h
)

set(LINKDEF  LmdFitLinkDef.h)
set(LIBRARY_NAME LmdFit)

set(DEPENDENCIES MathMore PndData LmdTool Model boost_filesystem boost_regex)

GENERATE_LIBRARY()
