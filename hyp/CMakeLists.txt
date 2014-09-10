# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
${SIMPATH}/include
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/HypData
${CMAKE_SOURCE_DIR}/pnddata/PidData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/PndTools/generalTools
${CMAKE_SOURCE_DIR}/hyp
${CMAKE_SOURCE_DIR}/hyp/hypData
${CMAKE_SOURCE_DIR}/hyp/hypDigi
${CMAKE_SOURCE_DIR}/hyp/hypTools
${CMAKE_SOURCE_DIR}/hyp/hypReco
${CMAKE_SOURCE_DIR}/hyp/hypTracking
${CMAKE_SOURCE_DIR}/hyp/hypFullAna
${CMAKE_SOURCE_DIR}/genfit
#${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/geane
#${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/rho
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndGeoHyp.cxx 
PndHyp.cxx 
PndGeoHypPar.cxx 
PndHypHitProducerIdeal.cxx    
PndHypContFact.cxx 
THParticle.cxx 
#PndHypDecayer.cxx 
#PndHypDecayReader.cxx
#PndHypAnaIdeal.cxx  
#HypStatDecay.cxx  

#hypData/PndHypCluster.cxx
#hypData/PndHypDigiPixel.cxx
#hypData/PndHypDigiStrip.cxx
#hypData/PndHypDigi.cxx
#hypData/PndHypClusterCand.cxx
#hypData/PndHypStripCluster.cxx
hypData/PndHypRecoHit.cxx
hypData/PndHypRecoSPHit.cxx

hypDigi/PndHypStripHitProducer.cxx
#hypDigi/PndHypPixelHitProducer.cxx
#hypDigi/PndHypCalcPixel.cxx
hypDigi/PndHypCalcStrip.cxx
#hypDigi/PndHypCalcFePixel.cxx
#hypDigi/PndHypGeoMappingPar.cxx
hypDigi/PndHypDigiPar.cxx
hypDigi/PndHypStripDigiPar.cxx

hypReco/PndHypIdealRecoTask.cxx
#hypReco/PndHypRecoTask.cxx
hypReco/PndHypStripClusterBuilder.cxx
hypReco/PndHypStripClusterTask.cxx

#hypFullAna/PndHypFullAna.cxx
#hypFullAna/PndHypMicroWriter.cxx
#hypFullAna/PndHypFullIdealAna.cxx
#hypFullAna/PndHypMicroIdealWriter.cxx

hypTools/PndHypGeoHandling.cxx
hypTools/PndStringVector.cxx
hypTools/PndHypMSAnaTask.cxx

hypTracking/PndHypIdealTrackFinderTask.cxx
#hypTracking/PndHypRiemannTrackFinderTask.cxx
hypTracking/PndHypIdealTrackingTask.cxx
hypTracking/PndHypKalmanTask.cxx
hypTracking/PndHypDKalmanTask.cxx
hypTracking/PndHypDPatternRecoTask.cxx
hypTracking/PndHypIdealPRTask.cxx
hypTracking/PndFieldAdaptor.cxx

hypPID/PndHypAdvancedPidAlgo.cxx
hypPID/PndHypPidIdealTask.cxx

)

set(LINKDEF  HypLinkDef.h)
set(LIBRARY_NAME Hyp)

set(DEPENDENCIES Base GeoBase ParBase PndData TrkBase AnalysisTools
generalTools Geane)

GENERATE_LIBRARY()
