# Create some checker helper apps for lumi fitting

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
