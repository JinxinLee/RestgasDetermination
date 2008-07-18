// -------------------------------------------------------------------------
// -----                      PndDrcPDPoint header file                   -----
// -----                  Created 09/04/08  by E. Fioravanti and A. Cecchi               -----
// -------------------------------------------------------------------------


/**  PndDrcPDPoint.h
  **/



#ifndef PNDDRCPDPOINT_H
#define PNDDRCPDPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "CbmMCPoint.h"

class PndDrcPDPoint : public CbmMCPoint
{

 public:    

  /** Default constructor **/
  PndDrcPDPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Point coordinates [cm]
   *@param mom      Momentum of track at MCPoint [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  PndDrcPDPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
		 Double_t tof, Double_t length, Int_t pdgCode, Int_t eventID);


 /** Copy constructor **/
  PndDrcPDPoint(const PndDrcPDPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndDrcPDPoint();


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
  Int_t fEventID;



  ClassDef(PndDrcPDPoint,1)

};



inline void PndDrcPDPoint::SetPosition(TVector3 pos) {
  fX = pos.X();
  fY = pos.Y();
  fZ = pos.Z();
}


inline void PndDrcPDPoint::SetMomentum(TVector3 mom) {
  fPx = mom.Px();
  fPy = mom.Py();
  fPz = mom.Pz();
}


#endif
