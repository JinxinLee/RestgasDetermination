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
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
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
  void fittingPass(Track*,int dir); // continues track from lastHitInFit

  /** @brief Calculates chi2 of a given hit with respect to a 
   * given track representation.
   */
  double getChi2Hit(AbsRecoHit*, AbsTrackRep*);

  /** @brief Sets the inital direction of the track fit (1 for inner to outer,
   * or -1 for outer to inner). The standard is 1 and is set in the ctor
   */
  void setInitialDirection(int d){_initialDirection=d;}

  /** @brief Set the blowup factor (see blowUpCovs() )
   */
  void setBlowUpFactor(double f){_blowUpFactor=f;}

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
  TMatrixT<double> calcGain(const TMatrixT<double>& cov, 
						const TMatrixT<double>& HitCov,
						const TMatrixT<double>& H);

  /** @brief this returns the reduced chi2 increment for a hit
   */
  double chi2Increment(const TMatrixT<double>& r,const TMatrixT<double>& H,
		       const TMatrixT<double>& cov,const TMatrixT<double>& V);

  /** @brief this is needed to blow up the covariance matrix before a fitting pass
   * drops off-diagonal elements and blows up diagonal by blowUpFactor
   */
  void blowUpCovs(Track* trk);

  double _blowUpFactor;
  int _initialDirection;
  bool _nullExtrapolation;
  int _fitPassCounter;

  Int_t _lazy; // controls throwing of exceptions
  Int_t _numIt;

};


#endif

/* @} **/
