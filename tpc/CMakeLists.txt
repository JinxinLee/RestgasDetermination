# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/tpc/test
${CMAKE_SOURCE_DIR}/tpc/spacecharge
${CMAKE_SOURCE_DIR}/tpc/TestBench
${CMAKE_SOURCE_DIR}/tpc/tpcreco
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/RKTrackRep
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
PndTpcClusterizerTask.cxx 	    
PndTpcGas.cxx 			    
LinearInterpolPolicy.cxx 	    
PndTpcDriftTask.cxx 		    
#PndTpcRadialDriftTask.cxx 		    
PndTpcGem.cxx 			    
PndTpcGemTask.cxx 		    
PndTpcAbsPadShape.cxx 		    
PndTpcPRLookupTable.cxx 	    
PndTpcPadPlane.cxx 		    
PndTpcPad.cxx 			    
PndTpcPadShapePool.cxx 		    
PndTpcPadShapeSquare.cxx 	    
PndTpcPadShapePolygon.cxx 	    
PndTpcPadResponseTask.cxx 	    
PndTpcFrontend.cxx 		    
PndTpcCRRCPulseshape.cxx 	    
PndTpcDigitizationPolicy.cxx 	    
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
PndTpcDevmapSLA.cxx	    
PndTpcEvtMixTask.cxx 		    
PndTpcMCEvtMixTask.cxx
PndTpcMCTracklet.cxx
PndTpcEvtTime.cxx 		    
PndTpcEvtTimeGenTask.cxx 		    
PndTpcElectronicsTask.cxx 	 
PndTpcClusterFinder.cxx   
PndTpcClusterFinderSimple.cxx   
PndTpcClusterFinderTask.cxx 	    
PndTpcClusterCOG.cxx
PndTpcClusterFitLikeli.cxx
PndTpcClusterFitter.cxx
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
#test/PndTpcTester.cxx 		    
PndTpcPSAplot.cxx  
#DebugLogger.cxx       
TCovEllipse.cxx
PndTpcPSA_TOT1.cxx
PndTpcSignalSplitterTask.cxx
PndTpcDigiAnalysisTask.cxx
PndTpcLaser.cxx
PndTpcLaserTask.cxx
PndTpcLaserFitTask.cxx
PndTpcLaserTrack.cxx
PndTpcLaserStat.cxx
PndTpcPSATask.cxx
spacecharge/BiCubSpline.cxx
spacecharge/BSpline.cxx
spacecharge/CubSpline.cxx
spacecharge/CubSplineFitter.cxx
spacecharge/BiCubSplineFitter.cxx
spacecharge/SplineTF1Interface.cxx
spacecharge/SplineTF2Interface.cxx
spacecharge/PndTpcLaserFitTaskStat.cxx
QAPlotCollection.cxx
#DEDX.cxx
#PndTpcMCDEdxTask.cxx
#PndTpcRawDEdxCollection.cxx
#PndTpcDEDXFits.cxx
#PndTpcHitMerger.cxx
#PndTpcDEDXStorageHelper.cxx
PndTpcTestChamberOutputTask.cxx 	    
PndTpcLaserCorrectionTask.cxx					    
TestBench/PndTpcSLResidualTask.cxx
TestBench/PndTpcSLPatternRecoTask.cxx
TestBench/Hypersurface2D.cxx
TestBench/Hough2DNode.cxx
TestBench/Hypersurface4D.cxx
TestBench/Hough4DNode.cxx
TestBench/PndTpcEvent.cxx
TestBench/PndTpcEventIdentifier.cxx
TestBench/PndTpcDataReaderTask.cxx
TestBench/PndTpcClusterCorrectionTask.cxx
#TestBench/PndTpcTCcrossTalk.cxx
#TestBench/PndTpcTCcrossTalkTask.cxx
#TestBench/PndTpcCTapplyTask.cxx
#TestBench/TCmappingManager.cxx
#TestBench/mappingData.cxx
#TestBench/TCconfigManager.cxx
PndTpcT2KPulseshape.cxx
)

SET(TPC_SRCS ${TPC_SRCS_RULECHECKER} PndTpcAbsField.cxx PndTpcFieldCylGrid.cxx 		    
) 

