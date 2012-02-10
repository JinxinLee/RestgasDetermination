#ifndef PNDLUMI_H
#define PNDLUMI_H

// ROOT includes
#include "TClonesArray.h"
#include "TVector3.h"
#include "TString.h"
#include "TLorentzVector.h"

// Include base and geobase
#include "FairDetector.h"
#include "FairRun.h"
#include "FairVolume.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairBaseParSet.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRuntimeDb.h"
#include "FairGeoMedia.h"
#include "FairGeoVolume.h"
#include "FairRunSim.h"
#include "FairVolume.h"

//VMC
#include "TVirtualMC.h"
#include "TParticle.h"
#include "TObjArray.h"
#include "TList.h"
#include "TKey.h"
#include "TGeoManager.h"
#include "TGeoVoxelFinder.h"
#include "TGeoMatrix.h"

//pandaroot includes
#include "PndStack.h"

// LM includes
#include "PndLumiGeo.h"

#include "PndLumiPoint.h"
#include "PndLumiGeoPar.h"


class TGeoNode;


class PndLumi : public FairDetector
{
 public:
    /** Default constructor */
    PndLumi();

    /** Standard constructor.
	\param name    detector name
	\param active  sensitivity flag
    */
    PndLumi(const char* name, Bool_t active = kTRUE);

    /** Destructor */
    virtual ~PndLumi();

    /** Defines the action to be taken when a step is inside the
	active volume. Creates PndLumiPoints and adds them to the
	collection.

	\param vol  Pointer to the active volume
    */
    virtual void Initialize();

    virtual Bool_t ProcessHits(FairVolume* vol = 0);

    /** If verbosity level is set, print hit collection at the
	end of the event and resets it afterwards */
    virtual void EndOfEvent();

    /** Registers the hit collection with the ROOT manager */
    virtual void Register();

    /** Accessor to the hit collection */
    virtual TClonesArray* GetCollection(Int_t iColl) const;

    /** Screen output of hit collection */
    virtual void Print() const;

    /** Clears the point collection */
    virtual void Reset();

    /** Constructs the lumi geometry */
    virtual void ConstructGeometry();
    virtual void ConstructASCIIGeometry();
    //virtual void ConstructrootGeometry();

 private:
    /** Track information to be stored during tracking through a volume */
    Int_t          fTrackID;    	  // track index
    Int_t          fVolumeID;   	  // volume id
    TLorentzVector fPosIn;     		  // entry position in global frame
    TLorentzVector fPosOut;      	  // exit position in global frame
    TLorentzVector fMomIn;            // momentum at the entrance of one detector element
    TLorentzVector fMomOut;	          // momentum at the exit of one detector element
    Double32_t     fTime;             // time
    Double32_t     fLength;           // length
    Double32_t     fEnergyLoss;       // energy loss
    TString        fDetName;          // det name
    TClonesArray*  fLumiPointCollection;  // Pointer to hit collection

    std::vector<std::string> fListOfSensitives;

    /** Adds a FairTrdPoint to the HitCollection */
    PndLumiPoint* AddPoint(Int_t trackID, Int_t detID, TString detname,
    		TVector3 posIn, TVector3 posOut,
    		TVector3 momIn, TVector3 momOut,
    		Double_t time, Double_t length, Double_t eLoss)const;

    /** Resets the private members for the track parameters */
    void ResetParameters();
    bool CheckIfSensitive(std::string name);

    ClassDef(PndLumi,5);
};

#endif

