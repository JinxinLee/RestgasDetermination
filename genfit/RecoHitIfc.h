//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      RecoHit interface class
//      specializes AbsRecoHit
//      how the detector plane is obtained can be defined with a policy
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef RECOHITIFC_HH
#define RECOHITIFC_HH

// Base Class Headers ----------------
#include "AbsRecoHit.h"

// Collaborating Class Headers -------
#include "DetPlane.h"
#include "TMatrixT.h"

// Collaborating Class Declarations --


template<class detplane_policy>
class RecoHitIfc : public AbsRecoHit, public detplane_policy{
 public:
  RecoHitIfc(int dim) : AbsRecoHit(dim){;}
  virtual ~RecoHitIfc(){;}

  virtual const DetPlane& getDetPlane(AbsTrackRep* rep){return detPlane(this,rep);}
  virtual TMatrixT<double> getHitCoord(const DetPlane& plane)
  {return hitCoord(this,plane);}
  virtual TMatrixT<double> getHitCov(const DetPlane& plane)
  {return hitCov(this,plane);}
};

#endif



//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
