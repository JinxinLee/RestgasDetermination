//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//      
//	Class TMomTensor - a simple object to calculate Sphericity etc.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Scott Metzler 		      Original author of BetaTools/TThrust
//      Gautier Hamel de Monchenault  New interface for Cornelius++
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TMOMTENSOR_H
#define TMOMTENSOR_H

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
 
class TMomTensor  : public TObject {

public:

  // Constructor
  TMomTensor();

  // Destructor
  virtual ~TMomTensor();

  // member functions
  void Compute( TCandList& list );
  void BoostAndCompute( TCandList& list,
			TBooster* booster );
  void Reset();

  // accessors
  double Sphericity() const            { return _sph; }
  double Aplanarity() const            { return _apl; }
  double Planarity()  const            { return _pla; }
  const TVector3& Axis()   const     { return _sphAx; }
  const TVector3& SecondAxis() const { return _secAx; }
  const TVector3& ThirdAxis() const  { return _thrAx; }
  const TVector&  EValues() const    { return _E; }
  
private:

  // Member data 
  double     _sph; //!
  double     _apl; //!
  double     _pla; //!
  TVector3 _sphAx; //!
  TVector3 _secAx; //!
  TVector3 _thrAx; //!
  TVector  _E;  //!

public:
    ClassDef(TMomTensor,1) // A simple object to calculate Sphericity etc.
};

#endif
