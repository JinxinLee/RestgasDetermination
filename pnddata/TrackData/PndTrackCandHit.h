//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTrackCand
//      see PndTrackCand.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Tobias Stockmanns (IKP - Jülich) during the Panda Meeting 03/09
//
//
//-----------------------------------------------------------

#ifndef PNDTRACKCANDHIT_HH
#define PNDTRACKCANDHIT_HH

// Root Class Headers ----------------
#include "TObject.h"

#include <iostream>
#include <vector>
#include <map>

class PndTrackCandHit : public TObject{
public:
 PndTrackCandHit():fHitId(-1), fDetId(-1), fRho(0){}
 PndTrackCandHit(Int_t detId, Int_t hitId, Double_t rho):fDetId(detId), fHitId(hitId), fRho(rho){}
  friend bool operator< (const PndTrackCandHit& lhs, const PndTrackCandHit& rhs)
  {return lhs.GetRho()<rhs.GetRho();};
  bool operator== (const PndTrackCandHit& hit){
    if (GetHitId() == hit.GetHitId() && GetDetId() == hit.GetDetId())
      return true;
    return false;
  }
  
  Int_t GetHitId()const {return fHitId;}
  Int_t GetDetId()const {return fDetId;}
  Double_t GetRho()const {return fRho;}
  
  void Print();
  
 private :
  Int_t fHitId;
  Int_t fDetId;
  Double_t fRho;		///< sorting parameter
  
  ClassDef(PndTrackCandHit,1);
};



#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
