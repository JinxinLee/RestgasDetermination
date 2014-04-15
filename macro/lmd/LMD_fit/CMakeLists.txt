# Create some checker helper apps for lumi fitting

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/lmd/LmdFit/LumiImpl
${CMAKE_SOURCE_DIR}/lmd/LmdFit/ModelFramework
${SIMPATH}/include
${ROOT_INCLUDE_DIR}
)

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${SIMPATH}/lib
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})


########################### create lumiFitData app ##################################

add_executable(createLumiFitData createLumiFitData.cxx)
target_link_libraries(createLumiFitData LmdFit)

########################### create mergeLmdData app ##################################

add_executable(mergeLmdData mergeLmdData.cxx)
target_link_libraries(mergeLmdData LmdFit boost_regex)

########################### create lumiFit app ##################################

add_executable(runLumiFit runLumiFit.cxx)
target_link_libraries(runLumiFit LmdFit)

########################### create determineResolution app ##################################

add_executable(determineResolution determineResolution.cxx)
target_link_libraries(determineResolution LmdFit)

########################### create determineBeamOffset app ##################################

add_executable(determineBeamOffset determineBeamOffset.cxx)
target_link_libraries(determineBeamOffset LmdFit)

########################### create lumiFitSystematic app ##################################

add_executable(determineLumiFitSystematics determineLumiFitSystematics.cxx)
target_link_libraries(determineLumiFitSystematics LmdFit boost_regex)

########################### create lumiFitComparison app ##################################

add_executable(combineLumiFitResults combineLumiFitResults.cxx)
target_link_libraries(combineLumiFitResults LmdFit)

########################### create acceptanceComparison app ##################################

add_executable(compareAcceptances compareAcceptances.cxx)
target_link_libraries(compareAcceptances LmdFit)

########################### create fitRawDPMElasticData app ##################################

add_executable(fitRawDPMElasticData fitRawDPMElasticData.cxx)
target_link_libraries(fitRawDPMElasticData LmdFit)

########################### create checkFramework app ##################################

add_executable(checkFramework checkFramework.cxx)
target_link_libraries(checkFramework $ENV{ROOT_LIBRARIES} Model)

########################### create checkMCInfoTrackMatching app ##################################
set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/lmd/LmdQA
${CMAKE_SOURCE_DIR}/PndTools
${CMAKE_SOURCE_DIR}/pnddata)

include_directories(${INCLUDE_DIRECTORIES})

add_executable(checkMCInfoTrackMatching checkMCInfoTrackMatching.cxx)
target_link_libraries(checkMCInfoTrackMatching $ENV{ROOT_LIBRARIES})

########################### create BeamOffsetResult plotter app ##################################

add_executable(plotIPDistribution plotIPDistribution.cxx)
target_link_libraries(plotIPDistribution LmdFit)

########################### create LumiFit plotter app ##################################

add_executable(plotLumiFitResults plotLumiFitResults.cxx)
target_link_libraries(plotLumiFitResults LmdFit)

########################### create lumi resolution plotter app ##################################

add_executable(plotResolutionParametrization plotResolutionParametrization.cxx)
target_link_libraries(plotResolutionParametrization LmdFit)

########################### create DPMModel plotter app ##################################

add_executable(plotDPMModelParts plotDPMModelParts.cxx)
target_link_libraries(plotDPMModelParts LmdFit)

########################### create convolution plotter app ##################################

add_executable(convolutionThesisPlots convolutionThesisPlots.cxx)
target_link_libraries(convolutionThesisPlots $ENV{ROOT_LIBRARIES} Model LmdFit)

########################### create elastic cross section integrator app ##################################

add_executable(calculateElasticCrossSection calculateElasticCrossSection.cxx)
target_link_libraries(calculateElasticCrossSection LmdFit)
