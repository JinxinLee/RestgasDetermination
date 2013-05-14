# Create a library called "libLumi" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CLHEP_SOURCE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/pndbase 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/lumi
${CMAKE_SOURCE_DIR}/lumi/LumiMC
${CMAKE_SOURCE_DIR}/lumi/LumiData
${CMAKE_SOURCE_DIR}/lumi/LumiTools
${CMAKE_SOURCE_DIR}/lumi/LumiDigi
${CMAKE_SOURCE_DIR}/lumi/LumiReco
${CMAKE_SOURCE_DIR}/lumi/LumiTracking
${CMAKE_SOURCE_DIR}/trackrep
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/genfit
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

######################### create libLumi #####################################
set(SRCS
LumiMC/PndLumi.cxx
LumiMC/PndLumiGeo.cxx 
LumiMC/PndLumiGeoPar.cxx

LumiData/PndLumiPoint.cxx
LumiData/PndLumiHit.cxx

LumiTools/PndLumiStrip.cxx
LumiTools/PndLumiTransposition.cxx
LumiTools/PndLumiContFact.cxx

LumiDigi/PndLumiCalcStripDigi.cxx
LumiDigi/PndLumiDigiPara.cxx
LumiDigi/PndLumiDigi.cxx
LumiDigi/PndLumiDigiProducer.cxx

LumiReco/PndLumiSimpleStripClusterFinder.cxx
LumiReco/PndLumiStripClusterFinder.cxx
LumiReco/PndLumiStripClusterBuilder.cxx
LumiReco/PndLumiClusterTask.cxx
LumiReco/PndLumiCluster.cxx

LumiTracking/PndLumiTrackFinder.cxx
LumiTracking/PndLumiTrack.cxx
LumiTracking/PndLinTrack.cxx
LumiTracking/PndLumiLinFitPar.cxx
LumiTracking/PndLumiTrackFitter.cxx
)

set(LINKDEF  LumiLinkDef.h)
set(LIBRARY_NAME Lumi)

set(DEPENDENCIES Base GeoBase ParBase PndData Lmd)

GENERATE_LIBRARY()
