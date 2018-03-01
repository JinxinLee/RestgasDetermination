// -------------------------------------------------------------------------
// -----                 PndEmcApdHitProducer header file                 -----
// -----                 Created 07/04/08  by S.Spataro                -----
// -------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCAPDHITPRODUCER_H
#define PNDEMCAPDHITPRODUCER_H


#include "PndBranchTask.h"
#include "PndEmcApdHit.h"
#include "TVector3.h"
#include "TArrayI.h"
#include <map>
#include <string>
#include "PndEmcStructure.h"

class TClonesArray;
class TObjectArray;

class PndEmcApdHitProducer : public PndBranchTask
{

 public:
	typedef std::map<Int_t, Float_t> mapper;
	
	/** Default constructor **/  
	PndEmcApdHitProducer();
	
	
	/** Destructor **/
	~PndEmcApdHitProducer();
	
	
	/** Virtual method Init **/
	virtual InitStatus Init();


	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

    //PndEmcApdHit* AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time);  // not implemented
   
   PndEmcApdHit* AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time, Int_t npoint, Int_t pointIndex[10]);

    //PndEmcApdHit* AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time,std::vector<PndEmcApdPoint*> pointList);  // not implemented
  //void CreateStructure();
  

 private: 
   
	/** Input array of PndEmcApdPoints **/
	TClonesArray* fPointArray;
	
	/** Output array of PndEmcApdHit **/
	TClonesArray* fDigiArray;  
	
	TObjArray *fVolumeArray;
	
	/** Geo file to use **/
	Int_t fMapVersion;
  
	mapper emcX;
	mapper emcY;
	mapper emcZ;
	
	PndEmcStructure *fEmcStr;

	PndEmcApdHitProducer(const  PndEmcApdHitProducer& L);
	PndEmcApdHitProducer& operator= (const  PndEmcApdHitProducer&) {return *this;}

  ClassDef(PndEmcApdHitProducer,1);
  
};

#endif
