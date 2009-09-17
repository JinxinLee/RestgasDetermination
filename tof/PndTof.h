// -------------------------------------------------------------------------
// -----                     CbmHyp header file                        -----
// -----               created by A. Sanchez                  -----
// -------------------------------------------------------------------------

#ifndef PNDTOF_H
#define PNDTOF_H


//#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairDetector.h"
#include "PndGeoTofPar.h"

class TClonesArray;
class PndTofPoint;
class PndTofSciFPoint;

class FairVolume; 

class PndTof : public FairDetector 
{

 public:

  /** Default constructor **/
  PndTof();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndTof(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~PndTof();


  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for MUO detector and mirror.
   **/
  virtual void Initialize();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates PndTofPoints and PndTofMirrorPoints and adds 
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

  PndTofPoint* AddHit(Int_t trackID, Int_t evtID, 
		      Int_t detID, TString detName,
		      TVector3 posin, 
		      TVector3 momin,
		      TVector3 posout, 
		      TVector3 momout,
		      Double_t tof, 
		      Double_t length, 
		      Double_t eLoss,
		      Double_t charge, 
		      Double_t mass, 
		      Int_t pdgCode,
		      Double_t dist,
		      Double_t PLin,
		      Double_t PLout);

  PndTofPoint* AddSciFHit(Int_t trackID, Int_t evtID, 
			    Int_t detID,TString detName,
			    TVector3 posin, 
			    TVector3 momin,
			    TVector3 posout, 
			    TVector3 momout,
			  Double_t tof, 
			    Double_t length,
			    Double_t eLoss,
			    Double_t charge, 
			    Double_t mass, 
			    Int_t pdgCode,
			    Double_t dist,
			    Double_t PLin,
			    Double_t PLout);

 

 private:
  
//  PndGeoTofPar *par;
  Int_t          fTrackID;           //  track index
  Int_t          fVolumeID;          //  volume id
  Int_t          fEventID;           //  event id
  TLorentzVector fPosIn;               //   position
  TLorentzVector fMomIn;               //  momentum
  TLorentzVector fPosOut;               //  position
  TLorentzVector fMomOut;               //  momentum
  
  Double_t fPLout,fPLin;               //  total momentum
 
  
  Double32_t     fTime;              //   time
  Double32_t     fLength;            //   length
  Double32_t     fELoss;             //   energy loss
  Int_t fPosIndex;      // 
  Int_t fpdgCode;      //  MC volume ID of MUO
  Int_t  SiId,CId,alId,beId,CpipeId ;     
  
  Double_t fcharge;  
  Double_t fmass, fdist;
  
  TClonesArray* fTofCollection;        //! Hit collection
  TClonesArray* fTofSciFCollection;        // Hit collection(Absorver)
 
  
  // reset all parameters   
  void ResetParameters();

  ClassDef(PndTof,2)

}; 


inline void PndTof::ResetParameters() {
  fTrackID = fVolumeID = 0;fEventID = -1;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);
 
  fTime = fLength = fELoss = 0;

};



#endif
