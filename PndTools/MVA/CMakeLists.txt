# Create a library called "libMva".

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${BOOST_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/PndTools/MVA
${CMAKE_SOURCE_DIR}/PndTools/MVA/Clusters
${CMAKE_SOURCE_DIR}/PndTools/MVA/KNN
${CMAKE_SOURCE_DIR}/PndTools/MVA/LVQ
${CMAKE_SOURCE_DIR}/PndTools/MVA/TMVAkd_KNN
${CMAKE_SOURCE_DIR}/PndTools/MVA/TMVA_MCL
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})
# ###################################
# Set HEADER files
SET (MVA_HEADERS
PndMvaClassifier.h
PndMvaDataSet.h
PndMvaTrainer.h
PndMvaUtil.h
PndMvaVariable.h
PndMvaClass.h
PndMvaVarPCATransform.h
Clusters/PndMvaCluster.h
Clusters/PndMvaSOMTrainer.h
KNN/PndStdKnnClassify.h
KNN/PndPrzWindowClassify.h
LVQ/PndLVQClassify.h
LVQ/PndLVQTrain.h
TMVAkd_KNN/PndKnnClassify.h
TMVAkd_KNN/PndKnnTrain.h
# TMVAkd_KNN/PndProjectedKNN.h
)
# MCL TMVA Headers.
SET (TMVA_ALG_HEADR
TMVA_MCL/PndMultiClassMlpTrain.h
TMVA_MCL/PndMultiClassMlpClassify.h
TMVA_MCL/PndMultiClassBdtTrain.h
TMVA_MCL/PndMultiClassBdtClassify.h
)

# Set SOURCE files
SET (MVA_SRCS
PndMvaClassifier.cpp
PndMvaDataSet.cpp
PndMvaTrainer.cpp
PndMvaUtil.cpp
PndMvaVarPCATransform.cpp
Clusters/PndMvaCluster.cpp
Clusters/PndMvaSOMTrainer.cpp
KNN/PndStdKnnClassify.cpp
KNN/PndPrzWindowClassify.cpp
LVQ/PndLVQClassify.cpp
LVQ/PndLVQTrain.cpp
TMVAkd_KNN/PndKnnClassify.cpp
TMVAkd_KNN/PndKnnTrain.cpp
# TMVAkd_KNN/PndProjectedKNN.cpp
)
# MCL TMVA Sources.
SET (TMVA_ALG_SOURCE
TMVA_MCL/PndMultiClassMlpTrain.cpp
TMVA_MCL/PndMultiClassMlpClassify.cpp
TMVA_MCL/PndMultiClassBdtTrain.cpp
TMVA_MCL/PndMultiClassBdtClassify.cpp
)

# If we can build mlp into the lib
# The version number is calculated major*10000 + minor * 100 + patch version
# 5.28/00 results in 52800
If(${ROOT_VERSION_NUMBER} LESS 52800)
   MESSAGE("-- <MVA_WARN> ROOT version is too old for Muliclass support.")
Else(${ROOT_VERSION_NUMBER} LESS 52800)
   MESSAGE("-- <MVA_INFO> Building MVA with multiClass.")
   SET(MVA_HEADERS ${MVA_HEADERS} ${TMVA_ALG_HEADR})
   SET(MVA_SRCS ${MVA_SRCS} ${TMVA_ALG_SOURCE})
EndIf(${ROOT_VERSION_NUMBER} LESS 52800)

# ###################################
# if(RULE_CHECKER_FOUND)
#  CHECK_RULES("${MVA_SRCS}" "${INCLUDE_DIRECTORIES}" MVA_RULES)
# endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
# CHANGE_FILE_EXTENSION(*.cpp *.cxx *.h *.hpp MVA_HEADERS "${MVA_SRCS}")

set(MVA_LINKDEF  MVALinkDef.h)
set(MVA_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/MVADict.cxx) 

ROOT_GENERATE_DICTIONARY("${MVA_HEADERS}" "${MVA_LINKDEF}" "${MVA_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(MVA_SRCS ${MVA_SRCS} ${MVA_DICTIONARY})

add_library(Mva SHARED ${MVA_SRCS})
target_link_libraries(Mva ${ROOT_LIBRARIES})
set_target_properties(Mva PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Mva DESTINATION ${CMAKE_BINARY_DIR}/lib)
