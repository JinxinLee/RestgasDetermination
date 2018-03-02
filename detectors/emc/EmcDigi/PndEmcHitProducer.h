// -------------------------------------------------------------------------
// -----                 PndEmcHitProducer header file                 -----
// -----                 Created 14/08/06  by S.Spataro                -----
// -------------------------------------------------------------------------
#ifndef PNDEMCHITPRODUCER_H
#define PNDEMCHITPRODUCER_H

#include <PndPersistencyTask.h>
#include <map>
#include <string>
#include <limits>

#include "TH1F.h"
#include "TFile.h"
#include "TVector3.h"

#include "PndEmcHit.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"		


class TClonesArray;
class TObjectArray;

class PndEmcDigiPar;
class PndEmcGeoPar;
class PndEmcDigiNonuniformityPar;

using std::cout;
using std::endl;
using std::map;

/**
 * @brief creates PndEmcHit%s from PndEmcPoint%s
 * @ingroup PndEmc
 */
class PndEmcHitProducer : public PndPersistencyTask
{
  
 public:
  typedef std::map<Int_t, Float_t> mapper;
  
  /** Default constructor **/  
  PndEmcHitProducer();
  
  PndEmcHitProducer(Bool_t val);
  
  /** Destructor **/
  ~PndEmcHitProducer();
  
  
  /** Virtual method Init **/
  virtual InitStatus Init();
  
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  PndEmcHit* AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time,std::vector <Int_t> &mctruth, FairMultiLinkedData entering, FairMultiLinkedData exiting);

  //PndEmcHit* AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time,std::vector<PndEmcPoint*> pointList);
  // not implemented
  //void CreateStructure();
  
	void SetStorageOfData(Bool_t val); // Method to specify whether hits are stored or not.
	void SetNonuniformityFile(const char * filename){fNonuniformityFile = filename;};

	void FinishTask();
 private: 
	Int_t fUse_nonuniformity;
	TString fNonuniformityFile;

	/** Input array of PndEmcPoints **/
	TClonesArray* fPointArray;
	TClonesArray* fMCTrackArray;

	/** Output array of PndEmcHit **/
	TClonesArray* fHitArray;  
	//TClonesArray* fMcTrackArray;

	TObjArray *fVolumeArray;

	/** Geo file to use **/
	Int_t fMapVersion;
	Float_t fEnergyThreshold;

	mapper emcX;
	mapper emcY;
	mapper emcZ;

	PndEmcStructure* fEmcStr;
	PndEmcMapper* fMapper;

	PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/
	PndEmcGeoPar*     fGeoPar;       /** Geometry parameter container **/
	PndEmcDigiNonuniformityPar *fNonuniformityPar;

	/** Get parameter containers **/
	virtual void SetParContainers();

	Bool_t fStoreHits; // Flag which specify whether hits are stored or not

	void cleansortmclist( std::vector <Int_t> &newlist,TClonesArray* mcTrackArray);

	PndEmcHitProducer(const  PndEmcHitProducer& L);
	PndEmcHitProducer& operator= (const  PndEmcHitProducer&) {return *this;}

	ClassDef(PndEmcHitProducer,1);

	map<Int_t, Float_t> fTrackEnergy;
	map<Int_t, Float_t> fTrackTime;  //time of first point
	map<Int_t, std::vector <Int_t> > fTrackMcTruth;  //McTruth
	map<Int_t, std::vector <Int_t> > fPointMatch; //DetId , PointIds with same DetId
	map<Int_t, FairMultiLinkedData> fTrackEntering; //DetId, link to tracks entering same DetId
	map<Int_t, FairMultiLinkedData> fTrackExiting; //DetId, link to track exiting same DetId
};
#endif
