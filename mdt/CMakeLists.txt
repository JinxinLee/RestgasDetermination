# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/mdt 
${CMAKE_SOURCE_DIR}/mdt/MdtMC
${CMAKE_SOURCE_DIR}/pnddata/MdtData 
${CMAKE_SOURCE_DIR}/mdt/MdtDigi
${CMAKE_SOURCE_DIR}/mdt/MdtReco
${GEANT3_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/geane
)


include_directories( 
${INCLUDE_DIRECTORIES}
)

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(MDT_SRCS
MdtMC/PndGeoMdtPar.cxx
MdtMC/PndMdt.cxx
MdtMC/PndMdtMagnet.cxx
MdtMC/PndMdtCoil.cxx
MdtMC/PndMdtMuonFilter.cxx
MdtMC/PndMdtMFIron.cxx
MdtMC/PndMdtForward.cxx
MdtMC/PndMdtGeoConstructorFast.cxx
MdtDigi/PndMdtHitProducerIdeal.cxx
MdtDigi/PndMdtHitProducer.cxx
MdtDigi/PndMdtDigiProducer.cxx
MdtDigi/PndMdtClusterTask.cxx
MdtReco/PndMdtTrkProducerIdeal.cxx
MdtReco/PndMdtTrkProducer.cxx
MdtReco/PndMdtRecoPar.cxx
PndMdtContFact.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${MDT_SRCS}" "${INCLUDE_DIRECTORIES}" MDT_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h MDT_HEADERS "${MDT_SRCS}")

set(MDT_LINKDEF  MdtLinkDef.h)
set(MDT_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndMdtDict.cxx) 

ROOT_GENERATE_DICTIONARY("${MDT_HEADERS}" "${MDT_LINKDEF}" "${MDT_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(MDT_SRCS ${MDT_SRCS} ${MDT_DICTIONARY})

add_library(Mdt SHARED ${MDT_SRCS})
target_link_libraries(Mdt ${ROOT_LIBRARIES})
set_target_properties(Mdt PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Mdt DESTINATION ${CMAKE_BINARY_DIR}/lib)
