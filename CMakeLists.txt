# The name of our project is "PANDAROOT".  CMakeLists files in this project can
# refer to the root source directory of the project as ${PANDAROOT_SOURCE_DIR} 
# or as ${CMAKE_SOURCE_DIR} and to the root binary directory of the project as 
# ${PANDAROOT_BINARY_DIR} or ${CMAKE_BINARY_DIR}.
# This difference is important for the base classes which are in FAIRROOT 
# and PANDAROOT.

# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

# Set name of our project to "PANDAROOT". Has to be done
# after check of cmake version since this is a new feature                  
project(PANDAROOT)
ENABLE_LANGUAGE(Fortran)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ 
# is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# Load some basic macros which are needed later on
include(FairMacros)
include(WriteConfigFile)
include(CTest)
include(CheckCompiler)
include(CheckFortran)

#Check the compiler and set the compile and link flags
If(NOT CMAKE_BUILD_TYPE)
  Message("Set BuildType DEBUG")
  set(CMAKE_BUILD_TYPE Debug)
EndIf(NOT CMAKE_BUILD_TYPE)
Check_Compiler()

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

# Check if the user wants to build the project in the source
# directory
CHECK_OUT_OF_SOURCE_BUILD()

# Check if we are on an UNIX system. If not stop with an error
# message
IF(NOT UNIX)
  MESSAGE(FATAL_ERROR "You're not on an UNIX system. The project was up to now only tested on UNIX systems, so we break here. If you want to go on please edit the CMakeLists.txt in the source directory.")
ENDIF(NOT UNIX)  

# Check if we are at GSI
# If we are GSI set some variables
IF(NOT DEFINED ENV{SIMPATH})
   MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again.") 
ENDIF(NOT DEFINED ENV{SIMPATH})
STRING(REGEX MATCHALL "[^:]+" PATH $ENV{PATH})
SET(SIMPATH $ENV{SIMPATH})

# Set the build type. Possibilities are None, Debug, Release, 
# RelWithDebInfo and MinSizeRel 
#SET(CMAKE_BUILD_TYPE Debug)

# searches for needed packages
# REQUIRED means that cmake will stop if this packages are not found
# For example the framework can run without GEANT4, but ROOT is
# mandatory

SET(ROOT_MIN_VERSION "5.24/00")

find_package(ROOT REQUIRED)

IF(ROOT_INSTALLED_VERSION_TOO_OLD)
  MESSAGE( FATAL_ERROR "Invalid ROOT version \"${ROOTERSION}\", at least version  \"5.24/00\" is required, or external packages jul09")
ENDIF (ROOT_INSTALLED_VERSION_TOO_OLD)

find_package(PLUTO REQUIRED)
find_package(glpk REQUIRED)
find_package(Pythia8 REQUIRED)
find_package(GENERATORS REQUIRED)
find_package(GEANT3 REQUIRED)
find_package(GEANT4)
find_package(GEANT4DATA)
find_package(GEANT4VMC)
find_package(CLHEP)
find_package(CERNLIB)

find_package(RuleChecker)


# Set the library version in the main CMakeLists.txt
SET(FAIRROOT_MAJOR_VERSION 0)
SET(FAIRROOT_MINOR_VERSION 0)
SET(FAIRROOT_PATCH_VERSION 0)
SET(FAIRROOT_VERSION "${FAIRROOT_MAJOR_VERSION}.${FAIRROOT_MINOR_VERSION}.${FAIRROOT_PATCH_VERSION}")
SET(FAIRROOT_LIBRARY_PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES}
    VERSION "${FAIRROOT_VERSION}"
    SOVERSION "${FAIRROOT_MAJOR_VERSION}"
    SUFFIX ".so"
)
# Configure FairVersion.h
# ------------------------------
find_package(Subversion) 
if(Subversion_FOUND)
  if(EXISTS ${PROJECT_SOURCE_DIR}/.svn/ )
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} PROJECT)
    set(FAIRROOT_SVN_REVISION ${PROJECT_WC_REVISION})
    set(FAIRROOT_SVN_BRANCH ${PROJECT_WC_URL})
    string(REGEX MATCH "[^/]+$" FAIRROOT_SVN_BRANCH ${FAIRROOT_SVN_BRANCH})
  endif(EXISTS ${PROJECT_SOURCE_DIR}/.svn/ )

  message(STATUS "FairRoot Revision - ${FAIRROOT_SVN_REVISION} Branch - ${FAIRROOT_SVN_BRANCH}") 
  if(EXISTS ${PROJECT_SOURCE_DIR}/FairVersion.h)
    file(REMOVE ${PROJECT_SOURCE_DIR}/FairVersion.h)
  endif(EXISTS ${PROJECT_SOURCE_DIR}/FairVersion.h)
  configure_file(${PROJECT_SOURCE_DIR}/cmake/scripts/FairVersion.h.tmp ${PROJECT_SOURCE_DIR}/FairVersion.h @ONLY)
