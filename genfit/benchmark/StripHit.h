#ifndef STRIPHIT_HH
#define STRIPHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "PlanarHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --

typedef RecoHitIfc<PlanarHitPolicy> PlanarRecoHit;

class StripHit : public PlanarRecoHit {
public:

  // Constructors/Destructors ---------
  StripHit();
  StripHit(TVector3 point,int proj,double res);

  virtual ~StripHit();

  virtual AbsRecoHit* clone();
  
  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);


private:

  // Private Data Members ------------
  static const int NparHitRep = 1;

  // Private Methods -----------------

public:
  ClassDef(StripHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
