# Create a library called "libgenfit" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES 
  ${SIMPATH}/include
  ${BASE_INCLUDE_DIRECTORIES} 
  ${ROOT_INCLUDE_DIR} 
  ${CLHEP_INCLUDE_DIR} 
  ${CMAKE_SOURCE_DIR}/genfit2
  #{CMAKE_SOURCE_DIR}/genfit/test
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

set(GENFIT_SRCS
./eventDisplay/src/EventDisplay.cc
./core/src/DetPlane.cc
./core/src/AbsMeasurement.cc
./core/src/AbsFitter.cc
./core/src/AbsFitterInfo.cc
./core/src/MaterialProperties.cc
./core/src/AbsFinitePlane.cc
./core/src/Exception.cc
./core/src/TrackCandHit.cc
./core/src/FieldManager.cc
./core/src/TrackPoint.cc
./core/src/TrackCand.cc
./core/src/ThinScatterer.cc
./core/src/FitStatus.cc
./core/src/StateOnPlane.cc
./core/src/Tools.cc
./core/src/MeasuredStateOnPlane.cc
./core/src/MeasurementOnPlane.cc
./core/src/AbsTrackRep.cc
./core/src/Track.cc
./finitePlanes/src/RectangularFinitePlane.cc
./trackReps/src/RKTrackRep.cc
./trackReps/src/StepLimits.cc
./trackReps/src/RKTools.cc
./trackReps/src/TGeoMaterialInterface.cc
./trackReps/src/MaterialEffects.cc
./measurements/src/WirePointMeasurement.cc
./measurements/src/SpacepointMeasurement.cc
./measurements/src/HMatrixPhi.cc
./measurements/src/FullMeasurement.cc
./measurements/src/WireTrackCandHit.cc
./measurements/src/PlanarMeasurement.cc
./measurements/src/ProlateSpacepointMeasurement.cc
./measurements/src/HMatrixUnit.cc
./measurements/src/HMatrixU.cc
./measurements/src/WireMeasurement.cc
./measurements/src/WireMeasurementNew.cc
./measurements/src/HMatrixUV.cc
./measurements/src/HMatrixV.cc
./fitters/src/KalmanFitter.cc
./fitters/src/KalmanFitterInfo.cc
./fitters/src/AbsKalmanFitter.cc
./fitters/src/KalmanFitterRefTrack.cc
./fitters/src/DAF.cc
./fitters/src/KalmanFitStatus.cc
./fitters/src/KalmanFittedStateOnPlane.cc
./fitters/src/ReferenceStateOnPlane.cc
./fields/src/ConstField.cc
)


link_directories( ${LINK_DIRECTORIES})
# Set GENFIT directories
SET(GF_INC_DIRS
        ./core/include/ 
        ./eventDisplay/include/
        ./fields/include/
        ./finitePlanes/include/ 
        ./fitters/include/
#       ./GBL/include/
        ./measurements/include/ 
        ./trackReps/include/
        ./utilities/include/
    ${GF_INC_DIRS}
)


#search for boost, provide some information to user
FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    MESSAGE("-- Boost is found at: ${Boost_INCLUDE_DIR}")
    INSTALL( DIRECTORY 
            ${GF_INC_DIRS}
            DESTINATION ./include/genfit PATTERN ".svn" EXCLUDE 
         )
ELSE()
    MESSAGE( " Boost was not found on your system, using the one shipped with GENFIT.")
    INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/extern/ )
    INSTALL( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/extern/ ${GF_INC_DIRS}  DESTINATION ./include/genfit PATTERN ".svn" EXCLUDE )
ENDIF()

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# include directories 
INCLUDE_DIRECTORIES( 
        BEFORE 
        ${GF_INC_DIRS}
        ${ROOT_INCLUDE_DIRS})


# declare the ROOT library path
LINK_DIRECTORIES(${ROOT_LIBRARY_DIR})

# add the list of source code files -- careful, when a new file is added!
AUX_SOURCE_DIRECTORY( ./core/src          library_sources )
AUX_SOURCE_DIRECTORY( ./eventDisplay/src  library_sources )
AUX_SOURCE_DIRECTORY( ./fields/src        library_sources )
AUX_SOURCE_DIRECTORY( ./finitePlanes/src  library_sources )
AUX_SOURCE_DIRECTORY( ./fitters/src       library_sources )
#AUX_SOURCE_DIRECTORY( ./GBL/src           library_sources )
AUX_SOURCE_DIRECTORY( ./measurements/src  library_sources )
AUX_SOURCE_DIRECTORY( ./trackReps/src     library_sources )
#AUX_SOURCE_DIRECTORY( ./utilities/src     library_sources )


