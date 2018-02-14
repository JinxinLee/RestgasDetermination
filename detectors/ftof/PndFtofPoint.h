// ------------------------------------------------------------------------
// -----                     PndFtofpoint header file                  -----
// -----               Created by A.Sanchez                 -----
// ------------------------------------------------------------------------

#ifndef PNDFTOFPOINT_H
#define PNDFTOFPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"
#include "TString.h"

class PndFtofPoint : public FairMCPoint 
{

 public:

  /** Default constructor **/
  PndFtofPoint();


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
  
  PndFtofPoint(Int_t trackID, Int_t evtID,
	      Int_t detID,TString detName,
	      TVector3 posin, 
	      TVector3 momin, 
	      TVector3 posout, 
	      TVector3 momout,
	      Double_t tof, 
	      Double_t length, 
	      Double_t eLoss,
	      Double_t charge, 
	      Double_t mass, 
	      Int_t pdgCode,
	      Double_t distance,
	      Double_t PLin, Double_t PLout);
  
  /** Copy constructor **/
  PndFtofPoint(const PndFtofPoint& point) : FairMCPoint(point) { *this = point; };
  

  /** Destructor **/
  virtual ~PndFtofPoint();
  

  /** Accessors **/
  Int_t    GetVolumeID() const { return fVolumeID; };
  Double_t GetXin()          const { return fXin; };
  Double_t GetYin()          const { return fYin; };
  Double_t GetZin()          const { return fZin; };
  Double_t GetPxin()         const { return fPxin; };
  Double_t GetPyin()         const { return fPyin; };
  Double_t GetPzin()         const { return fPzin; };

 

  Double_t GetPLin()         const { return fPLin; };
  Double_t GetPLout()         const { return fPLout; };
  
  Double_t GetXout()          const { return fXout; };
  Double_t GetYout()          const { return fYout; };
  Double_t GetZout()          const { return fZout; };
  Double_t GetPxout()         const { return fPxout; };
  Double_t GetPyout()         const { return fPyout; };
  Double_t GetPzout()         const { return fPzout; };
  Double_t Getdist()         const { return fdist; };
  //Double_t Gettheta()         const { return ftheta; };
  
  TString GetDetName() const { return fDetName;};
  Double_t GetMass() const { return fmass; };
  
  void PositionIn(TVector3& pos)   { pos.SetXYZ(fXin, fYin, fZin); };
  void MomentumIn(TVector3& mom)   { mom.SetXYZ(fPxin, fPyin, fPzin); };
 
  void PositionOut(TVector3& pos)   { pos.SetXYZ(fXout, fYout, fZout); };
  void MomentumOut(TVector3& mom)   { mom.SetXYZ(fPxout, fPyout, fPzout); };
   
 
 
  
  /** Modifiers **/
  
  void SetPositionIn(const TVector3& pos);
  void SetMomentumIn(const TVector3& mom);
  void SetPositionOut(const TVector3& pos);
  void SetMomentumOut(const TVector3& mom);
  
   
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


 protected:
  
  Double32_t fPLin, fPLout;    //  momentum
  Double32_t  fmass, fcharge;
  Double32_t  fXin, fYin, fZin;
  Double32_t  fPxin,fPyin,fPzin;
  Double32_t  fXout, fYout, fZout;
  Double32_t  fPxout,fPyout,fPzout, fdist;
 
  TString fDetName;
  
  Int_t fVolumeID,fpdgCode;
  
  ClassDef(PndFtofPoint,1)

};

inline void PndFtofPoint::SetPositionIn(const TVector3& pos) {
  fXin = pos.X();
  fYin = pos.Y();
  fZin = pos.Z();
}

inline void PndFtofPoint::SetMomentumIn(const TVector3& mom) {
  fPxin = mom.X();
  fPyin = mom.Py();
  fPzin = mom.Pz();
}

inline void PndFtofPoint::SetPositionOut(const TVector3& pos) {
  fXout = pos.X();
  fYout = pos.Y();
  fZout = pos.Z();
}

inline void PndFtofPoint::SetMomentumOut(const TVector3& mom) {
  fPxout = mom.Px();
  fPyout = mom.Py();
  fPzout = mom.Pz();
}



#endif
