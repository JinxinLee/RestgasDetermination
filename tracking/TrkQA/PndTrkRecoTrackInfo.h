/** PndTrkRecoTrackInfo
 ** 
 ** @author Lia Lavezzi 
 **/

#ifndef PNDTRKRECOTRACKINFO_H
#define PNDTRKRECOTRACKINFO_H 1


#include "PndTrkMCTrackInfo.h"



class PndTrkRecoTrackInfo : public TObject
{
  
 public:    

  PndTrkRecoTrackInfo();
  PndTrkRecoTrackInfo(int recotrackid);
 ~PndTrkRecoTrackInfo();    

 void SetMCTrackInfo(PndTrkMCTrackInfo *info) {
  fMCTrackInfo = *info; 
  }

 double GetPurity() { return (double) GetNofRecoTrueHits()/GetNofRecoHits(); }
 double GetSttParalPurity() { return (double) GetNofSttParalTrueHits()/GetNofSttParalHits(); }
 double GetSttSkewPurity() { return (double) GetNofSttSkewTrueHits()/GetNofSttSkewHits(); }
 double GetSttPurity() { return (double) GetNofSttTrueHits()/GetNofSttHits(); }
 double GetMvdPixPurity() { return (double) GetNofMvdPixTrueHits()/GetNofMvdPixHits(); }
 double GetMvdStrPurity() { return (double) GetNofMvdStrTrueHits()/GetNofMvdStrHits(); }
 double GetMvdPurity() { return (double) GetNofMvdTrueHits()/GetNofMvdHits(); }
 double GetGemPurity() { return (double) GetNofGemTrueHits()/GetNofGemHits(); }
 double GetSciTilPurity() { return (double) GetNofSciTilTrueHits()/GetNofSciTilHits(); }

 double GetContamination() { return (double) GetNofRecoFakeHits()/GetNofRecoHits(); }
 double GetSttParalContamination() { return (double) GetNofSttParalFakeHits()/GetNofSttParalHits(); }
 double GetSttSkewContamination() { return (double) GetNofSttSkewFakeHits()/GetNofSttSkewHits(); }
 double GetSttContamination() { return (double) GetNofSttFakeHits()/GetNofSttHits(); }
 double GetMvdPixContamination() { return (double) GetNofMvdPixFakeHits()/GetNofMvdPixHits(); }
 double GetMvdStrContamination() { return (double) GetNofMvdStrFakeHits()/GetNofMvdStrHits(); }
 double GetMvdContamination() { return (double) GetNofMvdFakeHits()/GetNofMvdHits(); }
 double GetGemContamination() { return (double) GetNofGemFakeHits()/GetNofGemHits(); }
 double GetSciTilContamination() { return (double) GetNofSciTilFakeHits()/GetNofSciTilHits(); }

 double GetEfficiency();
 double GetMvdPixEfficiency();
 double GetMvdStrEfficiency();
 double GetMvdEfficiency();
 double GetSttParalEfficiency();
 double GetSttSkewEfficiency();
 double GetSttEfficiency();
 double GetGemEfficiency();
 double GetSciTilEfficiency();

 double GetInefficiency();
 double GetMvdPixInefficiency();
 double GetMvdStrInefficiency();
 double GetMvdInefficiency();
 double GetSttParalInefficiency();
 double GetSttSkewInefficiency();
 double GetSttInefficiency();
 double GetGemInefficiency();
 double GetSciTilInefficiency();


 void SetPositionFirst(TVector3 pos) { fPosFirst = pos; }
 void SetMomentumFirst(TVector3 mom) { fMomFirst = mom; }
 void SetPositionLast(TVector3 pos) { fPosLast = pos; }
 void SetMomentumLast(TVector3 mom) { fMomLast = mom; }

 TVector3 GetPositionFirst() { return fPosFirst; }
 TVector3 GetMomentumFirst() { return fMomFirst; }
 TVector3 GetPositionLast() { return fPosLast; }
 TVector3 GetMomentumLast() { return fMomLast; }

 void SetCharge(int ch) { fCharge = ch; }
 void SetMCTrackID(int mctrkid) { fMCTrackID = mctrkid; }

