// -------------------------------------------------------------------------
// -----                     PndDrcHit header file                     -----
// -----               Created 11/10/06 by Annalisa Cecchi             -----
// -----                                                               -----
// -------------------------------------------------------------------------


/**  PndDrcHit.h
 **
 ** Hits of MC tracks in Drc Photodetector including
 ** detector geometry and efficiency
 **/


#ifndef PNDDRCHIT_H
#define PNDDRCHIT_H

#include "CbmHit.h"

class PndDrcHit : public CbmHit {

public:    
  
  /** Default constructor **/
  PndDrcHit();

  PndDrcHit(Int_t trackID, 
	    Int_t copyNo, 
	    TVector3 pos, 
	    TVector3 mom, 
	    Double_t time, 
	    Double_t eLoss,
	    Double_t mass,
	    Int_t pdgCode,
	    Int_t nPhotons,
	    Int_t eventID) 
    : fTrackID(trackID), 
    fCopyNo(copyNo), 
    fPos(pos), 
    fMom(mom),
    fTime(time),
    fELoss(eLoss),
    fMass(mass),
    fPdgCode(pdgCode),
    fNPhotons(nPhotons),
    fEventID(eventID)
      {};
    
  /** Destructor **/
  virtual ~PndDrcHit();    
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt ="") const;
  
  /** Modifiers **/
  virtual void SetNPhotons (Int_t n)      {fNPhotons = n;   }
    
  /** Accessors **/
  virtual Double_t GetX()  {return fX;}
  virtual Double_t GetY()  {return fY;}
  virtual Double_t GetZ()  {return fZ;}
  virtual Double_t GetPx() {return fPx;}
  virtual Double_t GetPy() {return fPy;}
  virtual Double_t GetPz() {return fPz;}
  virtual Int_t    GetNPhotons() {return fNPhotons; }
  virtual Int_t    GetPdgCode() {return fPdgCode; }
  virtual Double_t GetEnergyLoss() {return fELoss; }
  
  virtual void Position(TVector3& pos) { pos.SetXYZ(fX,fY,fZ); }
  virtual void Momentum(TVector3& mom) { mom.SetXYZ(fPx,fPy,fPz); }

 protected:
  
  Int_t fTrackID;
  Int_t fCopyNo;
  Double_t fTime;
  Double_t fELoss; 
  Int_t fPdgCode;
  Int_t fNPhotons;
  Int_t fEventID;
  Double_t fMass;
  TVector3 fPos, fMom;
  Double_t fX,fY,fZ;
  Double_t fPx,fPy,fPz;

  ClassDef(PndDrcHit,1)
};

#endif //PNDDRCHIT_H
