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
set(LMD_SRCS

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

########################### create libLmdReco ##################################
set(LMDRECO_SRCS

LmdReco/PndLmdStripClusterTask.cxx
LmdReco/PndLmdPixelClusterTask.cxx
LmdReco/PndLmdHitMergeTask.cxx
LmdReco/PndLmdSimplePixelClusterFinder.cxx
LmdReco/PndLmdChargeWeightedPixelMapping.cxx
LmdReco/PndSdsMergedHit.cxx
#LmdReco/PndLmdIdealRecoTask.cxx
#LmdMC/PndLmdDim.cxx
)
set (LMD_RULECHECKER_SRCS ${LMD_RULECHECKER_SRCS} ${LMDRECO_SRCS})

#if(RULE_CHECKER_FOUND)
#  CHECK_RULES("${MVD_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" MVD_RULES)
#endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h LMDRECO_HEADERS "${LMDRECO_SRCS}")
set(LMDRECO_LINKDEF  LmdRecoLinkDef.h)
set(LMDRECO_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/LmdRecoDict.cxx)

ROOT_GENERATE_DICTIONARY("${LMDRECO_HEADERS}" "${LMDRECO_LINKDEF}" "${LMDRECO_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(LMDRECO_SRCS ${LMDRECO_SRCS} ${LMDRECO_DICTIONARY})

add_library(LmdReco SHARED ${LMDRECO_SRCS})
target_link_libraries(LmdReco ${ROOT_LIBRARIES})
set_target_properties(LmdReco PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

########################### create libLmdTrk ##################################
set(LMDTRK_SRCS

LmdTrack/PndLmdTrackFinderTask.cxx
LmdTrack/PndLmdTrackFinderCATask.cxx
#LmdTrack/PndLmdMCPointTrackFinderTask.cxx
LmdTrack/PndLmdLinFitTask.cxx
LmdTrack/PndLmdLineTask.cxx
#LmdTrack/PndLmdMCPointLinFitTask.cxx
LmdTrack/PndLmdIdealFitTask.cxx
LmdTrack/PndLinTrack.cxx
#LmdTrack/PndLmdTrackCandToGFTrackCandConvertorTask.cxx
LmdTrack/PndLmdKalmanTask.cxx
LmdTrack/PndLmdGeaneTask.cxx
#LmdTrack/PndLmdBPRungeKuttaTask.cxx	
)
set (LMD_RULECHECKER_SRCS ${LMD_RULECHECKER_SRCS} ${LMDTRK_SRCS})

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LMD_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" LMD_RULES)
endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h LMDTRK_HEADERS "${LMDTRK_SRCS}")
set(LMDTRK_LINKDEF  LmdTrkLinkDef.h)
set(LMDTRK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/LmdTrkDict.cxx)

ROOT_GENERATE_DICTIONARY("${LMDTRK_HEADERS}" "${LMDTRK_LINKDEF}" "${LMDTRK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(LMDTRK_SRCS ${LMDTRK_SRCS} ${LMDTRK_DICTIONARY})

add_library(LmdTrk SHARED ${LMDTRK_SRCS})
target_link_libraries(LmdTrk ${ROOT_LIBRARIES})
set_target_properties(LmdTrk PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


########################### create libLmdTools ##################################
set(LMDTOOLS_SRCS

LmdQA/PndLmdQATask.cxx
)
set (LMD_RULECHECKER_SRCS ${LMD_RULECHECKER_SRCS} ${LMDTOOLS_SRCS})

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LMD_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" LMD_RULES)
endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h LMDTOOLS_HEADERS "${LMDTOOLS_SRCS}")
set(LMDTOOLS_LINKDEF  LmdToolLinkDef.h)
set(LMDTOOLS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/LmdToolDict.cxx)

ROOT_GENERATE_DICTIONARY("${LMDTOOLS_HEADERS}" "${LMDTOOLS_LINKDEF}" "${LMDTOOLS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(LMDTOOLS_SRCS ${LMDTOOLS_SRCS} ${LMDTOOLS_DICTIONARY})

add_library(LmdTool SHARED ${LMDTOOLS_SRCS})
target_link_libraries(LmdTool ${ROOT_LIBRARIES})
set_target_properties(LmdTool PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

# ################################ create libLmdTesting ##########################
# set(LMDTEST_SRCS
# LmdTrack/PndLmdPerformanceTask.cxx
# )

# set (LMD_RULECHECKER_SRCS ${LMD_RULECHECKER_SRCS} ${LMDTEST_SRCS})

# if(RULE_CHECKER_FOUND)
#   CHECK_RULES("${LMD_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}"
# LMD_RULES)
# endif(RULE_CHECKER_FOUND)

# # fill list of header files from list of source files
# # by exchanging the file extension
# CHANGE_FILE_EXTENSION(*.cxx *.h LMDTEST_HEADERS "${LMDTEST_SRCS}")
# set(LMDTEST_LINKDEF  LmdTestsLinkDef.h)
# set(LMDTEST_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/LmdTestsDict.cxx)

# ROOT_GENERATE_DICTIONARY("${LMDTEST_HEADERS}" "${LMDTEST_LINKDEF}"
# "${LMDTEST_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
# SET(LMDTEST_SRCS ${LMDTEST_SRCS} ${LMDTEST_DICTIONARY})

# add_library(LmdTesting SHARED ${LMDTEST_SRCS})
# target_link_libraries(LmdTesting ${ROOT_LIBRARIES})
# set_target_properties(LmdTesting PROPERTIES
# ${FAIRROOT_LIBRARY_PROPERTIES})

#Model framework needs c++11 because of shared_ptrs which are part of std there
add_definitions(-W -Wall -Wextra -pedantic -std=c++0x)

########################### create libModel  ##################################
set(LMDTRK_SRCS

LmdFit/Model.cxx 
LmdFit/Model1D.cxx
LmdFit/Model2D.cxx 
LmdFit/ModelParameterHandler.cxx 
LmdFit/ModelParSet.cxx 
LmdFit/ModelPar.cxx
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

add_definitions(-W -Wall -Wextra -pedantic -std=c++0x)

set (LMD_RULECHECKER_SRCS ${LMD_RULECHECKER_SRCS} ${LMDTRK_SRCS})

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LMD_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" LMD_RULES)
endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h LMDTRK_HEADERS "${LMDTRK_SRCS}")

add_library(Model SHARED ${LMDTRK_SRCS})


########################### create libLmdFit ##################################
set(LMDTRK_SRCS

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
LmdFit/PndLmdLumiModelFitter.cxx 
LmdFit/PndLmdModelFactory.cxx 
LmdFit/PndLmdSmearingGaussianModelParametrization1D.cxx
LmdFit/PndLmdROOTDataModel1D.cxx 
LmdFit/PndLmdSmearingDoubleGaussianModelParametrization1D.cxx 
LmdFit/PndLmdResultPlotter.cxx
)
set (LMD_RULECHECKER_SRCS ${LMD_RULECHECKER_SRCS} ${LMDTRK_SRCS})

set(LMDFITDICT_HEADERS
LmdFit/PndLmdLumiFitResult.h
LmdFit/PndLmdLumiFitOptions.h 
LmdFit/PndLmdDataInterface.h
LmdFit/PndLmdDataBase.h
LmdFit/PndLmdData.h
LmdFit/PndLmdAcceptance.h
LmdFit/PndLmdLumiHelper.h 
LmdFit/PndLmdResultPlotter.h
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LMD_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" LMD_RULES)
endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h LMDTRK_HEADERS "${LMDTRK_SRCS}")
set(LMDTRK_LINKDEF  LmdFitLinkDef.h)
set(LMDTRK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/LmdFitDict.cxx)

ROOT_GENERATE_DICTIONARY("${LMDFITDICT_HEADERS}" "${LMDTRK_LINKDEF}" "${LMDTRK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(LMDTRK_SRCS ${LMDTRK_SRCS} ${LMDTRK_DICTIONARY})

add_library(LmdFit SHARED ${LMDTRK_SRCS})
target_link_libraries(LmdFit Model ${ROOT_LIBRARIES})
set_target_properties(LmdFit PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})



################################## install #####################################
install(TARGETS Lmd DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS LmdReco DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS LmdTrk DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS LmdFit DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS LmdTool DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS Model DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS LmdFit DESTINATION ${CMAKE_BINARY_DIR}/lib)
#install(TARGETS LmdTesting DESTINATION ${CMAKE_BINARY_DIR}/lib)
