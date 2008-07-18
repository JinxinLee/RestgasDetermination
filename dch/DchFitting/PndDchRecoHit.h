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
//
//-----------------------------------------------------------

#ifndef PNDDCHRECOHIT_H
#define PNDDCHRECOHIT_H 1

// Pnd includes
#include "RecoHitIfc.h"
#include "PlanarHitPolicy.h"

// c++ headers
#include <ostream> 

class PndDchCylinderHit;


typedef RecoHitIfc<PlanarHitPolicy> PlanarRecoHit;

class PndDchRecoHit : public PlanarRecoHit {

public:

  /** Constructors  **/
  PndDchRecoHit();
  PndDchRecoHit(double r, double wireposx, double angle, double z,
	       double sigr);
  PndDchRecoHit(const PndDchCylinderHit* cylHit);
  
  /** Destructor  **/
  virtual ~PndDchRecoHit();
  
  /** Public method clone() **/
  virtual AbsRecoHit* clone();
  
  /** Operations **/
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);

  virtual TMatrixT<double> residualVector(const AbsTrackRep* stateVector,
                                const TMatrixT<double>& state);
  
  virtual Double_t residualScalar(AbsTrackRep* stateVector,
		                        const TMatrixT<Double_t>& state) { return 0;}
private:

  static const int NparHitRep = 1;
  double _sangle;  ///<  sine of the angle of the wire
  double _cangle;  ///<  cosine of the angle of the wire
  double _wirepos; ///<  u coordinate of wire in detector plane

  ClassDef(PndDchRecoHit,1)

};

#endif
