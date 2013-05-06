# Create a library called "libPGen" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${PYTHIA8_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/generators
${CMAKE_SOURCE_DIR}/pgenerators
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(PGEN_SRCS
PndRestGas.cxx
PndDpmGenerator.cxx
PndEvtGenGenerator.cxx
PndDpmDirect.cxx
PndBoxGenerator.cxx
PndUrqmdSmmGenerator.cxx
PndJRGenerator.cxx
PndPythia6Generator.cxx
PndPythia8Generator.cxx
PndFlatParticleGenerator.cxx
PndFlukaGenerator.cxx
PndGasGenerator.cxx
PndHypBupGenerator.cxx
PndGiBuuGenerator.cxx
PndGiBuuTrack.cxx
PndGiBuuDecayerAbs.cxx
PndGiBuuJPsiEE.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${PGEN_SRCS}" "${INCLUDE_DIRECTORIES}" PGEN_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h PGEN_HEADERS "${PGEN_SRCS}")

set(PGEN_LINKDEF  PGenLinkDef.h)
set(PGEN_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndPGenDict.cxx) 

ROOT_GENERATE_DICTIONARY("${PGEN_HEADERS}" "${PGEN_LINKDEF}" "${PGEN_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(PGEN_SRCS ${PGEN_SRCS} ${PGEN_DICTIONARY})

add_library(PGen SHARED ${PGEN_SRCS})
target_link_libraries(PGen ${ROOT_LIBRARIES})
set_target_properties(PGen PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS PGen DESTINATION ${CMAKE_BINARY_DIR}/lib)
