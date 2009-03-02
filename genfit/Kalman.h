//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      A generic Kalman Filter Implementation
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
// Original Authors: Christian Hoeppner & Sebastian Neubert (TUM)
//
//
//-----------------------------------------------------------

#ifndef FITTER_KALMAN_H
#define FITTER_KALMAN_H

// Base Class Headers ----------------

// Collaborating Class Headers -------
#include "TMatrixT.h"
#include <map>

// Collaborating Class Declarations --

class AbsRecoHit;
class AbsTrackRep;
class Track;

class Kalman {
public:

  friend class KalmanTester; // gives the Tester access to private methods

  // Constructors/Destructors ---------
  Kalman();
  ~Kalman();

  // Operators
  inline void operator()(Track* track){processTrack(track);}
  inline void operator()(std::pair<int,Track*> tr){processTrack(tr.second);}

  // Operations ----------------------

  void setLazy(Int_t flag){_lazy=flag;}
  void setNumIterations(Int_t i){_numIt=i;}

  void processTrack(Track*);
  void continueTrack(Track*,int dir); // continues track from lastHitInFit

  double getChi2Hit(AbsRecoHit*, AbsTrackRep*);

  void smoothing(Track*);

  // Private Methods -----------------
private:
  void processHit(AbsRecoHit*, AbsTrackRep*, int);
  void switchDirection(Track* trk); // switches the direction of propagation for all reps

  TMatrixT<double> gain(const TMatrixT<double>& cov, 
						const TMatrixT<double>& HitCov,
						const TMatrixT<double>& H);

  Int_t _lazy; // controls throw of exceptions
  Int_t _numIt;

};


#endif //FITTER_KALMAN_H

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
