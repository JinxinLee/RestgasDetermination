//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Hit-Track-Correlator using Helix fit
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Johanness Rauch    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCHELIXHTCORRELATOR_HH
#define TPCHELIXHTCORRELATOR_HH

// Base Class Headers ----------------
#include "PndTpcAbsHitTrackCorrelator.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TH1D;


class PndTpcHelixHTCorrelator : public PndTpcAbsHitTrackCorrelator {
public:

  // Constructors/Destructors ---------
  PndTpcHelixHTCorrelator(double hdist);
  ~PndTpcHelixHTCorrelator(){;}

  // Accessors -----------------------
  TH1D* getPlaneHisto() {return _helixdist;}
  

  // Modifiers -----------------------


  // Operations ----------------------
  virtual bool corr(PndTpcRiemannTrack* trk,
		    PndTpcRiemannHit* rhit,
		    bool& survive,
		    double& matchQuality);

private:

  // Private Data Members ------------
  double _hdistcut;

  // histograms for logging
  TH1D* _helixdist;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
