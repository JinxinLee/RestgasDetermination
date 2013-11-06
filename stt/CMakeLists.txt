# Create a library called "libStt" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.


set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/SciTData
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/stt

)

include_directories( ${INCLUDE_DIRECTORIES})


set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndSttMatchTracks.cxx 		
PndGeoStt.cxx    			
PndStt.cxx   			
PndGeoSttPar.cxx 			
PndSttContFact.cxx 			
PndSttDigiPar.cxx 			
PndSttParRootFileIo.cxx 		
PndSttParAsciiFileIo.cxx 		
PndSttTrackFinder.cxx 		
PndSttFindTracks.cxx   		
PndSttTrackFinderIdeal.cxx 		
PndSttTrackFitter.cxx 		
PndSttFitTracks.cxx    		
PndSttTrackMatch.cxx 		
PndSttHitProducerIdeal.cxx  		
PndSttSingleStraw.cxx				
PndSttHelixTrackFitter.cxx 		
PndSttTrackFitterQATask.cxx 	
PndSttHitProducerRealFast.cxx
PndSttHitProducerRealFull.cxx            
PndSttHelixHitProducer.cxx
# PndSttTrackFinderReal.cxx
PndSttMapCreator.cxx
PndSttGeometryMap.cxx
PndSttTube.cxx
PndSttTubeParameters.cxx
PndSttSignalOverlap.cxx

PndSttHitWriteoutBuffer.cxx
PndSttHitRingSorter.cxx
PndSttHitSorterTask.cxx
PndSttStrawMap.cxx
)

set(LINKDEF  SttLinkDef.h)
set(LIBRARY_NAME Stt)

set(DEPENDENCIES Base GeoBase ParBase PndData TrkBase Geane Minuit)

GENERATE_LIBRARY()
