#ifndef PNDSTTRECOHIT_HH
#define PNDSTTRECOHIT_HH

// Base Class Headers ----------------
#include "GFRecoHitIfc.h"
#include "GFWirepointHitPolicy.h"
#include "PndSttHit.h"
#include "PndSttHelixHit.h"


// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class PndSttHit;
class PndSttHelixHit;

typedef GFRecoHitIfc<GFWirepointHitPolicy> WirepointRecoHit;

class PndSttRecoHit : public WirepointRecoHit {
public:

  // Constructors/Destructors ---------
  PndSttRecoHit();
  PndSttRecoHit(PndSttHit *currenthit); 
  PndSttRecoHit(PndSttHelixHit *currenthit); 

  virtual ~PndSttRecoHit();

  virtual GFAbsRecoHit* clone(){return new PndSttRecoHit(*this);};

  // Operations ----------------------
  virtual TMatrixT<double> getHMatrix(const GFAbsTrackRep* stateVector);

private:

  // Private Data Members ------------
 
 protected:
 static const int NparHitRep = 8;

  // Private Methods -----------------

public:
  ClassDef(PndSttRecoHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
