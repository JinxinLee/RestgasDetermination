

###################
PROJECT(alignment)
#SET(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 2.4)


INCLUDE_DIRECTORIES($ENV{ROOTSYS}/include $ENV{ROOTSYS}/include/Math/ /home/sverre/work/gemMonitor/gemMonitor/src/CsGEM/ ../../src)
LINK_DIRECTORIES(../../../ /home/sverre/work/gemMonitor/gemMonitor/lib/)

 
SET(SOURCES tracking.cxx helpers.cpp ../../../src/TChough2.cxx)


EXEC_PROGRAM(
		$ENV{ROOTSYS}/bin/root-config
		ARGS --glibs --cflags
		OUTPUT_VARIABLE ROOTCONFIG)
		
SET(ROOT_LIBS ${ROOTCONFIG} -lGraf -lMathCore -lMathMore)

SET(CMAKE_CXX_FLAGS "-g -Wall -Wno-long-long -msse2")

#TARGET_LINK_LIBRARIES(test ${ROOTLIBS})

ADD_EXECUTABLE(tracker ${SOURCES})
TARGET_LINK_LIBRARIES(tracker  ${ROOT_LIBS})
TARGET_LINK_LIBRARIES(tracker /home/sverre/work/gemMonitor/gemMonitor/lib/libCsGEM.so)
TARGET_LINK_LIBRARIES(tracker testChamber)
SET_TARGET_PROPERTIES(tracker PROPERTIES LINKER_LANGUAGE CXX)