 void SetNofMvdPixTrueHits(int nofpix) { fNofMvdPixTrueHits = nofpix; }
 void SetNofMvdStrTrueHits(int nofstr) { fNofMvdStrTrueHits = nofstr; }
 void SetNofSttParalTrueHits(int nofsttparal) { fNofSttParalTrueHits = nofsttparal; }
 void SetNofSttSkewTrueHits(int nofsttskew) { fNofSttSkewTrueHits = nofsttskew; }
 void SetNofGemTrueHits(int nofgem) { fNofGemTrueHits = nofgem; }
 void SetNofSciTilTrueHits(int nofscitil) { fNofSciTilTrueHits = nofscitil; }

 Int_t GetNofMvdPixTrueHits() { return fNofMvdPixTrueHits; }
 Int_t GetNofMvdStrTrueHits() { return fNofMvdStrTrueHits; }
 Int_t GetNofMvdTrueHits() {return fNofMvdPixTrueHits + fNofMvdStrTrueHits ;}
 Int_t GetNofSttParalTrueHits() { return fNofSttParalTrueHits; }
 Int_t GetNofSttSkewTrueHits() { return fNofSttSkewTrueHits; }
 Int_t GetNofSttTrueHits() {return fNofSttParalTrueHits + fNofSttSkewTrueHits ;}
 Int_t GetNofGemTrueHits() {return fNofGemTrueHits; }
 Int_t GetNofSciTilTrueHits() {return fNofSciTilTrueHits; }
 Int_t GetNofRecoTrueHits() {return  fNofMvdPixTrueHits + fNofMvdStrTrueHits + fNofSttParalTrueHits + fNofSttSkewTrueHits + fNofGemTrueHits + fNofSciTilTrueHits; }
 
 void SetNofMvdPixFakeHits(int nofpix) { fNofMvdPixFakeHits = nofpix; }
 void SetNofMvdStrFakeHits(int nofstr) { fNofMvdStrFakeHits = nofstr; }
 void SetNofSttParalFakeHits(int nofsttparal) { fNofSttParalFakeHits = nofsttparal; }
 void SetNofSttSkewFakeHits(int nofsttskew) { fNofSttSkewFakeHits = nofsttskew; }
 void SetNofGemFakeHits(int nofgem) { fNofGemFakeHits = nofgem; }
 void SetNofSciTilFakeHits(int nofscitil) { fNofSciTilFakeHits = nofscitil; }

 Int_t GetNofMvdPixFakeHits() { return fNofMvdPixFakeHits; }
 Int_t GetNofMvdStrFakeHits() { return fNofMvdStrFakeHits; }
 Int_t GetNofMvdFakeHits() {return fNofMvdPixFakeHits + fNofMvdStrFakeHits ;}
 Int_t GetNofSttParalFakeHits() { return fNofSttParalFakeHits; }
 Int_t GetNofSttSkewFakeHits() { return fNofSttSkewFakeHits; }
 Int_t GetNofSttFakeHits() {return fNofSttParalFakeHits + fNofSttSkewFakeHits ;}
 Int_t GetNofGemFakeHits() {return fNofGemFakeHits; }
 Int_t GetNofSciTilFakeHits() {return fNofSciTilFakeHits; }
 Int_t GetNofRecoFakeHits() {return  fNofMvdPixFakeHits + fNofMvdStrFakeHits + fNofSttParalFakeHits + fNofSttSkewFakeHits + fNofGemFakeHits + fNofSciTilFakeHits; } 

 Int_t GetNofMvdPixMissingHits() { return fNofMvdPixMissingHits; }
 Int_t GetNofMvdStrMissingHits() { return fNofMvdStrMissingHits; }
 Int_t GetNofMvdMissingHits() {return fNofMvdPixMissingHits + fNofMvdStrMissingHits ;}
 Int_t GetNofSttParalMissingHits() { return fNofSttParalMissingHits; }
 Int_t GetNofSttSkewMissingHits() { return fNofSttSkewMissingHits; }
 Int_t GetNofSttMissingHits() {return fNofSttParalMissingHits + fNofSttSkewMissingHits ;}
 Int_t GetNofGemMissingHits() {return fNofGemMissingHits; }
 Int_t GetNofSciTilMissingHits() {return fNofSciTilMissingHits; }
 Int_t GetNofMissingHits() {return  fNofMvdPixMissingHits + fNofMvdStrMissingHits + fNofSttParalMissingHits + fNofSttSkewMissingHits + fNofGemMissingHits + fNofSciTilMissingHits; }

