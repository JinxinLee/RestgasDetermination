# cuda does not work together with clang
# there is a incompatibility between nvcc and clang
# to overcome the problem set the cuda host compiler as gcc
# on newer mac versions gcc links to clang, so check for the
# known gcc compiler with different names
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
    message(STATUS "The default compiler ${CMAKE_C_COMPILER} does not work together with the CUDA compiler nvcc. We could not find a gcc compiler so we have to switch of the build of the CUDA eaxamples.")
    set (CUDA_BUILD_POSSIBLE FALSE)
  EndIf (Found_host_compiler)
else()
  set (CUDA_BUILD_POSSIBLE TRUE)  
endif()

find_package(CUDA REQUIRED)

If (CUDA_BUILD_POSSIBLE AND CUDA_FOUND)

  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/example)

  set(INCLUDE_DIRECTORIES
  	${ROOT_INCLUDE_DIR} 
	${CUDA_INCLUDE}
        ${BASE_INCLUDE_DIRECTORIES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/pnddata 
  )
  include_directories( ${INCLUDE_DIRECTORIES})

  set(LINK_DIRECTORIES
	${ROOT_LIBRARY_DIR}
  )
  link_directories( ${LINK_DIRECTORIES})

  set(SRCS
	PndCudaExampleTask.cxx
  )

  set(LINKDEF PndCudaLinkDef.h)
  set(LIBRARY_NAME PndCuda)

  set(DEPENDENCIES Base ${PNDCUDAEXAMPLESONAME})

  GENERATE_LIBRARY()

EndIf (CUDA_BUILD_POSSIBLE AND CUDA_FOUND)
