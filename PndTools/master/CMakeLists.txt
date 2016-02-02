# Create a library called "Master" which includes the source files given in
# the array .
# The header extension is already found. Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/PndTools/master
  ${CMAKE_SOURCE_DIR}/PndTools/generalTools
  ${CMAKE_SOURCE_DIR}/stt
  ${CMAKE_SOURCE_DIR}/mvd/MvdDigi
  ${CMAKE_SOURCE_DIR}/mvd/MvdReco
  ${CMAKE_SOURCE_DIR}/emc/EmcMC	
  ${CMAKE_SOURCE_DIR}/emc/EmcDigi
  ${CMAKE_SOURCE_DIR}/emc/EmcData
  ${CMAKE_SOURCE_DIR}/emc/EmcTools
  ${CMAKE_SOURCE_DIR}/emc/EmcReco
  ${CMAKE_SOURCE_DIR}/emc/EmcCorr
  ${CMAKE_SOURCE_DIR}/scitil/SciTMC		
  ${CMAKE_SOURCE_DIR}/scitil/SciTDigi
  ${CMAKE_SOURCE_DIR}/mdt/MdtDigi
  ${CMAKE_SOURCE_DIR}/mdt/MdtReco
  ${CMAKE_SOURCE_DIR}/drc
  ${CMAKE_SOURCE_DIR}/gem
  ${CMAKE_SOURCE_DIR}/fts
  ${CMAKE_SOURCE_DIR}/ftof
  ${CMAKE_SOURCE_DIR}/rich
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData
  ${CMAKE_SOURCE_DIR}/pnddata/MdtData	
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/DrcData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/pnddata/RichData
  ${CMAKE_SOURCE_DIR}/tracking
  ${CMAKE_SOURCE_DIR}/sttmvdtracking
  ${CMAKE_SOURCE_DIR}/fts/FtsTracking
  ${CMAKE_SOURCE_DIR}/PndMCMatch
  ${CMAKE_SOURCE_DIR}/GenfitTools/recotasks
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/pid/PidCorr
  ${CMAKE_SOURCE_DIR}/pid/PidClassifier
  
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndMasterTask.cxx
PndMasterDigiTask.cxx
PndMasterRecoTask.cxx
PndMasterPidTask.cxx
PndMasterRunAna.cxx
)

set(LINKDEF  MasterLinkDef.h)
set(LIBRARY_NAME Master)

set(DEPENDENCIES Base generalTools MvdReco Tracking SttMvdTracking MCMatch RecoTasks Pid)

GENERATE_LIBRARY()
