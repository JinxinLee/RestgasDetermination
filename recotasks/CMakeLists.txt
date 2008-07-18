# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/mcstack 
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/mvd
${CMAKE_SOURCE_DIR}/mvd/MvdData 
${CMAKE_SOURCE_DIR}/mvd/MvdTools
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/tpc 
${CMAKE_SOURCE_DIR}/tpc/tpcreco
${CMAKE_SOURCE_DIR}/tpcreco
${CMAKE_SOURCE_DIR}/recotasks 
${CMAKE_SOURCE_DIR}/recotasks/demo
${CMAKE_SOURCE_DIR}/trackrep
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/trackbase
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(RECOTASKS_SRCS
CbmFieldAdaptor.cxx 
demo/DemoRecoHit.cxx 
demo/DemoSPHit.cxx 
demo/DemoPatternRecoTask.cxx 
demo/DemoKalmanTask.cxx
demo/DemoToolsTask.cxx
KalmanTask.cxx
TrackVisTask.cxx 
TrackFitStat.cxx
TrackFitStatTask.cxx
V0Selector.cxx
LambdaSelector.cxx
LambdaStatTask.cxx
MCTruthAnnex.cxx
TpcMCEvtDeconvTask.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${RECOTASKS_SRCS}" "${INCLUDE_DIRECTORIES}" RECOTASKS_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h RECOTASKS_HEADERS "${RECOTASKS_SRCS}")

set(RECOTASKS_LINKDEF  recotasksLinkDef.h)
set(RECOTASKS_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/recotasksDict.cxx) 

ROOT_GENERATE_DICTIONARY("${RECOTASKS_HEADERS}" "${RECOTASKS_LINKDEF}" "${RECOTASKS_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(RECOTASKS_SRCS ${RECOTASKS_SRCS} ${RECOTASKS_DICTIONARY})

add_library(recotasks SHARED ${RECOTASKS_SRCS})
target_link_libraries(recotasks ${ROOT_LIBRARIES})
set_target_properties(recotasks PROPERTIES ${CBMROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS recotasks DESTINATION ${CMAKE_BINARY_DIR}/lib)
