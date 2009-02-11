# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/mcstack
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/tpc/test
${CMAKE_SOURCE_DIR}/tpc/spacecharge

)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

# There is a problem in the RuleChecker parsing some files. So
# we define different sets of files for the RuleChecker and
# the normal library. This setting is needed until we solve the
# problem with the RuleChecker

set(TPC_SRCS_RULECHECKER
PndTpcGeo.cxx 			    
PndTpcGeoPar.cxx 		    
PndTpcDigiPar.cxx
PndTpcContFact.cxx 		    
PndTpcDetector.cxx 		    
PndTpcPoint.cxx 	
PndTpcClusterizerTask.cxx 	    
PndTpcGas.cxx 			    
PndTpcPrimaryCluster.cxx 	    
LinearInterpolPolicy.cxx 	    
PndTpcDriftedElectron.cxx 	    
PndTpcDriftTask.cxx 		    
PndTpcAvalanche.cxx 		    
PndTpcGem.cxx 			    
PndTpcGemTask.cxx 		    
PndTpcSignal.cxx 		    
PndTpcSignalAge.cxx 		    
McId.cxx 			    
PndTpcAbsPadShape.cxx 		    
PndTpcPRLookupTable.cxx 	    
PndTpcPadPlane.cxx 		    
PndTpcPad.cxx 			    
PndTpcPadShapePool.cxx 		    
PndTpcPadShapeSquare.cxx 	    
PndTpcPadShapePolygon.cxx 	    
PndTpcPadResponseTask.cxx 	    
PndTpcFrontend.cxx 		    
McIdCollection.cxx 		    
PndTpcSample.cxx 		    
PndTpcCRRCPulseshape.cxx 	    
PndTpcDigitizationPolicy.cxx 	    
PndTpcDigi.cxx 			    
PndTpcDigiAge.cxx 		    
PndTpcDigiAmplitude.cxx 		    
PndTpcDigiMapper.cxx 		    
PndTpcSimplePSAStrategy.cxx 	    	    
PndTpcSpaceChargeTask.cxx 	    
#PndTpcAbsField.cxx 		    
#PndTpcFieldCylGrid.cxx 		    
PndTpcEFieldCyl.cxx 		    
PndTpcEFieldCylLoader.cxx 	    
PndTpcDevmapCyl.cxx 		    
PndTpcDevmapCylLoader.cxx	    
PndTpcEvtMixTask.cxx 		    
PndTpcMCEvtMixTask.cxx
PndTpcMCTracklet.cxx
PndTpcEvtTime.cxx 		    
PndTpcElectronicsTask.cxx 	 
PndTpcClusterFinder.cxx   
PndTpcClusterFinderSimple.cxx   
PndTpcClusterFinderTask.cxx 	    
PndTpcCluster.cxx	    
PndTpcClusterCOG.cxx
PndTpcClusterRadius.cxx 
PndTpcClusterZ.cxx
PndTpcClusterDist.cxx	    
TORPadProcessor.cxx 		    
TORPPState_Initial.cxx 	    
TORPPState_Compare.cxx 	    
TORPPState_Output.cxx 	    
TORPPState_Send.cxx 		    
TORPPState_Wait.cxx 
TORPPState_End.cxx 
PndTpcSectorProcessor.cxx 	    
PndTpcRungeKutta.cxx 		    
PndTpcInhFieldDrifter.cxx 	    
test/PndTpcTester.cxx 		    
PndTpcPSAplot.cxx  
DebugLogger.cxx       
TCovEllipse.cxx
PndTpcPSA_TOT1.cxx
PndTpcSignalSplitterTask.cxx
PndTpcDigiAnalysisTask.cxx
PndTpcLaser.cxx
PndTpcLaserTask.cxx
PndTpcLaserFitTask.cxx
PndTpcLaserTrack.cxx
PndTpcLaserStat.cxx
spacecharge/BiCubSpline.cxx
spacecharge/BSpline.cxx
spacecharge/CubSpline.cxx
spacecharge/CubSplineFitter.cxx
spacecharge/BiCubSplineFitter.cxx
spacecharge/SplineTF1Interface.cxx
spacecharge/SplineTF2Interface.cxx
QAPlotCollection.cxx
DEDX.cxx
PndTpcMCDEdxTask.cxx
PndTpcRawDEdxCollection.cxx
PndTpcDEDXFits.cxx
PndTpcHitMerger.cxx
PndTpcDEDXStorageHelper.cxx
PndTpcTestChamberOutputTask.cxx 	    
)

SET(TPC_SRCS ${TPC_SRCS_RULECHECKER} PndTpcAbsField.cxx PndTpcFieldCylGrid.cxx 		    
) 		    


if(RULE_CHECKER_FOUND)
  CHECK_RULES("${TPC_SRCS_RULECHECKER}" "${INCLUDE_DIRECTORIES}" TPC_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TPC_HEADERS "${TPC_SRCS}")


set(TPC_LINKDEF  tpcLinkDef.h)
set(TPC_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/tpcDict.cxx) 

ROOT_GENERATE_DICTIONARY("${TPC_HEADERS}" "${TPC_LINKDEF}" "${TPC_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(TPC_SRCS ${TPC_SRCS} ${TPC_DICTIONARY} dbgstream.cxx)

add_library(tpc SHARED ${TPC_SRCS})
target_link_libraries(tpc ${ROOT_LIBRARIES})
set_target_properties(tpc PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS tpc DESTINATION ${CMAKE_BINARY_DIR}/lib)
