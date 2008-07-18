//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Policy class for spacepoint hits
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Christian Hoeppner   TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef SPACEPOINTHITPOLICY_HH
#define SPACEPOINTHITPOLICY_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include "DetPlane.h"
#include "TMatrixT.h"

// Collaborating Class Declarations --
class AbsRecoHit;
class AbsTrackRep;


class SpacepointHitPolicy {
public:


  SpacepointHitPolicy(){;}
  



  

  // Modifiers -----------------------
  void setPos(const TVector3& p){_pos=p;}

  // Operations ----------------------
  const DetPlane& detPlane(AbsRecoHit*, AbsTrackRep*);
  TMatrixT<double> hitCoord(AbsRecoHit*,const DetPlane&);
  TMatrixT<double> hitCov(AbsRecoHit*,const DetPlane&);

protected:
  // policy destructors have to be protected
  // see Alexandrescu
  virtual ~SpacepointHitPolicy(){;}
  TVector3 _pos; // position of spacepoint;

private:

  // Private Data Members ------------
  DetPlane _plane;
  
  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
