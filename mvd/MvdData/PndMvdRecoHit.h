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
#ifndef PNDMVDRECOHIT_HH
#define PNDMVDRECOHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "PlanarHitPolicy.h"
#include "PndMvdGeoHandling.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class CbmMCPoint;
class PndMvdMCPoint;
class PndMvdHit;


class PndMvdRecoHit : public RecoHitIfc<PlanarHitPolicy> {
public:

  // Constructors/Destructors ---------
  PndMvdRecoHit();

  PndMvdRecoHit(PndMvdMCPoint* point);//from lab MC points
  PndMvdRecoHit(PndMvdHit* hit);//from lab cluster hits TODO change to local
//   PndMvdRecoHit(const TVector3& hit, const TMatrixT<Double_t>& cov,
//              const DetPlane* detplane);//from lab
//   PndMvdRecoHit(const TVector3& hit, const TMatrixT<Double_t>& cov,
//              const TVector3& o,const TVector3& u,const TVector3& v);//from lab
//   PndMvdRecoHit(const TMatrixT<Double_t>& xy,const TMatrixT<Double_t>& cov,
//              const DetPlane* detplane);//from sensor plane

  virtual ~PndMvdRecoHit();

  virtual AbsRecoHit* clone(){return new PndMvdRecoHit(*this);};



  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<Double_t>& state);


  virtual Double_t residualScalar(AbsTrackRep* stateVector,
                                const TMatrixT<Double_t>& state);

  
private:

  // Private Data Members ------------
  static const Int_t fNparHitRep = 2;//3;
  PndMvdGeoHandling* fGeoH;

  // Private Methods -----------------

public:
  ClassDef(PndMvdRecoHit,1);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
