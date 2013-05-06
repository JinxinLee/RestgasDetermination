# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES} 
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
)
 
link_directories( ${LINK_DIRECTORIES})

set(HYP_SRCS
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

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${HYP_SRCS}" "${INCLUDE_DIRECTORIES}" HYP_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HYP_HEADERS "${HYP_SRCS}")

set(HYP_LINKDEF  HypLinkDef.h)
set(HYP_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndHypDict.cxx) 

ROOT_GENERATE_DICTIONARY("${HYP_HEADERS}" "${HYP_LINKDEF}" "${HYP_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(HYP_SRCS ${HYP_SRCS} ${HYP_DICTIONARY})

add_library(Hyp SHARED ${HYP_SRCS})
target_link_libraries(Hyp ${ROOT_LIBRARIES})
set_target_properties(Hyp PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Hyp DESTINATION ${CMAKE_BINARY_DIR}/lib)
