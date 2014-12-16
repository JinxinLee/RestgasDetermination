# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.
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
${SIMPATH}/include
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES}  
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/GemData
${CMAKE_SOURCE_DIR}/pnddata/MdtData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/FtsData
${CMAKE_SOURCE_DIR}/pnddata/SciTData
${CMAKE_SOURCE_DIR}/trackbase
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
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/stt/sttreco
${CMAKE_SOURCE_DIR}/fts
${CMAKE_SOURCE_DIR}/lmd/LmdMC
${CMAKE_SOURCE_DIR}/lmd/LmdDigi
${CMAKE_SOURCE_DIR}/lmd/LmdReco
${CMAKE_SOURCE_DIR}/lmd/LmdTrack
${CMAKE_SOURCE_DIR}/lmd/LmdTools
${CMAKE_SOURCE_DIR}/lmd/LmdFit
${CMAKE_SOURCE_DIR}/lmd
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/RKTrackRep

${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/GenfitTools/recohits
${CMAKE_SOURCE_DIR}/GenfitTools/recotasks

${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}

)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${GLPK_LIB_DIR}
)

link_directories( ${LINK_DIRECTORIES})

################################## install #####################################
add_executable(model_vs_data Anastasia/tests/ModelUncert/models_vs_data.cxx)
target_link_libraries(model_vs_data $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas)
add_executable(calc_cs Anastasia/tests/ModelUncert/CrossSecLim.C)
target_link_libraries(calc_cs $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas)
add_executable(calc_cs_un Anastasia/tests/ModelUncert/DPM_ModelUncert.C)
target_link_libraries(calc_cs_un $ENV{ROOT_LIBRARIES} MathMore LmdFit Model GL X11 GLU gsl gslcblas)
#add_executable(simple_mc_rec_match simpleMCandRECmatch.C)
add_executable(createRootGeometry_lmd_HV_MAPS create_Lmd_HV_MAPS.C)	
target_link_libraries(createRootGeometry_lmd_HV_MAPS -lrt ${ROOT_LIBRARIES} LmdTool  GeoBase ParBase Geom Sds Proof  PndData TrkBase VMC EG GeomPainter generalTools FairTools MathMore FairDB Base)
#target_link_libraries(simple_mc_rec_match -lrt ${ROOT_LIBRARIES} LmdTool MathMore)

add_subdirectory(LMD_alignment)
