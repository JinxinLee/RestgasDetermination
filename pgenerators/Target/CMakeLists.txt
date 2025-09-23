# Create a library called "libPGen" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${CLHEP_INCLUDE_DIR}
  ${GEANT4_INCLUDE_DIR}
  ${PYTHIA8_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pgenerators
  ${CMAKE_SOURCE_DIR}/pgenerators/Target
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/fairtools
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${PYTHIA8_LIB_DIR}
${FAIRROOT_LIBRARY_DIR}
${GEANT4_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndRestGas.cxx
PndGasGenerator.cxx
PndTargetGenerator.cxx
)

set(LINKDEF  TargetLinkDef.h)
set(LIBRARY_NAME PGenTarget)

set(DEPENDENCIES Base Hist MathCore pythia8 EG EGPythia6 ${VMCLIB})

GENERATE_LIBRARY()


