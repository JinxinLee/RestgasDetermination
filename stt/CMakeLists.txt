# Create a library called "libStt" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

#set(GLPK_INCLUDE_DIR 
#${SIMPATH}/basics/glpk/include
#${SIMPATH}/include
#)

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GLPK_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/SciTData
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/stt

)

include_directories( ${INCLUDE_DIRECTORIES})

#set(GLPK_LIB_DIR 
#${SIMPATH}/basics/glpk/lib
#${SIMPATH}/lib
#)

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${GLPK_LIB_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(STT_SRCS
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
PndSttTrackFinderReal.cxx
PndSttMapCreator.cxx
PndSttTube.cxx
PndSttSignalOverlap.cxx

PndSttHitWriteoutBuffer.cxx
PndSttHitRingSorter.cxx
PndSttHitSorterTask.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${STT_SRCS}" "${INCLUDE_DIRECTORIES}" STT_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h STT_HEADERS "${STT_SRCS}")

set(STT_LINKDEF  SttLinkDef.h)
set(STT_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndSttDict.cxx) 

ROOT_GENERATE_DICTIONARY("${STT_HEADERS}" "${STT_LINKDEF}" "${STT_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(STT_SRCS ${STT_SRCS} ${STT_DICTIONARY})

add_library(Stt SHARED ${STT_SRCS})
target_link_libraries(Stt ${ROOT_LIBRARIES} ${GLPK_LIB_DIR}/libglpk.so)
set_target_properties(Stt PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS Stt DESTINATION ${CMAKE_BINARY_DIR}/lib)
