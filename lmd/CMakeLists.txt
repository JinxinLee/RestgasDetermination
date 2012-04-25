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
${CMAKE_SOURCE_DIR}/PndTools/buffers
${CMAKE_SOURCE_DIR}/PndTools/
${CMAKE_SOURCE_DIR}/sds
${CMAKE_SOURCE_DIR}/sds/SdsMC
${CMAKE_SOURCE_DIR}/sds/SdsDigi
${CMAKE_SOURCE_DIR}/sds/SdsReco
${CMAKE_SOURCE_DIR}/sds/SdsTools
${CMAKE_SOURCE_DIR}/lmd
${CMAKE_SOURCE_DIR}/lmd/LmdMC
${CMAKE_SOURCE_DIR}/lmd/LmdDigi
${CMAKE_SOURCE_DIR}/lmd/LmdReco
${CMAKE_SOURCE_DIR}/lmd/LmdTrack
${CMAKE_SOURCE_DIR}/lmd/LmdFit
${CMAKE_SOURCE_DIR}/lmd/LmdTools
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libLmd ######################################
set(LMD_SRCS
LmdTools/PndLmdContFact.cxx
LmdTools/PndLmdAlignPar.cxx
LmdDigi/PndLmdPixelHitProducerFast.cxx
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
#LmdReco/PndLmdIdealClusterTask.cxx
#LmdReco/PndLmdIdealRecoTask.cxx
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
#LmdTrack/PndLmdMCPointLinFitTask.cxx
LmdTrack/PndLmdIdealFitTask.cxx
LmdTrack/PndLinTrack.cxx
LmdTrack/PndLmdGeaneTask.cxx
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

########################### create libLmdFit ##################################
set(LMDTRK_SRCS

LmdFit/PndLmdcs_th.cxx
LmdFit/PndLmdLumiFit.cxx
)
set (LMD_RULECHECKER_SRCS ${LMD_RULECHECKER_SRCS} ${LMDTRK_SRCS})

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${LMD_RULECHECKER_SRCS}" "${INCLUDE_DIRECTORIES}" LMD_RULES)
endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h LMDTRK_HEADERS "${LMDTRK_SRCS}")
set(LMDTRK_LINKDEF  LmdFitLinkDef.h)
set(LMDTRK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/LmdFitDict.cxx)

ROOT_GENERATE_DICTIONARY("${LMDTRK_HEADERS}" "${LMDTRK_LINKDEF}" "${LMDTRK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(LMDTRK_SRCS ${LMDTRK_SRCS} ${LMDTRK_DICTIONARY})

add_library(LmdFit SHARED ${LMDTRK_SRCS})
target_link_libraries(LmdFit ${ROOT_LIBRARIES})
set_target_properties(LmdFit PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################################## install #####################################
install(TARGETS Lmd DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS LmdReco DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS LmdTrk DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS LmdFit DESTINATION ${CMAKE_BINARY_DIR}/lib)

