//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//      
//	Class TFWMoments - a simple class to calculate Fox-Wolfram Moments
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author :
//      Gautier Hamel de Monchenault  
//
// From :
//      Alex Samuel, Theresa Champion, original authors
//             of Fox-Wolfram classes in BetaTools
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TFWMOMENTS_H
#define TFWMOMENTS_H

#include "TObject.h"

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
class TCandidate;
class TBooster;
class TCandList;
#include "TVector3.h"
#include "TVector.h"

//		---------------------
// 		-- Class Interface --
//		---------------------
 
class TFWMoments : public TObject {

public:

  // Constructor
  TFWMoments( int order=4 );

  // Destructor
  virtual ~TFWMoments();

  // member functions
  void Compute(TCandList& list );
  void BoostAndCompute( TCandList& list,
			TBooster* booster );
  void Reset();

  // accessors
  const TVector& Moments() const      { return _FWarray; }   
  const TVector& SumArray() const     { return _sumarray; }  
  double H(int order)  const     { return _FWarray(order); }
  double ZerothMoment() const     { return _FWarray(0); } 
  double R( int order ) const;  // normalized to zeroth-moment

  // Legendre polynomials
  static double Legendre( int l, int m, double x );
  
private:

  // Member data 
  int _nmom;//! Do not stream
  TVector _FWarray;//! Do not stream
  TVector _sumarray;//! Do not stream

public:
  ClassDef(TFWMoments,1) // calculate Fox Wolfram Moments
};

#endif
