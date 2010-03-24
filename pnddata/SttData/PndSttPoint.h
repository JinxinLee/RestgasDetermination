#ifndef PNDSTTPOINT_H
#define PNDSTTPOINT_H


#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"


class PndSttPoint : public FairMCPoint 
{

 public:

  /** Default constructor **/
  PndSttPoint();


  /** Constructor with arguments
   *@param trackID       Index of MCTrack
   *@param detID         Detector ID
   *@param pos           Coordinates at wire center of active volume [cm]
   *@param posInLocal    Local coordinates at entrance to active volume [cm]
   *@param posOutLocal   Local coordinates at exit of active volume [cm]
   *@param momIn         Momentum of track at entrance [GeV]
   *@param momOut        Momentum of track at exit [GeV]
   *@param tof           Time since event start [ns]
   *@param length        Track length since creation [cm]
   *@param eLoss         Energy deposit [GeV]
   **/
  PndSttPoint(Int_t trackID, Int_t detID, TVector3 pos, 
	      TVector3 posInLocal, TVector3 posOutLocal, 
	      TVector3 momIn, TVector3 momOut, 
	      Double_t tof, Double_t length, Double_t eLoss, Double_t mass, TVector3 postot); // da cancellare postot


  /** Copy constructor **/
  PndSttPoint(const PndSttPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndSttPoint();

  // da cancellare
  Double_t GetXtot()  const { return fxtot; }
  Double_t GetYtot()  const { return fytot; }
  Double_t GetZtot()  const { return fztot; }
  void SetPositiontot(TVector3 postot);
  Double_t GetPXtot()  const { return fpxtot; }
  Double_t GetPYtot()  const { return fpytot; }
  Double_t GetPZtot()  const { return fpztot; }
  //void SetMomentumtot(TVector3 momtot);  // not implemented
  //////

  /** Accessors **/
  Double_t GetXOutLocal()  const { return fX_out_local; }
  Double_t GetYOutLocal()  const { return fY_out_local; }
  Double_t GetZOutLocal()  const { return fZ_out_local; }
  Double_t GetXInLocal()  const { return fX_in_local; }
  Double_t GetYInLocal()  const { return fY_in_local; }
  Double_t GetZInLocal()  const { return fZ_in_local; }

  Double_t GetPxOut() const { return fPx_out; }
  Double_t GetPyOut() const { return fPy_out; }
  Double_t GetPzOut() const { return fPz_out; }

  Double_t GetMass() const {return fMass;}
 
  void PositionOutLocal(TVector3& pos) { pos.SetXYZ(fX_out_local,fY_out_local,fZ_out_local); }
  void PositionInLocal(TVector3& pos)  { pos.SetXYZ(fX_in_local,fY_in_local,fZ_in_local); }
  void MomentumOut(TVector3& mom) { mom.SetXYZ(fPx_out,fPy_out,fPz_out); }

  /** Modifiers **/
  void SetPositionOutLocal(TVector3 pos);
  void SetPositionInLocal(TVector3 pos);
  void SetMomentumOut(TVector3 mom);

  // tube ID // CHECK added
  void SetTubeID(Int_t tubeid) { fTubeID = tubeid; }
  Int_t GetTubeID() { return fTubeID; }

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;



 protected:
  // exit coordinates in straw frame
  Double_t fX_out_local,  fY_out_local,  fZ_out_local;
  // entry coordinates in straw frame
  Double_t fX_in_local,  fY_in_local,  fZ_in_local;

  Double_t fPx_out, fPy_out, fPz_out;

  // stt1 - particle mass
  Double_t fMass;

  // da cancellare
  Double_t fxtot, fytot, fztot; 
  Double_t fpxtot, fpytot, fpztot; 
 
  Int_t fTubeID; // CHECK added

 //////

  ClassDef(PndSttPoint,1)

    };



inline void PndSttPoint::SetPositionOutLocal(TVector3 pos) {
  fX_out_local = pos.X();
  fY_out_local = pos.Y();
  fZ_out_local = pos.Z();
}

inline void PndSttPoint::SetPositionInLocal(TVector3 pos) {
  fX_in_local = pos.X();
  fY_in_local = pos.Y();
  fZ_in_local = pos.Z();
}


inline void PndSttPoint::SetMomentumOut(TVector3 mom) {
  fPx_out = mom.Px();
  fPy_out = mom.Py();
  fPz_out = mom.Pz();
}

// da cancellare
inline void PndSttPoint::SetPositiontot(TVector3 postot){
  fxtot = postot.X();
  fytot = postot.Y();
  fztot = postot.Z();
}
////////
#endif
