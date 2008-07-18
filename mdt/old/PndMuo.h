#ifndef PNDMUO_H
#define PNDMUO_H


#include "CbmDetector.h"
#include "PndGeoMuoPar.h"

//#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"

class TClonesArray;
class PndMuoPoint;
class CbmVolume; 



class PndMuo : public CbmDetector 
{

 public:

  /** Default constructor **/
  PndMuo();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndMuo(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~PndMuo();


  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for MUO detector and mirror.
   **/
  virtual void Initialize();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates PndMuoPoints and adds 
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

  PndMuoPoint* AddHit(Int_t trackID, Int_t detID,  Int_t evtID, TVector3 pos, TVector3 mom,
		      Double_t tof, Double_t length, Double_t eLoss, Short_t flag,
		      Short_t sec, Short_t plane, Short_t box, Short_t tube);

 private:
  
  PndGeoMuoPar *par;
  Int_t          fTrackID;           //!  track index
  Int_t          fVolumeID;          //!  volume id
  Int_t          fEventID;           //!  event id
  TLorentzVector fPos;               //!  position
  TLorentzVector fMom;    //!  momentum
  Double32_t     fTime;              //!  time
  Double32_t     fLength;            //!  length
  Double32_t     fELoss;             //!  energy loss
  Int_t fPosIndex;      //! 
  //Int_t volDetector;    //!  MC volume ID of MUO
  Short_t nFlag;
  Short_t nSec;
  
  TClonesArray* fMuoCollection;        //! Hit collection

  // reset all parameters   
  void ResetParameters();

  Short_t GetStrip(TVector3 pos);
  void MuoTransform(TVector3 &pos);
  
  ClassDef(PndMuo,1)

}; 


#endif
