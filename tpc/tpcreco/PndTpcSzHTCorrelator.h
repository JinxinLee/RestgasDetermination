//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Hit-Track-Correlator using sz fit
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Johannes Rauch       TUM
//
//
//-----------------------------------------------------------

#ifndef TPCSZHTCORRELATOR_HH
#define TPCSZHTCORRELATOR_HH

// Base Class Headers ----------------
#include "PndTpcAbsHitTrackCorrelator.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TH1D;


class PndTpcSzHTCorrelator : public PndTpcAbsHitTrackCorrelator {
public:

  // Constructors/Destructors ---------
  PndTpcSzHTCorrelator(double szcut);
  ~PndTpcSzHTCorrelator(){;}

  // Accessors -----------------------
  TH1D* getSZHisto() {return _hsz;}
  

  // Modifiers -----------------------


  // Operations ----------------------
  virtual bool corr(PndTpcRiemannTrack* trk,
		    PndTpcRiemannHit* rhit,
		    bool& survive,
		    double& matchQuality);

private:

  // Private Data Members ------------
  double _szcut;

  // histograms for logging
  TH1D* _hsz;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
