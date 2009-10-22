

###################
PROJECT(alignment)
#SET(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 2.4)


INCLUDE_DIRECTORIES($ENV{ROOTSYS}/include 
  $ENV{ROOTSYS}/include/Math/ 
  /afs/e18/panda/SIM/sdorheim/gemMonitors/gemMonitorSUHL/src/CsGEM/ 
  $ENV{VMCWORKDIR}/tpc/TestChamber/src/ 
  $ENV{VMCWORKDIR}/tpc/tpcCuda/
  $ENV{VMCWORKDIR}/tpc
  $ENV{SIMPATH}/basics/boost/boost)
LINK_DIRECTORIES(../../../ /afs/e18/panda/SIM/sdorheim/gemMonitors/gemMonitorSUHL/lib/  $ENV{SIMPATH}/basics/boost/lib)

 
SET(SOURCES convert.cxx)
SET(SOURCES2 patternReco.cxx ../../../src/TCfast2Dhough.cxx ../../../../tpcCuda/Hyperplane2D.cxx ../../../../tpcCuda/Hough2DNode.cxx)
SET(SOURCES3 fit.cxx histContainers.cxx)
SET(SOURCES4 trackHist.cxx)
SET(SOURCES5 fit_both.cxx histContainers.cxx)


EXEC_PROGRAM(
  $ENV{ROOTSYS}/bin/root-config
  ARGS --glibs --cflags
  OUTPUT_VARIABLE ROOTCONFIG)
		
SET(ROOT_LIBS ${ROOTCONFIG} -lGraf -lMathCore -lMathMore)

SET(CMAKE_CXX_FLAGS "-g -Wall -Wno-long-long -msse2")

#TARGET_LINK_LIBRARIES(test ${ROOTLIBS})

ADD_EXECUTABLE(converter ${SOURCES})
TARGET_LINK_LIBRARIES(converter  ${ROOT_LIBS})
TARGET_LINK_LIBRARIES(converter /afs/e18/panda/SIM/sdorheim/gemMonitors/gemMonitorSUHL/lib/libCsGEM.so)
TARGET_LINK_LIBRARIES(converter testChamber)
SET_TARGET_PROPERTIES(converter PROPERTIES LINKER_LANGUAGE CXX)
ADD_EXECUTABLE(patternReco ${SOURCES2})
TARGET_LINK_LIBRARIES(patternReco  ${ROOT_LIBS})
TARGET_LINK_LIBRARIES(patternReco testChamber)
SET_TARGET_PROPERTIES(patternReco PROPERTIES LINKER_LANGUAGE CXX)
ADD_EXECUTABLE(fit ${SOURCES3})
TARGET_LINK_LIBRARIES(fit  ${ROOT_LIBS})
TARGET_LINK_LIBRARIES(fit testChamber)
SET_TARGET_PROPERTIES(fit PROPERTIES LINKER_LANGUAGE CXX)
ADD_EXECUTABLE(fitBoth ${SOURCES5})
TARGET_LINK_LIBRARIES(fitBoth  ${ROOT_LIBS})
TARGET_LINK_LIBRARIES(fitBoth testChamber)
SET_TARGET_PROPERTIES(fitBoth PROPERTIES LINKER_LANGUAGE CXX)
ADD_EXECUTABLE(hist ${SOURCES4})
TARGET_LINK_LIBRARIES(hist  ${ROOT_LIBS})
TARGET_LINK_LIBRARIES(hist testChamber)
SET_TARGET_PROPERTIES(hist PROPERTIES LINKER_LANGUAGE CXX)
