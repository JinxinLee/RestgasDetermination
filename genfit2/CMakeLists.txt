# Create a library called "libgenfit" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
  ${BASE_INCLUDE_DIRECTORIES} 
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR} 
)

Set(INCLUDE_DIRECTORIES 
  ${CMAKE_SOURCE_DIR}/genfit2
  ${CMAKE_SOURCE_DIR}/genfit2/core/include
  ${CMAKE_SOURCE_DIR}/genfit2/eventDisplay/include
  ${CMAKE_SOURCE_DIR}/genfit2/fields/include
  ${CMAKE_SOURCE_DIR}/genfit2/finitePlanes/include
  ${CMAKE_SOURCE_DIR}/genfit2/fitters/include
  ${CMAKE_SOURCE_DIR}/genfit2/measurements/include
  ${CMAKE_SOURCE_DIR}/genfit2/trackReps/include
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
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


set(HEADERS
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

set(LINKDEF  genfit2LinkDef.h)
set(LIBRARY_NAME genfit2)

set(DEPENDENCIES ${ROOT_LIBS})

GENERATE_LIBRARY()
