//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Hit-Track-Correlator using cluster shape arguments
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

#ifndef TPCCLUSTERSHAPECORRELATOR_HH
#define TPCCLUSTERSHAPECORRELATOR_HH

// Base Class Headers ----------------
#include "PndTpcAbsHitTrackCorrelator.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --



class PndTpcClustershapeCorrelator : public PndTpcAbsHitTrackCorrelator {
public:

  // Constructors/Destructors ---------
  PndTpcClustershapeCorrelator(double cut);
  ~PndTpcClustershapeCorrelator(){;}

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
