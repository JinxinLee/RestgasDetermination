//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Policy class for planar hits
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
// Original Authors: Christian Hoeppner & Sebastian Neubert (TUM)
//
//
//-----------------------------------------------------------

#ifndef PLANARHITPOLICY_HH
#define PLANARHITPOLICY_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include "DetPlane.h"
#include "TMatrixT.h"

// Collaborating Class Declarations --
class AbsRecoHit;
class AbsTrackRep;


class PlanarHitPolicy {
public:

  // Constructors/Destructors ---------
  PlanarHitPolicy(){;}
  

  // Accessors -----------------------
  const DetPlane& detPlane(AbsRecoHit*,const AbsTrackRep*);
  

  // Modifiers -----------------------
  void setDetPlane(const DetPlane& p){_physicalDetPlane=p;}

  // Operations ----------------------
  TMatrixT<double> hitCoord(AbsRecoHit*,const DetPlane&);
  TMatrixT<double> hitCov(AbsRecoHit*,const DetPlane&);

protected:
  // policy destructors have to be protected
  // see Alexandrescu
  virtual ~PlanarHitPolicy(){;}


private:

  // Private Data Members ------------
  DetPlane _physicalDetPlane;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
