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


/** @addtogroup genfit
 * @{
 */

#ifndef KALMAN_H
#define KALMAN_H

#include <map>

#include "TMatrixT.h"


class AbsRecoHit;
class AbsTrackRep;
class Track;

/** @brief Generic Kalman Filter implementation
 *
 * The Kalman Filter operates on genfit Track objects. It is an implementation
 * of the Kalman Filter algebra that uses the genfit interface classes 
 * AbsRecoHit and AbsTrackRep in order to be independent from the specific
 * detector geometry and the details of the track parameterization /
 * track extraoplation engine.
 *
 * The Kalman Filter can use hits from several detectors in a single fit 
 * to estimate the parameters of several track representations in parallel.
 */
class Kalman {
public:

  friend class KalmanTester; // gives the Tester access to private methods

  // Constructors/Destructors ---------
  Kalman();
  ~Kalman();

  // Operators
  /** @brief Operator for use with STL.
   *
   * This operator allows to use the std::foreach algorithm with an
   * STL container o Track* objects.
   */
  inline void operator()(Track* track){processTrack(track);}
  
  /** @brief Operator for use with STL.
   *
   * This operator allows to use the std::foreach algorithm with an
   * STL container o Track* objects.
   */
  inline void operator()(std::pair<int,Track*> tr){processTrack(tr.second);}

  // Operations ----------------------

  /** @brief Switch lazy error handling.
   *
   * If _lazy=true the filter will not abort when a fitting error occurs. It
   * will produce an error message and discard the rest of the hits 
   * that have not been fitted until the error occured.
   *
   * If _lazy=false (default) the Filter will throw an exception in case
   * an error is deteted.
   */
  void setLazy(Int_t flag){_lazy=flag;}

  /** @brief Set number of iterations for Kalman Filter
   *
   * One iteration is one forward pass plus one backward pass
   */
  void setNumIterations(Int_t i){_numIt=i;}

  /** @brief Performs fit on a Track.
   *
   * The hits are processed in the order in which they are stored in the Track
   * object. Sorting of hits in space has to be done before!
   */
  void processTrack(Track*);

  /** @brief Performs fit on a Track beginning with the current hit.
   */
  void continueTrack(Track*,int dir); // continues track from lastHitInFit

  /** @brief Calculates chi2 of a given hit with respect to a 
   * given track representation.
   */
  double getChi2Hit(AbsRecoHit*, AbsTrackRep*);

  /** @brief NOT IMPLEMENTED
   */
  void smoothing(Track*);

  // Private Methods -----------------
private:
  /** @brief One Kalman step.
   *
   * Performs
   * - Extrapolation to detector plane of the hit
   * - Calculation of residual and Kalman Gain
   * - Update of track representation state and chi2
   */
  void processHit(AbsRecoHit*, AbsTrackRep*, int);
  
  /** @brief Used to switch between forward and backward filtering
   */
  void switchDirection(Track* trk); // switches the direction of propagation for all reps

  /** @brief Calculate Kalman Gain
   */
  TMatrixT<double> gain(const TMatrixT<double>& cov, 
						const TMatrixT<double>& HitCov,
						const TMatrixT<double>& H);

  Int_t _lazy; // controls throw of exceptions
  Int_t _numIt;

};


#endif

/* @} **/
