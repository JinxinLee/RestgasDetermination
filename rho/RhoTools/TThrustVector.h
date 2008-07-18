//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TThrustVector.h,v 1.2 1997/1/20 10:05:13 
//
// Description:
//      
//	Class TThrustVector - a simple object to calculate thrust 
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Scott Metzler 		      Original author of BetaTools/TThrust
//      Gautier Hamel de Monchenault  New interface for Cornelius++
//      John Back                     Re-implemented to work without
//                                    using TSelectors.
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TTHRUSTVECTOR_H
#define TTHRUSTVECTOR_H

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
class TCandidate;
class TBooster;
class TSelector;
class TCandList;
#include "TVector3.h"

//		---------------------
// 		-- Class Interface --
//		---------------------
 
class TThrustVector : public TObject {

//--------------------
// Instance Members --
//--------------------

public:

  // Constructors
  TThrustVector();

  // Destructor
  virtual ~TThrustVector();

  // member functions
  void BoostAndCompute( TCandList& list,
			TBooster* booster, Bool_t cutInCms = kFALSE, 
			Bool_t checkCharge = kFALSE, Bool_t charged = kFALSE);
  void Compute(TCandList& list);
  void Reset();

  // accessors
  // return the thrust
  double Thrust() const                { return _thrust; } 
  // return the thrust axis
  const TVector3& ThrustAxis() const { return _axis; } 

private:

  // Member data 
  double     _thrust; //!
  TVector3 _axis; //!
  TBooster* _booster; //!
  double     _denom_sum; //!
  Bool_t _cutInCms, _charged, _checkCharge; //!

  //
  // These are original functions by Scott
  //
  void init(const double * valX, const double * valY, const double * valZ,
	    const double denominator, const unsigned nTracks );

  void calc_denom(const double* X, const double* Y,
		  const double* Z, const unsigned nTracks);

  double calc_thrust(const double* theAxis, const double* X,
		     const double* Y, const double* Z,
		     const unsigned nTracks) const; 

  double * get_initial_axis(const double* X, const double* Y,
			    const double* Z, const unsigned nTracks) const;

  double * get_final_axis(double thetaInit, double phiInit, const double*  X,
			  const double* Y, const double* Z,
			  const unsigned nTracks) const;

  double * get_axis(double theta,double phi) const; // take care of memory

  Bool_t acceptedTrack(const TCandidate* cand) const;

public:
    ClassDef(TThrustVector,1) // A simple object to calculate thrust
};

#endif
