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
//	Ralf Kliemt	     TUD	    (application to PndMvd)
//      Radoslaw Karabowicz  MVD            (conversion to GEM)
//
//-----------------------------------------------------------

//! A planar (x,y) reco hit & its sensor plane
/** @author Radoslaw Karabowicz GSI
 */
#ifndef PNDGEMRECOHIT_H
#define PNDGEMRECOHIT_H

// Base Class Headers ----------------
#include "GFRecoHitIfc.h"
#include "GFPlanarHitPolicy.h"
//#include "PndGemGeoHandling.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairMCPoint;
class PndGemMCPoint;
class PndGemHit;


class PndGemRecoHit : public GFRecoHitIfc<GFPlanarHitPolicy> {
public:

  // Constructors/Destructors ---------
  PndGemRecoHit();

  PndGemRecoHit(PndGemMCPoint* point);//from lab MC points
  PndGemRecoHit(PndGemHit* hit);//from lab cluster hits

  virtual ~PndGemRecoHit();

  virtual GFAbsRecoHit* clone(){return new PndGemRecoHit(*this);};



  // Operations ----------------------
  virtual TMatrixT<double> getHMatrix(const GFAbsTrackRep* stateVector);


  virtual Double_t residualScalar(GFAbsTrackRep* stateVector,
                                const TMatrixT<Double_t>& state);


private:

  // Private Data Members ------------
  static const Int_t fNparHitRep = 2;//3;
  //  PndGemGeoHandling* fGeoH;

  // Private Methods -----------------

//public:
  ClassDef(PndGemRecoHit,1);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
