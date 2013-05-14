# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
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
