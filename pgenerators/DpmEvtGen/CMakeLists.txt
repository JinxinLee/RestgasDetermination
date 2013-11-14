# Create a library called "libDpmEvtGen" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)



set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/pgenerators/DpmEvtGen
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${CMAKE_CURRENT_BINARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(DPMEVTGEN_SRCS
common.f
decay.f
dpm_gen.f
init.f
rndmget.f
string.f
)

set(DPMEVTGEN_EXE_SRCS
main.cc
)

If(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  SET_SOURCE_FILES_PROPERTIES(dpm_gen.f PROPERTIES COMPILE_FLAGS
                           "-fsecond-underscore")
Else(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  SET_SOURCE_FILES_PROPERTIES(dpm_gen.f PROPERTIES COMPILE_FLAGS
                           "-assume 2underscores")
EndIf(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")

add_library(DpmEvtGen SHARED ${DPMEVTGEN_SRCS})
#target_link_libraries(DpmEvtGen ${ROOT_LIBRARIES})
target_link_libraries(DpmEvtGen )
set_target_properties(DpmEvtGen PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

add_executable(DpmEvtGen_exe ${DPMEVTGEN_EXE_SRCS})
if (CMAKE_SYSTEM_NAME MATCHES Darwin)
  target_link_libraries(DpmEvtGen_exe ${ROOT_LIBRARIES} EG "DpmEvtGen")
else (CMAKE_SYSTEM_NAME MATCHES Darwin)
  target_link_libraries(DpmEvtGen_exe EG DpmEvtGen)
endif (CMAKE_SYSTEM_NAME MATCHES Darwin)
set_target_properties(DpmEvtGen_exe PROPERTIES OUTPUT_NAME DPMGen)

################ install ###################
install(TARGETS DpmEvtGen DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS DpmEvtGen_exe DESTINATION ${CMAKE_BINARY_DIR}/bin)
