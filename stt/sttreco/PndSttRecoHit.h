#ifndef PNDSTTRECOHIT_HH
#define PNDSTTRECOHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "WirepointHitPolicy.h"
#include "PndSttHit.h"
#include "PndSttHelixHit.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class PndSttHit;
class PndSttHelixHit;

typedef RecoHitIfc<WirepointHitPolicy> WirepointRecoHit;

class PndSttRecoHit : public WirepointRecoHit {
public:

  // Constructors/Destructors ---------
  PndSttRecoHit();
  PndSttRecoHit(PndSttHit *currenthit); 
  PndSttRecoHit(PndSttHelixHit *currenthit); 

  virtual ~PndSttRecoHit();

  virtual AbsRecoHit* clone(){return new PndSttRecoHit(*this);};

  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);

  virtual double residualScalar(AbsTrackRep* stateVector,
                                const TMatrixT<double>& state);

  const DetPlane& detPlane(AbsRecoHit*, AbsTrackRep*);
  virtual const DetPlane& getDetPlane(AbsTrackRep*); 

private:

  // Private Data Members ------------
  static const int NparHitRep = 8;
  DetPlane _detPlane;

  // Private Methods -----------------

public:
  ClassDef(PndSttRecoHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
