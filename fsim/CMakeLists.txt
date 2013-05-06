# Create a library called "libfsim" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/fsim
${CMAKE_SOURCE_DIR}/pid
${CMAKE_SOURCE_DIR}/rho
${CMAKE_SOURCE_DIR}/rho/RhoBase
${CMAKE_SOURCE_DIR}/rho/RhoSelector
${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(fsim_SRCS
PndFastSim.cxx
PndFsmTrack.cxx
PndFsmResponse.cxx
PndFsmAbsDet.cxx
#StrTok.cxx
PndFsmDetFactory.cxx
PndFsmEmcBarrel.cxx
PndFsmEmcBwCap.cxx
PndFsmEmcFwCap.cxx
PndFsmEmcFS.cxx
PndFsmStt.cxx
PndFsmTpc.cxx
PndFsmMdcFS.cxx
PndFsmMdcTS.cxx
PndFsmRich.cxx
PndFsmDrcDisc.cxx
PndFsmDrcBarrel.cxx
PndFsmEffTracker.cxx
PndFsmMvd.cxx
PndFsmMvd2.cxx
PndFsmTof.cxx
PndFsmSimpleTracker.cxx
PndFsmCmpDet.cxx
PndFsmCombiDet.cxx
PndFsmRandom.cxx
#PndSimpleAnalysis.cxx
#PndAnalysis.cxx
#PndListProvider.cxx
#PndListDefiner.cxx
#PndEventLoop.cxx
)

set(fsim_HEADERS
#ArgList.h
FsmDetTypes.h
PndFastSim.h
PndFsmTrack.h
PndFsmResponse.h
PndFsmAbsDet.h
#StrTok.h
PndFsmDetFactory.h
PndFsmEmcBarrel.h
PndFsmEmcBwCap.h
PndFsmEmcFwCap.h
PndFsmEmcFS.h
PndFsmStt.h
PndFsmTpc.h
PndFsmMdcFS.h
PndFsmMdcTS.h
PndFsmRich.h
PndFsmDrcDisc.h
PndFsmDrcBarrel.h
PndFsmEffTracker.h
PndFsmMvd.h
PndFsmMvd2.h
PndFsmTof.h
PndFsmSimpleTracker.h
PndFsmCmpDet.h
PndFsmCombiDet.h
PndFsmRandom.h
#PndSimpleAnalysis.h
#PndAnalysis.h
#PndListProvider.h
#PndListDefiner.h
#PndEventLoop.h
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${fsim_SRCS}" "${INCLUDE_DIRECTORIES}" FSIM_RULES)
endif(RULE_CHECKER_FOUND)


#CHANGE_FILE_EXTENSION(*.cxx *.h fsim_HEADERS "${Rho_SRCS}")

#set (fsim_HEADERS ${fsim_HEADERS} ArgList.h FsmDetTypes.h StrTok.h)

set(fsim_LINKDEF fsimLinkDef.h)
set(fsim_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/fsimDict.cxx) 

ROOT_GENERATE_DICTIONARY("${fsim_HEADERS}" "${fsim_LINKDEF}" "${fsim_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(fsim_SRCS ${fsim_SRCS} ${fsim_DICTIONARY})

add_library(fsim SHARED ${fsim_SRCS})
target_link_libraries(fsim ${ROOT_LIBRARIES})
set_target_properties(fsim PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS fsim DESTINATION ${CMAKE_BINARY_DIR}/lib)
