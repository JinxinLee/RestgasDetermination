# Create a library called "libCHIGEN" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

#set(INCLUDE_DIRECTORIES
#   ${PYTHIA8_INCLUDE_DIR}
#   ${ROOT_INCLUDE_DIR}
#   ${PHOTOS_INCLUDE_DIR}
#   ${TAUOLA_INCLUDE_DIR}
#)
set(EVTGEN_INCLUDE_DIR   ${CMAKE_SOURCE_DIR}/pgenerators/EvtGen/EvtGen/R01-03-00)

set(INCLUDE_DIRECTORIES
${PYTHIA8_INCLUDE_DIR}
${SIMPATH}/include
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/base/sim
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/generators
${CMAKE_SOURCE_DIR}/pgenerators
${CMAKE_SOURCE_DIR}/parbase
${EVTGEN_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/pgenerators/chigen
${HEPMC_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
   ${ROOT_LIBRARY_DIR}
   ${HEPMC_LIB_DIR}
   ${PYTHIA8_LIB_DIR}
   ${FORTRAN_LIBDIR}
)

link_directories( ${LINK_DIRECTORIES})

set(CHIGEN_SRCS
ChiGenRandomEngine.cxx
ChiGenContext.cxx
PartonicModel.cxx
ColorSingletModels.cxx
PythiaChiGen.cxx
PndChiGen.cxx
ChiGen.cxx
PndChiGenExclusive.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${CHIGEN_SRCS}" "${INCLUDE_DIRECTORIES}" CHIGEN_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h CHIGEN_HEADERS "${CHIGEN_SRCS}")

set(CHIGEN_LINKDEF  CHIGENLinkDef.h)
set(CHIGEN_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndCHIGENDict.cxx) 

ROOT_GENERATE_DICTIONARY("${CHIGEN_HEADERS}" "${CHIGEN_LINKDEF}" "${CHIGEN_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(CHIGEN_SRCS 
  ${CHIGEN_SRCS} 
  ${CHIGEN_DICTIONARY}  
)

add_library(CHIGEN SHARED ${CHIGEN_SRCS})
target_link_libraries(CHIGEN ${ROOT_LIBRARIES})
set_target_properties(CHIGEN PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS CHIGEN DESTINATION ${CMAKE_BINARY_DIR}/lib)
