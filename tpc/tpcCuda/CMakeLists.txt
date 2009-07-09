PROJECT(testprog)

cmake_minimum_required(VERSION 2.6)

INCLUDE_DIRECTORIES($ENV{ROOTSYS}/include)

SET(SOURCES hello.cxx)

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


SET(CMAKE_CXX_FLAGS "-g")

#ADD_LIBRARY(${SOURCES})


#-------------------------------------------------
#------ Root Dictionary --------------------------
#-------------------------------------------------

#FIND_PROGRAM(ROOT_CINT_EXECUTABLE
#  NAMES rootcint
#  PATHS ${ROOT_BIN}
#  NO_DEFAULT_PATH
# )

#ADD_CUSTOM_COMMAND(OUTPUT testprogDict.cc
#      COMMAND ${ROOT_CINT_EXECUTABLE}
#      ARGS -f testprogDict.cc -c -DHAVE_CONFIG_H ${INCLUDE_DIRECTORIES} testprogLinkDef.h)



ADD_EXECUTABLE(hello hello.cxx)
TARGET_LINK_LIBRARIES(hello ${ROOT_LIBS})
