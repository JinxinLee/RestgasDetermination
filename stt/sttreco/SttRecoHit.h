#ifndef STTRECOHIT_HH
#define STTRECOHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "WirepointHitPolicy.h"
#include "PndSttHit.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class PndSttHit;

typedef RecoHitIfc<WirepointHitPolicy> WirepointRecoHit;

class SttRecoHit : public WirepointRecoHit {
public:

  // Constructors/Destructors ---------
  SttRecoHit();
  SttRecoHit(PndSttHit *currenthit); 

  virtual ~SttRecoHit();

  virtual AbsRecoHit* clone(){return new SttRecoHit(*this);};

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
  ClassDef(SttRecoHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