 void SetNofMvdPixMissingHits(int nofpix) { fNofMvdPixMissingHits = nofpix; }
 void SetNofMvdStrMissingHits(int nofstr) { fNofMvdStrMissingHits = nofstr; }
 void SetNofSttParalMissingHits(int nofsttparal) { fNofSttParalMissingHits = nofsttparal; }
 void SetNofSttSkewMissingHits(int nofsttskew) { fNofSttSkewMissingHits = nofsttskew; }
 void SetNofGemMissingHits(int nofgem) { fNofGemMissingHits = nofgem; }
 void SetNofSciTilMissingHits(int nofscitil) { fNofSciTilMissingHits = nofscitil; }

 Int_t GetNofTrueHits() { return fNofMvdPixTrueHits + fNofMvdStrTrueHits + fNofSttParalTrueHits + fNofSttSkewTrueHits + fNofGemTrueHits + fNofSciTilTrueHits; }
 Int_t GetNofFakeHits() { return fNofMvdPixFakeHits + fNofMvdStrFakeHits + fNofSttParalFakeHits + fNofSttSkewFakeHits + fNofGemFakeHits + fNofSciTilFakeHits; }
 Int_t GetNofRecoHits() { return GetNofRecoTrueHits() + GetNofRecoFakeHits(); }

 Int_t GetNofMvdPixHits() { return GetNofMvdPixTrueHits() + GetNofMvdPixFakeHits(); }
 Int_t GetNofMvdStrHits() { return GetNofMvdStrTrueHits() + GetNofMvdStrFakeHits(); }
 Int_t GetNofMvdHits() { return GetNofMvdPixHits() + GetNofMvdStrHits(); }
 Int_t GetNofSttParalHits() { return GetNofSttParalTrueHits() + GetNofSttParalFakeHits(); }
 Int_t GetNofSttSkewHits() { return GetNofSttSkewTrueHits() + GetNofSttSkewFakeHits(); }
 Int_t GetNofSttHits() { return GetNofSttParalHits() + GetNofSttSkewHits(); }
 Int_t GetNofGemHits() { return GetNofGemTrueHits() + GetNofGemFakeHits(); }
 Int_t GetNofSciTilHits() { return GetNofSciTilTrueHits() + GetNofSciTilFakeHits(); }

 Bool_t IsFull()       { return GetEfficiency() > 0.9999; }
 Bool_t IsAlmostFull() { return GetEfficiency() < 0.9999 && GetEfficiency() > 0.90; }
 Bool_t IsPartial()    { return GetEfficiency() < 0.90 && GetEfficiency() > 0.60; }
 Bool_t IsScarce()     { return GetEfficiency() < 0.60 && GetEfficiency() > 0.; }

 Bool_t IsClean()       { return GetContamination() < 0.0001; }
 Bool_t IsAlmostClean() { return GetContamination() > 0.0001 && GetContamination() < 0.1; }
 Bool_t IsNoisy()       { return GetContamination() > 0.1; }


 Int_t GetMCTrackID()  { return fMCTrackID; }
 Int_t GetRecoTrackID() { return fRecoTrackID; }

 Int_t GetCharge() { return fCharge; }

 PndTrkMCTrackInfo GetMCTrackInfo() { return fMCTrackInfo; }

 void SetTrue() { fFlag = 0; }
 Bool_t IsTrue() { return fFlag == 0; }

 void SetClone() { fFlag = 1; }
 Bool_t IsClone() { return fFlag == 1; }

 protected:
 
 PndTrkMCTrackInfo fMCTrackInfo;
 Int_t fNofMvdPixTrueHits,  fNofMvdStrTrueHits,  fNofSttParalTrueHits, fNofSttSkewTrueHits, fNofGemTrueHits, fNofSciTilTrueHits;
 Int_t fNofMvdPixFakeHits,  fNofMvdStrFakeHits,  fNofSttParalFakeHits, fNofSttSkewFakeHits, fNofGemFakeHits, fNofSciTilFakeHits;
 Int_t fNofMvdPixMissingHits,  fNofMvdStrMissingHits,  fNofSttParalMissingHits, fNofSttSkewMissingHits, fNofGemMissingHits, fNofSciTilMissingHits;
 // Double_t fPercFake, fPercTrue;
 Int_t fMCTrackID, fRecoTrackID;
 TVector3 fPosFirst, fMomFirst;
 TVector3 fPosLast, fMomLast;
 Int_t fCharge;

 // true = 0, clone = 1
 Int_t fFlag;
  ClassDef(PndTrkRecoTrackInfo,1);
};


#endif
