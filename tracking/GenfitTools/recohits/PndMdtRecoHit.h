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
//      Stefano Spataro      UNITO          (adapted for MDT - 29/01/10)
//
//
//-----------------------------------------------------------

#ifndef MDTRECOHIT_HH
#define MDTRECOHIT_HH

// Base Class Headers ----------------
#include "GFRecoHitIfc.h"
#include "GFSpacepointHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

#include"TRandom3.h"

// Collaborating Class Declarations --
class PndMdtHit;
class PndMdtPoint;

typedef GFRecoHitIfc<GFSpacepointHitPolicy> SpacepointRecoHit;

class PndMdtRecoHit : public SpacepointRecoHit {

public:

  // Constructors/Destructors ---------
  PndMdtRecoHit();
  PndMdtRecoHit(double x, double y, double z,
	      double sigx, double sigy, double sigz);
  PndMdtRecoHit(const TVector3& pos,
	      const TVector3& sig,
	      bool smear=false);
  PndMdtRecoHit(PndMdtPoint* point);
  PndMdtRecoHit(PndMdtHit* hit);

  virtual ~PndMdtRecoHit();

  virtual GFAbsRecoHit* clone();
  
  // Operations ----------------------
  virtual TMatrixT<double> getHMatrix(const GFAbsTrackRep* stateVector);

  PndMdtHit* GetHit(){return fHit;}
  
private:

  // Private Data Members ------------
  static const int NparHitRep = 3;
 
  PndMdtHit* fHit;
  static TRandom3 rand;
  // Private Methods -----------------

public:
  ClassDef(PndMdtRecoHit,1)

};

#endif
