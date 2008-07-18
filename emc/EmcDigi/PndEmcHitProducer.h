// -------------------------------------------------------------------------
// -----                 PndEmcHitProducer header file                 -----
// -----                 Created 14/08/06  by S.Spataro                -----
// -------------------------------------------------------------------------

#ifndef PNDEMCHITPRODUCER_H
#define PNDEMCHITPRODUCER_H 1


#include "CbmTask.h"
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

class PndEmcHitProducer : public CbmTask
{

 public:
	typedef std::map<Int_t, Float_t> mapper;
	
	/** Default constructor **/  
	PndEmcHitProducer();
	
	
	/** Destructor **/
	~PndEmcHitProducer();
	
	
	/** Virtual method Init **/
	virtual InitStatus Init();


	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

   PndEmcHit* AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time);

   PndEmcHit* AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time,std::vector<PndEmcPoint*> pointList);
  //void CreateStructure();
  

 private: 
   
	/** Input array of PndEmcPoints **/
	TClonesArray* fPointArray;
	
	/** Output array of PndEmcHit **/
	TClonesArray* fDigiArray;  
	
	TObjArray *fVolumeArray;
	
	/** Geo file to use **/
	Int_t fMapVersion;
	Float_t fEnergyThreshold;
  
	mapper emcX;
	mapper emcY;
	mapper emcZ;
	
	PndEmcStructure *fEmcStr;

	PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/

	/** Get parameter containers **/
	virtual void SetParContainers();

 
  ClassDef(PndEmcHitProducer,1);
  
};

#endif
