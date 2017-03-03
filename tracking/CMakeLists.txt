# Create a library called "libTracking" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.


Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${BASE_INCLUDE_DIRECTORIES} 
)
Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/stt
  ${CMAKE_SOURCE_DIR}/fts
  ${CMAKE_SOURCE_DIR}/sttmvdtracking
  ${CMAKE_SOURCE_DIR}/mvd/MvdTracking
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/gem 
  ${CMAKE_SOURCE_DIR}/pnddata/GemData 
  ${CMAKE_SOURCE_DIR}/geane 
  ${CMAKE_SOURCE_DIR}/PndTools/AnalysisTools
  ${CMAKE_SOURCE_DIR}/PndTools/PndParticleFitters 
  ${CMAKE_SOURCE_DIR}/rho
  ${CMAKE_SOURCE_DIR}/rho/RhoBase
  ${CMAKE_SOURCE_DIR}/rho/RhoMath
  ${CMAKE_SOURCE_DIR}/rho/RhoFitter
  ${CMAKE_SOURCE_DIR}/tracking
  ${CMAKE_SOURCE_DIR}/PndTools/generalTools/
  ${CMAKE_SOURCE_DIR}/tracking/TrkSecondary
  ${CMAKE_SOURCE_DIR}/tracking/TrkStructure
  ${CMAKE_SOURCE_DIR}/tracking/TrkAlgo
  ${CMAKE_SOURCE_DIR}/tracking/TrkData
#  ${CMAKE_SOURCE_DIR}/tracking/TrkQA
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})


set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndTrkBoundaryParStraws.cxx
PndTrkBoundaryParStraws2.cxx
PndTrkCategorizeStt.cxx
PndTrkChi2Fits.cxx
PndTrkCleanup.cxx
PndTrkComparisonMCtruth.cxx
PndTrkCTFindTrackInXY.cxx
PndTrkCTFindTrackInXY2.cxx
PndTrkCTGeometryCalculations.cxx
PndTrkLegendreFits.cxx
PndTrkMergeSort.cxx
PndTrkSttConformalFilling.cxx
PndTrkPlotMacros.cxx
PndTrkPlotMacros2.cxx
PndTrkPrintouts.cxx
PndTrkTracking2.cxx
PndTrkSttAdjacencies.cxx
PndTrkSttClusterFinder.cxx
#
TrkAlgo/PndTrkLegendreTransform.cxx
TrkAlgo/PndTrkConformalTransform.cxx
TrkAlgo/PndTrkTools.cxx
TrkAlgo/PndTrkClean.cxx
TrkAlgo/PndTrkFitter.cxx
TrkAlgo/PndTrkGemCombinatorial.cxx
#
TrkData/PndTrkHit.cxx
TrkData/PndTrkIndivisibleHit.cxx
TrkData/PndTrkSkewHit.cxx
TrkData/PndTrkHitList.cxx
TrkData/PndTrkSdsHitList.cxx
TrkData/PndTrkSttHitList.cxx
TrkData/PndTrkGemHitList.cxx
TrkData/PndTrkSciTHitList.cxx
TrkData/PndTrkCluster.cxx
TrkData/PndTrkLegendreCluster.cxx 
TrkData/PndTrkClusterList.cxx
TrkData/PndTrkTrackList.cxx      
TrkData/PndTrkTrack.cxx
TrkData/PndTrkConformalHit.cxx
TrkData/PndTrkConformalHitList.cxx
TrkData/PndTrkSkewHitList.cxx
TrkData/PndTrkNeighboringMap.cxx
#
TrkSecondary/PndTrkTrackFinder.cxx
TrkSecondary/PndTrkAddTCA.cxx
TrkSecondary/PndTrkGemCombiTask.cxx
#
#TrkStructure/PndTrkQATask.cxx
#TrkQA/PndTrkQualityAssuranceTask.cxx
#TrkQA/PndTrkMCTrackInfo.cxx
#TrkQA/PndTrkRecoTrackInfo.cxx
#TrkQA/PndTrkAnaTask.cxx

)

set(LINKDEF  TrackingLinkDef.h)
set(LIBRARY_NAME Tracking)

set(DEPENDENCIES Base ParBase PndData Stt Fts TrkBase ParticleFitters)

GENERATE_LIBRARY()
