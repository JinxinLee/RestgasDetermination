// -------------------------------------------------------------------------
// -----                     PndSciT  header file                        -----
// -----               created by A. Sanchez                  -----
// -------------------------------------------------------------------------

#ifndef PNDSCIT_H
#define PNDSCIT_H


//#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairDetector.h"
#include "PndGeoSciTPar.h"
#include <string>
#include <vector>
#include "PndGeoHandling.h"

class TClonesArray;
class PndSciTPoint;
class FairVolume; 

class PndSciT : public FairDetector 
{

 public:

  /** Default constructor **/
  PndSciT();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndSciT(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~PndSciT();


  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for MUO detector and mirror.
   **/
  virtual void Initialize();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates PndSciTPoints and PndSciTMirrorPoints and adds 
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

  //void ConstructASCIIGeometry();


  PndSciTPoint* AddHit(Int_t trackID, Int_t evtID, 
		      Int_t detID, TString detName,
		      TVector3 posin, 
		      TVector3 momin,
		      TVector3 posout, 
		      TVector3 momout,
		      Double_t tof, 
		      Double_t length, 
		      Double_t eLoss);

 

 
 private:

  std::vector<std::string> fListOfSensitives;
  bool CheckIfSensitive(std::string name);
  
  PndGeoSciTPar *par;//!
  Int_t          fTrackID;           //  track index
  Int_t          fVolumeID;          //  volume id
  Int_t          fEventID;           //  event id
  TLorentzVector fPosIn;               //   position
  TLorentzVector fMomIn;               //  momentum
  TLorentzVector fPosOut;               //  position
  TLorentzVector fMomOut;               //  momentum
  
  PndGeoHandling* fGeoH;             //! Gives Access to the Path info of a hit
 
  
  Double32_t     fTime;              //   time
  Double32_t     fLength;            //   length
  Double_t     fELoss;             //   energy loss
  Int_t fPosIndex;      // 
 
  Int_t pvId;

  
  TClonesArray* fSciTCollection;        // Hit collection

  // reset all parameters   
  void ResetParameters();

  ClassDef(PndSciT,3)

}; 


inline void PndSciT::ResetParameters() {
  fTrackID = fVolumeID = 0;fEventID = -1;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);
 
  fTime = fLength = fELoss = 0;

};



#endif
