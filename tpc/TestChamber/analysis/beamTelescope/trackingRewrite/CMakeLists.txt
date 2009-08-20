

###################
PROJECT(alignment)
#SET(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 2.4)


INCLUDE_DIRECTORIES($ENV{ROOTSYS}/include 
  $ENV{ROOTSYS}/include/Math/ 
  /home/sverre/work/gemMonitor/gemMonitor/src/CsGEM/ 
  ../../../src/ 
  ../../../../tpcCuda/
  ../../../../)
LINK_DIRECTORIES(../../../ /home/sverre/work/gemMonitor/gemMonitor/lib/)

 
SET(SOURCES convert.cxx)
SET(SOURCES2 patternReco.cxx ../../../src/TCfast2Dhough.cxx ../../../../tpcCuda/Hyperplane2D.cxx ../../../../tpcCuda/Hough2DNode.cxx)


EXEC_PROGRAM(
  $ENV{ROOTSYS}/bin/root-config
  ARGS --glibs --cflags
  OUTPUT_VARIABLE ROOTCONFIG)
		
SET(ROOT_LIBS ${ROOTCONFIG} -lGraf -lMathCore -lMathMore)

SET(CMAKE_CXX_FLAGS "-g -Wall -Wno-long-long -msse2")

#TARGET_LINK_LIBRARIES(test ${ROOTLIBS})

ADD_EXECUTABLE(converter ${SOURCES})
TARGET_LINK_LIBRARIES(converter  ${ROOT_LIBS})
TARGET_LINK_LIBRARIES(converter /home/sverre/work/gemMonitor/gemMonitor/lib/libCsGEM.so)
TARGET_LINK_LIBRARIES(converter testChamber)
SET_TARGET_PROPERTIES(converter PROPERTIES LINKER_LANGUAGE CXX)
ADD_EXECUTABLE(patternReco ${SOURCES2})
TARGET_LINK_LIBRARIES(patternReco  ${ROOT_LIBS})
TARGET_LINK_LIBRARIES(patternReco testChamber)
SET_TARGET_PROPERTIES(patternReco PROPERTIES LINKER_LANGUAGE CXX)
