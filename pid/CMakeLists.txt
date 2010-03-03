# Create a library called "libPid" 

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/pndbase/PndStdUtils 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/tpc/tpcreco
${CMAKE_SOURCE_DIR}/mvd/MvdTools
${CMAKE_SOURCE_DIR}/mvd/MvdData
${CMAKE_SOURCE_DIR}/emc/EmcData
${CMAKE_SOURCE_DIR}/emc/EmcReco
${CMAKE_SOURCE_DIR}/emc/EmcTools
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/TofData
${CMAKE_SOURCE_DIR}/pnddata/MdtData
${CMAKE_SOURCE_DIR}/pnddata/DrcData
${CMAKE_SOURCE_DIR}/pnddata/MvdData
${CMAKE_SOURCE_DIR}/pnddata/GemData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/stt/sttreco
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/trackrep
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/rho
${CMAKE_SOURCE_DIR}/rho/RhoBase
${CMAKE_SOURCE_DIR}/pid
${CMAKE_SOURCE_DIR}/pid/PidData
${CMAKE_SOURCE_DIR}/pid/PidCorr
${CMAKE_SOURCE_DIR}/pid/PidClassifier
${CMAKE_SOURCE_DIR}/pid/PidClassifier/KNN
${CMAKE_SOURCE_DIR}/pid/PidClassifier/LVQ1
${CMAKE_SOURCE_DIR}/pid/PidClassifier/TMVAkd_KNN
# ${CMAKE_SOURCE_DIR}/pid/PidClassifier/TMVA_PDE_FOAM
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(Pid_SRCS
PidData/PndPidCandidate.cxx
PidData/PndPidProbability.cxx
PidCorr/PndPidCorrPar.cxx
PidCorr/PndPidCorrelator.cxx
PidCorr/PndPidTrackInfo.cxx
PidCorr/PndPidMvdInfo.cxx
PidCorr/PndPidSttInfo.cxx
PidCorr/PndPidTpcInfo.cxx
PndPidContFact.cxx

# ######### Classifiers sources
PidClassifier/PndMvaUtil.cpp
PidClassifier/PndMvaDataSet.cpp
PidClassifier/PndGpidClassifier.cpp
PidClassifier/KNN/PndStdKnnClassify.cpp
PidClassifier/LVQ1/PndLVQClassify.cpp
PidClassifier/TMVAkd_KNN/PndKnnClassify.cpp
PidClassifier/TMVAkd_KNN/PndProjectedKNN.cpp
# PidClassifier/TMVA_PDE_FOAM/PndPDEFoamClassify.cpp

###### Trainers.
PidClassifier/PndMvaTrainer.cpp
PidClassifier/LVQ1/PndLVQTrain.cpp
# PidClassifier/TMVA_PDE_FOAM/PndPDEFoamTrain.cpp

###### Associator
PidClassifier/PndPidIdealAssociatorTask.cxx
PidClassifier/PndPidMvaAssociatorTask.cxx
PidClassifier/PndPidMvdAssociatorTask.cxx
PidClassifier/PndPidDrcAssociatorTask.cxx

###### Parameters
PidClassifier/PndPidMvdPar.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${Pid_SRCS}" "${INCLUDE_DIRECTORIES}" Pid_RULES)
endif(RULE_CHECKER_FOUND)


set(Pid_HEADERS
PidData/PndPidCandidate.h
PidData/PndPidProbability.h
PidCorr/PndPidCorrPar.h
PidCorr/PndPidCorrelator.h
PndPidContFact.h

############ Classifier headers
PidClassifier/PndMvaClass.h
PidClassifier/PndMvaVariable.h

###### Dummy
PidClassifier/PndPidIdealAssociatorTask.h
PidClassifier/PndPidMvaAssociatorTask.h
PidClassifier/PndPidMvdAssociatorTask.h
PidClassifier/PndPidDrcAssociatorTask.h

###### Parameters
PidClassifier/PndPidMvdPar.h

)



set(Pid_LINKDEF PidLinkDef.h)
set(Pid_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PidDict.cxx) 

ROOT_GENERATE_DICTIONARY("${Pid_HEADERS}" "${Pid_LINKDEF}" "${Pid_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(Pid_SRCS ${Pid_SRCS} ${Pid_DICTIONARY})

add_library(Pid SHARED ${Pid_SRCS})
target_link_libraries(Pid ${ROOT_LIBRARIES})
set_target_properties(Pid PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Pid DESTINATION ${CMAKE_BINARY_DIR}/lib)
