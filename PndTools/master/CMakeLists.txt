# Create a library called "Master" which includes the source files given in
# the array .
# The header extension is already found. Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${GEANT4_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/PndTools/master
  ${CMAKE_SOURCE_DIR}/PndTools/generalTools
  ${CMAKE_SOURCE_DIR}/tracking/PndTrackingTools
  ${CMAKE_SOURCE_DIR}/tracking/PndIdealTrackFinder
  ${CMAKE_SOURCE_DIR}/tracking/CentralTracking
  ${CMAKE_SOURCE_DIR}/tracking/sttmvdtracking
  ${CMAKE_SOURCE_DIR}/pgenerators
  ${CMAKE_SOURCE_DIR}/pgenerators/eventFilter
  ${CMAKE_SOURCE_DIR}/pgenerators/EvtGen/EvtGen/R01-03-00	
  ${CMAKE_SOURCE_DIR}/pgenerators/EvtGen/EvtGenDirect
  ${CMAKE_SOURCE_DIR}/pgenerators/Direct
  ${CMAKE_SOURCE_DIR}/pgenerators/EMFFgenerators
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsMC
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMC
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdDigi
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdReco
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcMC	
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcDigi
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcData
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcTools
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcReco
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcCorr
  ${CMAKE_SOURCE_DIR}/detectors/scitil/SciTMC		
  ${CMAKE_SOURCE_DIR}/detectors/scitil/SciTDigi
  ${CMAKE_SOURCE_DIR}/detectors/mdt/MdtMC
  ${CMAKE_SOURCE_DIR}/detectors/mdt/MdtDigi
  ${CMAKE_SOURCE_DIR}/detectors/mdt/MdtReco
  ${CMAKE_SOURCE_DIR}/detectors/drc
  ${CMAKE_SOURCE_DIR}/detectors/dsk/DskMC
  ${CMAKE_SOURCE_DIR}/detectors/gem
  ${CMAKE_SOURCE_DIR}/detectors/fts
  ${CMAKE_SOURCE_DIR}/detectors/ftof
  ${CMAKE_SOURCE_DIR}/detectors/rich
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData
  ${CMAKE_SOURCE_DIR}/pnddata/MdtData	
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/DrcData
  ${CMAKE_SOURCE_DIR}/pnddata/DskData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/pnddata/RichData
  ${CMAKE_SOURCE_DIR}/detectors/fts/FtsTracking
  ${CMAKE_SOURCE_DIR}/PndMCMatchNewLinks
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/recotasks
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/pid/PidCorr
  ${CMAKE_SOURCE_DIR}/pid/PidClassifier
  
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${BASE_LINK_DIRECTORIES}
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
${FORTRAN_LIBDIR}
${PYTHIA8_LIB_DIR}
${HEPMC_LIB_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndMasterTask.cxx
PndMasterSimTask.cxx
PndMasterDigiTask.cxx
PndMasterRecoTask.cxx
PndMasterRecoIdealTask.cxx
PndMasterPidTask.cxx
PndMasterRunSim.cxx
PndMasterRunAna.cxx
)

set(LINKDEF  MasterLinkDef.h)
set(LIBRARY_NAME Master)

# set(DEPENDENCIES Base generalTools MvdReco Tracking SttMvdTracking MCMatch RecoTasks Pid Field Mvd SciT Drc Dsk Mdt Rich PGen EvtGenDirect FtfGenDirect)
set(DEPENDENCIES Base generalTools MvdReco CentralTracking TrackingTools IdealTrackFinder SttMvdTracking MCMatchNewLinks RecoTasks Pid Field Mvd SciT Drc Dsk Mdt Rich PGen EvtGenDirect FtfGenDirect)

GENERATE_LIBRARY() 
