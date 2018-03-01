//-----------------------------------------------------------------------
// File and Version Information:
// $Id: $
// STUB FOR MARCEL TIEMENS to implement online clustering algorithm in pandaroot
//---------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCMAKECLUSTERONLINE_H
#define PNDEMCMAKECLUSTERONLINE_H

#include "PndBranchTask.h"
#include <vector>		

#include "FairTSBufferFunctional.h"
#include "TStopwatch.h"

class TClonesArray;
class TObjectArray;
class PndEmcDigi;
class PndEmcCluster;
class PndEmcGeoPar;
class PndEmcRecoPar;
class BinaryFunctor;


class PndEmcMakeClusterOnline : public PndBranchTask
{
public:

  // Constructors

  PndEmcMakeClusterOnline(Int_t verbose=0, Bool_t storeclusters=kTRUE);

  // Destructor

  virtual ~PndEmcMakeClusterOnline( );

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void FinishTask();

  void SetStorageOfData(Bool_t val); 	//!< Method to specify whether clusters are stored or not.

  /// Restoring digis makes only sense if reconstruction is done timebased	
  void StoreClusterBaseDigis(Bool_t val = kTRUE) { fStoreClusterBase = val; }
 
  ///  Override EmcRecoPar cluster active time parameter ..to be set in ns!!! 
  void SetClusterActiveTime(Double_t time) { fClusterActiveTime = time; } 
  

protected:
  virtual void FinishClusters();
  void FinishCluster(PndEmcCluster* tmpcluster);
  
private:
	/** Input array of CbmDigis **/
	TClonesArray* fDigiArray;

	/** Output array of PndEmcClusters **/
	TClonesArray* fClusterArray;	//!< active clusters

	PndEmcGeoPar*     fGeoPar;       /** Geometry parameter container **/
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	
	Double_t fDigiEnergyTresholdBarrel; 
	Double_t fDigiEnergyTresholdFWD; 
	Double_t fDigiEnergyTresholdBWD; 
	Double_t fDigiEnergyTresholdShashlyk;

	Double_t fClusterActiveTime; //!< Defines how long clusters are kept open in timebased reconstruction
	BinaryFunctor* fDigiFunctor;

	Bool_t fStoreClusterBase; 	//restore digis in case of a timebased run

	/** Get parameter containers **/
	virtual void SetParContainers();

        PndEmcMakeClusterOnline(const  PndEmcMakeClusterOnline& L);
        PndEmcMakeClusterOnline& operator= (const  PndEmcMakeClusterOnline&) {return *this;};

	TStopwatch fTimer;

	ClassDef(PndEmcMakeClusterOnline, 1)
};
#endif // PNDEMCMAKECLUSTERONLINE_HH
