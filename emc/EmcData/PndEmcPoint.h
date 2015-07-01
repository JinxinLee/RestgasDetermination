// ------------------------------------------------------------------------
// -----                     PndEmcPoint header file                  -----
// -----               Created 14/08/06  by S.Spataro                 -----
// ------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCPOINT_H
#define PNDEMCPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"
#include "PndDetectorList.h"

class PndEmcPoint : public FairMCPoint 
{

 public:

  /** Default constructor **/
  PndEmcPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param posIn    Ccoordinates at entrance to active volume [cm]
   *@param posOut   Coordinates at exit of active volume [cm]
   *@param momIn    Momentum of track at entrance [GeV]
   *@param momOut   Momentum of track at exit [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  
  PndEmcPoint(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos, TVector3 mom,
	      Double_t tof, Double_t length, Double_t eLoss, 
	      Short_t mod, Short_t row, Short_t crys, Short_t copy);
  
  /** Copy constructor **/
  PndEmcPoint(const PndEmcPoint& point);
  

  /** Destructor **/
  virtual ~PndEmcPoint();
  

  /** Accessors **/
  Double_t GetTheta()      const { return fX == 0.0 && fY == 0.0 && fZ == 0.0 ? 0.0 : TMath::ATan2(sqrt(fX*fX+fY*fY),fZ)*TMath::RadToDeg() ;};
  Double_t GetPhi()        const { return fX == 0.0 && fY == 0.0 ? 0.0 : TMath::ATan2(fY,fX)*TMath::RadToDeg()  ;};
  Short_t GetXPad()        const;
  Short_t GetYPad()        const;
  
  Short_t GetModule()      const { return (fDetectorID/100000000);};
  Short_t GetRow()         const { return ((fDetectorID/1000000)%100);};
  Short_t GetCrystal()     const { return (fDetectorID%10000);};
  Short_t GetCopy()        const { return ((fDetectorID/10000)%100);};
  /** Modifiers **/

  void SetModule(Short_t mod)        { nModule     = mod;};
  void SetRow(Short_t row)           { nRow        = row;};
  void SetCrystal(Short_t crys)      { nCrystal    = crys;};
  virtual void SetTrackID(Int_t trackId) {FairMCPoint::SetTrackID(trackId); SetLink(FairLink("MCTrack", trackId));}  //14.09.10 Stefano FIX
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


 protected:
  Short_t nModule;              // Module number
  Short_t nRow;                 // Row number
  Short_t nCrystal;             // Crystal number
  Short_t nCopy;                // Copy number
    
  ClassDef(PndEmcPoint,1)

};


#endif
