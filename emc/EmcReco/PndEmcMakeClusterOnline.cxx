//---------------------------------------------------------------------
// File and Version Information:
// 	$Id: $
//
// Description:
//	This object constructs a list of clusters from a list of digis.
//
// Environment:
//	Software developed for the PANDA Detector at GSI.
//
// STUB FOR MARCEL TIEMENS to implement online clustering algorithm in pandaroot


#include "PndEmcMakeClusterOnline.h"

#include "PndEmcClusterProperties.h"
#include "PndEmcXClMoments.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TROOT.h"


#include <iostream>

using std::cout;
using std::endl;


static Int_t evtCounter = 0;
static Int_t digiCounter = 0;

PndEmcMakeClusterOnline::PndEmcMakeClusterOnline(Int_t verbose, Bool_t storeclusters):
	FairTask("EmcClusteringTask", verbose),
	fDigiArray(NULL), 
	fClusterArray(NULL), 
	fGeoPar(new PndEmcGeoPar()), 
	fRecoPar(new PndEmcRecoPar()), 
	fDigiEnergyTresholdBarrel(0.), fDigiEnergyTresholdFWD(0.), fDigiEnergyTresholdBWD(0.), fDigiEnergyTresholdShashlyk(0.), 
	fClusterActiveTime(0.),
	fDigiFunctor(NULL), 
	fStoreClusters(storeclusters), 
	fStoreClusterBase(kTRUE)
{
}

//--------------
// Destructor --
//--------------

PndEmcMakeClusterOnline::~PndEmcMakeClusterOnline()
{ 
}

// -----   Public method Init   -------------------------------
InitStatus PndEmcMakeClusterOnline::Init() {

	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcMakeClusterOnline::Init: " << "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	if(!FairRunAna::Instance()->IsTimeStamp()) {
		cout << "-E- PndEmcMakeClusterOnline::Init: " << "This task can be activated only online" << endl;
		return kFATAL;
	}

	// Get input array
	fDigiArray = (TClonesArray*) ioman->GetObject("EmcDigiSorted");
	if (!fDigiArray) {
		cout << "-W- PndEmcMakeClusterOnline::Init: " << "No PndEmcDigi array!" << endl;
		return kERROR;
	}

	// Create and register output array
	fClusterArray = new TClonesArray("PndEmcCluster");
	ioman->Register("EmcCluster","Emc", fClusterArray, fStoreClusters);

	// for subsequent methods we need the "event grouping" as given by the TS buffer 
	// --> fDigiArray becomes an output array. 
	//
	// can be removed once PndEmcCluster object has been rewritten
	fDigiArray = new TClonesArray(fDigiArray->GetClass());		//still needed to activate array 
	ioman->Register("EmcDigiClusterBase", "Emc", fDigiArray, fStoreClusterBase);

	// searching for time gaps in digi stream
	fDigiFunctor = new TimeGap();

	fGeoPar->InitEmcMapper();  
	PndEmcStructure::Instance();

	// get some parameters from the parbase
	fDigiEnergyTresholdBarrel=fRecoPar->GetEnergyThresholdBarrel();
	fDigiEnergyTresholdFWD=fRecoPar->GetEnergyThresholdFWD();
	fDigiEnergyTresholdBWD=fRecoPar->GetEnergyThresholdBWD();
	fDigiEnergyTresholdShashlyk=fRecoPar->GetEnergyThresholdShashlyk();

	//convert from seconds to nanoseconds...in parfile everything is seconds...here we need nanoseconds
	fClusterActiveTime=fRecoPar->GetClusterActiveTime() * 1.0e9; 

	cout << "-I- PndEmcMakeClusterOnline: Intialization successfull" << endl;
	return kSUCCESS;
}


