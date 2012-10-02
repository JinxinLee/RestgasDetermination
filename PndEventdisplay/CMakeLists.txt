# Create a library called "libPndEventDisplay" 
# 18.06.09  T.Stockmanns

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${GEANT3_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/PndTools/generalTools 
${CMAKE_SOURCE_DIR}/PndMCMatch
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/trackbase 
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/lhetrack
${CMAKE_SOURCE_DIR}/eventdisplay
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/pnddata/MvdData
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/mvd/MvdDigi
${CMAKE_SOURCE_DIR}/sds/SdsDigi
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/PndEventdisplay
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(PNDEVEDISP_SRCS
PndTrackCandDraw.cxx
PndTrackDraw.cxx
PndSttMCPointDraw.cxx
PndMvdDigiPixelDraw.cxx
PndSttIsochroneDraw.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${PNDEVEDISP_SRCS}" "${INCLUDE_DIRECTORIES}" PNDEVEDISP_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h PNDEVEDISP_HEADERS "${PNDEVEDISP_SRCS}")

set(PNDEVEDISP_LINKDEF PndEventDisplayLinkDef.h)
set(PNDEVEDISP_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/PndEventDisplayDict.cxx) 

ROOT_GENERATE_DICTIONARY("${PNDEVEDISP_HEADERS}" "${PNDEVEDISP_LINKDEF}" "${PNDEVEDISP_DICTIONARY}" "${INCLUDE_DIRECTORIES}")


set(PNDEVEDISP_SRCS ${PNDEVEDISP_SRCS} ${PNDEVEDISP_DICTIONARY})

add_library(PndEventDisplay SHARED ${PNDEVEDISP_SRCS})
target_link_libraries(PndEventDisplay ${ROOT_LIBRARIES})
set_target_properties(PndEventDisplay PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS PndEventDisplay DESTINATION ${CMAKE_BINARY_DIR}/lib)
