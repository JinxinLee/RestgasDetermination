// -------------------------------------------------------------------------
// -----                      PndGemPoint header file                  -----
// -----                  Created 27/10/08  by R. Kliemt               -----
// -------------------------------------------------------------------------


/**  PndGemMCPoint.h
 *@author R.Kliemt
 *
 * Interception of MC track with a MVD detetcor. Holds in addition
 * to the base class the coordinates and momentum at the exit from
 * the active volume.
 **/


#ifndef PNDGEMMCPOINT_H
#define PNDGEMMCPOINT_H


#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"
#include "TString.h"

#include <stdio.h>
#include <iostream>

using namespace std;



class PndGemMCPoint : public FairMCPoint 
{
  friend std::ostream& operator<< (std::ostream& out, const PndGemMCPoint& point){
    out << "-I- PndGemMCPoint: PndGem Point for track " << point.GetTrackID() 
         << " in detector " << point.GetDetectorID() << " with sensorID "
       << point.GetSensorId() << std::endl;
    out << "    Position in  (" << point.GetX() << ", " << point.GetY() << ", " << point.GetZ() << ") cm" << std::endl;
    out << "    Position out (" << point.GetXOut() << ", " << point.GetYOut() << ", " << point.GetZOut() << ") cm" << std::endl;
    out << "    Momentum (" << point.GetPx() << ", " << point.GetPy() << ", " << point.GetPz()
       << ") GeV" << std::endl;
    out << "    Time " << point.GetTime() << " ns,  Length " << point.GetLength() 
       << " cm,  Energy loss " << point.GetEnergyLoss()*1.0e06 << " keV" << std::endl;
    return out;
  }

 public:

  /** Default constructor **/
  PndGemMCPoint();

  /** Constructor with arguments
   *@param trackID       Index of MCTrack
   *@param detID         Detector ID
   *@param sensID        Sensor ID
   *@param posIn         Coordinates of ingoing point[cm]
   *@param posOut	 Coordinates of outgoing point[cm]
   *@param momIn         Momentum of track at entrance [GeV]
   *@param momOut        Momentum of track at exit [GeV]
   *@param tof           Time since event start [ns]
   *@param length        Track length since creation [cm]
   *@param eLoss         Energy deposit [GeV]
   **/
  PndGemMCPoint(Int_t trackID, Int_t detID, Int_t sensID,
  	      TVector3 posIn, TVector3 posOut,
	      TVector3 momIn, TVector3 momOut,
	      Double_t tof, Double_t length, Double_t eLoss);


  /** Copy constructor **/
  PndGemMCPoint(const PndGemMCPoint& point) :
      fXfOut(0.), fYfOut(0.), fZfOut(0.), 
      fPxfOut(0.), fPyfOut(0.), fPzfOut(0.),
      fSensorId(0)
      { *this = point; };


  /** Destructor **/
  virtual ~PndGemMCPoint();


  /** Accessors **/
  Double_t GetXOut() const { return fXfOut;};
  Double_t GetYOut() const { return fYfOut;};
  Double_t GetZOut() const { return fZfOut;};

  Double_t GetPxOut() const { return fPxfOut; }
  Double_t GetPyOut() const { return fPyfOut; }
  Double_t GetPzOut() const { return fPzfOut; }
  Int_t    GetSensorId() const { return fSensorId;}
  TVector3 GetPosition() const { return TVector3(fX, fY, fZ);}
  TVector3 GetPositionOut() const { return TVector3(fXfOut, fYfOut, fZfOut);}

  void PositionOut(TVector3& pos) const { pos.SetXYZ(fXfOut, fYfOut, fZfOut);};
  void MomentumOut(TVector3& mom) const { mom.SetXYZ(fPxfOut,fPyfOut,fPzfOut); }


  /** Modifiers **/
  void SetPositionOut(TVector3 pos);
  void SetMomentumOut(TVector3 mom);
  void SetSensorId   (Int_t sensId)    { fSensorId = sensId; };


  /** Output to screen **/
  virtual void Print(const Option_t* opt = 0) const;


 protected:

  Double32_t fXfOut, fYfOut, fZfOut;

  Double32_t fPxfOut, fPyfOut, fPzfOut;
  Int_t      fSensorId;


  ClassDef(PndGemMCPoint,1);

};

inline void PndGemMCPoint::SetPositionOut(TVector3 pos) {
	fXfOut = pos.X();
	fYfOut = pos.Y();
	fZfOut = pos.Z();
}


inline void PndGemMCPoint::SetMomentumOut(TVector3 mom) {
  fPxfOut = mom.Px();
  fPyfOut = mom.Py();
  fPzfOut = mom.Pz();
}



#endif
