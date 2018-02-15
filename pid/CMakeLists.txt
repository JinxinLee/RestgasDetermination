# Create a library called "libPid"

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pndbase/PndStdUtils
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/tools
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcMC
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcData
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcReco
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcTools
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcCorr
  ${CMAKE_SOURCE_DIR}/detectors/gem
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData
  ${CMAKE_SOURCE_DIR}/pnddata/MdtData
  ${CMAKE_SOURCE_DIR}/pnddata/DrcData
  ${CMAKE_SOURCE_DIR}/pnddata/DskData
  ${CMAKE_SOURCE_DIR}/pnddata/RichData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/detectors/ftof
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/detectors/stt/sttreco
  ${CMAKE_SOURCE_DIR}/detectors/fts
  ${CMAKE_SOURCE_DIR}/geane
  ${CMAKE_SOURCE_DIR}/trackrep
  ${CMAKE_SOURCE_DIR}/analysis/rho
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoTools
  ${CMAKE_SOURCE_DIR}/pid
  ${CMAKE_SOURCE_DIR}/pid/PidData
  ${CMAKE_SOURCE_DIR}/pid/PidCorr
  ${CMAKE_SOURCE_DIR}/pid/PidClassifier
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/recotasks
  ${CMAKE_SOURCE_DIR}/tools/MVA
  ${CMAKE_SOURCE_DIR}/tools/MVA/KNN
  ${CMAKE_SOURCE_DIR}/tools/MVA/LVQ
  ${CMAKE_SOURCE_DIR}/tools/MVA/TMVAkd_KNN
  ${CMAKE_SOURCE_DIR}/tools/MVA/TMVA_MCL
# ${CMAKE_SOURCE_DIR}/tools/MVA/TMVA_PDE_FOAM
  ${CMAKE_SOURCE_DIR}/detectors/rich
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
PidCorr/PndPidCorrPar.cxx
PidCorr/PndPidCorrelator.cxx
PidCorr/PndPidTrackInfo.cxx
PidCorr/PndPidMvdInfo.cxx
PidCorr/PndPidSttInfo.cxx
PidCorr/PndPidGemInfo.cxx
PidCorr/PndPidFtsInfo.cxx
PidCorr/PndPidFscInfo.cxx
PidCorr/PndPidFtofInfo.cxx
PidCorr/PndPidMdtInfo.cxx
PidCorr/PndPidDskInfo.cxx
PidCorr/PndPidDrcInfo.cxx
PidCorr/PndPidRichInfo.cxx
PidCorr/PndPidEmcInfo.cxx
PidCorr/PndPidTofInfo.cxx
PidCorr/PndPidBremCorrector.cxx
PndPidContFact.cxx

###### Associator
PidClassifier/PndPidIdealAssociatorTask.cxx
PidClassifier/PndPidIdealFwdOnlyAssociatorTask.cxx
PidClassifier/PndPidMvdAssociatorTask.cxx
PidClassifier/PndPidDrcAssociatorTask.cxx
PidClassifier/PndPidDiscAssociatorTask.cxx
PidClassifier/PndPidRichAssociatorTask.cxx
PidClassifier/PndPidMdtHCAssociatorTask.cxx
PidClassifier/PndPidMvaAssociatorTask.cxx
PidClassifier/PndPidEmcAssociatorTask.cxx
PidClassifier/PndPidSttAssociatorTask.cxx
PidClassifier/PndPidEmcBayesAssociatorTask.cxx
PidClassifier/PndPidSciTAssociatorTask.cxx
PidClassifier/PndPidFtofAssociatorTask.cxx

###### Parameters
PidClassifier/PndPidMvdPar.cxx
)

set(HEADERS
PidCorr/PndPidCorrPar.h
PidCorr/PndPidCorrelator.h
PidCorr/PndPidBremCorrector.h
PndPidContFact.h

###### Associator Headers
PidClassifier/PndPidIdealAssociatorTask.h
PidClassifier/PndPidIdealFwdOnlyAssociatorTask.h
PidClassifier/PndPidMvdAssociatorTask.h
PidClassifier/PndPidDrcAssociatorTask.h
PidClassifier/PndPidDiscAssociatorTask.h
PidClassifier/PndPidRichAssociatorTask.h
PidClassifier/PndPidMdtHCAssociatorTask.h
PidClassifier/PndPidMvaAssociatorTask.h
PidClassifier/PndPidEmcAssociatorTask.h
PidClassifier/PndPidSttAssociatorTask.h
PidClassifier/PndPidEmcBayesAssociatorTask.h
PidClassifier/PndPidSciTAssociatorTask.h
PidClassifier/PndPidFtofAssociatorTask.h

###### Parameters
PidClassifier/PndPidMvdPar.h
)

set(LINKDEF PidLinkDef.h)
set(LIBRARY_NAME Pid)

set(DEPENDENCIES Base GeoBase ParBase RecoTasks generalTools Mva Emc
Ftof Fts Geane PndData Stt Rich TrkBase Rho)

GENERATE_LIBRARY()
