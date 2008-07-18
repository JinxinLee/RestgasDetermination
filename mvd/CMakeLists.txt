# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/mcstack
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/mvd/MvdMC
${CMAKE_SOURCE_DIR}/mvd/MvdData
${CMAKE_SOURCE_DIR}/mvd/MvdDigi
${CMAKE_SOURCE_DIR}/mvd/PndMvdReco
${CMAKE_SOURCE_DIR}/mvd/MvdTools
${CMAKE_SOURCE_DIR}/mvd/transport
${CMAKE_SOURCE_DIR}/mvd
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/trackrep
${CMAKE_SOURCE_DIR}/geane
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libMvd ######################################
set(MVD_SRCS

MvdData/PndMvdDigi.cxx
MvdData/PndMvdDigiPixel.cxx
MvdData/PndMvdDigiStrip.cxx
MvdData/PndMvdMCPoint.cxx

MvdDigi/PndMvdCalcFePixel.cxx
MvdDigi/PndMvdCalcPixel.cxx
MvdDigi/PndMvdCalcStrip.cxx
MvdDigi/PndMvdDigiAna.cxx
MvdDigi/PndMvdHybridHitProducer.cxx
MvdDigi/PndMvdStripHitProducer.cxx
MvdDigi/PndMvdHitProducerIdeal.cxx
MvdDigi/PndMvdDigiTask.cxx

MvdMC/PndMvdDetector.cxx
MvdMC/PndMvdGeo.cxx
MvdMC/PndMvdGeoPar.cxx
MvdMC/PndMvdMaterialDet.cxx

MvdTools/PndMvdContFact.cxx
MvdTools/PndMvdPixelDigiPar.cxx
MvdTools/PndMvdStripDigiPar.cxx
MvdTools/PndMvdAccessRTDBTask.cxx
MvdTools/PndStringVector.cxx
MvdTools/PndMvdGeoHandling.cxx
MvdTools/PndMvdEventAna.cxx
MvdTools/PndMvdMCEventAna.cxx
MvdTools/PndMvdDigiEventAna.cxx
MvdTools/PndMvdAllDataEventAna.cxx
MvdTools/PndMvdFileNameCreator.cxx
MvdTools/PndGeoHitList.cxx
MvdTools/PndEventDisplay.cxx

)
set (MVD_RULECHECKER_SRCS ${MVD_SRCS})

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h MVD_HEADERS "${MVD_SRCS}")
# SET(MVD_SRCS ${MVD_SRCS} ${DIGI_SRCS})
# SET(MVD_HEADERS ${MVD_HEADERS} ${DIGI_HEADERS})

set(MVD_LINKDEF  MvdLinkDef.h)
set(MVD_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/MvdDict.cxx)

ROOT_GENERATE_DICTIONARY("${MVD_HEADERS}" "${MVD_LINKDEF}" "${MVD_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(MVD_SRCS ${MVD_SRCS} ${MVD_DICTIONARY})

add_library(Mvd SHARED ${MVD_SRCS})
target_link_libraries(Mvd ${ROOT_LIBRARIES})
set_target_properties(Mvd PROPERTIES ${CBMROOT_LIBRARY_PROPERTIES})

########################### create libMvdReco ##################################
set(MVDRECO_SRCS
MvdData/PndMvdRecoHit.cxx
MvdData/PndMvdPidCand.cxx
MvdData/PndMvdCluster.cxx
MvdData/PndMvdHit.cxx
MvdData/PndMvdHitInfo.cxx

MvdPid/PndMvdPidIdealTask.cxx
MvdPid/PndMvdIdealPidAlgo.cxx
MvdPid/PndMvdSimplePidAlgo.cxx
MvdPid/PndMvdAdvancedPidAlgo.cxx

MvdReco/PndMvdRecoTask.cxx
MvdReco/PndMvdClusterTask.cxx
MvdReco/PndMvdPixelClusterFinder.cxx
MvdReco/PndMvdSimplePixelClusterFinder.cxx
MvdReco/PndMvdIdealPixelClusterFinder.cxx
MvdReco/PndMvdPixelClusterTask.cxx
MvdReco/PndMvdIdealClusterTask.cxx
MvdReco/PndMvdChargeWeightedPixelMapping.cxx
MvdReco/PndMvdStripClusterBuilder.cxx
MvdReco/PndMvdStripClusterTask.cxx
MvdReco/PndMvdIdealRecoTask.cxx

MvdTracking/PndMvdIdealTrackFinderTask.cxx
# MvdTracking/PndMvdIdealTrackingTask.cxx
MvdTracking/PndMvdKalmanTask.cxx
#MvdTracking/PndMvdRiemannTrackFinderTask.cxx
#MvdTracking/PndMvdTrackFinderAnaTask.cxx

)
set (MVD_RULECHECKER_SRCS ${MVD_RULECHECKER_SRCS} ${MVDRECO_SRCS})

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${MVD_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" MVD_RULES)
endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h MVDRECO_HEADERS "${MVDRECO_SRCS}")
set(MVDRECO_LINKDEF  MvdRecoLinkDef.h)
set(MVDRECO_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/MvdRecoDict.cxx)

ROOT_GENERATE_DICTIONARY("${MVDRECO_HEADERS}" "${MVDRECO_LINKDEF}" "${MVDRECO_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(MVDRECO_SRCS ${MVDRECO_SRCS} ${MVDRECO_DICTIONARY})

add_library(MvdReco SHARED ${MVDRECO_SRCS})
target_link_libraries(MvdReco ${ROOT_LIBRARIES})
set_target_properties(MvdReco PROPERTIES ${CBMROOT_LIBRARY_PROPERTIES})


################################## install #####################################
install(TARGETS Mvd DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS MvdReco DESTINATION ${CMAKE_BINARY_DIR}/lib)


