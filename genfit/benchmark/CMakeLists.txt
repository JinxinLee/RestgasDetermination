# Create a library called "libgenfit" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/trackrep
${CMAKE_SOURCE_DIR}/genfit/benchmark
${CMAKE_SOURCE_DIR}/genfit/test
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(BENCHMARK_SRCS
SPhit.cxx
StripHit.cxx
SPtestTask.cxx
IllustrateTask.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h BENCHMARK_HEADERS "${BENCHMARK_SRCS}")


set(BENCHMARK_LINKDEF  benchmarkLinkdef.h)
set(BENCHMARK_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/benchmarkDict.cxx) 

#########
set(BENCHMARK_HEADERS  SPhit.h StripHit.h SPtestTask.h IllustrateTask.h)
#########

ROOT_GENERATE_DICTIONARY("${BENCHMARK_HEADERS}" "${BENCHMARK_LINKDEF}" "${BENCHMARK_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(BENCHMARK_SRCS ${BENCHMARK_SRCS} ${BENCHMARK_DICTIONARY})

add_library(benchmark SHARED ${BENCHMARK_SRCS})
target_link_libraries(benchmark ${ROOT_LIBRARIES})
set_target_properties(benchmark PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
################ install ###################
install(TARGETS benchmark DESTINATION ${CMAKE_BINARY_DIR}/lib)
