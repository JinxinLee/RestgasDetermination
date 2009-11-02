# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/pndbase/proxydict
${CMAKE_SOURCE_DIR}/pndbase/basetasks
${CMAKE_SOURCE_DIR}/pndbase/gblenv
${CMAKE_SOURCE_DIR}/pndbase
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(pndbase_SRCS
 #proxydict/AbsArgCast.cxx
proxydict/AbsArg.cxx
#proxydict/AbsArgVal.cxx
#proxydict/IfdAliasProxy.cxx
#proxydict/Ifd.cxx
proxydict/IfdCompositeKey.cxx
#proxydict/IfdCompositeKey..cxx
proxydict/IfdCounter.cxx
#proxydict/IfdDataProxy.cxx
proxydict/IfdDataProxyIFace.cxx
#proxydict/IfdDataProxyTemplate.cxx
proxydict/IfdDictKey.cxx
proxydict/IfdIntKey.cxx
proxydict/IfdKey.cxx
proxydict/IfdKeyHash.cxx
proxydict/IfdProxyDict.cxx
proxydict/IfdProxyDictFrontEnd.cxx
proxydict/IfdSimpleProxyDict.cxx
proxydict/IfdSimpleProxyDictEntry.cxx
#proxydict/IfdStdVectorProxy.cxx
proxydict/IfdStrKey.cxx
proxydict/IfdTestClasses.cxx
#proxydict/IfdTypeKey.cxx
proxydict/IfdTypeKeyIFace.cxx
gblenv/AbsEnv.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${pndbase_SRCS}" "${INCLUDE_DIRECTORIES}" pndbase_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h pndbase_HEADERS "${pndbase_SRCS}")

set(pndbase_LINKDEF  pndbaseLinkDef.h)
set(pndbase_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/pndbaseDict.cxx) 

ROOT_GENERATE_DICTIONARY("${pndbase_HEADERS}" "${pndbase_LINKDEF}" "${pndbase_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(pndbase_SRCS ${pndbase_SRCS} ${pndbase_DICTIONARY})

add_library(PndBase SHARED ${pndbase_SRCS})
target_link_libraries(PndBase ${ROOT_LIBRARIES})
set_target_properties(PndBase PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS PndBase DESTINATION ${CMAKE_BINARY_DIR}/lib)
