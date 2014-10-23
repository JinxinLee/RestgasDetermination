# Create some checker helper apps for lumi fitting
set(CMAKE_MODULE_PATH "$ENV{VMCWORKDIR}/cmake/modules")
Set(CheckSrcDir "$ENV{SIMPATH}/share/fairbase/cmake/checks")
set(CheckSrcDir "$ENV{VMCWORKDIR}/cmake/checks")
include(CheckCXX11Features)
IF(HAS_CXX11_SHAREDPOINTER)
  Add_Definitions(-DHAS_SHAREDPOINTER)
ENDIF(HAS_CXX11_SHAREDPOINTER)

add_definitions(${CXX11_COMPILER_FLAGS})

MACRO(HEADER_DIRECTORIES return_list dir)
    FILE(GLOB_RECURSE new_list ${dir}/*.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

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

########################### create phiSlicing app ##################################

#add_executable(phiSliceData phiSliceData.cxx)
#target_link_libraries(phiSliceData Lmd LmdTool MathMore Eve)

########################### create phiSlicingPlotter app ##################################

add_executable(plotPhiSlicedLumiFitResults plotPhiSlicedLumiFitResults.cxx)
target_link_libraries(plotPhiSlicedLumiFitResults LmdFit MathMore Eve boost_regex)

########################### create lumiFitData app ##################################

add_executable(createLumiFitData createLumiFitData.cxx)
target_link_libraries(createLumiFitData LmdFit boost_regex)

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

########################### create DPM vs E760 app ##################################
add_executable(dpm_vs_e760 ../Anastasia/tests/ModelUncert/DPM_vs_E760.cxx)
target_link_libraries(dpm_vs_e760 $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_regex)

########################### create DPM fit params app ##################################
add_executable(dpm_fit_params ../Anastasia/tests/ModelUncert/DPM_fit.cxx)
target_link_libraries(dpm_fit_params $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_regex)

########################### create E760 fit params app ##################################
add_executable(e760_fit_params ../Anastasia/tests/ModelUncert/E760_fit.cxx)
target_link_libraries(e760_fit_params $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_regex)

########################### create E760 and DPM fit params comparision with exp data app ##################################
add_executable(data_dpg_e760 ../Anastasia/tests/ModelUncert/E760_fit_DPM_fit_vs_ExpDATA.cxx)
target_link_libraries(data_dpg_e760 $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_regex)