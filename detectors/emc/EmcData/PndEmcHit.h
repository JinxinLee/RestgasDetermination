// -------------------------------------------------------------------------
// -----                     PndEmcHit header file                     -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------

/**  PndEmcHit.h
 *@author S.Spataro
 **
 ** Hits in EMC
 **/

//#pragma once
#ifndef PNDEMCHIT_H
#define PNDEMCHIT_H

#include "FairHit.h"
#include "TMath.h"
#include <vector>

class PndEmcPoint;

/**
 * @brief represents the deposited energy of one emc crystal from simulation
 * @ingroup PndEmc
 */
class PndEmcHit : public FairHit
{
 public:    
  
  
  /** Default constructor **/
  PndEmcHit();
  
  PndEmcHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z);

  PndEmcHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z, std::vector<PndEmcPoint*> PointList);

  PndEmcHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z, std::vector<Int_t> McTruth, FairMultiLinkedData enteringTrack, FairMultiLinkedData exitingTrack);

  /** Copy **/
  PndEmcHit(const PndEmcHit &);
  
  /** Destructor **/
  virtual ~PndEmcHit();    
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt ="") const;
  
  /** Modifiers **/
  virtual void SetEnergy(Double32_t energy) { fEnergy     = energy ;};
  virtual void SetTime(Double32_t time)     { fTime       = time   ; fTimeStamp = time * 1E9;};
   
  /** Accessors **/
  virtual Double_t GetEnergy()  const { return fEnergy     ;};
  virtual Double_t GetTime()    const { return fTime       ;};
  Float_t GetTheta()            const { return fX == 0.0 && fY == 0.0 && fZ == 0.0 ? 0.0 : TMath::ATan2(sqrt(fX*fX+fY*fY),fZ)*TMath::RadToDeg() ;};
  Float_t GetPhi()              const { return fX == 0.0 && fY == 0.0 ? 0.0 : TMath::ATan2(fY,fX)*TMath::RadToDeg()  ;};
  Short_t GetModule()           const { return (fDetectorID/100000000)    ;};
  Short_t GetRow()              const { return ((fDetectorID/1000000)%100);};
  Short_t GetCrystal()          const { return (fDetectorID%10000)        ;};
  Short_t GetCopy()             const { return ((fDetectorID/10000)%100)  ;};
 
  Short_t GetXPad()        const;
  Short_t GetYPad()        const;
  FairMultiLinkedData GetTrackEntering() const { return fTrackEntering;}
  FairMultiLinkedData GetTrackExiting()  const { return fTrackExiting;}
  
  std::vector<PndEmcPoint*> &GetPointList() {return fPointList;}
  const std::vector<Int_t> &GetMcList() {return fMcList;}

 protected:
  Double32_t fTime;      // time
  Double32_t fEnergy;    // hit amplitude
 

  std::vector<Int_t> fMcList; //  Mc TrackIndex contributed to hit
  std::vector<PndEmcPoint*> fPointList; //  points contributed to hit
  FairMultiLinkedData fTrackEntering; // Links to tracks entering the crystal
  FairMultiLinkedData fTrackExiting;  // Links to tracks exiting the crystal

  ClassDef(PndEmcHit,2)
};

#endif //PNDEMCHIT_H
