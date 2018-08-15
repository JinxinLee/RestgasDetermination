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
//			Ralf Kliemt					 TUD						(application to PndSds)
//
//-----------------------------------------------------------

//! A planar (x,y) reco hit & its sensor plane
/** @author Ralf Kliemt TUD
 */
#ifndef PNDSDSRECOHIT_H
#define PNDSDSRECOHIT_H

// Base Class Headers ----------------
#include "GFRecoHitIfc.h"
#include "GFPlanarHitPolicy.h"
#include "PndGeoHandling.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairMCPoint;
class PndSdsMCPoint;
class PndSdsHit;


class PndSdsRecoHit : public GFRecoHitIfc<GFPlanarHitPolicy> {
public:

  // Constructors/Destructors ---------
  PndSdsRecoHit();

  PndSdsRecoHit(PndSdsMCPoint* point);//from lab MC points
  PndSdsRecoHit(PndSdsHit* hit);//from lab cluster hits

  virtual ~PndSdsRecoHit();

  virtual GFAbsRecoHit* clone(){return new PndSdsRecoHit(*this);};



  // Operations ----------------------
  virtual TMatrixT<double> getHMatrix(const GFAbsTrackRep* stateVector);


  virtual Double_t residualScalar(GFAbsTrackRep* stateVector,
                                const TMatrixT<Double_t>& state);


private:

  // Private Data Members ------------
  static const Int_t fNparHitRep = 2;//3;
  PndGeoHandling* fGeoH;    //!

  // Private Methods -----------------

//public:
  ClassDef(PndSdsRecoHit,2);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
