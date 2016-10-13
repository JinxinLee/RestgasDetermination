// $Id: genfit2LinkDef.h,v 1.0 Radoslaw Karabowicz, 03.06.2016 $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#ifdef __CLING__
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ namespace genfit;
#endif

#pragma link C++ class genfit::AbsFinitePlane+;
#pragma link C++ class genfit::AbsHMatrix+;
#pragma link C++ class genfit::RectangularFinitePlane+;
#pragma link C++ class genfit::FitStatus+;
#pragma link C++ class genfit::MaterialProperties+;
#pragma link C++ class genfit::PruneFlags+;
#pragma link C++ class genfit::TrackCand+;
#pragma link C++ class genfit::TrackCandHit+;
#pragma link C++ class genfit::AbsTrackRep+;
#pragma link C++ class genfit::RKTrackRep-;
#pragma link C++ class genfit::MeasuredStateOnPlane+;
#pragma link C++ class genfit::AbsMeasurement+; // trackPoint_
#pragma link C++ class genfit::AbsFitterInfo-; // trackPoint_, rep_, sharedPlanePtr
#pragma link C++ class genfit::DetPlane-;  // scoped_ptr<> finitePlane_
#pragma link C++ class genfit::MeasurementOnPlane-; // scoped_ptr<> hMatrix_
#pragma link C++ class genfit::StateOnPlane-;  // rep_, sharedPlanePtr
#pragma link C++ class genfit::ThinScatterer-; // sharedPlanePtr
#pragma link C++ class genfit::Track-;
#pragma link C++ class genfit::TrackPoint-; // track_, fixup the map

#pragma read sourceClass="genfit::FitStatus" version="[1]" \
  targetClass="genfit::FitStatus"                          \
  source="bool trackIsPruned_;" target="pruneFlags_"       \
  code="{ pruneFlags_.setFlags(); }"
// Prune flag wasn't actually written as no streamer was available.
#pragma read sourceClass="genfit::FitStatus" version="[2]" \
  targetClass="genfit::FitStatus"                          \
  source="" target="pruneFlags_"                           \
  code="{ pruneFlags_.setFlags(); }"

// Time for the TrackCand was only introduced in version 2.  Default to zero.
#pragma read sourceClass="genfit::TrackCand" version="[1]" \
  targetClass="genfit::TrackCand"                          \
  source="" target="time_"                                 \
  code="{ time_ = 0; }"

#pragma link C++ class genfit::EventDisplay+;

#pragma link C++ class genfit::AbsKalmanFitter+;
#pragma link C++ class genfit::KalmanFitStatus;
#pragma link C++ class genfit::KalmanFitterRefTrack+;

// these inherit from classes that need custom streamers
#pragma link C++ class genfit::KalmanFittedStateOnPlane+;
#pragma link C++ class genfit::ReferenceStateOnPlane+;

// Classes that needed manually written streamers:
#pragma link C++ class genfit::KalmanFitter-;
#pragma link C++ class genfit::KalmanFitterInfo-;
#pragma link C++ class genfit::DAF-;


#pragma link C++ class genfit::HMatrixU+;
#pragma link C++ class genfit::HMatrixUnit+;
#pragma link C++ class genfit::HMatrixV+;
#pragma link C++ class genfit::HMatrixUV+;
#pragma link C++ class genfit::ProlateSpacepointMeasurement+;
#pragma link C++ class genfit::WireMeasurement+;
#pragma link C++ class genfit::WireMeasurementNew+;
#pragma link C++ class genfit::WirePointMeasurement+;

#pragma link C++ class genfit::HMatrixPhi-;
#pragma link C++ class genfit::FullMeasurement-;
#pragma link C++ class genfit::PlanarMeasurement-;
#pragma link C++ class genfit::SpacepointMeasurement-;

#pragma link C++ class genfit::WireTrackCandHit+;

#pragma link C++ class genfit::HMatrixU+;
#pragma link C++ class genfit::HMatrixUnit+;
#pragma link C++ class genfit::HMatrixV+;
#pragma link C++ class genfit::HMatrixUV+;
#pragma link C++ class genfit::ProlateSpacepointMeasurement+;
#pragma link C++ class genfit::WireMeasurement+;
#pragma link C++ class genfit::WireMeasurementNew+;
#pragma link C++ class genfit::WirePointMeasurement+;

#pragma link C++ class genfit::HMatrixPhi-;
#pragma link C++ class genfit::FullMeasurement-;
#pragma link C++ class genfit::PlanarMeasurement-;
#pragma link C++ class genfit::SpacepointMeasurement-;

#pragma link C++ class genfit::WireTrackCandHit+;

#endif
