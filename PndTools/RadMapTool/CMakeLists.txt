# Create a library called "libRadMapTool" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

#if(COMMAND cmake_policy)
#  cmake_policy(SET CMP0003 OLD)
#endif(COMMAND cmake_policy)

Set(SYSTEM_INCLUDE_DIRECTORIES 
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${CLHEP_INCLUDE_DIR}
  ${GEANT4_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  )

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/PndTools/RadMapTool
  ${CMAKE_SOURCE_DIR}/passive
  ${CMAKE_SOURCE_DIR}/stt
  ${CMAKE_SOURCE_DIR}/mvd/MvdMC
  ${CMAKE_SOURCE_DIR}/emc/EmcData
  ${CMAKE_SOURCE_DIR}/emc/EmcDigi
  ${CMAKE_SOURCE_DIR}/emc/EmcDigi/PndEmcPSAFPGA
  ${CMAKE_SOURCE_DIR}/emc/EmcMC
  ${CMAKE_SOURCE_DIR}/emc/EmcReco
  ${CMAKE_SOURCE_DIR}/emc/EmcTools
  ${CMAKE_SOURCE_DIR}/emc/EmcProtoTools
  ${CMAKE_SOURCE_DIR}/emc/EmcCorr
  ${CMAKE_SOURCE_DIR}/emc/EmcTimeBased
  ${CMAKE_SOURCE_DIR}/emc
  ${CMAKE_SOURCE_DIR}/mdt/MdtMC
  ${CMAKE_SOURCE_DIR}/sds/SdsMC
  ${CMAKE_SOURCE_DIR}/gem
  ${CMAKE_SOURCE_DIR}/dsk/DskMC
  ${CMAKE_SOURCE_DIR}/drc
  ${CMAKE_SOURCE_DIR}/fts
  ${CMAKE_SOURCE_DIR}/scitil/SciTMC
  ${CMAKE_SOURCE_DIR}/ftof
  ${CMAKE_SOURCE_DIR}/rich
  ${CMAKE_SOURCE_DIR}/pgenerators
  ${CMAKE_SOURCE_DIR}/pgenerators/EvtGen/EvtGenDirect
  ${CMAKE_SOURCE_DIR}/pgenerators/EvtGen/EvtGen/R01-03-00/
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/PndTools/generalTools
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/DrcData
  ${CMAKE_SOURCE_DIR}/pnddata/DskData
  ${CMAKE_SOURCE_DIR}/pnddata/DiscData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/HypData
  ${CMAKE_SOURCE_DIR}/pnddata/HypGData
  ${CMAKE_SOURCE_DIR}/pnddata/MdtData
  ${CMAKE_SOURCE_DIR}/pnddata/MvdData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/pnddata/RichData
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/scitil
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${CLHEP_LIBRARY_DIR}
${GEANT4_LIBRARY_DIR}
${CMAKE_CURRENT_BINARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(RADMAP_SRCS
 Mesh.cpp
)
set(LINKDEF  RadMapLinkDef.h)
set(LIBRARY_NAME RadMapTool)

set(DEPENDENCIES Base GeoBase ParBase MathCore MathMore)
# PndData generalTools Geane Sds TrkBase )

GENERATE_LIBRARY()

set(RADMAP_EXE_SRCS
 sim_radmap.C
)


add_executable(RadMapTool_exe ${RADMAP_EXE_SRCS})
target_link_libraries(RadMapTool_exe ${ROOT_LIBRARIES} G4materials G4particles G4processes G4clhep G4global G4geometry G4physicslists G4track EG FtfEvtGen DpmEvtGen RadMapTool Base ParBase  Field  Emc Stt Mdt Gem Dsk Drc Fts Sds Lmd Mvd SciT Ftof Rich Gen PhotosCxxInterface PhotosFortran EvtGen EvtGenDirect PGen generalTools PndData Passive)
set_target_properties(RadMapTool_exe PROPERTIES OUTPUT_NAME sim_radmap)

################ install ###################
install(TARGETS RadMapTool_exe DESTINATION ${CMAKE_BINARY_DIR}/bin)




