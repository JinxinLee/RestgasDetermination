#ifndef PNDMUOPOINT_H
#define PNDMUOPOINT_H


#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"

class PndMuoPoint : public FairMCPoint 
{

 public:

  /** Default constructor **/
  PndMuoPoint();


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
  
  PndMuoPoint(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos, TVector3 mom,
	      Double_t tof, Double_t length, Double_t eLoss, Short_t flag, 
	      Short_t sec, Short_t plane, Short_t strip, Short_t box, Short_t tube);
  
  /** Copy constructor **/
  PndMuoPoint(const PndMuoPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndMuoPoint();
  

  /** Accessors **/
  Int_t    GetTrackID()    const { return fTrackID; }; 
  Int_t    GetEventID()    const { return fEventID; };
  Int_t    GetDetectorID() const { return fDetectorID; };
  Double_t GetX()          const { return fX; };
  Double_t GetY()          const { return fY; };
  Double_t GetZ()          const { return fZ; };
  Double_t GetPx()         const { return fPx; };
  Double_t GetPy()         const { return fPy; };
  Double_t GetPz()         const { return fPz; };
  Double_t GetTime()       const { return fTime; };
  Double_t GetLength()     const { return fLength; };
  Double_t GetEnergyLoss() const { return fELoss; };
  void Position(TVector3& pos)   { pos.SetXYZ(fX, fY, fZ); };
  void Momentum(TVector3& mom)   { mom.SetXYZ(fPx, fPy, fPz); };
  Short_t GetFlag()        const { return kFlag; };
  Short_t GetSector()      const { return nSec;}
  Short_t GetPlane()       const { return nPlane;}
  Short_t GetStrip()       const { return nStrip;}
  Short_t GetBox()         const { return nBox;}
  Short_t GetTube()        const { return nTube;}

  /** Modifiers **/
  void SetTrackID(Int_t id)          { fTrackID    = id; }; 
  void SetEventID(Int_t id)          { fEventID    = id; };
  void SetDetectorID(Int_t id)       { fDetectorID = id; };
  void SetTime(Double_t time)        { fTime       = time; };
  void SetLength(Double_t length)    { fLength     = length; };
  void SetEnergyLoss(Double_t eLoss) { fELoss      = eLoss; };
  void SetPosition(const TVector3& pos);
  void SetMomentum(const TVector3& mom);
  void SetFlag(Short_t flag)         { kFlag       = flag;};
  void SetSector(Short_t sec)        { nSec        = sec;};
  void SetPlane(Short_t pla)         { nPlane      = pla;};
  void SetStrip(Short_t strip)       { nStrip      = strip;};
  void SetBox(Short_t box)           { nBox        = box;};
  void SetTube(Short_t tub)          { nTube       = tub;};
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


 protected:
  Int_t fTrackID;               // Track index
  Int_t fDetectorID;            // Detector ID
  Int_t fEventID;               // Event ID
  Double32_t fX, fY, fZ;        // Point coordinates [cm]
  Double32_t fPx, fPy, fPz;     // Momentum components [GeV]
  Double32_t fTime;             // Time since event start [ns]
  Double32_t fLength;           // Track length since creation [cm]
  Double32_t fELoss;            // Energy loss at this point [GeV]
  Short_t kFlag;                // Is entering = -1, is exiting = +1
  TLorentzVector fPos;          //!  position
  TLorentzVector fMom;          //!  momentum
  Short_t nSec;                 // Sector number
  Short_t nPlane;               // Plane number
  Short_t nStrip;               //Strip number
  Short_t nBox;                 // Box number
  Short_t nTube;                // Tube number
    
  ClassDef(PndMuoPoint,1)

};

inline void PndMuoPoint::SetPosition(const TVector3& pos) {
  fX = pos.X();
  fY = pos.Y();
  fZ = pos.Z();
}

inline void PndMuoPoint::SetMomentum(const TVector3& mom) {
  fPx = mom.Px();
  fPy = mom.Py();
  fPz = mom.Pz();
}

#endif
