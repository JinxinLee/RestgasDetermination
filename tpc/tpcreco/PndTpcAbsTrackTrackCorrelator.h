//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      abstract interface for a track-track correlator
//      for the riemann track finder
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Johannes Rauch    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCABSTRACKTRACKCORRELATOR_HH
#define TPCABSTRACKTRACKCORRELATOR_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------


// Collaborating Class Declarations --
class PndTpcRiemannTrack;
class PndTpcRiemannHit;


class PndTpcAbsTrackTrackCorrelator {
public:

  // Constructors/Destructors ---------
  PndTpcAbsTrackTrackCorrelator(){}
  virtual ~PndTpcAbsTrackTrackCorrelator(){}

  // correlation method:
  // returns true if correlator is applicable to this track/track combo
  // survival is set to true if tracks survives the correlator
  // in this case matchQuality is 
  // a measure of how well the tracks correlate
  virtual bool corr(PndTpcRiemannTrack* trk1,
        PndTpcRiemannTrack* trk2,
		    bool& survive,
		    double& matchQuality)=0;

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
