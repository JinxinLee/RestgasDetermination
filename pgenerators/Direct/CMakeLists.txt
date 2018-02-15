# Create a library called "libPGen" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${CLHEP_INCLUDE_DIR}
  ${GEANT4_INCLUDE_DIR}
  ${GEANT4VMC_INCLUDE_DIR}
  ${PYTHIA8_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pgenerators/Direct
  ${CMAKE_SOURCE_DIR}/pgenerators/FtfEvtGen
  ${CMAKE_SOURCE_DIR}/pgenerators/FtfEvtGen/include
  ${CMAKE_SOURCE_DIR}/pgenerators/eventFilter
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/analysis/rho
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoSelector
  ${CMAKE_SOURCE_DIR}/fairtools
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${GEANT4_LIBRARY_DIR}
${GEANT4VMC_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndDpmDirect.cxx
PndFtfDirect.cxx
PndG4DummyPhysicsList.cxx
PndG4DummyDetectorConstruction.cxx
PndG4DummyActionInitialization.cxx
)

set(LINKDEF  FtfLinkDef.h)
set(LIBRARY_NAME FtfGenDirect)
set(DEPENDENCIES Base geant4vmc G4clhep G4global G4geometry G4physicslists G4materials EG  FtfEvtGen DpmEvtGen)
#Set(DEFINITIONS G4GLOB_ALLOC_EXPORT=1) # Is it really needed??
GENERATE_LIBRARY()
