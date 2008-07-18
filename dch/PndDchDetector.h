// -------------------------------------------------------------------------
// -----                PndDchDetector header file                     -----
// -----                Created 15/05/07  by P. Hawranek               -----
// -------------------------------------------------------------------------

/** \class PndDchDetector
 *  \author P.Hawranek, A.Wronska <wronska@if.uj.edu.pl>
 *  \date
 *  \brief Class for conversion digis to spatial cylinder hits
 * 
 * This class represents a physical detector: defines how to construct
 * its geometry, action to be undertaken when particles pass its volumes,
 * the way and conditions to store MC information about hits
 **/

#ifndef PNDDCHDETECTOR_H
#define PNDDCHDETECTOR_H

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "CbmDetector.h"

class TClonesArray;
class PndDchPoint;
class TGeoVolume; 
class TGeoNode; 


class PndDchDetector : public CbmDetector 
{

 public:

  /** Default constructor **/
  PndDchDetector();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndDchDetector(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~PndDchDetector();


  /** Virtual method ProcessHits
   ** defines the action to be taken when a step is inside the
   ** active volume. Creates PndDchPoints and adds 
   ** them to the collections.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(CbmVolume* vol = 0);


  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent();

  /** Virtual method Register
   **
   ** Registers the hit collection in the ROOT manager.
   **/
  virtual void Register();


  /** Accessor to the hit collection **/
  virtual TClonesArray* GetCollection(Int_t iColl) const;


  /** Virtual method Print
   **
   ** Screen output of hit collection.
   **/
  virtual void Print() const;    


  /** Virtual method Reset
   **
   ** Clears the hit collection
   **/
  virtual void Reset();


  /** Virtual method Construct geometry
   **
   **/
  virtual void ConstructGeometry();

  
  PndDchPoint* AddPoint(Int_t trackID, Int_t evtID,  Int_t chamber,  Int_t plane,
		   TVector3 pos, TVector3 mom, 
		   Double_t tof, Double_t length, Double_t eLoss) const;

 private:

  Int_t          fTrackID;     ///<  track index
  Int_t          fChamber;     ///<  volume id (copy no)
  Int_t          fPlane;       ///<  module id
  TLorentzVector fPos;         ///<  position
  TLorentzVector fMom;         ///<  momentum
  Double32_t     fTime;        ///<  time
  Double32_t     fLength;      ///<  length
  Double32_t     fELoss;       ///<  energy loss
  Int_t fPosIndex;             /// 
  Int_t fVolDch;               ///<  MC volume ID of DCH
  
  TClonesArray* fDchPointCollection; ///< collection of MCPoints from dch
  Int_t fEvent;                ///<  event number from MC

  // reset all parameters   
  void ResetParameters();
  // construct detector from .geo file
  void ConstructASCIIGeometry();
  // construct detector from .root file 
  void ConstructRootGeometry();
  // recursively add volumes to the geoManager
  void ExpandNode(TGeoVolume *fVol, TGeoVolume *Cave);

  ClassDef(PndDchDetector,1)

}; 

inline void PndDchDetector::ResetParameters() {
  fTrackID = -1; 
  fChamber = -1; 
  fPlane   = -1;
  fVolDch  = -1;
  fEvent   = -1;
  fPos.SetXYZT(0., 0., 0., 0.);
  fMom.SetXYZT(0., 0., 0., 0.);
  fTime   = 0.;  
  fLength = 0.;
  fELoss  = 0.; 
}


#endif
