/*  Copyright © 2008, Christian Hoeppner & Sebastian Neubert
    All rights reserved.

    This file is part of Genfit.

    Genfit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Genfit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with Genfit.  If not, see <http://www.gnu.org/licenses/>. */

// Description:
//      A generic Kalman Filter Implementation

#ifndef FITTER_KALMAN_H
#define FITTER_KALMAN_H

#include <map>

#include "TMatrixT.h"


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


#endif

