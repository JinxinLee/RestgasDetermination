// -------------------------------------------------------------------------
// -----                  PndDchRecoHit2 header file                   -----
// -----                Created 29.09.2008 A. Wronska                  -----
// -------------------------------------------------------------------------

/** \class PndDchRecoHit2
 *  \author A.Wronska 
 *  \date 29.09.2008
 *  \brief PndDchRecoHit2 is a representation of reconstructed dch hits used by genfit;
 **/

#ifndef PNDDCHRECOHIT2_H
#define PNDDCHRECOHIT2_H 1

// Pnd includes
#include "RecoHitIfc.h"
#include "WirepointHitPolicy.h"

class PndDchCylinderHit;

typedef RecoHitIfc<WirepointHitPolicy> WirepointRecoHit;

class PndDchRecoHit2 : public WirepointRecoHit {

public:

  PndDchRecoHit2();

  /** Default Constructor for dch **/
  PndDchRecoHit2(const PndDchCylinderHit* cylHit);

  /** virtual destructor **/
  virtual ~PndDchRecoHit2();

  /** Public method clone(...) creates a clone of argument**/
   virtual AbsRecoHit* clone(){return new PndDchRecoHit2(*this);};

  /** Public method Print() **/
  virtual void  Print();

 // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);

  const DetPlane& detPlane(AbsRecoHit*, AbsTrackRep*);
  virtual const DetPlane& getDetPlane(AbsTrackRep*); 

 private:
  
  double _sangle;  ///<  sine of the angle of the wire
  double _cangle;  ///<  cosine of the angle of the wire
  double _wirepos; ///<  u coordinate of wire in detector plane
  DetPlane _detPlane;
	
 protected:
  static const int NparHitRep = 8;

 public:
  ClassDef(PndDchRecoHit2,1)

};

#endif
