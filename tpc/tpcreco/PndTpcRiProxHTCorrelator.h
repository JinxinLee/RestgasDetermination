//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Hit-Track-Correlator using riemann fit
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

#ifndef TPCRIPROXHTCORRELATOR_HH
#define TPCRIPROXHTCORRELATOR_HH

// Base Class Headers ----------------
#include "PndTpcAbsHitTrackCorrelator.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TH1D;


class PndTpcRiProxHTCorrelator : public PndTpcAbsHitTrackCorrelator {
public:

  // Constructors/Destructors ---------
  PndTpcRiProxHTCorrelator(double riproxcut);
  ~PndTpcRiProxHTCorrelator(){;}

  // Accessors -----------------------
  TH1D* getProxHisto() {return _hprox;}
  

  // Modifiers -----------------------


  // Operations ----------------------
  virtual bool corr(PndTpcRiemannTrack* trk,
		    PndTpcRiemannHit* rhit,
		    bool& survive,
		    double& matchQuality);

private:

  // Private Data Members ------------
  double _riproxcut; 

  // histograms for logging
  TH1D* _hprox;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
