# Create a library called "libPGen" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

add_subdirectory (DpmEvtGen)
add_subdirectory (FtfEvtGen)
add_subdirectory (Direct)
add_subdirectory (EvtGen)
add_subdirectory (chigen)
#add_subdirectory (PndFlukaGen)
#add_subdirectory (EMFFgenerators)


Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${CLHEP_INCLUDE_DIR}
  ${GEANT4_INCLUDE_DIR}
  ${PYTHIA8_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pgenerators
  ${CMAKE_SOURCE_DIR}/pgenerators/FtfEvtGen
  ${CMAKE_SOURCE_DIR}/pgenerators/FtfEvtGen/include
  ${CMAKE_SOURCE_DIR}/pgenerators/GiBuuReader
  ${CMAKE_SOURCE_DIR}/pgenerators/eventFilter
  ${CMAKE_SOURCE_DIR}/pgenerators/particleguns
  ${CMAKE_SOURCE_DIR}/pgenerators/FileReaders
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/analysis/rho
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoSelector
  ${CMAKE_SOURCE_DIR}/fairtools
  ${CMAKE_SOURCE_DIR}/detectors/hyp
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
PndPythia8Generator.cxx
PndGasGenerator.cxx
particleguns/PndBoxGenerator.cxx
particleguns/PndFixStepParticleGun.cxx
FileReaders/PndDpmGenerator.cxx
FileReaders/PndFtfGenerator.cxx
FileReaders/PndEvtGenGenerator.cxx
FileReaders/PndUrqmdSmmGenerator.cxx
FileReaders/PndJRGenerator.cxx
FileReaders/PndPythia6Generator.cxx
FileReaders/PndFlukaGenerator.cxx
FileReaders/PndHypBupGenerator.cxx
GiBuuReader/PndGiBuuGenerator.cxx
GiBuuReader/PndGiBuuTrack.cxx
GiBuuReader/PndGiBuuDecayerAbs.cxx
GiBuuReader/PndGiBuuJPsiEE.cxx
eventFilter/FairFilteredPrimaryGenerator.cxx
eventFilter/FairEvtFilterParams.cxx
eventFilter/FairEvtFilter.cxx
eventFilter/FairEvtFilterOnSingleParticleCounts.cxx
eventFilter/PndEvtFilter.cxx
eventFilter/PndEvtFilterOnInvMassCounts.cxx
)

set(LINKDEF  PGenLinkDef.h)
set(LIBRARY_NAME PGen)

set(DEPENDENCIES Base pythia8)

GENERATE_LIBRARY()


