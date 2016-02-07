# Create a library called "libRich" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${PANDAROOT_SOURCE_DIR}/pnddata
  ${PANDAROOT_SOURCE_DIR}/pnddata/RichData
  ${PANDAROOT_SOURCE_DIR}/rich
#
${GEANT3_INCLUDE_DIR}
#
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/PndTools/generalTools
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/sttmvdtracking
${CMAKE_SOURCE_DIR}/mvd/MvdTracking
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/SciTData
${CMAKE_SOURCE_DIR}/gem
${CMAKE_SOURCE_DIR}/pnddata/GemData
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
#Put here your sourcefiles
PndRich.cxx
PndRichContFact.cxx
PndRichGeo.cxx
PndRichGeoPar.cxx
PndRichReco.cxx
PndRichHitProducer.cxx
PndRichResolution.cxx
PndRichRecoTask.cxx
)

set(LINKDEF RichLinkDef.h)
set(LIBRARY_NAME Rich)

set(DEPENDENCIES Base PndData GeoBase ParBase TrkBase Geane Gem Stt riemann)

GENERATE_LIBRARY()
