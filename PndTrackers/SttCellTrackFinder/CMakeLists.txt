
if (NOT DEFINED CUDA_HOST_COMPILER AND CMAKE_C_COMPILER_ID STREQUAL "Clang")
  If (CMAKE_SYSTEM_NAME MATCHES Darwin)
    Find_Program(Found_host_compiler 
      NAMES 686-apple-darwin10-gcc-4.2.1 i686-apple-darwin11-llvm-gcc-4.2
      PATHS /usr/bin/
    )
  Else (CMAKE_SYSTEM_NAME MATCHES Darwin)
    Find_Program(Found_host_compiler 
      NAMES gcc
      PATHS /usr/bin/
    )
  EndIf (CMAKE_SYSTEM_NAME MATCHES Darwin)
  If (Found_host_compiler)
    set(CUDA_HOST_COMPILER ${Found_host_compiler} CACHE FILEPATH "Host side compiler used by NVCC")
    message(STATUS "Setting CUDA_HOST_COMPILER to ${Found_host_compiler} instead of ${CMAKE_C_COMPILER}.")
    set (CUDA_BUILD_POSSIBLE TRUE)
  Else (Found_host_compiler)
    message(STATUS "The default compiler ${CMAKE_C_COMPILER} does not work together with the CUDA compiler nvcc. We could not find a gcc compiler so we have to switch off the build of the CUDA examples.")
    set (CUDA_BUILD_POSSIBLE FALSE)
  EndIf (Found_host_compiler)
else()
  set (CUDA_BUILD_POSSIBLE TRUE)
endif()

find_package(CUDA)

find_file(NVCC_IS_FOUND nvcc)

if (CUDA_BUILD_POSSIBLE AND CUDA_FOUND AND NVCC_IS_FOUND)
	set(DO_THE_CUDA_DANCE TRUE)
endif (CUDA_BUILD_POSSIBLE AND CUDA_FOUND AND NVCC_IS_FOUND)

If (DO_THE_CUDA_DANCE)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cuda)
	add_definitions(-DTHERE_IS_CUDA)  # so we can actually do different things in the .cpp files depending if there is CUDA or not
	message(STATUS "Gentlemen, we are running with CUDA")
	# message(STATUS ${DO_THE_CUDA_DANCE})
	# message(STATUS DO_THE_CUDA_DANCE)
EndIf (DO_THE_CUDA_DANCE)


set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${SIMPATH}/include
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/PndTrackers/sttmvdtracking
${CMAKE_SOURCE_DIR}/PndTrackers/SttCellTrackFinder
${CMAKE_SOURCE_DIR}/PndTrackers/riemannfit
${CMAKE_SOURCE_DIR}/PndTrackers/PndIdealTrackFinder
)

If (DO_THE_CUDA_DANCE)
        LIST(APPEND INCLUDE_DIRECTORIES ${CUDA_INCLUDE})
EndIf (DO_THE_CUDA_DANCE)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndSttCellTrackFinderTask.cxx
PndSttCellTrackFinder.cxx
PndSttCellTrackFinderData.cxx
PndSttCellTrackletGenerator.cxx
PndSttHitCorrector.cxx
#PndSttCellTrackFinderAnalysisTask.cxx
PndSttSkewedCombineTask.cxx
)


# if(RULE_CHECKER_FOUND)
#   CHECK_RULES("${CELL_TRACKFINDER_SRCS}" "${INCLUDE_DIRECTORIES}" RULES)
# endif(RULE_CHECKER_FOUND)

set(LINKDEF  PndSttCellTrackFinderLinkDef.h)
set(LIBRARY_NAME SttCellTrackFinder)


set(DEPENDENCIES Base PndData TrkBase Stt riemann)
if (CUDA_BUILD_POSSIBLE AND CUDA_FOUND)
        LIST(APPEND DEPENDENCIES ${PNDSTTCELLTRACKFINDERCUDA})
endif(CUDA_BUILD_POSSIBLE AND CUDA_FOUND)

GENERATE_LIBRARY()
