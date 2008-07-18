#ifndef PNDDRCPOINT_H
#define PNDDRCPOINT_H


#include "TObject.h"
#include "TVector3.h"
#include "CbmMCPoint.h"

using namespace std;


class PndDrcPoint : public CbmMCPoint 
{

 public:

  /** Default constructor **/
  PndDrcPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos    Ccoordinates at entrance to active volume [cm]
   *@param mom    Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  //PndDrcPoint(Int_t trackID, Int_t detID, TVector3 posIn, 
  //	      TVector3 posOut, TVector3 momIn, TVector3 momOut,
  //	      Double_t tof, Double_t length, Double_t eLoss);

 
  PndDrcPoint(Int_t trackID, 
	      Int_t copyNo, 
	      TVector3 pos,
	      TVector3 mom, 
	      Double_t time, 
	      Double_t length, 
	      Double_t eLoss,
	      Double_t charge, 
	      Double_t mass, 
	      Bool_t flag,
	      TString nam,
	      Int_t pdgCode,
	      Int_t eventID);



  /** Copy constructor **/
  PndDrcPoint(const PndDrcPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndDrcPoint();


  /** Accessors **/
  Double_t GetX()  const { return fX; }
  Double_t GetY()  const { return fY; }
  Double_t GetZ()  const { return fZ; }
  Double_t GetPx() const { return fPx; }
  Double_t GetPy() const { return fPy; }
  Double_t GetPz() const { return fPz; }
   Double_t GetMass() const { return fmass; }
  
  Int_t GetTrackID()   const { return fTrackID; }
  Int_t GetEventID()   const { return fEventID; }
  Int_t GetCopyNo()   const  { return fCopyNo; }
  Bool_t GetFlag()const  { return fflag;}
  Double_t GetEnergyLoss() const { return fELoss; }
  Int_t GetPdgCode() const{return fPdgCode;}
  void Position(TVector3& pos) { pos.SetXYZ(fX,fY,fZ); }
  void Momentum(TVector3& mom) { mom.SetXYZ(fPx,fPy,fPz); }

  TString GetNam()   const {return fnam;}

  /** Modifiers **/
  void SetPosition(TVector3 pos);
  void SetMomentum(TVector3 mom); 

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


 protected:
  Double32_t fX,  fY,  fZ;
  Double32_t fPx, fPy, fPz;
  Double_t fmass;
  Bool_t fflag;
  Double_t fcharge;
  Int_t  fCopyNo;
  TString fnam;
  Int_t fTrackID;
  Int_t fPdgCode;
  Double_t fELoss;
  Int_t fEventID;
  ClassDef(PndDrcPoint,1)

};


inline void PndDrcPoint::SetPosition(TVector3 pos) {
  fX = pos.X();
  fY = pos.Y();
  fZ = pos.Z();

}

inline void PndDrcPoint::SetMomentum(TVector3 mom) {
  fPx = mom.Px();
  fPy = mom.Py();
  fPz = mom.Pz();
 }

#endif
