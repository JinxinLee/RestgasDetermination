// -------------------------------------------------------------------------
// -----                  PndDchPoint header file                      -----
// -----                Created 15.05.2007  by P.Hawranek              -----
// -------------------------------------------------------------------------

/** \class PndDchPoint
 *  \author P.Hawranek, A.Wronska <wronska@if.uj.edu.pl>
 *  \date 15.05.2007
 *  \brief Class for keeping MCpoints registered in dch
 **/

#ifndef PNDDCHPOINT_H
#define PNDDCHPOINT_H

// Pnd includes
#include "CbmMCPoint.h"
#include "PndDchMapper.h"
// ROOT includes
#include "TObject.h"
#include "TVector3.h"

class PndDchPoint : public CbmMCPoint 
{

 public:
  
  /** Default constructor **/
  PndDchPoint();
  
  /** Constructor with arguments
  *  \param trackID  Index of MCTrack
  *  \param evtID    event number
  *  \param chamber  number od chamber
  *  \param plane    number of plane
  *  \param pos      Spacial coordinates [cm]
  *  \param mom      Momentum of track [GeV]
  *  \param tof      Time since event start [ns]
  *  \param length   Track length since creation [cm]
  *  \param eLoss    Energy deposit [GeV]
  **/
  PndDchPoint(Int_t trackID, Int_t evtID, Int_t chamber, Int_t plane, 
	   TVector3 pos, TVector3 mom,  
	   Double_t tof, Double_t length, Double_t eLoss);
  
  /** Copy constructor **/
  PndDchPoint(const PndDchPoint& point) { *this = point; };
  
  /** Destructor **/
  virtual ~PndDchPoint();

  /** Accessors **/
  Int_t GetTrackID()   const { return fTrackID; }
  Int_t GetEventID()   const { return fEventID; }
  Int_t GetPlane()     const { return PndDchMapper::CalculatePlane(fDetectorID); }
  Int_t GetChamber()   const { return PndDchMapper::CalculateChamber(fDetectorID); }
  
  /** Output to screen **/
  virtual void Print(const Option_t*) const;
  
 protected:
  Int_t fEventID; ///< number of event
  
 private: 
  /** Modifiers **/
  void SetDetectorID(Int_t ch, Int_t pl)  { 
    fDetectorID = PndDchMapper::CalculateDetectorID(ch,pl);
  }
  
  ClassDef(PndDchPoint,1);

};

#endif
