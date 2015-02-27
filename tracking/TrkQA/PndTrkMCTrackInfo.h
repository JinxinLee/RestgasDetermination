/** PndTrkMCTrackInfo
 ** 
 ** @author Lia Lavezzi 
 **/

#ifndef PNDTRKMCTRACKINFO_H
#define PNDTRKMCTRACKINFO_H 1


// #include "PndTrack.h"
#include "TVector3.h"
#include "TArrayI.h"


class PndTrkMCTrackInfo : public TObject
{
  
 public:    

  PndTrkMCTrackInfo();
  PndTrkMCTrackInfo(int nofmvdpix, int nofmvdstr, int nofsttparal, int nofsttskew, int nofgem, int nofscitil);
  PndTrkMCTrackInfo(const PndTrkMCTrackInfo& info);
  ~PndTrkMCTrackInfo();    

  PndTrkMCTrackInfo& operator=(const PndTrkMCTrackInfo& info); 


 void SetPositionFirst(TVector3 pos) { fPosFirst = pos; }
 void SetMomentumFirst(TVector3 mom) { fMomFirst = mom; }
 void SetPositionLast(TVector3 pos) { fPosLast = pos; }
 void SetMomentumLast(TVector3 mom) { fMomLast = mom; }


 void SetCharge(int ch) { fCharge = ch; }


 Int_t GetNofMvdPixPoints() { return fNofMvdPixPoints; }
 Int_t GetNofMvdStrPoints() { return fNofMvdStrPoints; }
 Int_t GetNofMvdPoints() {return fNofMvdPixPoints + fNofMvdStrPoints ;}
 Int_t GetNofSttParalPoints() { return fNofSttParalPoints; }
 Int_t GetNofSttSkewPoints() { return fNofSttSkewPoints; }
 Int_t GetNofSttPoints() {return fNofSttParalPoints + fNofSttSkewPoints ;}
 Int_t GetNofGemPoints() {return fNofGemPoints; }
 Int_t GetNofSciTilPoints() {return fNofSciTilPoints; }
 Int_t GetNofMCPoints() {return  fNofMvdPixPoints + fNofMvdStrPoints + fNofSttParalPoints + fNofSttSkewPoints + fNofGemPoints + fNofSciTilPoints; }
 
 Bool_t IsReconstructable() { return fIsReconstructable; }

 Int_t GetMCTrackID() { return fMCTrackID; }
 Short_t GetNofRecoTracks(void)     const { return fRecoTrackIDs.GetSize(); }
 Int_t GetRecoTrackID(Int_t i=0) const { 
   if(GetNofRecoTracks() == 0) return -1;
   return fRecoTrackIDs[i]; }

 TVector3 GetPositionFirst() { return fPosFirst; }
 TVector3 GetMomentumFirst() { return fMomFirst; }
 TVector3 GetPositionLast() { return fPosLast; }
 TVector3 GetMomentumLast() { return fMomLast; }

 Int_t GetCharge() { return fCharge; }

 void SetReconstructability(Bool_t reco) { fIsReconstructable = reco; }
 Bool_t GetReconstructability() { return fIsReconstructable; }

 void SetMCTrackID(Int_t mctrackid) { fMCTrackID = mctrackid; }
 void SetRecoTrackIDs(const TArrayI recotrkids) { fRecoTrackIDs = recotrkids; }
 void SetRecoTrackID(int recotrkid) { 
   int size = GetNofRecoTracks();
   fRecoTrackIDs.Set(size + 1);
   fRecoTrackIDs[size] = recotrkid; 
 }
 Bool_t IsReconstructed() { return GetNofRecoTracks() > 0;}

 protected:
 
  Int_t fNofMvdPixPoints,  fNofMvdStrPoints,  fNofSttParalPoints, fNofSttSkewPoints, fNofGemPoints, fNofSciTilPoints;
  Bool_t fIsReconstructable;
  Int_t fMCTrackID;
  TArrayI fRecoTrackIDs;
  TVector3 fPosFirst, fMomFirst;
  TVector3 fPosLast, fMomLast;
  Int_t fCharge;

  ClassDef(PndTrkMCTrackInfo,1);
};


#endif
