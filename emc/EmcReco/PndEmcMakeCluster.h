//-----------------------------------------------------------------------
// File and Version Information:
// $Id: $
// Software developed for the PANDA Detector at GSI.		
// Author List:
//	Jan Zhong            
//---------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCMAKECLUSTER_H
#define PNDEMCMAKECLUSTER_H

#include "FairTask.h"
//#include <string>
#include <vector>		
//#include <list>		

class TClonesArray;
class TObjectArray;
class PndEmcCluster;
class PndEmcGeoPar;
class PndEmcDigiPar;
class PndEmcRecoPar;

class PndEmcMakeCluster : public FairTask
{


public:

  // Constructors

  PndEmcMakeCluster(Int_t verbose=0, Bool_t storeclusters=kTRUE);

  // Destructor

  virtual ~PndEmcMakeCluster( );

  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetStorageOfData(Bool_t val); // Method to specify whether clusters are stored or not.
	void FinishTask();

  
protected:

  
private:
	/** Input array of CbmDigis **/
	TClonesArray* fDigiArray;
	
	/** Input array of Hits and MC Tracks ... needed for MC **/
	TClonesArray* fHitArray;
	TClonesArray* fMCTrackArray;

	/** Output array of PndEmcClusters **/
	TClonesArray* fClusterArray;
	  
	std::vector<PndEmcCluster*> fClusterList;
	
	Double_t fDigiEnergyTresholdBarrel;
	Double_t fDigiEnergyTresholdFWD;
	Double_t fDigiEnergyTresholdBWD;
	Double_t fDigiEnergyTresholdShashlyk;
	
	std::vector<Double_t> fClusterPosParam;
	Int_t fMapVersion;
	
	static Int_t fEventCounter;
	
	PndEmcGeoPar*     fGeoPar;       /** Geometry parameter container **/
	PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	/** Get parameter containers **/
	virtual void SetParContainers();
	
	/** Verbosity level **/
	Int_t fVerbose;

	Bool_t fStoreClusters;
	//Bool_t fTimeOrderedDigi;
	//for time-order reconstruction
  //std::vector<PndEmcDigi*> fBufferofDigisToBeDetermined;

	void cleansortmclist( std::vector <Int_t> &newlist,TClonesArray* mcTrackArray);

	PndEmcMakeCluster(const  PndEmcMakeCluster& L);
	PndEmcMakeCluster& operator= (const  PndEmcMakeCluster&) {return *this;};

	//task counter
	Int_t HowManyDigi;
	Int_t HowManyCluster;
	//coefficients
	//TVectorD CoeffMod3;
	//TVectorD CoeffMod5;
	//TVectorD CoeffModo;

	ClassDef(PndEmcMakeCluster,1)

};
#endif // PNDEMCMAKECLUSTER_HH
