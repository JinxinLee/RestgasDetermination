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

#include "FairTSBufferFunctional.h"
#include "TStopwatch.h"

class TClonesArray;
class TObjectArray;
class PndEmcDigi;
class PndEmcCluster;
class PndEmcGeoPar;
class PndEmcDigiPar;
class PndEmcRecoPar;
class BinaryFunctor;


/*! 
 * \brief Task to cluster PndEmcDigis
 *
 * Task to cluster PndEmcDigis. In the eventbased version clusters are built within one event, all adjacent digis go in one cluster.
 * The timebased version assigns an active time to every cluster. Once expired, no digis can be added any more to cluster. 
 * The spatial cluster condition remains the same as in the eventbased reconstruction
 */

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

  //Finish clusters after subtasks have been executed
  virtual void ExecuteTasks(Option_t* option) {		
	FairTask::ExecuteTasks(option);
	FinishClusters();
  }

  void SetStorageOfData(Bool_t val); 	//!< Method to specify whether clusters are stored or not.

  /*! 
   * \brief Method to specify whether underlying digis are stored or not.
   *
   *	Restoring digis makes only sense if reconstruction is done timebased	
   */
  void StoreClusterBaseDigis(Bool_t val = kTRUE) { fStoreClusterBase = kTRUE; }


 
  void SetClusterActiveTime(Double_t time) { fClusterActiveTime = time; } //!<  Override EmcRecoPar cluster active time parameter ..to be set in ns!!! 
  
protected:

  
private:
	/** Input array of CbmDigis **/
	TClonesArray* fDigiArray;
	
	/** Input array of Hits and MC Tracks ... needed for MC **/
	TClonesArray* fHitArray;
	TClonesArray* fMCTrackArray;

	/** Output array of PndEmcClusters **/
	TClonesArray* fClusterArray;	//!< active clusters
	TClonesArray* fWriteOutArray;	//!< expired clusters
	  
	BinaryFunctor* fDigiFunctor;
	std::vector<PndEmcCluster*> fClusterList;
	
	Double_t fDigiEnergyTresholdBarrel;
	Double_t fDigiEnergyTresholdFWD;
	Double_t fDigiEnergyTresholdBWD;
	Double_t fDigiEnergyTresholdShashlyk;
	Double_t fClusterActiveTime; //!< Defines how long clusters are kept open in timebased reconstruction
	
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
	Bool_t fStoreClusterBase; 	//restore digis in case of a timebased run

	void cleansortmclist( std::vector <Int_t> &newlist,TClonesArray* mcTrackArray);

	virtual void FinishClusters();

	/*! 
	 * \brief Assign final parameters to cluster
	 *
	 * Assign final parameters to cluster. Subtasks might introduce changes in cluster composition. To keep track of them, function is called after subtasks have been executed
	 */
	void FinishCluster(PndEmcCluster* tmpcluster);
	bool HasExpired(PndEmcDigi* latestDigi, PndEmcCluster* theCluster, Int_t clusterIdx);

        PndEmcMakeCluster(const  PndEmcMakeCluster& L);
        PndEmcMakeCluster& operator= (const  PndEmcMakeCluster&) {return *this;};

	TStopwatch fTimer;

	ClassDef(PndEmcMakeCluster,2)
};
#endif // PNDEMCMAKECLUSTER_HH
