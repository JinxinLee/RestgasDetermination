# Create some checker helper apps for lumi fitting

# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

set(CMAKE_MODULE_PATH "$ENV{VMCWORKDIR}/cmake/modules")
include(CheckCXX11Features)
IF(HAS_CXX11_SHAREDPOINTER)
  Add_Definitions(-DHAS_SHAREDPOINTER)
ENDIF(HAS_CXX11_SHAREDPOINTER)

add_definitions(${CXX11_COMPILER_FLAGS})

# This is a macro that returns a list of directories where all .h (header) files are
# located including subdirectories
# return_list is the variable that is written with that list and dir is the directory
# which is scanned
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

set(CMAKE_BUILD_TYPE Debug)

set(LINK_DIRECTORIES
$ENV{ROOT_LIBRARY_DIR}
$ENV{SIMPATH}/lib
$ENV{SIMPATH}/../basics/boost/stage/lib
$ENV{VMCWORKDIR}/build/lib
/usr/X11R6/lib
)

include_directories($ENV{VMCWORKDIR}/lmd/LmdFit)

########################### create lumiFitData app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(createLumiFitData createLumiFitData.cxx)
target_link_libraries(createLumiFitData $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_system boost_filesystem boost_regex)

########################### create mergeLmdData app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(mergeLmdData mergeLmdData.cxx)
target_link_libraries(mergeLmdData $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_system boost_filesystem boost_regex)

########################### create lumiFit app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(runLumiFit runLumiFit.cxx)
target_link_libraries(runLumiFit $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_system boost_filesystem boost_regex)

########################### create determineResolution app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(determineResolution determineResolution.cxx)
target_link_libraries(determineResolution $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_system boost_filesystem boost_regex)

########################### create determineBeamOffset app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(determineBeamOffset determineBeamOffset.cxx)
target_link_libraries(determineBeamOffset $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_system boost_filesystem boost_regex)



########################### create lumiFitSystematic app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(determineLumiFitSystematics determineLumiFitSystematics.cxx)
target_link_libraries(determineLumiFitSystematics $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_system boost_filesystem boost_regex)


########################### create lumiFitComparison app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(combineLumiFitResults combineLumiFitResults.cxx)
target_link_libraries(combineLumiFitResults $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_system boost_filesystem boost_regex)


########################### create acceptanceComparison app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(compareAcceptances compareAcceptances.cxx)
target_link_libraries(compareAcceptances $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_system boost_filesystem boost_regex)


########################### create fitRawDPMElasticData app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(fitRawDPMElasticData fitRawDPMElasticData.cxx)
target_link_libraries(fitRawDPMElasticData $ENV{ROOT_LIBRARIES} EG MathMore LmdFit Model GL X11 GLU gsl gslcblas boost_system boost_filesystem boost_regex)


########################### create checkFramework app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(checkFramework checkFramework.cxx)
target_link_libraries(checkFramework $ENV{ROOT_LIBRARIES} EG MathMore Model GL X11 GLU gsl gslcblas)


########################### create checkMCInfoTrackMatching app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES
$ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES
$ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
$ENV{VMCWORKDIR}/lmd/LmdQA
$ENV{VMCWORKDIR}/PndTools
$ENV{VMCWORKDIR}/pnddata)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES}
${VMCWORKDIR}/lib)

add_executable(checkMCInfoTrackMatching checkMCInfoTrackMatching.cxx)
target_link_libraries(checkMCInfoTrackMatching $ENV{ROOT_LIBRARIES} MathMore)


########################### create BeamOffsetResult plotter app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(plotIPDistribution plotIPDistribution.cxx)
target_link_libraries(plotIPDistribution $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas)



########################### create LumiFit plotter app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(plotLumiFitResults plotLumiFitResults.cxx)
target_link_libraries(plotLumiFitResults $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas)



########################### create lumi resolution plotter app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(plotResolutionParametrization plotResolutionParametrization.cxx)
target_link_libraries(plotResolutionParametrization $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas)




########################### create DPMModel plotter app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(plotDPMModelParts plotDPMModelParts.cxx)
target_link_libraries(plotDPMModelParts $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas)


########################### create convolution plotter app ##################################

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/ModelFramework)
HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES $ENV{VMCWORKDIR}/lmd/LmdFit/LumiImpl)

set(INCLUDE_DIRECTORIES
$ENV{SIMPATH}/include
$ENV{ROOT_INCLUDE_DIR}
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
/usr/X11R6/include
)

include_directories(${INCLUDE_DIRECTORIES})

link_directories(${LINK_DIRECTORIES})

add_executable(convolutionThesisPlots convolutionThesisPlots.cxx)
target_link_libraries(convolutionThesisPlots $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas)
