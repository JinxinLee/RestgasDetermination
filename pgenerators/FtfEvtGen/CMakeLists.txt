# Create a library called "libEvtEvtGen" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

set(INCLUDE_DIRECTORIES
${CLHEP_INCLUDE_DIR}
${ROOT_INCLUDE_DIR}
${GEANT4_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/pgenerators/FtfEvtGen
${CMAKE_SOURCE_DIR}/pgenerators/FtfEvtGen/include
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${CLHEP_LIBRARY_DIR}
${GEANT4_LIBRARY_DIR}
${CMAKE_CURRENT_BINARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(FTFEVTGEN_SRCS
G4DecKineticTracks.cxx
Test30HadronProduction.cxx
Test30Material.cxx
Test30Physics.cxx
Test30VSecondaryGenerator.cxx
)

set(FTFEVTGEN_EXE_SRCS
main.cc
)

add_library(FtfEvtGen SHARED ${FTFEVTGEN_SRCS})
#target_link_libraries(FtfEvtGen ${ROOT_LIBRARIES})
target_link_libraries(FtfEvtGen Core RIO Physics Tree G4clhep G4materials G4global G4geometry G4physicslists EG)
set_target_properties(FtfEvtGen PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})


add_executable(FtfEvtGen_exe ${FTFEVTGEN_EXE_SRCS})
if (CMAKE_SYSTEM_NAME MATCHES Darwin)
  target_link_libraries(FtfEvtGen_exe ${ROOT_LIBRARIES} G4materials G4particles G4clhep G4global G4geometry G4physicslists EG "FtfEvtGen")
else (CMAKE_SYSTEM_NAME MATCHES Darwin)
  target_link_libraries(FtfEvtGen_exe G4materials G4particles G4processes G4clhep G4global G4geometry G4physicslists EG FtfEvtGen)
endif (CMAKE_SYSTEM_NAME MATCHES Darwin)
set_target_properties(FtfEvtGen_exe PROPERTIES OUTPUT_NAME FTFGen)

################ install ###################
install(TARGETS FtfEvtGen DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS FtfEvtGen_exe DESTINATION ${CMAKE_BINARY_DIR}/bin)


### FTF DIRECT ###
set(FTFGENDIRECT_SRCS PndFtfDirect.cxx)
CHANGE_FILE_EXTENSION(*.cxx *.h FTFGENDIRECT_HEADERS "${FTFGENDIRECT_SRCS}")
set(FTFGENDIRECT_LINKDEF  FtfLinkDef.h)
set(LIBRARY_NAME FtfGenDirect)
set(SRCS PndFtfDirect.cxx)
set(LINKDEF FtfLinkDef.h)
set(HEADERS PndFtfDirect.h)
set(DEPENDENCIES G4clhep G4global G4geometry G4physicslists G4materials EG FtfEvtGen)
Set(DEFINITIONS G4GLOB_ALLOC_EXPORT=1)
GENERATE_LIBRARY()



