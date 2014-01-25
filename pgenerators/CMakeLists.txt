# Create a library called "libPGen" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${PYTHIA8_INCLUDE_DIR}
${SIMPATH}/include
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/generators
${CMAKE_SOURCE_DIR}/pgenerators
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
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

set(LINKDEF  PGenLinkDef.h)
set(LIBRARY_NAME PGen)

set(DEPENDENCIES Base DpmEvtGen)

GENERATE_LIBRARY()
