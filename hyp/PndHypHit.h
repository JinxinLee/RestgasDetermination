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
  
  TString GetDetName()const { return fDetName; }
  //Int_t 	GetTrackID(){return fTrackID;};
  Double_t GetCharge()const{return fCharge;};
  Int_t 	GetNPixelHits() const { return fNPixelHits; }
  TVector3 GetPosition() const { return TVector3(fX, fY, fZ);	  }
  

  /** Screen output **/
  virtual void Print(const Option_t* opt = 0) const;
  //new version
  friend std::ostream& operator<< (std::ostream& out, const PndHypHit& hit){
    out << "Hybrid pixel hit in detector " << hit.GetDetName() << " at ("
	<< hit.GetX() << ", " << hit.GetY() << ", " << hit.GetZ() << ") cm "
	<< " with " << hit.GetCharge() << " e" << ", Cluster " 
	<< hit.GetRefIndex() << std::endl;
    return out;
  }
  
 public:

  TString fDetName;  // Detector name
  //Int_t fTrackID;
  Double_t fCharge;
  Int_t fNPixelHits;
  ClassDef(PndHypHit,7);

};

#endif
