// -------------------------------------------------------------------------
// -----                 PndEmcHitProducer header file                 -----
// -----                 Created 14/08/06  by S.Spataro                -----
// -------------------------------------------------------------------------
#pragma once
#ifndef PNDEMCHITPRODUCER_H
#define PNDEMCHITPRODUCER_H


#include "FairTask.h"
#include "TH1F.h"
#include "TFile.h"
#include "PndEmcHit.h"
#include "TVector3.h"
#include <map>
#include <string>
#include "PndEmcStructure.h"

class TClonesArray;
class TObjectArray;

class PndEmcDigiPar;

class PndEmcHitProducer : public FairTask
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

   PndEmcHit* AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time,std::vector <Int_t> &mctruth);

   PndEmcHit* AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time,std::vector<PndEmcPoint*> pointList);
  //void CreateStructure();
  
   void SetStorageOfData(Bool_t val); // Method to specify whether hits are stored or not.

 private: 
   
	/** Input array of PndEmcPoints **/
	TClonesArray* fPointArray;
	TClonesArray* fMCTrackArray;
	
	/** Output array of PndEmcHit **/
	TClonesArray* fDigiArray;  
	
	TObjArray *fVolumeArray;
	
	/** Geo file to use **/
	Int_t fMapVersion;
	Float_t fEnergyThreshold;
  
	mapper emcX;
	mapper emcY;
	mapper emcZ;
	
	PndEmcStructure* fEmcStr;

	PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/

	/** Get parameter containers **/
	virtual void SetParContainers();

	Bool_t         fStoreHits; // Flag which specify whether hits are stored or not

	void cleansortmclist( std::vector <Int_t> &newlist,TClonesArray* mcTrackArray);

  ClassDef(PndEmcHitProducer,1);
  
};

#endif