if(RULE_CHECKER_FOUND)
  CHECK_RULES("${GENFIT_SRCS}" "${INCLUDE_DIRECTORIES}" GENFIT_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
set(GENFIT_HEADERS
  ./eventDisplay/include/EventDisplay.h
  ./core/include/FitStatus.h
  ./core/include/MeasuredStateOnPlane.h
  ./core/include/AbsFinitePlane.h
  ./core/include/AbsTrackRep.h
  ./core/include/TrackCand.h
  ./core/include/DetPlane.h
  ./core/include/StateOnPlane.h
  ./core/include/MeasurementFactory.h
  ./core/include/MaterialProperties.h
  ./core/include/AbsBField.h
  ./core/include/AbsHMatrix.h
  ./core/include/AbsFitter.h
  ./core/include/FieldManager.h
  ./core/include/AbsFitterInfo.h
  ./core/include/AbsMeasurement.h
  ./core/include/ThinScatterer.h
  ./core/include/MeasurementOnPlane.h
  ./core/include/MeasurementProducer.h
  ./core/include/Track.h
  ./core/include/Tools.h
  ./core/include/TrackCandHit.h
  ./core/include/SharedMaterialPropertiesPtr.h
  ./core/include/Exception.h
  ./core/include/SharedPlanePtr.h
  ./core/include/TrackPoint.h
  ./finitePlanes/include/RectangularFinitePlane.h
  ./trackReps/include/StepLimits.h
  ./trackReps/include/AbsMaterialInterface.h
  ./trackReps/include/RKTools.h
  ./trackReps/include/MaterialEffects.h
  ./trackReps/include/RKTrackRep.h
  ./trackReps/include/TGeoMaterialInterface.h
  ./measurements/include/HMatrixPhi.h
  ./measurements/include/HMatrixUV.h
  ./measurements/include/ProlateSpacepointMeasurement.h
  ./measurements/include/PlanarMeasurement.h
  ./measurements/include/HMatrixUnit.h
  ./measurements/include/WirePointMeasurement.h
  ./measurements/include/SpacepointMeasurement.h
  ./measurements/include/WireTrackCandHit.h
  ./measurements/include/WireMeasurement.h
  ./measurements/include/WireMeasurementNew.h
  ./measurements/include/HMatrixV.h
  ./measurements/include/FullMeasurement.h
  ./measurements/include/HMatrixU.h
  ./fitters/include/KalmanFitterInfo.h
  ./fitters/include/KalmanFitStatus.h
  ./fitters/include/KalmanFittedStateOnPlane.h
  ./fitters/include/KalmanFitter.h
  ./fitters/include/ReferenceStateOnPlane.h
  ./fitters/include/KalmanFitterRefTrack.h
  ./fitters/include/AbsKalmanFitter.h
  ./fitters/include/DAF.h
  ./fields/include/ConstField.h
)

#set(GENFIT_LINKDEF  genfitLinkDef.h)
#set(GENFIT_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/genfitDict.cxx) 


SET(CORE_DICTIONARY_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/AbsFinitePlane.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/AbsFitterInfo.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/AbsHMatrix.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/AbsTrackRep.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/DetPlane.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/FitStatus.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/MaterialProperties.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/MeasuredStateOnPlane.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/MeasurementFactory.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/MeasurementOnPlane.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/MeasurementProducer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/StateOnPlane.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/AbsMeasurement.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/ThinScatterer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/Track.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/TrackCand.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/TrackCandHit.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include/TrackPoint.h
  ${CMAKE_CURRENT_SOURCE_DIR}/finitePlanes/include/RectangularFinitePlane.h
)



#SET(GENFIT_SRCS ${GENFIT_SRCS} ${GENFIT_DICTIONARY} ${INCLUDE_DIRECTORIES} )
SET(GENFIT_SRCS ${GENFIT_SRCS} ${INCLUDE_DIRECTORIES} )

ROOT_GENERATE_DICTIONARY("${CORE_DICTIONARY_SOURCES}"
                         "${CMAKE_CURRENT_SOURCE_DIR}/core/src/coreLinkDef.h"
                         "${CMAKE_CURRENT_BINARY_DIR}/coreRootDict.cxx"
                         "${CMAKE_CURRENT_SOURCE_DIR}/core/include"
)
                         
SET(library_sources "${CMAKE_CURRENT_BINARY_DIR}/coreRootDict.cxx" "${library_sources}")


SET(EVENTDISPLAY_DICTIONARY_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/eventDisplay/include/EventDisplay.h
)
ROOT_GENERATE_DICTIONARY("${EVENTDISPLAY_DICTIONARY_SOURCES}"
                         "${CMAKE_CURRENT_SOURCE_DIR}/eventDisplay/src/eventDisplayLinkDef.h"
                         "${CMAKE_CURRENT_BINARY_DIR}/eventDisplayRootDict.cxx"
                         "${CMAKE_CURRENT_SOURCE_DIR}/core/include;${CMAKE_CURRENT_SOURCE_DIR}/fitters/include;${CMAKE_CURRENT_SOURCE_DIR}/eventDisplay/include"
)
                        
SET(library_sources "${CMAKE_CURRENT_BINARY_DIR}/eventDisplayRootDict.cxx" "${library_sources}")



SET(FITTERS_DICTIONARY_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/fitters/include/AbsKalmanFitter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fitters/include/KalmanFittedStateOnPlane.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fitters/include/KalmanFitStatus.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fitters/include/ReferenceStateOnPlane.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fitters/include/KalmanFitterRefTrack.h

  ${CMAKE_CURRENT_SOURCE_DIR}/fitters/include/KalmanFitter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fitters/include/KalmanFitterInfo.h
  ${CMAKE_CURRENT_SOURCE_DIR}/fitters/include/DAF.h
)


ROOT_GENERATE_DICTIONARY("${FITTERS_DICTIONARY_SOURCES}"
                         "${CMAKE_CURRENT_SOURCE_DIR}/fitters/src/fittersLinkDef.h"
                         "${CMAKE_CURRENT_BINARY_DIR}/fittersRootDict.cxx"
                         "${CMAKE_CURRENT_SOURCE_DIR}/core/include;${CMAKE_CURRENT_SOURCE_DIR}/fitters/include"
)
SET(library_sources "${CMAKE_CURRENT_BINARY_DIR}/fittersRootDict.cxx" "${library_sources}")



SET(MEASUREMENTS_DICTIONARY_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/HMatrixPhi.h
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/HMatrixU.h
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/HMatrixUnit.h
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/HMatrixV.h
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/HMatrixUV.h
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/FullMeasurement.h
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/PlanarMeasurement.h
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/ProlateSpacepointMeasurement.h
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/SpacepointMeasurement.h
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/WireMeasurement.h
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/WireMeasurementNew.h
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/WirePointMeasurement.h
  ${CMAKE_CURRENT_SOURCE_DIR}/measurements/include/WireTrackCandHit.h
)
ROOT_GENERATE_DICTIONARY("${MEASUREMENTS_DICTIONARY_SOURCES}"
                         "${CMAKE_CURRENT_SOURCE_DIR}/measurements/src/measurementsLinkDef.h"
                         "${CMAKE_CURRENT_BINARY_DIR}/measurementsRootDict.cxx"
                         "${CMAKE_CURRENT_SOURCE_DIR}/core/include;${CMAKE_CURRENT_SOURCE_DIR}/measurements/include"
)
SET(library_sources "${CMAKE_CURRENT_BINARY_DIR}/measurementsRootDict.cxx" "${library_sources}")

SET(TRACKREPS_DICTIONARY_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/trackReps/include/RKTrackRep.h
)


ROOT_GENERATE_DICTIONARY("${TRACKREPS_DICTIONARY_SOURCES}"
                         "${CMAKE_CURRENT_SOURCE_DIR}/trackReps/src/trackRepsLinkDef.h"
                         "${CMAKE_CURRENT_BINARY_DIR}/trackRepsRootDict.cxx"
                         "${CMAKE_CURRENT_SOURCE_DIR}/core/include;${CMAKE_CURRENT_SOURCE_DIR}/trackReps/include"
)
SET(library_sources "${CMAKE_CURRENT_BINARY_DIR}/trackRepsRootDict.cxx" "${library_sources}")


#SET(UTILITIES_DICTIONARY_SOURCES
#  ${CMAKE_CURRENT_SOURCE_DIR}/utilities/include/HelixTrackModel.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/utilities/include/MeasurementCreator.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/utilities/include/mySpacepointDetectorHit.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/utilities/include/mySpacepointMeasurement.h
#)
#ROOT_GENERATE_DICTIONARY("${UTILITIES_DICTIONARY_SOURCES}"
#                         "${CMAKE_CURRENT_SOURCE_DIR}/core/include;${CMAKE_CURRENT_SOURCE_DIR}/utilities/include;${CMAKE_CURRENT_SOURCE_DIR}/measurements/include"
#                         "${CMAKE_CURRENT_SOURCE_DIR}/utilities/src/utilitiesLinkDef.h"
#                         "${CMAKE_CURRENT_BINARY_DIR}/utilitiesRootDict.cxx")
#SET(library_sources "${CMAKE_CURRENT_BINARY_DIR}/utilitiesRootDict.cxx" "${library_sources}")

set(DEPENDENCIES  ${ROOT_LIBS})

#set(LIBRARY_NAME genfit2)

#GENERATE_LIBRARY()
MESSAGE(${library_sources})
MESSAGE(${GENFIT_SRCS})
add_library(genfit2 SHARED ${library_sources} ${GENFIT_SRCS})
target_link_libraries(genfit2 ${ROOT_LIBRARIES} -lEve)
set_target_properties(genfit2 PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES} LINKER_LANGUAGE CXX)

#add_library(genfit SHARED ${GENFIT_SRCS})
#TARGET_LINK_LIBRARIES(genfit ${ROOT_LIBRARIES})
#set_target_properties(genfit PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})

#install(TARGETS genfit DESTINATION ${CMAKE_BINARY_DIR}/lib)
#INSTALL( TARGETS ${PROJECT_NAME} DESTINATION lib )
#ADD_CUSTOM_TARGET( tests )

#MACRO( ADD_GENFIT_TEST _testname )
 #  ADD_EXECUTABLE( ${_testname} EXCLUDE_FROM_ALL ${ARGN} )
 #  ADD_DEPENDENCIES( tests  ${_testname} )
#   TARGET_LINK_LIBRARIES( ${_testname} ${PROJECT_NAME}  )

