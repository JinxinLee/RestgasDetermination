//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      a planar (x,y) reco hit & its sensor plane
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//			Ralf Kliemt					 TUD						(application to PndMvd)
//
//-----------------------------------------------------------

//! A planar (x,y) reco hit & its sensor plane
/** @author Ralf Kliemt TUD
 */
#ifndef PNDMVDRECOHIT_H
#define PNDMVDRECOHIT_H

// Base Class Headers ----------------
#include "GFRecoHitIfc.h"
#include "GFPlanarHitPolicy.h"
#include "PndMvdGeoHandling.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairMCPoint;
class PndMvdMCPoint;
class PndMvdHit;


class PndMvdRecoHit : public GFRecoHitIfc<GFPlanarHitPolicy> {
public:

  // Constructors/Destructors ---------
  PndMvdRecoHit();

  PndMvdRecoHit(PndMvdMCPoint* point);//from lab MC points
  PndMvdRecoHit(PndMvdHit* hit);//from lab cluster hits

  virtual ~PndMvdRecoHit();

  virtual GFAbsRecoHit* clone(){return new PndMvdRecoHit(*this);};



  // Operations ----------------------
  virtual TMatrixT<double> getHMatrix(const GFAbsTrackRep* stateVector);


  virtual Double_t residualScalar(GFAbsTrackRep* stateVector,
                                const TMatrixT<Double_t>& state);


private:

  // Private Data Members ------------
  static const Int_t fNparHitRep = 2;//3;
  PndMvdGeoHandling* fGeoH;

  // Private Methods -----------------

//public:
  ClassDef(PndMvdRecoHit,1);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
