# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

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
${FAIRROOT_LIBRARY_DIR}
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
LmdTrack/PndLmdKalmanTask.cxx
LmdTrack/PndLmdGeaneTask.cxx
LmdTrack/PndLmdBPtestTask.cxx
LmdTrack/PndLmdBPRungeKuttaTask.cxx	
LmdTrack/PndLmdTrksFilterTask.cxx
)

set(LINKDEF  LmdTrkLinkDef.h)
set(LIBRARY_NAME LmdTrk)

set(DEPENDENCIES genfitAdapters RecoHits TrkBase Geane LmdReco)

GENERATE_LIBRARY()

########################### create libLmdTools ##################################
set(SRCS

LmdQA/PndLmdTrackQ.cxx
LmdQA/PndLmdTrkQTask.cxx
LmdQA/PndLmdQATask.cxx
)

set(LINKDEF  LmdToolLinkDef.h)
set(LIBRARY_NAME LmdTool)

set(DEPENDENCIES Sds LmdTrk)

GENERATE_LIBRARY()


########################### create libModel  ##################################
# modeling framework (completely independent of ROOT)

HEADER_DIRECTORIES(MODEL_INCLUDE_DIRECTORIES LmdFit/ModelFramework)

include_directories(${MODEL_INCLUDE_DIRECTORIES})

include_directories(/usr/X11R6/include/)
include_directories(/opt/X11/include/)
link_directories(/usr/X11R6/lib)
SET(EXTRA_LIBS GL X11 GLU glut)

FILE(GLOB_RECURSE MODEL_SRCS  LmdFit/ModelFramework/*.cxx)

add_definitions(${CXX11_COMPILER_FLAGS})
add_library(Model SHARED ${MODEL_SRCS})

TARGET_LINK_LIBRARIES(Model)


########################### create libLmdFit ##################################

HEADER_DIRECTORIES(TEMP_INCLUDE_DIRECTORIES LmdFit/LumiImpl)
set(LMDFIT_INCLUDE_DIRECTORIES
${TEMP_INCLUDE_DIRECTORIES}
${MODEL_INCLUDE_DIRECTORIES}
${SIMPATH}/include
)

include_directories(${LMDFIT_INCLUDE_DIRECTORIES})

FILE(GLOB_RECURSE LMDFIT_SRCS  LmdFit/LumiImpl/*.cxx)

set(SRCS ${LMDFIT_SRCS})

set(HEADERS
LmdFit/ModelFramework/fit/ModelFitResult.h
LmdFit/LumiImpl/fit/PndLmdLumiFitResult.h
LmdFit/LumiImpl/fit/PndLmdLumiFitOptions.h 
LmdFit/LumiImpl/data/PndLmdDataInterface.h
LmdFit/LumiImpl/data/PndLmdDataBase.h
LmdFit/LumiImpl/data/PndLmdData.h
LmdFit/LumiImpl/data/PndLmdAcceptance.h
LmdFit/LumiImpl/data/PndLmdResolution.h
LmdFit/LumiImpl/PndLmdLumiHelper.h 
LmdFit/LumiImpl/PndLmdResultPlotter.h
LmdFit/LumiImpl/LumiFitStructs.h
)

set(LINKDEF  LmdFitLinkDef.h)
set(LIBRARY_NAME LmdFit)

set(DEPENDENCIES PndData Lmd LmdTool Model)

GENERATE_LIBRARY()
