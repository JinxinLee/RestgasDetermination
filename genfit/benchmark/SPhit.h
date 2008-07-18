#ifndef SPHIT_HH
#define SPHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "SpacepointHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --

typedef RecoHitIfc<SpacepointHitPolicy> SpacepointRecoHit;

class SPhit : public SpacepointRecoHit {
public:

  // Constructors/Destructors ---------
  SPhit();
  SPhit(const TVector3&,
	    const TVector3&);
  SPhit(double,double,double,
		double,double,double);

  virtual ~SPhit();

  virtual AbsRecoHit* clone();
  
  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);


  virtual double residualScalar(AbsTrackRep* stateVector,
                                const TMatrixT<double>& state);

private:

  // Private Data Members ------------
  static const int NparHitRep = 3;

  // Private Methods -----------------

public:
  ClassDef(SPhit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
