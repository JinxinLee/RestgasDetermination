# Create a library called "libMva" which includes the source files given in
# the array .

set(
BOOST_INCLUDE_DIR ${SIMPATH}/basics/boost/include
BOOST_LIBRARY_DIR   ${SIMPATH}/basics/boost/lib
)
set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${BOOST_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/PndTools/MVA/kd_KNN
${CMAKE_SOURCE_DIR}/PndTools/MVA/TMVAkd_KNN
${CMAKE_SOURCE_DIR}/PndTools/MVA/LVQ1
${CMAKE_SOURCE_DIR}/PndTools/MVA
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${BOOST_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(MVA_SRCS
LVQ1/PndLVQClassify.cpp
LVQ1/PndLVQTrain.cpp

TMVAkd_KNN/PndKnnClassify.cpp
TMVAkd_KNN/PndKnnTrain.cpp

# kd_KNN/kdtree2.cpp
# kd_KNN/PndKnnClassify.cpp
# kd_KNN/PndKnnTrain.cpp

# #########################
# For the time being we do not want to use the 
# following implementations.
# KNN/PndKnnClassify.cpp
# KNN/PndKnnTrain.cpp
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${MVA_SRCS}" "${INCLUDE_DIRECTORIES}" MVA_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cpp *.h *.hpp MVA_HEADERS "${MVA_SRCS}")

set(MVA_LINKDEF  MVALinkDef.h)
set(MVA_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/MVADict.cxx) 

ROOT_GENERATE_DICTIONARY("${MVA_HEADERS}" "${MVA_LINKDEF}" "${MVA_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(MVA_SRCS ${MVA_SRCS} ${MVA_DICTIONARY})

add_library(Mva SHARED ${MVA_SRCS})
target_link_libraries(Mva ${ROOT_LIBRARIES})
set_target_properties(Mva PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Mva DESTINATION ${CMAKE_BINARY_DIR}/lib)
