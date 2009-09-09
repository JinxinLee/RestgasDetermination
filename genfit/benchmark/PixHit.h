#ifndef PIXHIT_HH
#define PIXHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "PlanarHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --

typedef RecoHitIfc<PlanarHitPolicy> PlanarRecoHit;

class PixHit : public PlanarRecoHit {
public:

  // Constructors/Destructors ---------
  PixHit();
  PixHit(TVector3 point,double res);

  virtual ~PixHit();

  virtual AbsRecoHit* clone();
  
  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);


private:

  // Private Data Members ------------
  static const int NparHitRep = 2;

  // Private Methods -----------------

public:
  ClassDef(PixHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
