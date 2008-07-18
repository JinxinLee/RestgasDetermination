#ifndef PNDRPCDETECTOR_H
#define PNDRPCDETECTOR_H

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "CbmDetector.h"
#include "PndRpcGeoPar.h"

using namespace std;
using std::ofstream;

class TClonesArray;
class PndRpcPoint;
class CbmVolume; 



class PndRpcDetector : public CbmDetector 
{

 public:

  /** Default constructor **/
  PndRpcDetector();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndRpcDetector(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~PndRpcDetector();

  /** Virtual method ProcessHits
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates RpcPoints and adds 
   ** them to the collections.
   *@param vol  Pointer to the active volume
   **/

 virtual void Initialize();

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

  
  PndRpcPoint* AddHit(Int_t trackID, Int_t detID,  
                      TVector3 pos, TVector3 mom,  Double_t time, 
                      Double_t length, Double_t eLoss,  Int_t eventID, 
                      Double_t t1, Double_t t2, TVector3 pos_sm); 




 private:

  Int_t          fTrackID;           //!  track index
  Int_t          fDetectorID;        //
  Int_t          fVolumeID;          //!  volume id (copy no)
  TLorentzVector fPos;               //!  position
  TLorentzVector fPos_sm;               //!  position
  TLorentzVector fMom;               //!  momentum
  Double_t     fTime;              //!  time
  Double_t     fLength;            //!  length
  Double_t     fELoss;             //!  energy loss
  Int_t          fPosIndex;          //! 
  Int_t          fEventID;            //!  current Event      

  TClonesArray* fRpcPointCollection;        //! Point collection
  // reset all parameters   
  void ResetParameters();

  ClassDef(PndRpcDetector,1)

}; 

inline void PndRpcDetector::ResetParameters() {
 fTrackID = fVolumeID = 0;
  fPos.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPos_sm.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMom.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = 0;
  fLength =0;
  fELoss = 0;
  fPosIndex = 0;
  fEventID = 0;
}


#endif
