// -------------------------------------------------------------------------
// -----                      PndSttTrack header file                  -----
// -----                  Created 28/03/06  by R. Castelijns           -----
// -------------------------------------------------------------------------


/**  PndSttTrack.h
 *@author R.Castelijns <r.castelijns@fz-juelich.de>
 **
 ** STT local track. Holds lists of PndSttHits and the fitted
 ** track parameters. The fit parameters are of type FairTrackParam
 ** and can only be accesssed and modified via this class.
 **/

#ifndef PNDSTTTRACK_H
#define PNDSTTTRACK_H 


#include <map>
#include "TArrayI.h"
#include "TObject.h"
#include "FairTrackParam.h"
#include "TClonesArray.h"
class PndSttHit;

class PndSttTrack : public TObject
{
 public:

  /** Default constructor **/
  PndSttTrack();


  /** Destructor **/
  virtual ~PndSttTrack();


  /** Public methods AddHit
   ** Adds the hit index to the index array
   **/
  void AddHit(Int_t hitID, PndSttHit* mHit);

  /** Public method Print
   ** Output to screen 
   **/
  void Print();


  /** Public method SortHits
   ** Sorts the hits in each array in downstream direction
   ** and writes the hit indizes into the member TArrayI
   **/
  void SortHits();


  /** Accessors  **/
  Int_t GetNofHits()                  const { return fHits.GetSize(); };
  Int_t GetNofHelixHits()             const { return fHelixHits.GetSize(); };
  Int_t GetNHits()                    const;
  Int_t GetHitIndex(Int_t iHit)       const { return fHits.At(iHit); };
  Int_t GetHelixHitIndex(Int_t iHit)  const { return fHelixHits.At(iHit); };
  Int_t GetPidHypo()                  const { return fPidHypo; };
  Int_t GetFlag()                     const { return fFlag; };
  Double_t GetChi2Long()              const { return fChi2Long; };
  Double_t GetChi2Rad()               const { return fChi2Rad; };
  Int_t GetNDF()                      const { return fNDF; };

  // stt1
  //  TClonesArray * GetHOT() const {return fHotArray;}

  FairTrackParam* GetParamFirst() { return &fParamFirst; }; 
  FairTrackParam* GetParamLast()  { return &fParamLast ; }; 
  Bool_t AlreadyHasHit(Int_t iHit);
 
  /** Modifiers  **/
  void SetPidHypo(Int_t pid)                { fPidHypo    = pid;  };
  void SetParamFirst(FairTrackParam& par)    { fParamFirst = par;  };
  void SetParamLast(FairTrackParam& par)     { fParamLast  = par;  };
  void SetFlag(Int_t flag)                  { fFlag       = flag; };
  void SetChi2Long(Double_t chi2)           { fChi2Long   = chi2; };
  void SetChi2Rad(Double_t chi2)            { fChi2Rad    = chi2; };
  void SetNDF(Int_t ndf)                    { fNDF        = ndf;  };
  // stt1
  //  void SetHOT(TClonesArray *hotarray) {fHotArray = hotarray;}
  void AddHelixHit(Int_t size, Int_t index, Int_t helixhitindex);

 private:
  Double_t fRefAngle;

  /** Arrays containg the indices of the hits attached to the track **/
  TArrayI fHits;

  /** Arrays containg the indices of the helixhits attached to the track **/
  TArrayI fHelixHits;

  /** PID hypothesis used by the track fitter **/
  Int_t fPidHypo;

  /** Track parameters at first and last fitted hit **/
  FairTrackParam fParamFirst;
  FairTrackParam fParamLast;

  /** Quality flag **/
  Int_t fFlag;

  /** RMS deviation of hit coordinates to track **/
  Double_t fChi2Long;
  Int_t fNDF;
  Double_t fChi2Rad;

  /** Maps from hit z position to hit index. STL map is used because it
   ** is automatically sorted. Temporary only; not for storgage.
   ** The Hit index arrys will be filled by the method SortHits.
   **/
  std::map<Double_t, Int_t> fHitMap;            //!
      
  ClassDef(PndSttTrack,1);
};


inline Int_t PndSttTrack::GetNHits() const 
{
  return GetNofHits(); 
}



#endif
