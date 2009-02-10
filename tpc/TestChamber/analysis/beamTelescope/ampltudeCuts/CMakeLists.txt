

###################
PROJECT(alignment)
#SET(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 2.4)


INCLUDE_DIRECTORIES($ENV{ROOTSYS}/include $ENV{ROOTSYS}/include/Math/ /afs/e18/panda/SIM/sdorheim/gemMonitor/src ../../src)
LINK_DIRECTORIES(../../../)

 
SET(SOURCES ampCuts.cpp helpers.cpp)


EXEC_PROGRAM(
		$ENV{ROOTSYS}/bin/root-config
		ARGS --glibs --cflags
		OUTPUT_VARIABLE ROOTCONFIG)
		
SET(ROOT_LIBS ${ROOTCONFIG} -lGraf -lMathCore -lMathMore)

SET(CMAKE_CXX_FLAGS "-g -Wall -pedantic -Wno-long-long -msse2")

#TARGET_LINK_LIBRARIES(test ${ROOTLIBS})

ADD_EXECUTABLE(eventDisp ${SOURCES})
TARGET_LINK_LIBRARIES(eventDisp  ${ROOT_LIBS})
TARGET_LINK_LIBRARIES(eventDisp /afs/e18/panda/SIM/sdorheim/gemMonitor/src/libCsGEMPlane_gcc4.1.2_ROOT5.20.00.so)
TARGET_LINK_LIBRARIES(eventDisp testChamber)
SET_TARGET_PROPERTIES(eventDisp PROPERTIES LINKER_LANGUAGE CXX)
