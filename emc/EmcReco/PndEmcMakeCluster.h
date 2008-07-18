//-----------------------------------------------------------------------
// File and Version Information:
// $Id: $
// Software developed for the PANDA Detector at GSI.		
// Author List:
//	Jan Zhong            
//---------------------------------------------------------------------

#ifndef PNDEMCMAKECLUSTER_HH
#define PNDEMCMAKECLUSTER_HH

#include "CbmTask.h"
#include <string>
#include <vector>		

class TClonesArray;
class TObjectArray;
class PndEmcCluster;
class PndEmcDigiPar;
class PndEmcRecoPar;

class PndEmcMakeCluster : public CbmTask
{


public:

  // Constructors

  PndEmcMakeCluster(Int_t verbose=0);

  // Destructor

  virtual ~PndEmcMakeCluster( );

  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


protected:

  
private:
	/** Input array of CbmDigis **/
	TClonesArray* fDigiArray;
	
	/** Output array of PndEmcClusters **/
	TClonesArray* fClusterArray;
	  
	std::vector<PndEmcCluster*> fClusterList;
	
	Double_t fDigiEnergyTresholdBarrel;
	Double_t fDigiEnergyTresholdFWD;
	Double_t fDigiEnergyTresholdBWD;
	Double_t fDigiEnergyTresholdShashlyk;
	Int_t fMapVersion;
	
	static Int_t fEventCounter;
	
	PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	/** Get parameter containers **/
	virtual void SetParContainers();
	
	/** Verbosity level **/
	Int_t fVerbose;
	
	ClassDef(PndEmcMakeCluster,1)
};
#endif // PNDEMCMAKECLUSTER_HH
