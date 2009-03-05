

###################
PROJECT(alignment)
#SET(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 2.4)

enable_language(Fortran)

INCLUDE_DIRECTORIES($ENV{ROOTSYS}/include)
LINK_DIRECTORIES(../)


SET(SOURCES src/track.cpp src/detector.cpp src/millepede.f src/main.cpp src/alignment.cpp)


EXEC_PROGRAM(
		$ENV{ROOTSYS}/bin/root-config
		ARGS --glibs --cflags
		OUTPUT_VARIABLE ROOTCONFIG)
		
SET(ROOT_LIBS ${ROOTCONFIG})

SET(CMAKE_CXX_FLAGS "-g -Wall -pedantic -Wno-long-long")

#TARGET_LINK_LIBRARIES(align ${ROOTLIBS})

ADD_EXECUTABLE(alignment ${SOURCES})
TARGET_LINK_LIBRARIES(alignment ${ROOT_LIBS})
TARGET_LINK_LIBRARIES(alignment gfortran)
TARGET_LINK_LIBRARIES(alignment testChamber)
SET_TARGET_PROPERTIES(alignment PROPERTIES LINKER_LANGUAGE CXX)
