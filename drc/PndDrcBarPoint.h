// -------------------------------------------------------------------------
// -----                      PndDrcBarPoint header file                   -----
// -----                  Created 09/04/08  by E. Fioravanti and A. Cecchi               -----
// -------------------------------------------------------------------------


/**  PndDrcBarPoint.h
  **/



#ifndef PNDDRCBARPOINT_H
#define PNDDRCBARPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "CbmMCPoint.h"

class PndDrcBarPoint : public CbmMCPoint
{

 public:    

  /** Default constructor **/
  PndDrcBarPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Point coordinates [cm]
   *@param mom      Momentum of track at MCPoint [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  PndDrcBarPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
		 Double_t tof, Double_t length, Int_t pdgCode, Double_t angIn, Double_t thetaC, Int_t nBar, Int_t eventID);


 /** Copy constructor **/
  PndDrcBarPoint(const PndDrcBarPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndDrcBarPoint();


  /** Accessors **/
  Int_t    GetTrackID()    const { return fTrackID; };
  Int_t    GetDetectorID() const { return fCopyNo; };
  Double_t GetX()          const { return fX; };
  Double_t GetY()          const { return fY; };
  Double_t GetZ()          const { return fZ; };
  Double_t GetPx()         const { return fPx; };
  Double_t GetPy()         const { return fPy; };
  Double_t GetPz()         const { return fPz; };
  Double_t GetTime()       const { return fTime; };
  Double_t GetLength()     const { return fLength; };
  Int_t    GetEventID()    const { return fEventID;};
  Int_t    GetPdgCode()    const { return fPdgCode;};
  Double_t GetAngIn()      const { return fAngIn; };
  Double_t GetThetaC()     const { return fThetaC; };
  Int_t    GetNBar()       const { return fNBar;};
  void Position(TVector3& pos) { pos.SetXYZ(fX, fY, fZ); };
  void Momentum(TVector3& mom) { mom.SetXYZ(fPx, fPy, fPz); };



  /** Modifiers **
/*   void SetTrackID(Int_t id)          { fTrackID = id; }; */
/*   void SetDetectorID(Int_t id)       { fCopyNo = id; }; */
/*   void SetTime(Double_t time)        { fTime = time; }; */
/*   void SetLength(Double_t length)    { fLength = length; }; */
  void SetPosition(TVector3 pos);
  void SetMomentum(TVector3 mom);


  /** Output to screen **/
  //  virtual void Print(const Option_t* opt = 0) const = 0;
  virtual void Print(const Option_t* opt) const ;

		   
 protected:

  Int_t fTrackID;               // Track index
  Int_t fCopyNo;                // Detector ID
  Double32_t fX, fY, fZ;        // Point coordinates [cm]
  Double32_t fPx, fPy, fPz;     // Momentum components [GeV]
  Double32_t fTime;             // Time since event start [ns]
  Double32_t fLength;           // Track length since creation [cm]
  Int_t fPdgCode;
  Double_t fAngIn;
  Double_t fThetaC;
  Int_t fNBar;
  Int_t fEventID;



  ClassDef(PndDrcBarPoint,1)

};



inline void PndDrcBarPoint::SetPosition(TVector3 pos) {
  fX = pos.X();
  fY = pos.Y();
  fZ = pos.Z();
}


inline void PndDrcBarPoint::SetMomentum(TVector3 mom) {
  fPx = mom.Px();
  fPy = mom.Py();
  fPz = mom.Pz();
}


#endif
