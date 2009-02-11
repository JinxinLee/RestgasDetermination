# Create a library called "libgpid" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${ROOT_INCLUDE_DIR}/TMVA 
${GEANT3_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/mcstack
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/tof
${CMAKE_SOURCE_DIR}/emc/EmcData
${CMAKE_SOURCE_DIR}/emc/EmcReco
${CMAKE_SOURCE_DIR}/mvd
${CMAKE_SOURCE_DIR}/mvd/MvdData
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/gpid
${CMAKE_SOURCE_DIR}/drc
${CMAKE_SOURCE_DIR}/rho
${CMAKE_SOURCE_DIR}/rho/RhoBase
${CMAKE_SOURCE_DIR}/rho/RhoTools
${CMAKE_SOURCE_DIR}/rho/RhoSelector
${CMAKE_SOURCE_DIR}/rho/RhoMath
${CMAKE_SOURCE_DIR}/rho/RhoFitter
${CMAKE_SOURCE_DIR}/myobj
${CMAKE_SOURCE_DIR}/geane

${CMAKE_SOURCE_DIR}/PndTools/MVA/kd_KNN
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(Gpid_SRCS
PndGpidTask.cxx
PndGpidTaskLhe.cxx
PndPidCand.cxx
#PndGeaneTrTpcTof.cxx
#PndGeaneTrTpcIdealTof.cxx
PndMultiClassMVA.cxx
#PndMakeTrainData.cxx
#PndMakeTrainDataLhe.cxx
PndGpidTaskFast.cxx
)
CHANGE_FILE_EXTENSION(*.cxx *.h Gpid_HEADERS "${Gpid_SRCS}")

set(Gpid_LINKDEF GpidLinkDef.h)
set(Gpid_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/GpidDict.cxx) 

ROOT_GENERATE_DICTIONARY("${Gpid_HEADERS}" "${Gpid_LINKDEF}" "${Gpid_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(Gpid_SRCS ${Gpid_SRCS} ${Gpid_DICTIONARY})

add_library(Gpid SHARED ${Gpid_SRCS})
target_link_libraries(Gpid ${ROOT_LIBRARIES})
set_target_properties(Gpid PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Gpid DESTINATION ${CMAKE_BINARY_DIR}/lib)