SET(TPC_BASE_SOURCES
  PndTpcPoint.cxx 
  PndTpcPrimaryCluster.cxx 	    
  PndTpcDriftedElectron.cxx 	    
  PndTpcAvalanche.cxx 		    
  PndTpcSignal.cxx 		    
  PndTpcSignalAge.cxx 		    
  McId.cxx 			    
  McIdCollection.cxx 			    
  PndTpcSample.cxx 		    
  PndTpcSampleAge.cxx 		    
  PndTpcDigi.cxx 			    
  PndTpcDigiAge.cxx 		    
  PndTpcCluster.cxx
  )

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${TPC_SRCS_RULECHECKER}" "${INCLUDE_DIRECTORIES}" TPC_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension

CHANGE_FILE_EXTENSION(*.cxx *.h TPC_HEADERS "${TPC_SRCS}")
CHANGE_FILE_EXTENSION(*.cxx *.h TPC_BASE_HEADERS "${TPC_BASE_SOURCES}")

set(TPC_LINKDEF  tpcLinkDef.h)
set(TPC_BASE_LINKDEF  tpcBaseLinkDef.h)


set(TPC_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/tpcDict.cxx) 
set(TPC_BASE_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/tpcBaseDict.cxx) 

ROOT_GENERATE_DICTIONARY("${TPC_HEADERS}" "${TPC_LINKDEF}" "${TPC_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
ROOT_GENERATE_DICTIONARY("${TPC_BASE_HEADERS}" "${TPC_BASE_LINKDEF}" "${TPC_BASE_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(TPC_SRCS ${TPC_SRCS} ${TPC_DICTIONARY})
SET(TPC_BASE_SOURCES ${TPC_BASE_SOURCES} ${TPC_BASE_DICTIONARY}) 

add_library(Tpc SHARED ${TPC_SRCS})
target_link_libraries(Tpc ${ROOT_LIBRARIES} -lEG -lTreePlayer -lGeom -lGed -lRGL -lEve)
set_target_properties(Tpc PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
add_library(TpcBase SHARED ${TPC_BASE_SOURCES})
target_link_libraries(TpcBase ${ROOT_LIBRARIES})
set_target_properties(TpcBase PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


FIND_PATH(TEvePath NAMES ../TEveEventManager.h PATHS
  ${SIMPATH}/tools/root/include
  NO_DEFAULT_PATH
)
find_package(OpenGL)

If(TEvePath AND OPENGL_FOUND AND OPENGL_GLU_FOUND)
  Message("Found TEve, OpenGL and OpenGL GLU  so PndTpcClustVis will be compiled.")
  
  SET(TPC_EVE_SOURCES
    TestBench/PndTpcClustVis.cxx
  )
  
  CHANGE_FILE_EXTENSION(*.cxx *.h TPC_EVE_HEADERS "${TPC_EVE_SOURCES}")
  
  set(TPC_EVE_LINKDEF  tpcEveLinkDef.h)
  
  set(TPC_EVE_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/tpcEveDict.cxx)
  
  ROOT_GENERATE_DICTIONARY("${TPC_EVE_HEADERS}" "${TPC_EVE_LINKDEF}" "${TPC_EVE_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
  
  SET(TPC_EVE_SOURCES ${TPC_EVE_SOURCES} ${TPC_EVE_DICTIONARY}) 
  
  add_library(TpcEve SHARED ${TPC_EVE_SOURCES})
  target_link_libraries(TpcEve ${ROOT_LIBRARIES})
  set_target_properties(TpcEve PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
  
  ################ install with TpcEve ###################
  install(TARGETS Tpc TpcBase TpcEve DESTINATION ${CMAKE_BINARY_DIR}/lib)

EndIf (TEvePath AND OPENGL_FOUND AND OPENGL_GLU_FOUND)


If(NOT (TEvePath AND OPENGL_FOUND AND OPENGL_GLU_FOUND))
  ############## install without TpcEve ##################
  install(TARGETS Tpc TpcBase DESTINATION ${CMAKE_BINARY_DIR}/lib)
EndIf(NOT (TEvePath AND OPENGL_FOUND AND OPENGL_GLU_FOUND))
