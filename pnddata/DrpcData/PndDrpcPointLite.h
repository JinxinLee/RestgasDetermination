#ifndef PNDDRPCPOINTLITE_H
#define PNDDRPCPOINTLITE_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"

class PndDrpcPointLite : public TObject
{

 public:

  /** Default constructor **/
  PndDrpcPointLite();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param posIn    Ccoordinates at entrance to active volume [cm]
   *@param posOut   Coordinates at exit of active volume [cm]
   *@param momIn    Momentum of track at entrance [GeV]
   *@param momOut   Momentum of track at exit [GeV]
   *@param posInLocal    Local coordinates at entrance to active volume [cm]
   *@param posOutLocal   Local coordinates at exit of active volume [cm]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  
  PndDrpcPointLite(Int_t trackID,
	      Int_t detID,
	      TLorentzVector& pos,
	      TLorentzVector& mom,
	      Double_t eLoss,
	      Double_t len,
	      Int_t pdgCode);
  
  /** Copy constructor **/
  PndDrpcPointLite(const PndDrpcPointLite& point) { *this = point; };
  
  Double_t X() const {return fX;}
  Double_t Y() const  {return fY;}
  Double_t Z() const  {return fZ;}
  Double_t T() const  {return fT;}

  Double_t E() const  {return fE;}
  Double_t Px() const  {return fPx;}
  Double_t Py() const  {return fPy;}
  Double_t Pz() const  {return fPz;}

  Double_t Len() const  {return fLen;}
  Double_t ELoss() const  {return fELoss;}

  Int_t TrackId() {return fTrackId;}
  Int_t DetId() {return fDetId;}
  Int_t PDGCode() {return fPDGCode;}

  void SetX(Double_t x) {fX=x;}
  void SetY(Double_t y) {fY=y;}
  void SetZ(Double_t z) {fZ=z;}
  void SetT(Double_t t) {fT=t;}
  void SetE(Double_t e) {fE=e;}
  void SetPx(Double_t px) {fPx=px;}
  void SetPy(Double_t py) {fPy=py;}
  void SetPz(Double_t pz) {fPz=pz;}
  void SetLen(Double_t len) {fLen=len;}
  void SetELoss(Double_t eloss) {fELoss=eloss;}
  void SetTrackId(Int_t id) {fTrackId=id;}
  void SetDetId(Int_t id) {fDetId=id;}
  void SetPdgCode(Int_t code) {fPDGCode=code;}
  TLorentzVector Mom() const {return TLorentzVector(fPx, fPy, fPz, fE);}
  void Mom(TLorentzVector& mom) const {mom.SetPxPyPzE(fPx, fPy, fPz, fE);}
  void Mom(TVector3& mom) const {mom.SetXYZ(fPx, fPy, fPz);}
  void SetMom(TVector3& mom) {fPx=mom.Px(); fPy=mom.Py(); fPz=mom.Pz();}
  void SetMom(TLorentzVector& mom)
    {fPx=mom.Px(); fPy=mom.Py(); fPz=mom.Pz(); fE=mom.E();}

  TLorentzVector Pos() const {return TLorentzVector(fX, fY, fZ, fT);}
  void Pos(TLorentzVector& pos) const {pos.SetXYZT(fX, fY, fZ, fT);}
  void Pos(TVector3& pos) const {pos.SetXYZ(fX, fY, fZ);}
  void SetPos(TVector3& pos) {fX=pos.X(); fY=pos.Y(); fZ=pos.Z();}
  void SetPos(TLorentzVector& pos)
    {fX=pos.X(); fY=pos.Y(); fZ=pos.Z(); fT=pos.T();}

  /** Destructor **/
  virtual ~PndDrpcPointLite();
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


 protected:
  Int_t fTrackId; 
  Double_t fELoss;
  Double_t fX;
  Double_t fY;
  Double_t fZ;
  Double_t fT;

  Double_t fPx;
  Double_t fPy;
  Double_t fPz;
  Double_t fE;

  Int_t fDetId;
  Int_t fPDGCode;
  Double_t fLen;
  
  ClassDef(PndDrpcPointLite,1)

};

#endif
