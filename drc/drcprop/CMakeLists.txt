# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/pnddata 
${CMAKE_SOURCE_DIR}/drc/drcprop 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(DRCPROP_SRCS
PndDrcEffiAbs.cxx
PndDrcEffiPerfect.cxx
PndDrcEffiBialkali.cxx
PndDrcOptBurle.cxx
PndDrcOptDev.cxx 
PndDrcOptDevManager.cxx 
PndDrcOptDevSys.cxx 
PndDrcOptMatAbs.cxx  
PndDrcOptMatLithotecQ0.cxx 
PndDrcOptMatNLAK33A.cxx 
PndDrcOptMatBK7.cxx 
PndDrcOptMatBK7G18.cxx 
PndDrcOptMatLaK9G15.cxx 
PndDrcOptMatF2G12.cxx 
PndDrcOptMatLF5G15.cxx 
PndDrcOptMatLF5G19.cxx 
PndDrcOptMatK5G20.cxx 
PndDrcOptMatSF6G05.cxx 
PndDrcOptMatMgF2.cxx 
PndDrcOptMatTiO2.cxx 
PndDrcOptMatMarcol7.cxx 
PndDrcOptMatLLF1.cxx 
PndDrcOptMatLiF.cxx 
PndDrcOptMatVacuum.cxx 
PndDrcOptReflSilver.cxx 
PndDrcOptReflGray.cxx 
PndDrcOptReflSurf.cxx 
PndDrcOptReflPerfect.cxx 
PndDrcOptReflAbs.cxx
PndDrcOptReflNone.cxx
PndDrcOptReflGeffcken.cxx
PndDrcOptVol.cxx 
PndDrcPhoton.cxx 
PndDrcSurfAbs.cxx 
PndDrcSurfCyl.cxx 
PndDrcSurfQuadFlatDiff.cxx 
PndDrcSurfPolyFlat.cxx 
PndDrcSurfPolySphere.cxx 
PndDrcSurfPolyCyl.cxx 
PndDrcSurfPolyPara.cxx 
PndDrcSurfPolyAsphere.cxx 
PndDrcUtil.cxx
PndDrcOptBrick.cxx
PndDrcOptTrd2.cxx
PndDrcConicSection.cxx
PndDrcOptLens.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${DRCPROP_SRCS}" "${INCLUDE_DIRECTORIES}" DRCPROP_RULES)
endif(RULE_CHECKER_FOUND)


# fill list of header files from list of source files
# by exchanging the file extension
#CHANGE_FILE_EXTENSION(*.cxx *.h DRCPROP_HEADERS "${DRCPROP_SRCS}")

#set(DRCPROP_LINKDEF  DrcPropLinkDef.h)
#set(DRCPROP_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/DrcPropDict.cxx) 

#ROOT_GENERATE_DICTIONARY(${DRCPROP_HEADERS} ${DRCPROP_LINKDEF} ${DRCPROP_DICTIONARY} ${INCLUDE_DIRECTORIES})

#SET(DRCPROP_SRCS ${DRCPROP_SRCS} ${DRCPROP_DICTIONARY})

add_library(DrcProp SHARED ${DRCPROP_SRCS})
target_link_libraries(DrcProp ${ROOT_LIBRARIES})
set_target_properties(DrcProp PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS DrcProp DESTINATION ${CMAKE_BINARY_DIR}/lib)
