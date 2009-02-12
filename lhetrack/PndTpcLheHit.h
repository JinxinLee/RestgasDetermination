#ifndef LHE_HIT_H
#define LHE_HIT_H

// *************************************************************************
//  Author: Oleg Rogachevsky             e-mail: rogach@sunhe.jinr.ru
//   
// hit information for LHE
//
// Created: 1-07-07
// Modified:
//
// *************************************************************************

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "PndDetectorList.h"
#include "PndTpcPoint.h"

class PndTpcLheTrack;

//  enum fDetectorType {
//     kUnknown      =  0,
//     kTpcPoint     =  1,
//     kTpcCluster   =  2,
//     kMVDPoint     =  3,
//     kMVDHitsStrip =  4,
//     kMVDHitsPixel =  5,
//     kEmcCluster   =  6,
//     kEmcBump      =  7,
//     kSttPoint     =  8,
//     kSttHit       =  9,
//     kSttHelixHit  = 10,
//  };

class PndTpcLheHit : public TObject {
  
private:
  
  TVector3  fCoord;         // vector of hit coordinates
  TVector3  fError;         // vector of errors on hit coordinates
  Int_t     fHitNumber;     // number of this hit in this event
  Int_t     fTrackID;       // track number to which this hit belongs to
  Int_t     fRefIndex;      // ref index on MC point
  Bool_t    fUsed;          // if the hit is assigned to a track 
  Int_t     fDetectorId;    // Type of detector 

 
  
public:
  
  PndTpcLheHit();                          // default constructor
  PndTpcLheHit(PndTpcPoint *point);        // constructor for data after hit finding
  PndTpcLheHit(Double_t *x, Int_t stn);    // constructor which take an arbitrary point
  virtual ~PndTpcLheHit();                 // destructor

  
  // getters
  TVector3 GetCoord()  { return fCoord; }
  TVector3 GetError()  { return fError; }
  
  Double_t GetX()          const { return fCoord.X(); }
  Double_t GetY()          const { return fCoord.Y(); }
  Double_t GetZ()          const { return fCoord.Z(); }
  Double_t GetXerr()       const { return fError.X(); }
  Double_t GetYerr()       const { return fError.Y(); }
  Double_t GetZerr()       const { return fError.Z(); }
  
  PndTpcLheTrack *GetTrack(TClonesArray *tracks) const;

  Bool_t   GetUsage()         const { return fUsed; }
  Int_t    GetHitNumber()     const { return fHitNumber; }
  Int_t    GetTrackID()       const { return fTrackID; }
  Int_t    GetRefIndex()      const { return fRefIndex; }
  Int_t    GetDetectorId()    const { return fDetectorId; };
  
  // setters  
  void  SetX(Double_t f)        { fCoord.SetX(f); }
  void  SetY(Double_t f)        { fCoord.SetY(f); } 
  void  SetZ(Double_t f)        { fCoord.SetZ(f); }
  void  SetXerr(Double_t f)     { fError.SetX(f); }
  void  SetYerr(Double_t f)     { fError.SetY(f); }
  void  SetZerr(Double_t f)     { fError.SetZ(f); }
  
  void  SetUsage(Bool_t f)               { fUsed =  f; }
  void  SetHitNumber(Int_t f)            { fHitNumber =  f; }
  void  SetTrackID(Int_t f)              { fTrackID =  f; }
  void  SetRefIndex(Int_t k)             { fRefIndex =  k; }
  void  SetDetectorId(fDetectorType det) { fDetectorId = det; };

  void Print();

  
  
  ClassDef(PndTpcLheHit, 1)   //
};

#endif
