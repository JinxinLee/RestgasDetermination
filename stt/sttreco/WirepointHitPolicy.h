#ifndef WIREPOINTHITPOLICY_HH
#define WIREPOINTHITPOLICY_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include "DetPlane.h"
#include "TMatrixT.h"

// Collaborating Class Declarations --
class AbsRecoHit;
class AbsTrackRep;


class WirepointHitPolicy {
public:


  WirepointHitPolicy(){;}
 
  // Operations ----------------------
  const DetPlane& detPlane(AbsRecoHit*, AbsTrackRep*);
  TMatrixT<double> hitCoord(AbsRecoHit*,const DetPlane&);
  TMatrixT<double> hitCov(AbsRecoHit*,const DetPlane&);
  void checkPlane(AbsRecoHit*,const DetPlane&);

protected:
  // policy destructors have to be protected
  // see Alexandrescu
  virtual ~WirepointHitPolicy(){;}


private:

  // Private Data Members ------------
 
  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
