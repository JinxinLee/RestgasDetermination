# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
  ${Boost_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/MvdData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/sttmvdtracking
  ${CMAKE_SOURCE_DIR}/passive
  ${CMAKE_SOURCE_DIR}/PndTools/generalTools
  ${CMAKE_SOURCE_DIR}/PndTools/riemannfit
  ${CMAKE_SOURCE_DIR}/PndTools/buffers
  ${CMAKE_SOURCE_DIR}/PndTools/MQ
  ${CMAKE_SOURCE_DIR}/PndMCMatch
  ${CMAKE_SOURCE_DIR}/mvd/MvdMC
  ${CMAKE_SOURCE_DIR}/mvd/MvdDigi
  ${CMAKE_SOURCE_DIR}/mvd/MvdReco
  ${CMAKE_SOURCE_DIR}/mvd/MvdTools
  ${CMAKE_SOURCE_DIR}/mvd/MvdDAQ
  ${CMAKE_SOURCE_DIR}/mvd/MvdPid
  ${CMAKE_SOURCE_DIR}/mvd/MvdTracking
  ${CMAKE_SOURCE_DIR}/mvd/MvdTracking/LinFit
  ${CMAKE_SOURCE_DIR}/mvd/transport
  ${CMAKE_SOURCE_DIR}/mvd
  ${CMAKE_SOURCE_DIR}/mvd/MvdOfflineTBAnalysis_Topix4
  ${CMAKE_SOURCE_DIR}/mvd/MvdMQ
  ${CMAKE_SOURCE_DIR}/sds/SdsMC
  ${CMAKE_SOURCE_DIR}/sds/SdsTools
  ${CMAKE_SOURCE_DIR}/sds/SdsDigi
  ${CMAKE_SOURCE_DIR}/sds/SdsReco
  ${CMAKE_SOURCE_DIR}/sds/SdsFEModel
  ${CMAKE_SOURCE_DIR}/sds
  ${CMAKE_SOURCE_DIR}/stt
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/geane
  ${CMAKE_SOURCE_DIR}/PndTools/riemannfit
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
#${Boost_LIBRARY_DIRS}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libMvdTBAnalysis ######################################
set(SRCS
PndMvdReadInTBData.cxx
PndMvdReadInToPix4TBData.cxx
PndMvdReadInTBDataTask.cxx
PndMapSorterTask.cxx
#PndMapSorter.cxx
#PndMapSorterTpl.cxx
PndMvdTopix4RingSorterTask.cxx
PndMvdTopix4RingSorter.cxx
mrftools.cxx
mrf_confitem.cxx
mrfdata_8b.cxx
PndTimeOffsetFinderTask.cxx
PndTSCorrectorTask.cxx
PndStraightLineTrackFinderTask.cxx
PndXYResidualTask.cxx
PndPosCorrectorTask.cxx
PndHammingDecoder.cxx
PndTopix4.cxx
PndMvdToPix4ClusterTask.cxx
)

set(HEADERTPL
	"../MvdTools/PndBurstVectorBuilder.h"
	"../MvdTools/PndBurstVectorBuilder.tpl"
)

install(FILES ${HEADERTPL} DESTINATION include)

set(LINKDEF  MvdTBAnalysisLinkDef.h)
set(LIBRARY_NAME MvdTBAnalysis)

set(DEPENDENCIES Base GeoBase ParBase PndData generalTools
Geane Sds TrkBase boost_serialization boost_thread MathCore MathMore pndMQ Mvd MvdReco
  # fairmq_logger
  boost_timer
  boost_system
  boost_filesystem
  boost_program_options
  boost_random
  boost_chrono)

GENERATE_LIBRARY()