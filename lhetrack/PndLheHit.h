#ifndef LHE_HIT_NEW_H
#define LHE_HIT_NEW_H

// *************************************************************************
//  Author: Stefano Spataro             e-mail: stefano.spataro@to.infn.it
//   
// hit information for LHE
//
// Created: 23-04-09
// Modified:
//
// *************************************************************************

#include "FairHit.h"

class PndLheHit : public FairHit {
  
private:
   
  Int_t     fHitNumber;     // number of this hit in this event
  Int_t     fTrackID;       // MCTrack number to which this hit belongs to
  Bool_t    fUsed;          // if the hit is assigned to a track 
  
public:
  
  PndLheHit();                          // default constructor
  virtual ~PndLheHit();                 // destructor
  
  // getters
  Bool_t   GetUsage()         const { return fUsed; }
  Int_t    GetHitNumber()     const { return fHitNumber; }
  Int_t    GetTrackID()       const { return fTrackID; }
  
  // setters  
  void  SetUsage(Bool_t f)               { fUsed =  f; }
  void  SetHitNumber(Int_t f)            { fHitNumber =  f; }
  void  SetTrackID(Int_t f)              { fTrackID =  f; }
 
  void Print();
  
  ClassDef(PndLheHit, 1)   //
};

#endif
