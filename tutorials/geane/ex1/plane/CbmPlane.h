#ifndef CBMPlane_H
#define CBMPlane_H


#include "TClonesArray.h"
//#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairDetector.h"


class TClonesArray;
//class CbmPlaneImPlanePoint;
class CbmPlanePoint;
class FairVolume; 



class CbmPlane : public FairDetector 
{

 public:

  /** Default constructor **/
  CbmPlane();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  CbmPlane(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~CbmPlane();


  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for Plane detector and mirror.
   **/
  virtual void Initialize();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates CbmPlanePoints and CbmPlaneMirrorPoints and adds 
   ** them to the collections.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume* vol = 0);


  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent();


  virtual void BeginEvent();
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


  /** Virtual method CopyClones
   **
   ** Copies the hit collection with a given track index offset
   *@param cl1     Origin
   *@param cl2     Target
   *@param offset  Index offset
   **/
  virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2,
			  Int_t offset);


  /** Virtual method Construct geometry
   **
   **/
  virtual void ConstructGeometry();

 /*  CbmPlanePoint* AddHit(Int_t trackID, Int_t detID, TVector3 posIn, */
/* 			TVector3 pos_out, TVector3 momIn,  */
/* 			TVector3 momOut, Double_t time,  */
/* 			Double_t length, Double_t eLoss); */

  CbmPlanePoint* AddHit1(Int_t trackID, Int_t detID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Bool_t preflag, Bool_t postflag, TString nam,Int_t evt);

CbmPlanePoint* AddHit2(Int_t trackID, Int_t detID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Bool_t preflag, Bool_t postflag, TString nam,Int_t evt);

CbmPlanePoint* AddHit3(Int_t trackID, Int_t detID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Bool_t preflag, Bool_t postflag, TString nam,Int_t evt);




  void plane();



 private:

  Int_t          fTrackID;           //!  track index
  Int_t          fVolumeID;          //!  volume id
  TLorentzVector fPosIn, fPosOut;    //!  position
  TLorentzVector fMomIn, fMomOut;    //!  momentum
  Double32_t     fTime;              //!  time
  Double32_t     fLength;            //!  length
  Double32_t     fELoss;             //!  energy loss
  Int_t fPosIndex;      //! 
  Int_t volDetector;    //!  MC volume ID of Plane
  Double_t fcharge;  
  Double_t fmass;
  Bool_t fpreflag, fpostflag;
  
  TString fcurrnam;
  TString fcurrnam2;

  TClonesArray* fPlane1Collection;        //! Hit collection
  TClonesArray* fPlane2Collection;        //! Hit collection
  TClonesArray* fPlane3Collection;        //! Hit collection

  Int_t fevent;
  // reset all parameters   
  void ResetParameters();

  ClassDef(CbmPlane,1)

}; 

inline void CbmPlane::ResetParameters() {
  fTrackID = -999; 
  fVolumeID = -999;
  fPosIn.SetXYZT(-999., -999., -999., -999.);
  fPosOut.SetXYZT(-999., -999., -999., -999.) ;
  fMomIn.SetXYZT(-999., -999., -999., -999.) ;
  fMomOut.SetXYZT(-999., -999., -999., -999.);
  fTime = -999;  
  fLength = -999;
  fELoss = 0; 
  fmass = -999;
  fpreflag = -999;
  fpostflag = -999;
}



#endif
