//---------------------------------------------------------------------
// File and Version Information:
// 	$Id: $
//
// Description:
//	This object constructs a list of clusters from a list of digis.
//      This module use a simple algorithm.
//      Digis which are in a Cluster are add to this cluster.
//      If a Digi is in more than one cluster, the clusters are merged.
//      Digis are added to the cluster if they have an energy above
//      _digiEnergyTreshold->value().
//
// Environment:
//	Software developed for the PANDA Detector at GSI.
//
// Author List:
//	Jan Zhong            //------------------------------------------------------------------------

#include "PndEmcMakeCluster.h"

#include "PndEmcStructure.h"
#include "PndEmcMapper.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "PndEmcClusterLiloPos.h"		
#include "PndMCTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TROOT.h"

#include <iostream>
//#include <vector>
//#include <set>

using std::cout;
using std::endl;

Int_t PndEmcMakeCluster::fEventCounter=0;

PndEmcMakeCluster::PndEmcMakeCluster(Int_t verbose, Bool_t storeclusters) 
{
	fVerbose=verbose;
	fStoreClusters=storeclusters;
}

//--------------
// Destructor --
//--------------

PndEmcMakeCluster::~PndEmcMakeCluster()
{ 
}

// -----   Public method Init   -------------------------------
InitStatus PndEmcMakeCluster::Init() {
	
  	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcMakeCluster::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}
	
	// Get input array
	fDigiArray = (TClonesArray*) ioman->GetObject("EmcDigi");
	if ( ! fDigiArray ) {
		cout << "-W- PndEmcMakeCluster::Init: "
		<< "No PndEmcDigi array!" << endl;
		return kERROR;
	}
	
	// Get input array
	fHitArray = (TClonesArray*) ioman->GetObject("EmcHit");
	if ( ! fHitArray ) {
		cout << "-W- PndEmcMakeCluster::Init: "
		<< "No PndEmcHit array! Needed for MC Truth" << endl;
//		return kERROR;
	}

	// Get input array
	fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( ! fMCTrackArray ) {
		cout << "-W- PndEmcMakeCluster::Init: "
		<< "No MCTrack array! Needed for MC Truth" << endl;
//		return kERROR;
	}

	// Create and register output array
	fClusterArray = new TClonesArray("PndEmcCluster");
	
	ioman->Register("EmcCluster","Emc",fClusterArray,fStoreClusters);
	
	fMapVersion=fDigiPar->GetMapperVersion();  
	PndEmcMapper::Instance(fMapVersion);
	PndEmcStructure::Instance();

	fDigiEnergyTresholdBarrel=fRecoPar->GetEnergyThresholdBarrel();
	fDigiEnergyTresholdFWD=fRecoPar->GetEnergyThresholdFWD();
	fDigiEnergyTresholdBWD=fRecoPar->GetEnergyThresholdBWD();
	fDigiEnergyTresholdShashlyk=fRecoPar->GetEnergyThresholdShashlyk();
	
	cout<<"PndEmcMakeCluster::fDigiEnergyTresholdBarrel: "<<fDigiEnergyTresholdBarrel<<endl;
	cout<<"PndEmcMakeCluster::fDigiEnergyTresholdFWD: "<<fDigiEnergyTresholdFWD<<endl;
	cout<<"PndEmcMakeCluster::fDigiEnergyTresholdBWD: "<<fDigiEnergyTresholdBWD<<endl;
	cout<<"PndEmcMakeCluster::fDigiEnergyTresholdShashlyk: "<<fDigiEnergyTresholdShashlyk<<endl;

	PndEmcClusterLiloPosData ClusterPositionParameters;
	
	if (!strcmp(fRecoPar->GetEmcClusterPosMethod(),"lilo"))
	{
		cout<<"Lilo cluster position method"<<endl;
		ClusterPositionParameters.OffsetParmA=fRecoPar->GetOffsetParmA();
		ClusterPositionParameters.OffsetParmB=fRecoPar->GetOffsetParmB();
		ClusterPositionParameters.OffsetParmC=fRecoPar->GetOffsetParmC();
		PndEmcCluster::selectCentroidMethod(PndEmcCluster::lilo,ClusterPositionParameters);
	}
	else if (!strcmp(fRecoPar->GetEmcClusterPosMethod(),"linear"))
	{
		cout<<"Linear cluster position method"<<endl;
		PndEmcCluster::selectCentroidMethod(PndEmcCluster::linear,ClusterPositionParameters);
	}
	else
	{
		cout<<"Wrong cluster position method"<<endl;
		abort();
	}
	
	cout << "-I- PndEmcMakeCluster: Intialization successfull" << endl;

	return kSUCCESS;
}


