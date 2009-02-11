// ------------------------------------------------------------------------
// -----                  PndEmcApdPoint header file                  -----
// -----               Created 07/04/08  by S.Spataro                 -----
// ------------------------------------------------------------------------
#pragma once
#ifndef PNDEMCAPDPOINT_H
#define PNDEMCAPDPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"

class PndEmcApdPoint : public FairMCPoint 
{

 public:

  /** Default constructor **/
  PndEmcApdPoint();


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
  
  PndEmcApdPoint(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos, TVector3 mom,
	      Double_t tof, Double_t length, Double_t eLoss, 
	      Short_t mod, Short_t row, Short_t crys, Short_t copy, Short_t flag);
  
  /** Copy constructor **/
  PndEmcApdPoint(const PndEmcApdPoint& point) { *this = point; };
  

  /** Destructor **/
  virtual ~PndEmcApdPoint();
  

  /** Accessors **/
  Int_t    GetTrackID()    const { return fTrackID; }; 
  Int_t    GetEventID()    const { return fEventID; };
  Int_t    GetDetectorID() const { return fDetectorID; };
  Double_t GetX()          const { return fX; };
  Double_t GetY()          const { return fY; };
  Double_t GetZ()          const { return fZ; };
  Double_t GetTheta()      const { return fX == 0.0 && fY == 0.0 && fZ == 0.0 ? 0.0 : TMath::ATan2(sqrt(fX*fX+fY*fY),fZ)*TMath::RadToDeg() ;};
  Double_t GetPhi()        const { return fX == 0.0 && fY == 0.0 ? 0.0 : TMath::ATan2(fY,fX)*TMath::RadToDeg()  ;};
  Double_t GetPx()         const { return fPx; };
  Double_t GetPy()         const { return fPy; };
  Double_t GetPz()         const { return fPz; };
  Double_t GetTime()       const { return fTime; };
  Double_t GetLength()     const { return fLength; };
  Double_t GetEnergyLoss() const { return fELoss; };
  Short_t    GetFlag()       const { return fFlag; }; 
  
  Short_t GetXPad()        const;
  Short_t GetYPad()        const;
  
  void Position(TVector3& pos)   { pos.SetXYZ(fX, fY, fZ); };
  void Momentum(TVector3& mom)   { mom.SetXYZ(fPx, fPy, fPz); };
  Short_t GetModule()      const { return (fDetectorID/100000000);};
  Short_t GetRow()         const { return ((fDetectorID/1000000)%100);};
  Short_t GetCrystal()     const { return (fDetectorID%10000);};
  Short_t GetCopy()        const { return ((fDetectorID/10000)%100);};
  /** Modifiers **/
  void SetTrackID(Int_t id)          { fTrackID    = id; }; 
  void SetEventID(Int_t id)          { fEventID    = id; };
  void SetDetectorID(Int_t id)       { fDetectorID = id; };
  void SetTime(Double_t time)        { fTime       = time; };
  void SetLength(Double_t length)    { fLength     = length; };
  void SetEnergyLoss(Double_t eLoss) { fELoss      = eLoss; };
  void SetPosition(const TVector3& pos);
  void SetMomentum(const TVector3& mom);
  void SetModule(Short_t mod)        { nModule     = mod;};
  void SetRow(Short_t row)           { nRow        = row;};
  void SetCrystal(Short_t crys)      { nCrystal    = crys;};
  void SetFlag(Short_t flag)         { fFlag       = flag;};
   
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


 protected:
  Int_t fEventID;               // Event ID
  TLorentzVector fPos;          //! position
  TLorentzVector fMom;          //! momentum
  Short_t nModule;              // Module number
  Short_t nRow;                 // Row number
  Short_t nCrystal;             // Crystal number
  Short_t nCopy;                // Copy number
  Short_t fFlag;                  // MC Flag: -1 entering, 0 crossing, 1 exiting, 
  
  ClassDef(PndEmcApdPoint,1)

};

inline void PndEmcApdPoint::SetPosition(const TVector3& pos) {
  fX = pos.X();
  fY = pos.Y();
  fZ = pos.Z();
}

inline void PndEmcApdPoint::SetMomentum(const TVector3& mom) {
  fPx = mom.Px();
  fPy = mom.Py();
  fPz = mom.Pz();
}

#endif
