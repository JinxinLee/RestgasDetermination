// -------------------------------------------------------------------------
// -----                         PndMvdMaterialDet header file                    -----
// -----                  Created 6/04/06  by T. Stockmanns            -----
// -----                          Based on PndMvdMaterialDet                      -----
// -------------------------------------------------------------------------

/**  PndMvdMaterialDet.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Defines the active detector PndMvdMaterialDet. Constructs the geometry and
 ** registers MCPoints.
 **/



#ifndef PNDMVDMATERIALDET_H
#define PNDMVDMATERIALDET_H


//#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "FairDetector.h"
#include "TString.h"
#include "PndMvdGeoHandling.h"

#include <string>
#include <vector>
#include <map>

class TClonesArray;
class TGeoNode;
class PndMvdMCPoint;
class FairVolume;

class PndMvdMaterialDet : public FairDetector
{

 public:

  /** Default constructor **/
  PndMvdMaterialDet();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndMvdMaterialDet(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~PndMvdMaterialDet();

  virtual void Initialize();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates CbmTrdPoints and adds them to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t  ProcessHits(FairVolume* vol = 0);


  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, Print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent();

  virtual void   FinishRun();


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
   ** Constructs the PndMvdMaterialDet geometry
   **/
  virtual void ConstructGeometry();
  void ConstructRootGeometry();
  void ConstructASCIIGeometry();
  void ExpandNode(TGeoNode *fN);


 private:

  /** Track information to be stored until the track leaves the
      active volume. **/
  Int_t          fTrackID;           //!  track index
  Int_t          fVolumeID;          //!  volume id
  TLorentzVector fPosIn;             //!  entry position in global frame
  TLorentzVector fPosOut;            //!  exit position in global frame
  TLorentzVector fMomIn;             //!  momentum
  TLorentzVector fMomOut;            //!  momentum
  Double32_t     fTime;              //!  time
  Double32_t     fLength;            //!  length
  Double32_t     fELoss;             //!  energy loss

  Double32_t     fTotAbso;           //!  total absorption length
  Double32_t     fTotRadl;           //!  total radiation length

//  std::map<TString, Int_t> fDetIDMap;	 //!  map to create a unique Detector ID
  PndMvdGeoHandling* fGeoH;	     //! Gives Access to the Path info of a hit
  Int_t fPosIndex;                   //!
  TClonesArray* fPndMvdCollection;      //! Hit collection

  std::vector<std::string> fListOfSensitives;

  bool CheckIfSensitive(std::string name) const;

  /** Private method AddHit
   **
   ** Adds a CbmTrdPoint to the HitCollection
   **/
  PndMvdMCPoint* AddHit(Int_t trackID, Int_t detID, TString detName,
  		      TVector3 posIn, TVector3 posOut,
		      TVector3 momIn, TVector3 momOut,
		      Double_t time, Double_t length, Double_t eLoss)const;


  /** Private method ResetParameters
   **
   ** Resets the private members for the track parameters
   **/
  void ResetParameters();


  ClassDef(PndMvdMaterialDet,5);

};


inline void PndMvdMaterialDet::ResetParameters() {
  fTrackID = fVolumeID = 0;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
};


#endif