endif(Subversion_FOUND)

SET(CBMLIBDIR ${CMAKE_BINARY_DIR}/lib)
SET(LD_LIBRARY_PATH  ${CBMLIBDIR} ${LD_LIBRARY_PATH})


#IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#  SET(CMAKE_INSTALL_PREFIX "./install" CACHE PATH "FOO install prefix" FORCE)
#  Message("Set default install path ...")
#ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

install(DIRECTORY geometry DESTINATION pnd_install 
          PATTERN ".svn" EXCLUDE
          )

# Recurse into the given subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.
add_subdirectory (fairtools)
add_subdirectory (base)
add_subdirectory (geobase)
add_subdirectory (parbase)
add_subdirectory (dbase)
add_subdirectory (PndTools/generalTools)
add_subdirectory (PndTools/buffers)
add_subdirectory (pnddata)
add_subdirectory (passive)
add_subdirectory (field)  
add_subdirectory (generators)
add_subdirectory (pgenerators)
add_subdirectory (pgenerators/DpmEvtGen)
add_subdirectory (pgenerators/EvtGen)
add_subdirectory (pgenerators/EvtGenDirect)
add_subdirectory (pgenerators/PndFlukaGen)
add_subdirectory (pgenerators/EMFFgenerators)
add_subdirectory (stt)
#add_subdirectory (stt/sttreco)
add_subdirectory (sds)
add_subdirectory (mvd)
add_subdirectory (sttmvdtracking)
add_subdirectory (mdt)
add_subdirectory (emc)
add_subdirectory (drc)
add_subdirectory (drc/drcprop)
add_subdirectory (fts)
add_subdirectory (hyp)
add_subdirectory (genfit)
add_subdirectory (scitil)
add_subdirectory (ftof)
add_subdirectory (gem)
add_subdirectory (macro)
add_subdirectory (trackbase)
add_subdirectory (geane)
add_subdirectory (GenfitTools)
#add_subdirectory(hypGe)
add_subdirectory (fsim)
add_subdirectory(rho)

FIND_PATH(TEvePath NAMES TEveEventManager.h PATHS
  ${SIMPATH}/tools/root/include
  ${SIMPATH}/include/root
  NO_DEFAULT_PATH
)

If(TEvePath)
#find_package(OpenGL)
#If (OPENGL_FOUND AND OPENGL_GLU_FOUND)
  Message("Found TEve, so the eventdisplay will be compiled.")
  add_subdirectory(eventdisplay)
  add_subdirectory(PndEventdisplay)
#EndIf (OPENGL_FOUND AND OPENGL_GLU_FOUND)
EndIf(TEvePath)

add_subdirectory (dsk)
add_subdirectory (PndTools/riemannfit)
add_subdirectory(PndTools/AnalysisTools)
add_subdirectory (PndTools/MVA)
add_subdirectory(lmd)
add_subdirectory (pid)
add_subdirectory (PndMCMatch)
add_subdirectory (global)

if(RULE_CHECKER_FOUND)

  ADD_CUSTOM_TARGET(RULES 
     COMMAND ${RULE_CHECKER_SCRIPT1} ${CMAKE_BINARY_DIR} viol > violations.html
     DEPENDS $ENV{ALL_RULES})

endif(RULE_CHECKER_FOUND)

WRITE_CONFIG_FILE(config.sh)
 
configure_file(${CMAKE_SOURCE_DIR}/CTestCustom.cmake 
               ${CMAKE_BINARY_DIR}/CTestCustom.cmake
              )


