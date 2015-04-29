// --------------------------------------------------------------------------
// -----                     PndSciTpoint header file                  	-----
// -----                    created by A. Sanchez                  	-----
// -----                   modified by D. Steinschaden                 	-----
// -----                   last update    04.2015 		       	-----
// --------------------------------------------------------------------------

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
  
  PndSciTPoint( Int_t eventID, Int_t trackID,
	      Int_t detectorID,TString detName,
	      TVector3 posin, 
	      TVector3 momin, 
	      TVector3 posout, 
	      TVector3 momout,
	      Double_t tof, 
	      Double_t length, 
	      Double_t eLoss
);
  
  /** Copy constructor **/
  PndSciTPoint(const PndSciTPoint& point) { *this = point; };
  

  /** Destructor **/
  virtual ~PndSciTPoint();
  
  /** Accessors **/

  // -- FairMCPoint --
  /*
   UInt_t   GetEventID()    const { return fEventId; }        /// event identifier
   Int_t    GetTrackID()    const { return fTrackID; }
   Int_t    GetDetectorID()   const { return fDetectorID;};  // = short sensorID

   Double_t GetX()             const { return fX;};
   Double_t GetY()             const { return fY; };
   Double_t GetZ()             const { return fZ; };
   void Position(TVector3& pos)       const;

   Double_t GetPx()         const { return fPx; }
   Double_t GetPy()         const { return fPy; }
   Double_t GetPz()         const { return fPz; }
   void Momentum(TVector3& mom) { mom.SetXYZ(fPx, fPy, fPz); }

   Double_t GetTime()       const { return fTime; }
   Double_t GetLength()     const { return fLength; }
   Double_t GetEnergyLoss() const { return fELoss; }
  */
  // --- FairMCPoint end ---


  TString GetDetName() const { return fDetName;};

  Double_t GetXout()          const { return fXout; };
  Double_t GetYout()          const { return fYout; };
  Double_t GetZout()          const { return fZout; };
  Double_t GetPxout()         const { return fPxout; };
  Double_t GetPyout()         const { return fPyout; };
  Double_t GetPzout()         const { return fPzout; };
 
  void PositionOut(TVector3& pos)   { pos.SetXYZ(fXout, fYout, fZout); };
  void MomentumOut(TVector3& mom)   { mom.SetXYZ(fPxout, fPyout, fPzout); };
   
 
 
  
  /** Modifiers **/
  // -- FairMCPoint --
  /*
    void SetEventID(UInt_t eventId) { fEventId = eventId; }
    virtual void SetTrackID(Int_t id) { fTrackID = id;}
    void SetDetectorID(Int_t detID) { fDetectorID = detID; }

    void SetX(Double_t x) { fX = x; }
    void SetY(Double_t y) { fY = y; }
    void SetZ(Double_t z) { fZ = z; }
    void SetXYZ(Double_t x, Double_t y, Double_t z);
    void SetPosition(const TVector3& pos);

    void SetMomentum(const TVector3& mom);

    void SetTime(Double_t time)        { fTime = time; }
    void SetLength(Double_t length)    { fLength = length; }
    void SetEnergyLoss(Double_t eLoss) { fELoss = eLoss; }
  */
  // -- FairMCPoint end --

  void SetPositionOut(const TVector3& pos);
  void SetMomentumOut(const TVector3& mom);  
   
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


 protected:

  TString fDetName;

  Double_t  fXout, fYout, fZout;
  Double_t  fPxout,fPyout,fPzout;
  
  
  ClassDef(PndSciTPoint,4)

};


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
