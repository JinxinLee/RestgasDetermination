//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      abstract interface for a hit-track correlator
//      for the riemann track finder
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCABSHITTRACKCORRELATOR_HH
#define TPCABSHITTRACKCORRELATOR_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------


// Collaborating Class Declarations --
class PndTpcRiemannTrack;
class PndTpcRiemannHit;


class PndTpcAbsHitTrackCorrelator {
public:

  // Constructors/Destructors ---------
  PndTpcAbsHitTrackCorrelator(){}
  virtual ~PndTpcAbsHitTrackCorrelator(){}

  // correlation method:
  // returns true if Correlator is applicable to this track/hit combo
  // survival is set to true if trk survives the correlator
  // in this case matchQuality is 
  // a measure of how well the hit lies on the track w.r.t. this correlator
  virtual bool corr(PndTpcRiemannTrack* trk,
		    PndTpcRiemannHit* rhit,
		    bool& survival,
		    double& matchQuality)=0;

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
