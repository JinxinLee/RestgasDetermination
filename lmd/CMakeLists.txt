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
${CMAKE_SOURCE_DIR}/lmd
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/RKTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/recohits
${CMAKE_SOURCE_DIR}/GenfitTools/adapters
${CMAKE_SOURCE_DIR}/GenfitTools
${CMAKE_SOURCE_DIR}/geane
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libLmd ######################################
set(SRCS

LmdMC/PndLmdDim.cxx
LmdTools/PndLmdContFact.cxx
LmdTools/PndLmdAlignPar.cxx
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

set(DEPENDENCIES Base ParBase GeoBase TrkBase Geane Hyp Sds PndData
genfitAdapters RecoHits generalTools trackrep Rho)

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

set(DEPENDENCIES Base ParBase GeoBase TrkBase Geane Hyp Sds PndData
genfitAdapters RecoHits generalTools Lmd)

GENERATE_LIBRARY()

########################### create libLmdTrk ##################################
set(SRCS

LmdTrack/PndLmdTrackFinderTask.cxx
LmdTrack/PndLmdTrackFinderCATask.cxx
LmdTrack/PndLmdLinFitTask.cxx
LmdTrack/PndLmdLineTask.cxx
LmdTrack/PndLmdIdealFitTask.cxx
LmdTrack/PndLinTrack.cxx
LmdTrack/PndLmdKalmanTask.cxx
LmdTrack/PndLmdGeaneTask.cxx
LmdTrack/PndLmdBPRungeKuttaTask.cxx	
LmdTrack/PndLmdTrksFilterTask.cxx
)

set(LINKDEF  LmdTrkLinkDef.h)
set(LIBRARY_NAME LmdTrk)

set(DEPENDENCIES Base ParBase GeoBase TrkBase Geane Hyp Sds PndData
genfitAdapters RecoHits generalTools Lmd)

GENERATE_LIBRARY()

########################### create libLmdTools ##################################
set(SRCS

LmdQA/PndLmdQATask.cxx
)

set(LINKDEF  LmdToolLinkDef.h)
set(LIBRARY_NAME LmdTool)

set(DEPENDENCIES Base ParBase GeoBase TrkBase Geane Hyp Sds PndData
genfitAdapters RecoHits generalTools Lmd)

GENERATE_LIBRARY()
 
########################### create libModel  ##################################
set(MODEL_INCLUDE_DIRECTORIES
${SIMPATH}/include
)

include_directories( ${MODEL_INCLUDE_DIRECTORIES})

set(LMDMODEL_SRCS

LmdFit/Model.cxx 
LmdFit/Model1D.cxx
LmdFit/IntegralStrategy1D.cxx
LmdFit/IntegralStrategyGSL1D.cxx
LmdFit/IntegralStrategySR1D.cxx
LmdFit/Model2D.cxx 
LmdFit/ModelParameterHandler.cxx 
LmdFit/ModelParSet.cxx 
LmdFit/ModelPar.cxx
LmdFit/ModelFitInterface.cxx
LmdFit/AdditionModel1D.cxx 
LmdFit/NumericConvolutionModel1D.cxx 
LmdFit/NumericConvolutionModel2D.cxx
LmdFit/ProductModel1D.cxx
LmdFit/Parametrization.cxx 
LmdFit/ParametrizationModel.cxx 
LmdFit/ParametrizationProxy.cxx 
LmdFit/GaussianModel1D.cxx 
LmdFit/DoubleGaussianModel1D.cxx 
LmdFit/ExponentialModel1D.cxx 
LmdFit/PolynomialModel1D.cxx
)

#set (LMD_RULECHECKER_SRCS ${LMD_RULECHECKER_SRCS} ${LMDMODEL_SRCS})
#
#if(RULE_CHECKER_FOUND)
#  CHECK_RULES("${LMD_RULECHECKER_SRCS}" "${MODEL_INCLUDE_DIRECTORIES}" LMD_RULES)
#endif(RULE_CHECKER_FOUND)
#
#
## fill list of header files from list of source files
## by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h LMDMODEL_HEADERS "${LMDMODEL_SRCS}")

add_library(Model SHARED ${LMDMODEL_SRCS})


include_directories( ${INCLUDE_DIRECTORIES})

########################### create libLmdFit ##################################
set(SRCS

LmdFit/PndLmdDataInterface.cxx 
LmdFit/PndLmdDataBase.cxx 
LmdFit/PndLmdAcceptance.cxx 
LmdFit/PndLmdData.cxx
LmdFit/PndLmdResolution.cxx 
LmdFit/PndLmdLumiFitOptions.cxx 
LmdFit/PndLmdLumiFitResult.cxx
LmdFit/PndLmdLumiHelper.cxx 
LmdFit/PndLmdDPMMTModel1D.cxx 
LmdFit/PndLmdDPMAngModel1D.cxx 
LmdFit/PndLmdDPMMTModel2D.cxx 
LmdFit/PndLmdDPMModelParametrization.cxx 
LmdFit/PndROOTModelFitter.cxx 
LmdFit/PndLmdModelFactory.cxx 
LmdFit/PndLmdSmearingGaussianModelParametrization1D.cxx
LmdFit/PndLmdROOTDataModel1D.cxx 
LmdFit/PndLmdSmearingDoubleGaussianModelParametrization1D.cxx 
LmdFit/PndLmdResultPlotter.cxx
)

set(HEADERS
LmdFit/PndLmdLumiFitResult.h
LmdFit/PndLmdLumiFitOptions.h 
LmdFit/PndLmdDataInterface.h
LmdFit/PndLmdDataBase.h
LmdFit/PndLmdData.h
LmdFit/PndLmdAcceptance.h
LmdFit/PndLmdLumiHelper.h 
LmdFit/PndLmdResultPlotter.h
)

set(LINKDEF  LmdFitLinkDef.h)
set(LIBRARY_NAME LmdFit)

set(DEPENDENCIES Base ParBase GeoBase TrkBase Geane Hyp Sds PndData
genfitAdapters RecoHits generalTools Lmd LmdTool Model)

GENERATE_LIBRARY()
