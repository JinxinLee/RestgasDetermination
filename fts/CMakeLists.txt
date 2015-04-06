# Create a library called "libFts" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${GEANT3_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/fts
  ${CMAKE_SOURCE_DIR}/fts/FtsTracking
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndGeoFtsPar.cxx
PndGeoFts.cxx
PndFtsDigiPar.cxx
PndFtsHitInfo.cxx
PndFts.cxx
PndFts2.cxx
PndFtsContFact.cxx
PndFtsMapCreator.cxx
PndFtsTube.cxx
PndFtsSingleStraw.cxx
PndFtsSignalOverlap.cxx
PndFtsHitProducerRealFast.cxx
PndFtsHitProducerIdeal.cxx
PndFtsHitProducerRealFull.cxx
PndFtsDataAccessor.cxx
PndFtsHitRingSorter.cxx
PndFtsHitWriteoutBuffer.cxx
PndFtsHitSorterTask.cxx
FtsTracking/PndFtsTrackerIdeal.cxx
FtsTracking/PndFtsHoughTracklet.cxx
FtsTracking/PndFtsHoughTrackCand.cxx
FtsTracking/PndFtsHoughSpace.cxx
FtsTracking/PndFtsHoughSpaceBinning.cxx
FtsTracking/PndFtsHoughSpacePeak.cxx
FtsTracking/PndFtsHoughTrackFinderParams.cxx
FtsTracking/PndFtsHoughTrackFinder.cxx
FtsTracking/PndFtsHoughTrackFinderQA.cxx
FtsTracking/PndFtsHoughTrackerTask.cxx
FtsTracking/PndFtsHoughTrackerTaskQA.cxx
FtsTracking/PndFtsHitProducerMcPointCoordinates.cxx
)

set(LINKDEF  FtsLinkDef.h)
set(LIBRARY_NAME Fts)

set(DEPENDENCIES Base GeoBase ParBase PndData TrkBase Spectrum)

GENERATE_LIBRARY()
