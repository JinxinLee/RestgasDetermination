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

#ifndef TPCRIEMANNHTCORRELATOR_HH
#define TPCRIEMANNHTCORRELATOR_HH

// Base Class Headers ----------------
#include "PndTpcAbsHitTrackCorrelator.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TH1D;


class PndTpcRiemannHTCorrelator : public PndTpcAbsHitTrackCorrelator {
public:

  // Constructors/Destructors ---------
  PndTpcRiemannHTCorrelator(double planecut, double proxcut, double szcut);
  ~PndTpcRiemannHTCorrelator(){;}

  // Accessors -----------------------
  TH1D* getPlaneHisto() {return _hplane;}
  TH1D* getProxHisto() {return _hprox;}
  TH1D* getSZHisto() {return _hsz;}
  

  // Modifiers -----------------------


  // Operations ----------------------
  virtual bool corr(PndTpcRiemannTrack* trk,
		    PndTpcRiemannHit* rhit,
		    bool& survive,
		    double& matchQuality);

private:

  // Private Data Members ------------
  double _planecut;
  double _proxcut; 
  double _szcut;

  // histograms for logging
  TH1D* _hplane;
  TH1D* _hprox;
  TH1D* _hsz;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
