//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Hit-Track-Correlator using proximity arguments
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

#ifndef TPCPROXIMITYHTCORRELATOR_HH
#define TPCPROXIMITYHTCORRELATOR_HH

// Base Class Headers ----------------
#include "PndTpcAbsHitTrackCorrelator.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --



class PndTpcProximityHTCorrelator : public PndTpcAbsHitTrackCorrelator {
public:

  // Constructors/Destructors ---------
  PndTpcProximityHTCorrelator(double cut);
  ~PndTpcProximityHTCorrelator(){;}

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual bool corr(PndTpcRiemannTrack* trk,
		    PndTpcRiemannHit* rhit,
		    bool& survive,
		    double& matchQuality);

private:

  // Private Data Members ------------
  double _proxcut;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
