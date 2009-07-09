SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

PROJECT(tpcCuda CUDA C CXX)

cmake_minimum_required(VERSION 2.6)

SET(ROOT_LIB $ENV{ROOTSYS}/lib)
SET(ROOT_BIN $ENV{ROOTSYS}/bin)
SET(ROOT_LIBS ${ROOT_LIB}/libCore.so
              ${ROOT_LIB}/libCint.so
              ${ROOT_LIB}/libRIO.so
              ${ROOT_LIB}/libNet.so
              ${ROOT_LIB}/libHist.so
              ${ROOT_LIB}/libGraf.so
              ${ROOT_LIB}/libGraf3d.so
              ${ROOT_LIB}/libGpad.so
              ${ROOT_LIB}/libThread.so
              ${ROOT_LIB}/libTree.so
              ${ROOT_LIB}/libRint.so
              ${ROOT_LIB}/libPostscript.so
              ${ROOT_LIB}/libMatrix.so
              ${ROOT_LIB}/libPhysics.so
              ${ROOT_LIB}/libMathCore.so
              ${ROOT_LIB}/libMinuit.so
              ${ROOT_LIB}/libMinuit2.so
	      libdl.so
              libm.so)

#ADD_LIBRARY(${SOURCES})

# Enable warnings and make them errors
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")


# Attempt to find dependant libraries
FIND_PACKAGE(CUDARuntime)

IF(NOT CUDA_RUNTIME_FOUND)
	MESSAGE(SEND_ERROR "CUDA runtime is required and could not be found.")
ELSE(NOT CUDA_RUNTIME_FOUND)
	INCLUDE_DIRECTORIES(${CUDA_RUNTIME_INCLUDE_DIR})
ENDIF(NOT CUDA_RUNTIME_FOUND)

INCLUDE_DIRECTORIES($ENV{ROOTSYS}/include $ENV{VMCWORKDIR}/tpc)

#SET(SOURCES hello.cxx)





ADD_EXECUTABLE(hello hello.cu)
TARGET_LINK_LIBRARIES(hello ${CUDA_RUNTIME_LIBRARIES} ${ROOT_LIBS})

