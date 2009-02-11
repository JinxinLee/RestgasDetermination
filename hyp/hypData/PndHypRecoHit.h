//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      an xyz reco hit 
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//			Ralf Kliemt					 TUD						(application to PndMvd)
//
//-----------------------------------------------------------

//! An xyz reco hit
/** @author Ralf Kliemt TUD
 */

// Base Class Headers ----------------


#ifndef PNDHYPRECOHIT_HH
#define PNDHYPRECOHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "PlanarHitPolicy.h"
#include "PndHypGeoHandling.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairMCPoint;
class PndHypPoint;
class PndHypHit;


class PndHypRecoHit : public RecoHitIfc<PlanarHitPolicy> {
public:

  // Constructors/Destructors ---------
  PndHypRecoHit();

  PndHypRecoHit(PndHypPoint* point);//from lab MC points
  PndHypRecoHit(PndHypHit* hit);//from lab cluster hits TODO change to local
//   PndHypRecoHit(const TVector3& hit, const TMatrixT<Double_t>& cov,
//              const DetPlane* detplane);//from lab
//   PndHypRecoHit(const TVector3& hit, const TMatrixT<Double_t>& cov,
//              const TVector3& o,const TVector3& u,const TVector3& v);//from lab
//   PndHypRecoHit(const TMatrixT<Double_t>& xy,const TMatrixT<Double_t>& cov,
//              const DetPlane* detplane);//from sensor plane

  virtual ~PndHypRecoHit();

  virtual AbsRecoHit* clone(){return new PndHypRecoHit(*this);};



  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<Double_t>& state);


  virtual Double_t residualScalar(AbsTrackRep* stateVector,
                                const TMatrixT<Double_t>& state);

  
private:

  // Private Data Members ------------
  static const Int_t fNparHitRep = 3;
  PndHypGeoHandling* fGeoH;

  // Private Methods -----------------

public:
  ClassDef(PndHypRecoHit,1);

};

#endif


//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
