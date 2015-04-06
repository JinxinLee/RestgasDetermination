# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
  ${GEANT3_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/mdt 
  ${CMAKE_SOURCE_DIR}/mdt/MdtMC
  ${CMAKE_SOURCE_DIR}/pnddata/MdtData 
  ${CMAKE_SOURCE_DIR}/mdt/MdtDigi
  ${CMAKE_SOURCE_DIR}/mdt/MdtReco
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/geane
)


Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
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

set(LINKDEF  MdtLinkDef.h)
set(LIBRARY_NAME Mdt)

set(DEPENDENCIES Base GeoBase ParBase PndData Geane)

GENERATE_LIBRARY()
