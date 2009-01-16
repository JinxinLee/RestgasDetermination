// -------------------------------------------------------------------------
// -----                    MvdHit header file                -----
// -----                  Created 07/04/05  by V. Friese               -----
// -------------------------------------------------------------------------


/** MvdHit.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** A hit in a hybrid pixel station of STS. In addition to the base class
 ** CbmHit, it holds the number of the pixel (column and row).
 **/

#ifndef PNDHYPHIT_H
#define PNDHYPHIT_H


#include "TVector3.h"
#include "TString.h"
#include "CbmHit.h"

#include <stdio.h>
#include <iostream>

class PndHypHit : public CbmHit
{
friend std::ostream& operator<< (std::ostream& out, const PndHypHit& hit){
    out << "Hybrid pixel hit in detector " << hit.GetDetName() << " at ("
	<< hit.GetX() << ", " << hit.GetY() << ", " << hit.GetZ() << ") cm "
	<< " with " << hit.GetCharge() << " e" << ", Cluster " 
	<< hit.GetRefIndex() << std::endl;
    return out;
  }

 public:

  /** Default constructor (not for use) **/
  PndHypHit();


  /** Standard constructor with all paramneters 
   *@param trackID Track ID
   *@param detID   Detector ID
   *@param detName Detector Name
   *@param pos     Position vector [cm]
   *@param dpos    Position error vector [cm]
   *@param charge  Charge measured in hit
   *@param NPixelHits Number of pixels fired for this event
   **/
  
  //PndHypHit(Int_t trackID, Int_t detID, TString detName,
  //    TVector3& pos, TVector3& dpos, Int_t index, Double_t charge, Int_t NPixelHits);

   PndHypHit( Int_t detID, TString detName,
  	    TVector3& pos, TVector3& dpos, Int_t index, Double_t charge, Int_t NPixelHits);

  //PndHypHit(PndHypHit& c);
  /** Destructor **/
  virtual ~PndHypHit();


  /** Accessors **/
  
  //void SetTrackID(Int_t id){fTrackID = id;};
  //new version const added
  void SetDetName(TString name) {fDetName = name;};
  void SetCharge(Double_t charge) {fCharge = charge;};
  void SetNPixelHits(Int_t pixel) {fNPixelHits = pixel;};
  void SetBotIndex(Int_t id)       { fBotIndex  = id;}
   
  TString GetDetName()const { return fDetName; }
  //Int_t 	GetTrackID(){return fTrackID;};
  Double_t GetCharge()const{return fCharge;};
  Int_t 	GetNPixelHits() const { return fNPixelHits; }
  TVector3 GetPosition() const { return TVector3(fX, fY, fZ);	  }
  Int_t     GetBotIndex()  const { return fBotIndex;}
  Double_t  GetEloss()     const { return (fCharge * 3.61e-9);}  // 3.6 eV/Electron in Silicon


  Double_t GetDxLocal() const { return fDx;};
  Double_t GetDyLocal() const { return fDy;};
  Double_t GetDzLocal() const { return fDz;};
  void PositionErrorLocal(TVector3& dpos) const;
  /** overloaded accessors **/
  
  Double_t GetDx() {return GetD(0);};
  Double_t GetDy() {return GetD(1);};
  Double_t GetDz() {return GetD(2);};
  void PositionError(TVector3& dpos);


  /** Screen output **/
  virtual void Print(const Option_t* opt = 0) const;
  //new version
  
  
 private:

   Double_t GetD(Int_t i);
   TString fDetName;  // Detector name
   //Int_t fTrackID;
   Double_t fCharge;
   Int_t fNPixelHits;
   Int_t fBotIndex; /// bottom side of strip clusters

   ClassDef(PndHypHit,7);

};

inline void PndHypHit::PositionErrorLocal(TVector3& dpos) const {
  dpos.SetXYZ(fDx, fDy, fDz);
}

inline void PndHypHit::PositionError(TVector3& dpos) {
  dpos.SetXYZ(GetDx(), GetDy(), GetDz());
}


#endif
