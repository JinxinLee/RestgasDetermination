// ------------------------------------------------------------------------
// -----                     PndSciTpoint header file                  -----
// -----               Created by A.Sanchez                 -----
// ------------------------------------------------------------------------

#ifndef PNDSCITPOINT_H
#define PNDSCITPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"
#include "TString.h"

class PndSciTPoint : public FairMCPoint 
{

 public:

  /** Default constructor **/
  PndSciTPoint();


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
  
  PndSciTPoint(Int_t trackID, Int_t evtID,
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
	      Int_t pdgCode);
  
  /** Copy constructor **/
  PndSciTPoint(const PndSciTPoint& point) { *this = point; };
  

  /** Destructor **/
  virtual ~PndSciTPoint();
  

  /** Accessors **/
  //Int_t    GetTrackID()    const { return fTrackID; }; 
  Int_t    GetEventID()    const { return fEventID; };
  Int_t    GetVolumeID() const { return fVolumeID; };
  Double_t GetXin()          const { return fXin; };
  Double_t GetYin()          const { return fYin; };
  Double_t GetZin()          const { return fZin; };
  Double_t GetPxin()         const { return fPxin; };
  Double_t GetPyin()         const { return fPyin; };
  Double_t GetPzin()         const { return fPzin; };
  
  Double_t GetXout()          const { return fXout; };
  Double_t GetYout()          const { return fYout; };
  Double_t GetZout()          const { return fZout; };
  Double_t GetPxout()         const { return fPxout; };
  Double_t GetPyout()         const { return fPyout; };
  Double_t GetPzout()         const { return fPzout; };
 
  
  TString GetDetName() const { return fDetName;};
  Double_t GetMass() const { return fmass; };
 
  
  void PositionIn(TVector3& pos)   { pos.SetXYZ(fXin, fYin, fZin); };
  void MomentumIn(TVector3& mom)   { mom.SetXYZ(fPxin, fPyin, fPzin); };
 
  void PositionOut(TVector3& pos)   { pos.SetXYZ(fXout, fYout, fZout); };
  void MomentumOut(TVector3& mom)   { mom.SetXYZ(fPxout, fPyout, fPzout); };
   
 
 
  
  /** Modifiers **/
  //void SetTrackID(Int_t id)          { fTrackID    = id; } ;
  void SetEventID(Int_t id)          { fEventID    = id; };
  //void SetTime(Double_t tof)        { fTime       = tof; };
  //void SetLength(Double_t length)    { fLength     = length; };
  //void SetEnergyLoss(Double_t eLoss) { fELoss      = eLoss; };
  
  void SetPositionIn(const TVector3& pos);
  void SetMomentumIn(const TVector3& mom);
  void SetPositionOut(const TVector3& pos);
  void SetMomentumOut(const TVector3& mom);

 

   
   
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


 protected:
  Int_t fEventID;               // Event ID
 
 
  
  Double_t fmass,fcharge;
  
  Double_t  fXin, fYin, fZin;
  Double_t  fPxin,fPyin,fPzin;
  Double_t  fXout, fYout, fZout;
  Double_t  fPxout,fPyout,fPzout;
 
  TString fDetName;
  
  Int_t fVolumeID,fpdgCode;
  
  
  ClassDef(PndSciTPoint,2)

};

inline void PndSciTPoint::SetPositionIn(const TVector3& pos) {
  fXin = pos.X();
  fYin = pos.Y();
  fZin = pos.Z();
}

inline void PndSciTPoint::SetMomentumIn(const TVector3& mom) {
  fPxin = mom.X();
  fPyin = mom.Py();
  fPzin = mom.Pz();
}

inline void PndSciTPoint::SetPositionOut(const TVector3& pos) {
  fXout = pos.X();
  fYout = pos.Y();
  fZout = pos.Z();
}

inline void PndSciTPoint::SetMomentumOut(const TVector3& mom) {
  fPxout = mom.Px();
  fPyout = mom.Py();
  fPzout = mom.Pz();
}



#endif