void PndEmcMakeClusterOnline::Exec(Option_t* opt) {

	if (fVerbose>2){
		fTimer.Start();
	}

	// Reset output arrays
	if ( ! fClusterArray ) Fatal("Exec", "No Cluster Array");
	fClusterArray->Delete();
	fDigiArray->Delete();

	
	// Get all digis up to a certain time gap specified by fClusterActiveTime
	fDigiArray->AbsorbObjects(FairRootManager::Instance()->GetData("EmcDigiSorted", fDigiFunctor, fClusterActiveTime));

	// --------------------- start coding here :-) ---------------------------


	Int_t nDigis = fDigiArray->GetEntriesFast();

	if (fVerbose>2){
		cout<<"Event no: " << evtCounter++ << "\tDigiList length: "<<nDigis<<endl;
		digiCounter += nDigis;
	}

	/* create a new cluster like this */
	PndEmcCluster* cluster_A = new((*fClusterArray)[fClusterArray->GetEntriesFast()]) PndEmcCluster();
	PndEmcCluster* cluster_B = new((*fClusterArray)[fClusterArray->GetEntriesFast()]) PndEmcCluster();


	//loop over all digis to add them to clusters
	
	for (Int_t iDigi=0; iDigi<nDigis; ++iDigi) {

		/* Get a new digi from the digi array */
		PndEmcDigi* theDigi = (PndEmcDigi*) fDigiArray->At(iDigi);

		// thresholds: if energy is below the corresponding threshold, digi is not considered in clustering
		{ 
			Int_t module=theDigi->GetModule();
			if ((module==1||module==2) && (theDigi->GetEnergy()< fDigiEnergyTresholdBarrel)) continue;
			if ((module==3) && (theDigi->GetEnergy()< fDigiEnergyTresholdFWD)) continue;
			if ((module==4) && (theDigi->GetEnergy()< fDigiEnergyTresholdBWD)) continue;
			if ((module==5) && (theDigi->GetEnergy()< fDigiEnergyTresholdShashlyk)) continue;
		}

		/* add digis alternating to cluster_A resp. to cluster_B */
		{
			if(iDigi%2) {
				cluster_A->addDigi(fDigiArray, iDigi);
				cluster_A->AddLink(theDigi->GetEntryNr());
			} else {
				cluster_B->addDigi(fDigiArray, iDigi);
				cluster_B->AddLink(theDigi->GetEntryNr());
			}
		}

		if(fVerbose>5) {
			/* function to ask weather a digi belongs to a cluster (i.e. is a neighbor) */
			if(cluster_A->isInCluster(theDigi, fDigiArray)) {
				cout << "incoming digi " << theDigi << " belongs to (is direct neighbor) cluster_A (" << cluster_A <<")" << endl;
			}
			if(cluster_B->isInCluster(theDigi, fDigiArray)) {
				cout << "incoming digi " << theDigi << " belongs to (is direct neighbor) cluster_B (" << cluster_A <<")" << endl;
			}
		}
	}

	/* merge two clusters */
	cluster_A->addCluster(cluster_B, fDigiArray);
	fClusterArray->Remove(cluster_B);		//remove cluster_B now from the list of clusters
	fClusterArray->Compress();			//remove open position from TClonesArray

	// --------------------- stop---------------------------------------------
	
	FinishClusters();

}


void PndEmcMakeClusterOnline::FinishClusters() {

	Int_t nCluster = fClusterArray->GetEntriesFast();
	for (Int_t i=0; i<nCluster; i++) {
		FinishCluster((PndEmcCluster*) (fClusterArray->At(i)));
	}

	if (fVerbose>2){
		fTimer.Stop();
		Double_t rtime = fTimer.RealTime();
		Double_t ctime = fTimer.CpuTime();
		fTimer.Reset();
		cout << "PndEmcMakeClusterOnline, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}	
}

void PndEmcMakeClusterOnline::FinishCluster(PndEmcCluster* cluster) {

	//----here you might set energy and time etc. to the clusters, e.g. like follows 
	
	std::vector<Int_t> list = cluster->DigiList();

	Double_t total_energy = 0;
	Double_t max_energy = 0;
	Int_t max_energy_idx = 0;

	for(size_t iDigi=0; iDigi<list.size(); ++iDigi) {
		Int_t idx = list[iDigi];
		PndEmcDigi* thedigi = (PndEmcDigi*) fDigiArray->UncheckedAt(idx);

		total_energy += thedigi->GetEnergy();
		if(thedigi->GetEnergy() > max_energy) {
			max_energy = thedigi->GetEnergy();
			max_energy_idx = idx;
		}
	}

	cluster->SetEnergy(total_energy);
	cluster->SetTimeStamp(static_cast<PndEmcDigi*>(fDigiArray->UncheckedAt(max_energy_idx))->GetTimeStamp());

	//-------------------stop-------------------------------------------------------------
}


void PndEmcMakeClusterOnline::FinishTask() {
	if(fVerbose>2) {
		std::cout << "Processed in total " << digiCounter << " digis within " << evtCounter << " events." << std::endl;
	}
}

void PndEmcMakeClusterOnline::SetParContainers() {

	// Get run and runtime database
	FairRun* run = FairRun::Instance();
	if ( ! run ) Fatal("SetParContainers", "No analysis run");

	FairRuntimeDb* db = run->GetRuntimeDb();
	if ( ! db ) Fatal("SetParContainers", "No runtime database");

	// Get Emc geometry parameter container
	fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");

	// Get Emc reconstruction parameter container
	fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");
}


void PndEmcMakeClusterOnline::SetStorageOfData(Bool_t val) {
	fStoreClusters=val;
	return;
}



ClassImp(PndEmcMakeClusterOnline)
