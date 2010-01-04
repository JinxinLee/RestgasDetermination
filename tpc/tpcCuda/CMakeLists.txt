SET(CMAKE_MODULE_PATH $ENV{VMCWORKDIR}/tpc/tpcCuda/cmake)
#SET(PANDA_BINARY_DIR /home/felix/simulation/build)
#SET(CMAKE_VERBOSE_MAKEFILE ON)
PROJECT(tpcCuda CUDA C CXX)

cmake_minimum_required(VERSION 2.6)

INCLUDE_DIRECTORIES($ENV{ROOTSYS}/include $ENV{VMCWORKDIR}/tpc $ENV{CUDA_SDK}/common/inc)


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
	      ${ROOT_LIB}/libGeom.so
	      ${ROOT_LIB}/libGeomPainter.so
	      ${ROOT_LIB}/libVMC.so
	      ${ROOT_LIB}/libEG.so
	      libdl.so
              libm.so
	      )

SET(PANDA_LIB $ENV{VMCWORKDIR}/../build/lib)
SET(TPC_LIB ${PANDA_LIB}/libtpc.so
            ${PANDA_LIB}/libtpcreco.so
	    ${PANDA_LIB}/libgenfit.so
	    ${PANDA_LIB}/librecotasks.so
)


#SET(PANDA_LIB ${PANDA_BINARY_DIR}/lib)
#SET(PANDA_LIBS ${PANDA_LIB}/libGeoBase.so 
#	       ${PANDA_LIB}/libBase.so
#	       ${PANDA_LIB}/libtpc.so
#	       ${PANDA_LIB}/libParBase.so
#               )

SET(SOURCES  ../McId.cxx 
             ../PndTpcDigi.cxx
             ../McIdCollection.cxx
             ../PndTpcDigiAge.cxx
             ../PndTpcDigiAmplitude.cxx
	     ../PndTpcCluster.cxx
	     Hough2DNode.cxx
	     Hough5DNode.cxx
	     Hyperplane5D.cxx
	     fastHoughGPU_IFC.cxx
#	     dataLoader.cc
	     tpcdataDict.cxx
)

SET(CUDASOURCES fastHoughGPU_IFC.cu
                #fastHoughGPU_kernel.cu not needed
		)
  
ADD_LIBRARY(tpccuda SHARED ${CUDASOURCES})
ADD_LIBRARY(tpcdata SHARED ${SOURCES})



FIND_PROGRAM(ROOT_CINT_EXECUTABLE
  NAMES rootcint
  PATHS $ENV{ROOTSYS}/bin
  NO_DEFAULT_PATH
)

ADD_CUSTOM_COMMAND(OUTPUT tpcdataDict.cxx
      COMMAND ${ROOT_CINT_EXECUTABLE}
      ARGS -f tpcdataDict.cxx -c -DHAVE_CONFIG_H ${INCLUDE_DIRECTORIES}
           ../PndTpcDigi.h ../McId.h ../McIdCollection.h ../PndTpcCluster.h 
	   tpcdataLinkDef.h)
	 

# With debug symbols
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
#SET(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -g")
#SET(CMAKE_CUDA_FLAGS "-g -G -arch=sm_11 --ptxas-options -v -use_fast_math")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
SET(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS}")
SET(CMAKE_CUDA_FLAGS "-arch=sm_11 --ptxas-options -v -use_fast_math")


# Attempt to find dependant libraries
FIND_PACKAGE(CUDARuntime)

IF(NOT CUDA_RUNTIME_FOUND)
  MESSAGE(SEND_ERROR "CUDA runtime is required and could not be found.")
ELSE(NOT CUDA_RUNTIME_FOUND)
  INCLUDE_DIRECTORIES(${CUDA_RUNTIME_INCLUDE_DIR})
ENDIF(NOT CUDA_RUNTIME_FOUND)


ADD_EXECUTABLE(fastHoughGPU fastHoughGPU.cxx)
TARGET_LINK_LIBRARIES(fastHoughGPU tpccuda tpcdata
  ${CUDA_RUNTIME_LIBRARIES} ${ROOT_LIBS})

ADD_EXECUTABLE(fastHoughGPU_pretty fastHoughGPU_pretty.cxx)
TARGET_LINK_LIBRARIES(fastHoughGPU_pretty tpccuda tpcdata
  ${CUDA_RUNTIME_LIBRARIES} ${ROOT_LIBS})

#ADD_EXECUTABLE(fastHoughCPU_5D fastHoughCPU_5D.cc)
#TARGET_LINK_LIBRARIES(fastHoughCPU_5D tpccuda tpcdata ${ROOT_LIBS})

ADD_EXECUTABLE(testBit testBitmask.cxx)



