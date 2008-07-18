
//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TFoxWolfMom.h
//
// Description:
//      Class TFoxWolfMom - a simple object to calculate Fox Wolfram 
//                            Moments
//
//      The constructor calculates the Fox Wolfram Moments up to a given 
//      maximum order supplied by user, or the default value of 4.
//      FWnorm(n) provides the nth order Fox Wolfram Moment normalised by 
//      the zeroth moment.
//      Access is also provided to the zeroth moment and the Legendre 
//      Polynomials.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Theresa Champion                   Original author
//
// Copyright Information:
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TFOXWOLFMOM_H
#define TFOXWOLFMOM_H

#include "TObject.h"
#include "TVector.h"

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

class TCandList;
class TCandidate;
class TBooster;
class TEventInfo;

class TFoxWolfMomData;

//              ---------------------
//              -- Class Interface --
//              ---------------------
 
class TFoxWolfMom : public TObject {

//--------------------
// Instance Members --
//--------------------

public:

  // Constructors
  TFoxWolfMom( TCandList* tracklist, 
		 const TEventInfo* evtinfo, int order = 4);
  
  // Destructor
  virtual ~TFoxWolfMom();
  
  // accessors to get at results
  
  double FWzero() const;
  double FWnorm(int order) const;
  const TVector &FWArray() const;
  const TVector &SumArray() const;


  // Deprecated
  const TVector & Legendre() const;
  double Legendre(int order) const;
  // Note - the Legedendre functions are essentially 
  // meaningless, since they relate to the last 
  // set of functions calculated, and the order of
  // calculation is not guaranteed

private:
  TFoxWolfMom();

  TFoxWolfMom( const TFoxWolfMom& );
  TFoxWolfMom& operator= ( const TFoxWolfMom& );
  // Member data 
  
  TFoxWolfMomData *_privateData; //! Do not stream

  void BoostTracks(TCandList&, const TBooster &);
  void Compute(int maxorder);

public:
  ClassDef(TFoxWolfMom,1) // calculate Fox Wolfram Moments
};

#endif
