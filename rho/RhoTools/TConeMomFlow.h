//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//      
//	Class TConeMomFlow - a simple object to calculate the momentum flow
//        of an event in polar angle intervals around a given axis (a la CLEO)
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Georges Vasseur
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TCONEMOMFLOW_H
#define TCONEMOMFLOW_H

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
 
class TConeMomFlow : public TObject  {

public:

  // Constructor
  TConeMomFlow( int nb_intervals=9 );

  // Destructor
  virtual ~TConeMomFlow();

  // member functions
  void Compute( TCandList& list, const TVector3& axis );
  void BoostAndCompute( TCandList& list,
			const TVector3& axis, TBooster* booster );
  void Reset();

  // accessors
  const TVector& MomFlows()   const  { return _flowarray; }   
  double Flow(int order) const  { return _flowarray(order); }
  double NormFlow( int order ) const;  // normalized to momenta sum
  
private:

  // Member data 
  int _ninter;//!Do not stream
  TVector _flowarray;//!Do not stream
  double    _momsum;//!Do not stream

public:
    ClassDef(TConeMomFlow,1) // Set of variables for charmless two-bodies decays
};

#endif
