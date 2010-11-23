// -------------------------------------------------------------------------
// -----                      PndGemPoint header file                  -----
// -----                  Created 27/10/08  by R. Kliemt               -----
// -------------------------------------------------------------------------


/**  PndTpcTBMCPoint.h
 *@author R.Kliemt
 *
 * Interception of MC track with a TpcTB-Detector. Holds in addition
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

class PndTpcTBMCPoint : public FairMCPoint 
{
  friend std::ostream& operator<< (std::ostream& out, 
				   const PndTpcTBMCPoint& point){
    out << "-I- PndTpcTBMCPoint: PndTpcTB Point for track " 
	<< point.GetTrackID() 
	<< " in detector " << point.GetDetectorID() << " with name "
	<< point.GetDetName() << std::endl;
    out << "    Position in  (" << point.GetX() 
	<< ", " << point.GetY() 
	<< ", " << point.GetZ() << ") cm" << std::endl;
    out << "    Position out (" << point.GetXOut() 
	<< ", " << point.GetYOut() 
	<< ", " << point.GetZOut() << ") cm" << std::endl;
    out << "    Momentum (" << point.GetPx() 
	<< ", " << point.GetPy() << ", " << point.GetPz()
	<< ") GeV" << std::endl;
    out << "    Time " << point.GetTime() 
	<< " ns,  Length " << point.GetLength() 
	<< " cm,  Energy loss " << point.GetEnergyLoss()*1.0e06 
	<< " keV" << std::endl;
    return out;
  }

 public:

  /** Default constructor **/
  PndTpcTBMCPoint();


  /** Constructor with arguments
   *@param trackID       Index of MCTrack
   *@param detID         Detector ID
   *@param detName		   Full nameId of the detector, see PndGemGeoHandling 
   *@param posIn         Coordinates of ingoing point[cm]
   *@param posOut	 Coordinates of outgoing point[cm]
   *@param momIn         Momentum of track at entrance [GeV]
   *@param momOut        Momentum of track at exit [GeV]
   *@param tof           Time since event start [ns]
   *@param length        Track length since creation [cm]
   *@param eLoss         Energy deposit [GeV]
   **/
  PndTpcTBMCPoint(Int_t trackID, Int_t detID, TString detName,
  	      TVector3 posIn, TVector3 posOut,
	      TVector3 momIn, TVector3 momOut,
	      Double_t tof, Double_t length, Double_t eLoss);


  /** Copy constructor **/
  PndTpcTBMCPoint(const PndTpcTBMCPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndTpcTBMCPoint();


  /** Accessors **/
  Double_t GetXOut() const { return fXfOut;};
  Double_t GetYOut() const { return fYfOut;};
  Double_t GetZOut() const { return fZfOut;};

  Double_t GetPxOut() const { return fPxfOut; }
  Double_t GetPyOut() const { return fPyfOut; }
  Double_t GetPzOut() const { return fPzfOut; }
  TString  GetDetName() const { return fDetName;}
  TVector3 GetPosition() const { return TVector3(fX, fY, fZ);}
  TVector3 GetPositionOut() const { return TVector3(fXfOut, fYfOut, fZfOut);}

  void PositionOut(TVector3& pos) const { pos.SetXYZ(fXfOut, fYfOut, fZfOut);};
  void MomentumOut(TVector3& mom) const { mom.SetXYZ(fPxfOut,fPyfOut,fPzfOut); }


  /** Modifiers **/
  void SetPositionOut(TVector3 pos);
  void SetMomentumOut(TVector3 mom);
  void SetDetName(TString name) {fDetName = name;};


  /** Output to screen **/
  virtual void Print(const Option_t* opt = 0) const;


 protected:

  Double32_t fXfOut, fYfOut, fZfOut;

  Double32_t fPxfOut, fPyfOut, fPzfOut;
  TString fDetName;


  ClassDef(PndTpcTBMCPoint,1);

};

inline void PndTpcTBMCPoint::SetPositionOut(TVector3 pos) {
	fXfOut = pos.X();
	fYfOut = pos.Y();
	fZfOut = pos.Z();
}


inline void PndTpcTBMCPoint::SetMomentumOut(TVector3 mom) {
  fPxfOut = mom.Px();
  fPyfOut = mom.Py();
  fPzfOut = mom.Pz();
}



#endif
