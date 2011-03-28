# Create a library called "libMva".

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${BOOST_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/PndTools/MVA
${CMAKE_SOURCE_DIR}/PndTools/MVA/Clusters
${CMAKE_SOURCE_DIR}/PndTools/MVA/KNN
${CMAKE_SOURCE_DIR}/PndTools/MVA/LVQ1
${CMAKE_SOURCE_DIR}/PndTools/MVA/TMVAkd_KNN
${CMAKE_SOURCE_DIR}/PndTools/MVA/TMVA_MLP
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})
# ###################################
# Set HEADER files
set(MVA_HEADERS
PndMvaClassifier.h
PndMvaDataSet.h
PndMvaTrainer.h
PndMvaUtil.h
PndMvaVariable.h
PndMvaClass.h
PndMvaVarPCATransform.h
Clusters/PndMvaCluster.h
KNN/PndStdKnnClassify.h
KNN/PndPrzWindowClassify.h
LVQ1/PndLVQClassify.h
LVQ1/PndLVQTrain.h
TMVAkd_KNN/PndKnnClassify.h
TMVAkd_KNN/PndKnnTrain.h
# TMVAkd_KNN/PndProjectedKNN.h
# TMVA_MLP/PndMultiClassMlpTrain.h
# TMVA_MLP/PndMultiClassMlpClassify.h
)

# Set SOURCE files
set(MVA_SRCS
PndMvaClassifier.cpp
PndMvaDataSet.cpp
PndMvaTrainer.cpp
PndMvaUtil.cpp
PndMvaVarPCATransform.cpp
Clusters/PndMvaCluster.cpp
KNN/PndStdKnnClassify.cpp
KNN/PndPrzWindowClassify.cpp
LVQ1/PndLVQClassify.cpp
LVQ1/PndLVQTrain.cpp
TMVAkd_KNN/PndKnnClassify.cpp
TMVAkd_KNN/PndKnnTrain.cpp
# TMVAkd_KNN/PndProjectedKNN.cpp
# TMVA_MLP/PndMultiClassMlpTrain.cpp
# TMVA_MLP/PndMultiClassMlpClassify.cpp
)
# ###################################
if(RULE_CHECKER_FOUND)
  CHECK_RULES("${MVA_SRCS}" "${INCLUDE_DIRECTORIES}" MVA_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
# CHANGE_FILE_EXTENSION(*.cpp *cxx *.h *.hpp MVA_HEADERS "${MVA_SRCS}")

set(MVA_LINKDEF  MVALinkDef.h)
set(MVA_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/MVADict.cxx) 

ROOT_GENERATE_DICTIONARY("${MVA_HEADERS}" "${MVA_LINKDEF}" "${MVA_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(MVA_SRCS ${MVA_SRCS} ${MVA_DICTIONARY})

add_library(Mva SHARED ${MVA_SRCS})
target_link_libraries(Mva ${ROOT_LIBRARIES})
set_target_properties(Mva PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Mva DESTINATION ${CMAKE_BINARY_DIR}/lib)
