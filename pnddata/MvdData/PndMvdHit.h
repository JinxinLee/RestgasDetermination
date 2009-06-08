// -------------------------------------------------------------------------
// -----                    PndMvdHit header file                      -----
// -----                  Created by T.Stockmanns            -----
// -------------------------------------------------------------------------


/** PndMvdHit.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** A hit in a silicon sensor. In addition to the base class
 ** FairHit, it holds the number of digis & charge entry.
 ** There is also a bottom index for double sided strip sensors.
 ** All coordinates are in the LAB frame.
 **/

#ifndef PNDMVDHIT_H
#define PNDMVDHIT_H


#include "TVector3.h"
#include "TString.h"
#include "FairHit.h"

#include <stdio.h>
#include <iostream>

class PndMvdHit : public FairHit
{
  friend std::ostream& operator<< (std::ostream& out, const PndMvdHit& hit){
    out << "Mvd hit in " << hit.GetDetName() << " at" << std::endl;
    out << "(" << hit.GetX() << ", " << hit.GetY() << ", " << hit.GetZ() << ") cm "
        << " with " << hit.GetCharge() << " e" << ", Cluster No. " << hit.GetClusterIndex();
    if (hit.GetBotIndex() > -1)
    	out << " and bottom " << hit.GetBotIndex();
    if (hit.GetRefIndex() > -1)
      out << ", mc point id = " << hit.GetRefIndex();
    else 
      out << ", noise hit without mc poit";
    out << std::endl;

    return out;
  }

 public:

  /** Default constructor (not for use) **/
  PndMvdHit();


  /** Standard constructor with all paramneters 
   *@param detID   Detector ID
   *@param detName Detector Name
   *@param pos     Position vector [cm]
   *@param dpos    Position error vector [cm]
   *@param index   Array index of ClusterCand
   *@param charge  Charge measured in hit
   *@param NDigiHits Number of pixels fired for this event
   **/
  PndMvdHit(Int_t detID, TString detName,
  	    TVector3& pos, TVector3& dpos, Int_t clindex, Double_t charge, Int_t NDigiHits, Int_t mcindex);

  //PndMvdHit(PndMvdHit& c);
  /** Destructor **/
  virtual ~PndMvdHit();


  /** Accessors **/
  
  void SetDetName(TString name)  { fDetName   = name;}
  void SetCharge(Double_t charge){ fCharge    = charge;}
  void SetNDigiHits(Int_t pixel) { fNDigiHits = pixel;}
  void SetClusterIndex(Int_t id) { fClusterIndex = id;}
  void SetBotIndex(Int_t id)     { fBotIndex  = id;}
  
  TString 	GetDetName()      const { return fDetName;}
  Double_t 	GetCharge()       const { return fCharge;}
  Int_t 	  GetNDigiHits()    const { return fNDigiHits;}
  TVector3  GetPosition()	    const { return TVector3(fX,fY,fZ);}
  Int_t     GetClusterIndex() const { return fClusterIndex;}
  Int_t     GetTopIndex()     const { return GetClusterIndex();}
  Int_t     GetBotIndex()     const { return fBotIndex;}
  Double_t  GetEloss()        const { return (fCharge * 3.61e-9);}  // 3.6 eV/Electron in Silicon


//   // CAUTION The errors in the MvdHit are LOCAL, but the coordinates are in the LAB
// 
//   Double_t GetDxLocal() const { return fDx;};
//   Double_t GetDyLocal() const { return fDy;};
//   Double_t GetDzLocal() const { return fDz;};
//   void PositionErrorLocal(TVector3& dpos) const;

//   /** overloaded accessors **/
//   Double_t GetDx() {return GetD(0);};
//   Double_t GetDy() {return GetD(1);};
//   Double_t GetDz() {return GetD(2);};
//   void PositionError(TVector3& dpos);




  /** Screen output **/
  virtual void Print(const Option_t* opt = 0) const;

 private:
//   Double_t GetD(Int_t i);

  TString fDetName;  // Detector name
  Double_t fCharge; /// deposited Charge
  Int_t fNDigiHits; /// number of fired Digis for this hit,
  Int_t fClusterIndex; /// top/pixel cluster index
  Int_t fBotIndex; /// bottom side of strip clusters
  ClassDef(PndMvdHit,8);

};

// inline void PndMvdHit::PositionErrorLocal(TVector3& dpos) const {
//   dpos.SetXYZ(fDx, fDy, fDz); 
// }
// 
// inline void PndMvdHit::PositionError(TVector3& dpos) {
//   dpos.SetXYZ(GetDx(), GetDy(), GetDz()); 
// }

#endif