void PndEmcMakeCluster::Exec(Option_t* opt) 
{
	TStopwatch timer;
	if (fVerbose>0){
		timer.Start();
	}
	// Reset output array
	if ( ! fClusterArray ) Fatal("Exec", "No Cluster Array");
	fClusterArray->Delete();

	Int_t nDigis = fDigiArray->GetEntriesFast();
	if (fVerbose>0){
		cout<<"DigiList length "<<nDigis<<endl;
	}
	
	Double_t totalDigiEnergy=0;
	//loop to build Cluster
	for (Int_t iDigi=0; iDigi<nDigis; iDigi++)
	{
		PndEmcDigi* theDigi = (PndEmcDigi*) fDigiArray->At(iDigi);
		theDigi->ValidateTCI(); // TCI in digi is not valide after reading from file
		
		Int_t module=theDigi->GetModule();
		
		// In the following lines there is separate threshold for forward endcup
		// and the same for barrel and backward and shashlyk EMC, but for last 2 threshold probably should be different 
		if ((module==1||module==2)&&(theDigi->GetEnergy()< fDigiEnergyTresholdBarrel)) continue;
		if ((module==3)&&(theDigi->GetEnergy()< fDigiEnergyTresholdFWD)) continue;
		if ((module==4)&&(theDigi->GetEnergy()< fDigiEnergyTresholdBWD)) continue;
		if ((module==5)&&(theDigi->GetEnergy()< fDigiEnergyTresholdShashlyk)) continue;
		
			bool isAdded = false;
		
			Int_t clustmarker=0;
			
			Int_t clustLength=fClusterArray->GetEntriesFast();
			
			for(Int_t i=0;i<clustLength;i++)
			{
				PndEmcCluster* cluster=(PndEmcCluster*) fClusterArray->At(i);
				if(cluster->isInCluster(theDigi))
				{
					if(!isAdded)
					{
						clustmarker=i;
						isAdded=true;
						cluster->addDigi(theDigi);
					}
					else
					{
						PndEmcCluster* clust_clustmarker=(PndEmcCluster*) fClusterArray->At(clustmarker);
						PndEmcCluster* clust_i=(PndEmcCluster*) fClusterArray->At(i);
						clust_clustmarker->addCluster(clust_i);
						fClusterArray->RemoveAt(i);
						fClusterArray->Compress();
						clustLength--;
						i--;
					}
				}
			}
	
			if (!isAdded)
			{
				PndEmcCluster* newcluster = new((*fClusterArray)[clustLength]) PndEmcCluster();
				newcluster->addDigi(theDigi);
			}
			
			totalDigiEnergy+=theDigi->GetEnergy();
			
	}
	
	// At that moment internal state fEnergy and fWhere of Clusters are not initialized, the following make it possible to see energy and position from output root file
	Int_t nCluster = fClusterArray->GetEntriesFast();
	for (Int_t i=0; i<nCluster; i++)
	{
		PndEmcCluster *tmpclust = (PndEmcCluster*) fClusterArray->At(i);
		tmpclust->energy();
		tmpclust->where();

		tmpclust->fMcList.clear();
		if(fHitArray && fMCTrackArray){
			// BS: this is a first order approximation only !!!!

			std::vector <Int_t> newlist;
			newlist.clear();
			for(Int_t j=0; j<tmpclust->fDigiList.size(); j++){
				PndEmcDigi*m;
				m=(PndEmcDigi*)(tmpclust->fDigiList[j]);

				Int_t inx;
				inx=m->GetHitIndex();

				if(inx>=0){
					Double_t e_max=-1.;
					Int_t inx_max=-1;
					std::vector <Int_t> &tmplist=((PndEmcHit*) fHitArray->At(inx))->GetMcList();

					// Two choices, either copy complete list or only the highest energy particle
					for(Int_t k=0; k<tmplist.size(); k++){
//						newlist.push_back(tmplist[k]);
						Int_t mc_inx;
						Double_t e;
						mc_inx=tmplist[k];// MC Track Id
						e=((PndMCTrack*)fMCTrackArray->At(mc_inx))->Get4Momentum().E();
						if(e>e_max){
							e_max=e;
							inx_max=mc_inx;
						}
					}
					if(inx_max>-1 && e_max>1e-2){// 10MeV Cut on most energetic particle entering cluster
						newlist.push_back(inx_max);
					}
				}
			}
			// Sort list...
			std::sort( newlist.begin(), newlist.end());
			// and copy every id only once (even so it might be in the list several times)
			std::unique_copy( newlist.begin(), newlist.end(), std::back_inserter( tmpclust->fMcList ) );

			// Now check if mother or (grand)^x-mother are already in the list
			// (which means i am a secondary)... if so, remove myself
			for(Int_t j=tmpclust->fMcList.size()-1; j>=0; j--){
				bool flag;
				PndMCTrack *pt;
				pt=((PndMCTrack*)fMCTrackArray->At(tmpclust->fMcList[j]));
				if(pt->GetMotherID()<0) continue;
				flag=false;
				while(!flag){
					Int_t id;
					id=pt->GetMotherID();
					if(id<0) break;
					pt=(PndMCTrack*)fMCTrackArray->At(id);

					for(Int_t k=j-1; k>=0; k--){
						if(tmpclust->fMcList[k]==id){
							tmpclust->fMcList.erase(tmpclust->fMcList.begin()+j);
							flag=true;
							break;
						}
					}
				}
			}
		}
	}
	
	fEventCounter++;
	cout<<"PndEmcMakeCluster, event: "<<fEventCounter<<endl;
	
	if (fVerbose>0){
		timer.Stop();
		Double_t rtime = timer.RealTime();
		Double_t ctime = timer.CpuTime();
		cout << "PndEmcMakeCluster, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}	
}

void PndEmcMakeCluster::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar*) db->getContainer("PndEmcDigiPar");
 
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");
}

void PndEmcMakeCluster::SetStorageOfData(Bool_t val)
{
  fStoreClusters=val;
  return;
}
  

ClassImp(PndEmcMakeCluster)
