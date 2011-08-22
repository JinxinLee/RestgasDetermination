//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Track-Track-Correlator using dip fit
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

#ifndef TPCPDIPTTCORRELATOR_HH
#define TPCPDIPTTCORRELATOR_HH

// Base Class Headers ----------------
#include "PndTpcAbsTrackTrackCorrelator.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TH1D;


class PndTpcDipTTCorrelator : public PndTpcAbsTrackTrackCorrelator {
public:

  // Constructors/Destructors ---------
  PndTpcDipTTCorrelator(double proxcut, double dipcut, double helixcut);
  ~PndTpcDipTTCorrelator(){;}

  // Accessors -----------------------
  /*
  TH1D* getPlaneHisto() {return _hplane;}
  TH1D* getProxHisto() {return _hprox;}
  TH1D* getSZHisto() {return _hsz;}
  */
  
  // Modifiers -----------------------


  // Operations ----------------------
  virtual bool corr(PndTpcRiemannTrack* trk1,
        PndTpcRiemannTrack* trk2,
		    bool& survive,
		    double& matchQuality);

private:

  // Private Data Members ------------
  double _proxcut;
  double _dipcut;
  double _helixcut;

  // histograms for logging
  /*
  TH1D* _hplane;
  TH1D* _hprox;
  TH1D* _hsz;
*/

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
