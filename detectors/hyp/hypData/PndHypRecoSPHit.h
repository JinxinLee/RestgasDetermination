//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      an xyz reco hit (z being considered as a free parameter)
//      hitCoord=(x,y)^T
//      s=z
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//  modified for hyp purpose by A. Sanchez
//-----------------------------------------------------------

#ifndef PNDHYPRECOSPHIT_HH
#define PNDHYPRECOSPHIT_HH

// Base Class Headers ----------------
#include "GFRecoHitIfc.h"
#include "GFSpacepointHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairMCPoint;class FairHit;
class PndHypHit;

typedef GFRecoHitIfc<GFSpacepointHitPolicy> SpacepointRecoHit;

class PndHypRecoSPHit : public SpacepointRecoHit {
public:

  // Constructors/Destructors ---------
  PndHypRecoSPHit();
  PndHypRecoSPHit(double x, double y, double z,
	       double sigx, double sigy, double sigz);
  PndHypRecoSPHit(const TVector3& pos,
	    const TVector3& sig);
  PndHypRecoSPHit(PndHypHit* point);

  virtual ~PndHypRecoSPHit();

  virtual GFAbsRecoHit* clone();
  
  // Operations ----------------------
  virtual TMatrixT<double> getHMatrix(const GFAbsTrackRep* stateVector);


  
private:

  // Private Data Members ------------
  static const int NparHitRep = 3;

  // Private Methods -----------------

public:
  ClassDef(PndHypRecoSPHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
