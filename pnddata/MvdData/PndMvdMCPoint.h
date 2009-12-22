// -------------------------------------------------------------------------
// -----                      CbmStsPoint header file                  -----
// -----                  Created 26/07/04  by V. Friese               -----
// -------------------------------------------------------------------------


/**  PndMvdMCPoint.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 *
 * Interception of MC track with a MVD detetcor. Holds in addition
 * to the base class the coordinates and momentum at the exit from
 * the active volume.
 **/


#ifndef PNDMVDMCPOINT_H
#define PNDMVDMCPOINT_H


#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"
#include "TString.h"
#include "PndDetectorList.h"

#include <stdio.h>
#include <iostream>

using namespace std;



class PndMvdMCPoint : public FairMCPoint
{
  friend std::ostream& operator<< (std::ostream& out, const PndMvdMCPoint& point){
    out << "-I- PndMvdMCPoint: PndMvd Point for track " << point.GetTrackID()
         << " in detector " << point.GetDetectorID() << " with name "
       << point.GetDetName() << std::endl;
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
  PndMvdMCPoint();


  /** Constructor with arguments
   *@param trackID       Index of MCTrack
   *@param detID         Detector ID
   *@param detName		   Full nameId of the detector, see PndMvdGeoHandling
   *@param posIn         Coordinates of ingoing point[cm]
   *@param posOut	 Coordinates of outgoing point[cm]
   *@param momIn         Momentum of track at entrance [GeV]
   *@param momOut        Momentum of track at exit [GeV]
   *@param tof           Time since event start [ns]
   *@param length        Track length since creation [cm]
   *@param eLoss         Energy deposit [GeV]
   **/
  PndMvdMCPoint(Int_t trackID, Int_t detID, TString detName,
  	      TVector3 posIn, TVector3 posOut,
	      TVector3 momIn, TVector3 momOut,
	      Double_t tof, Double_t length, Double_t eLoss);


  /** Copy constructor **/
  PndMvdMCPoint(const PndMvdMCPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndMvdMCPoint();


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
  virtual void SetTrackID(Int_t id){
	  //SetLink(kMCTrack, id);
	  FairMCPoint::SetTrackID(id);
  };



  /** Output to screen **/
  virtual void Print(const Option_t* opt = 0) const;


 protected:

  Double32_t fXfOut, fYfOut, fZfOut;

  Double32_t fPxfOut, fPyfOut, fPzfOut;
  TString fDetName;


  ClassDef(PndMvdMCPoint,3);

};

inline void PndMvdMCPoint::SetPositionOut(TVector3 pos) {
	fXfOut = pos.X();
	fYfOut = pos.Y();
	fZfOut = pos.Z();
}


inline void PndMvdMCPoint::SetMomentumOut(TVector3 mom) {
  fPxfOut = mom.Px();
  fPyfOut = mom.Py();
  fPzfOut = mom.Pz();
}



#endif
