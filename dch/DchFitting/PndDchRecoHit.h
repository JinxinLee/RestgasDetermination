// -------------------------------------------------------------------------
// -----                   PndDchRecoHit header file                   -----
// -----                 Created 5.03.2008  by S.Neubert, A. Wronska   -----
// -------------------------------------------------------------------------

/** \class PndDchRecoHit
 *  \author S.Neubert, A.Wronska 
 *  \date 5.03.2008
 *  \brief PndDchRecoHit is a representation of reconstructed dch hits used by genfit;
 *         an xyz reco hit (z being considered as a free parameter)
 *         hitCoord=(x,y)^T
 *         s=z
 **/

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

  virtual TMatrixT<double> residualVector(AbsTrackRep* stateVector,
                                const TMatrixT<double>& state);
  
  virtual void  Print();

private:

  static const int NparHitRep = 1;
  double _sangle;  ///<  sine of the angle of the wire
  double _cangle;  ///<  cosine of the angle of the wire
  double _wirepos; ///<  u coordinate of wire in detector plane

  ClassDef(PndDchRecoHit,1)

};

#endif
