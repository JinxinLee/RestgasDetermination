#ifndef CBMPlanePOINT_H
#define CBMPlanePOINT_H


#include "TObject.h"
#include "TVector3.h"
#include "CbmMCPoint.h"


class CbmPlanePoint : public CbmMCPoint 
{

 public:

  /** Default constructor **/
  CbmPlanePoint();


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
  //CbmPlanePoint(Int_t trackID, Int_t detID, TVector3 posIn, 
  //	      TVector3 posOut, TVector3 momIn, TVector3 momOut,
  //	      Double_t tof, Double_t length, Double_t eLoss);

 
  CbmPlanePoint(Int_t trackID, Int_t detID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t tof, Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Bool_t preflag, Bool_t postflag,TString nam,Int_t eventID);



  /** Copy constructor **/
  CbmPlanePoint(const CbmPlanePoint& point) { *this = point; };


  /** Destructor **/
  virtual ~CbmPlanePoint();


  /** Accessors **/
  Double_t GetXOut()  const { return fX_out; }
  Double_t GetYOut()  const { return fY_out; }
  Double_t GetZOut()  const { return fZ_out; }
  Double_t GetPxOut() const { return fPx_out; }
  Double_t GetPyOut() const { return fPy_out; }
  Double_t GetPzOut() const { return fPz_out; }
  Double_t GetXIn()  const { return fX_in; }
  Double_t GetYIn()  const { return fY_in; }
  Double_t GetZIn()  const { return fZ_in; }
  Double_t GetPxIn() const { return fPx_in; }
  Double_t GetPyIn() const { return fPy_in; }
  Double_t GetPzIn() const { return fPz_in; }
  Double_t GetMass() const { return fmass; }
  
  Int_t GetTrackID()   const { return ftrackID; }
  Int_t GetEventID()   const { return feventID; }
  Int_t GetCopyNo()   const  { return fCopyNo; }
  Bool_t GetPreFlag() const  { return fpreflag;}
  Bool_t GetPostFlag()const  { return fpostflag;}

  void PositionOut(TVector3& pos) { pos.SetXYZ(fX_out,fY_out,fZ_out); }
  void MomentumOut(TVector3& mom) { mom.SetXYZ(fPx_out,fPy_out,fPz_out); }
  TString GetNam()   const {return fPlanenam;}

  /** Modifiers **/
  void SetPositionOut(TVector3 pos);
  void SetMomentumOut(TVector3 mom);

  // aggiunte
  void SetPositionIn(TVector3 posin);
  void SetMomentumIn(TVector3 momin);


  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


 protected:
  Double32_t fX_in,  fY_in,  fZ_in;
  Double32_t fPx_in, fPy_in, fPz_in;
  Double32_t fX_out,  fY_out,  fZ_out;
  Double32_t fPx_out, fPy_out, fPz_out;
  Double_t fmass;
  Bool_t fpreflag, fpostflag;
  Double_t fcharge;
  Int_t  fCopyNo;
  TString fPlanenam;
  Int_t ftrackID;
  Int_t feventID;
  ClassDef(CbmPlanePoint,1)

};



inline void CbmPlanePoint::SetPositionOut(TVector3 pos) {
  fX_out = pos.X();
  fY_out = pos.Y();
  fZ_out = pos.Z();
}


inline void CbmPlanePoint::SetMomentumOut(TVector3 mom) {
  fPx_out = mom.Px();
  fPy_out = mom.Py();
  fPz_out = mom.Pz();
}

inline void CbmPlanePoint::SetPositionIn(TVector3 posin) {
  fX_in = posin.X();
  fY_in = posin.Y();
  fZ_in = posin.Z();
}

inline void CbmPlanePoint::SetMomentumIn(TVector3 momin) {
  fPx_in = momin.Px();
  fPy_in = momin.Py();
  fPz_in = momin.Pz();
}

#endif
