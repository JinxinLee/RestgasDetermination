/** PndTrackingQualityMCInfo
 ** 
 ** @author Lia Lavezzi 
 **/

#ifndef PNDTRACKINGQUALITYMCINFO_H
#define PNDTRACKINGQUALITYMCINFO_H

// #include "PndTrack.h"
#include "TVector3.h"
#include "TArrayI.h"


class PndTrackingQualityMCInfo : public TObject
{
  
 public:    

  PndTrackingQualityMCInfo();
  PndTrackingQualityMCInfo(int nofmvdpix, int nofmvdstr, int nofsttparal, int nofsttskew, int nofgem, int noffts);
  PndTrackingQualityMCInfo(const PndTrackingQualityMCInfo& info);
  ~PndTrackingQualityMCInfo();    

  PndTrackingQualityMCInfo& operator=(const PndTrackingQualityMCInfo& info); 


  void SetPositionFirst(TVector3 pos) { fPosFirst = pos; }
  void SetMomentumFirst(TVector3 mom) { fMomFirst = mom; }
  void SetPositionLast(TVector3 pos) { fPosLast = pos; }
  void SetMomentumLast(TVector3 mom) { fMomLast = mom; }


  void SetCharge(int ch) { fCharge = ch; }


  Int_t GetNofMvdPixelPoints() { return fNofMvdPixelPoints; }
  Int_t GetNofMvdStripPoints() { return fNofMvdStripPoints; }
  Int_t GetNofMvdPoints() {return fNofMvdPixelPoints + fNofMvdStripPoints ;}
  Int_t GetNofSttParalPoints() { return fNofSttParalPoints; }
  Int_t GetNofSttSkewPoints() { return fNofSttSkewPoints; }
  Int_t GetNofSttPoints() {return fNofSttParalPoints + fNofSttSkewPoints ;}
  Int_t GetNofGemPoints() {return fNofGemPoints; }
  Int_t GetNofFtsPoints() {return fNofFtsPoints; }
  Int_t GetNofMCPoints() {return  fNofMvdPixelPoints + fNofMvdStripPoints + fNofSttParalPoints + fNofSttSkewPoints + fNofGemPoints + fNofFtsPoints; }
 
  Int_t GetMCTrackID() { return fMCTrackID; }
  Short_t GetNofRecoTracks(void)     const { return fRecoTrackIDs.GetSize(); }
  Int_t GetRecoTrackID(Int_t i=0) const { 
    if(GetNofRecoTracks() == 0) return -1;
    return fRecoTrackIDs[i]; }
  Int_t GetAssoRecoTrackID() const { return fAssoRecoTrackID; }

  TVector3 GetPositionFirst() { return fPosFirst; }
  TVector3 GetMomentumFirst() { return fMomFirst; }
  TVector3 GetPositionLast() { return fPosLast; }
  TVector3 GetMomentumLast() { return fMomLast; }

  Int_t GetCharge() { return fCharge; }

  void SetPDGCode(int pdg) { fPDGCode = pdg; }
  Int_t GetPDGCode() { return fPDGCode; }

  void SetReconstructabilityStatus(Int_t reco) { fReconstructabilityStatus = reco; }
  Int_t GetReconstructabilityStatus() { return fReconstructabilityStatus; }

  void SetMCTrackID(Int_t mctrackid) { fMCTrackID = mctrackid; }
  void SetRecoTrackIDs(const TArrayI recotrkids) { fRecoTrackIDs = recotrkids; }
  void SetRecoTrackID(int recotrkid) { 
    int size = GetNofRecoTracks();
    fRecoTrackIDs.Set(size + 1);
    fRecoTrackIDs[size] = recotrkid; 
  }
  void SetAssoRecoTrackID(int asso) { fAssoRecoTrackID = asso; }
  Bool_t IsReconstructed() { return GetNofRecoTracks() > 0;}

  void SetQuality(int quality) { fQuality = quality; }
  Int_t GetQuality() { return fQuality; }

 protected:
 
  Int_t fNofMvdPixelPoints,  fNofMvdStripPoints,  fNofSttParalPoints, fNofSttSkewPoints, fNofGemPoints, fNofFtsPoints;
  Bool_t fReconstructabilityStatus;
  Int_t fMCTrackID;
  
  TArrayI fRecoTrackIDs;
  
  TVector3 fPosFirst, fMomFirst;
  TVector3 fPosLast, fMomLast;
  Int_t fCharge;
  Int_t fPDGCode;

  Int_t fQuality;
  Int_t fAssoRecoTrackID;

  ClassDef(PndTrackingQualityMCInfo,2);
};


#endif
