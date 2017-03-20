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
//	Philipp Mahlberg     //-------------added timebased reconstruction capabilities----------------

#include "PndEmcMakeCluster.h"

#include "PndEmcClusterProperties.h"
#include "PndEmcXClMoments.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "PndMCTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TROOT.h"


#include <iostream>
#include <exception>

using std::cout;
using std::endl;

Int_t PndEmcMakeCluster::fEventCounter=0;

PndEmcMakeCluster::PndEmcMakeCluster(Int_t verbose, Bool_t storeclusters):
FairTask("EmcClusteringTask", verbose),
fDigiArray(NULL), fHitArray(NULL), fMCTrackArray(NULL), fClusterArray(NULL), fWriteOutArray(NULL), fDigiFunctor(NULL), fClusterList(), fDigiEnergyTresholdBarrel(0), fDigiEnergyTresholdFWD(0), fDigiEnergyTresholdBWD(0), fDigiEnergyTresholdShashlyk(0), fClusterPosParam(), fMapVersion(0), fGeoPar(new PndEmcGeoPar()), fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()), fVerbose(verbose), fStoreClusters(storeclusters), fStoreClusterBase(kFALSE)
{
	fClusterList.clear();
	fClusterPosParam.clear();
}

//--------------
// Destructor --
//--------------
PndEmcMakeCluster::~PndEmcMakeCluster()
{ 
}

/**
 * @brief Init Task
 * 
 * Prepares the TClonesArray of PndEmcDigi for reading 
 * and of PndEmcCluster for writing. 
 * 
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcMakeCluster::Init() 
{

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
		fDigiArray = (TClonesArray*) ioman->GetObject("EmcDigiSorted");
		if ( ! fDigiArray ) {
			cout << "-W- PndEmcMakeCluster::Init: "
				<< "No PndEmcDigi array!" << endl;
			return kERROR;
		}
	}

	// Get input array
	fHitArray = (TClonesArray*) ioman->GetObject("EmcHit");
	if ( ! fHitArray ) {
		cout << "-W- PndEmcMakeCluster::Init: "
			<< "No PndEmcHit array! Needed for MC Truth" << endl;
	}

	// Get input array
	fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( ! fMCTrackArray ) {
		cout << "-W- PndEmcMakeCluster::Init: "
			<< "No MCTrack array! Needed for MC Truth" << endl;
	}

	// Create and register output array
	fClusterArray = new TClonesArray("PndEmcCluster");
	fWriteOutArray = new TClonesArray(fClusterArray->GetClass());

	ioman->Register("EmcCluster","Emc", fWriteOutArray, fStoreClusters);

	if(FairRunAna::Instance()->IsTimeStamp()) {	
		// for subsequent methods we need the "event grouping" as given by the TS buffer --> fDigiArray becomes an output array. 
		fDigiArray = new TClonesArray(fDigiArray->GetClass());		//still needed to activate array 
		ioman->Register("EmcDigiClusterBase", "Emc", fDigiArray, fStoreClusterBase);
	}
	fDigiFunctor = new TimeGap();

	fGeoPar->InitEmcMapper();  
	PndEmcStructure::Instance();

	fDigiEnergyTresholdBarrel=fRecoPar->GetEnergyThresholdBarrel();
	fDigiEnergyTresholdFWD=fRecoPar->GetEnergyThresholdFWD();
	fDigiEnergyTresholdBWD=fRecoPar->GetEnergyThresholdBWD();
	fDigiEnergyTresholdShashlyk=fRecoPar->GetEnergyThresholdShashlyk();
	fClusterActiveTime=fRecoPar->GetClusterActiveTime() * 1.0e9; //convert from seconds to nanoseconds

	cout<<"PndEmcMakeCluster::fDigiEnergyTresholdBarrel: "<<fDigiEnergyTresholdBarrel<<endl;
	cout<<"PndEmcMakeCluster::fDigiEnergyTresholdFWD: "<<fDigiEnergyTresholdFWD<<endl;
	cout<<"PndEmcMakeCluster::fDigiEnergyTresholdBWD: "<<fDigiEnergyTresholdBWD<<endl;
	cout<<"PndEmcMakeCluster::fDigiEnergyTresholdShashlyk: "<<fDigiEnergyTresholdShashlyk<<endl;
	cout<<"PndEmcMakeCluster::fClusterActiveTime in ns(!): "<<fClusterActiveTime<<endl;
	cout<<"PndEmcMakeCluster::ClusterPosMethod(): " << fRecoPar->GetEmcClusterPosMethod() << std::endl;

	if (!strcmp(fRecoPar->GetEmcClusterPosMethod(),"lilo"))
	{
		cout<<"Lilo cluster position method"<<endl;
		fClusterPosParam.push_back(fRecoPar->GetOffsetParmA());
		fClusterPosParam.push_back(fRecoPar->GetOffsetParmB());
		fClusterPosParam.push_back(fRecoPar->GetOffsetParmC());
	}	

	cout << "-I- PndEmcMakeCluster: Intialization successfull" << endl;
	return kSUCCESS;
}

/**
 * @brief Runs the task.
 * 
 * The task loops over the digis and adds neighboring digis (PndEmcTwoCoordIndex::IsNeighbour())
 * to clusters.
 * 
 * @param opt unused
 * @return void
 */
void PndEmcMakeCluster::Exec(Option_t*) 
{

	if (fVerbose>2){
		fTimer.Start();
	}
	// Reset output array
	if ( ! fClusterArray ) Fatal("Exec", "No Cluster Array");

	fWriteOutArray->Delete();

//	std::cout << "------ Event " << FairRootManager::Instance()->GetEntryNr() << " ------" << std::endl;

	if(FairRunAna::Instance()->IsTimeStamp()) {
		fDigiArray->Delete();
		fDigiArray->AbsorbObjects(FairRootManager::Instance()->GetData("EmcDigiSorted", fDigiFunctor, fClusterActiveTime));
	}	


	Int_t nDigis = fDigiArray->GetEntriesFast();
	if (fVerbose>2){
		cout<<"DigiList length "<<nDigis<<endl;
	}

	//loop to build Cluster
	for (Int_t iDigi=0; iDigi<nDigis; iDigi++)
	{
		PndEmcDigi* theDigi = (PndEmcDigi*) fDigiArray->At(iDigi);
//		std::cout << std::endl << "DigiArray: " << iDigi  << std::endl;
//		theDigi->Print();
//		std::cout << *theDigi->GetPointerToLinks() << std::endl;
//		std::cout << std::endl;
		Int_t module=theDigi->GetModule();

		// In the following lines there is separate threshold for forward endcap
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
			PndEmcCluster* cluster= (PndEmcCluster*) fClusterArray->At(i);
			if(HasExpired(theDigi, cluster, i)) {
				clustLength--;
				i--;
				continue;
			} else {
//					std::cout << "Clusterarray: " << i << std::endl;
			}

			if(cluster->isInCluster(theDigi, fDigiArray))
			{
				if(!isAdded) {
					clustmarker=i;
					isAdded=true;
					cluster->addDigi(fDigiArray, iDigi);
					//cluster->AddLink(FairLink("EmcDigi", iDigi));
					FairMultiLinkedData hitLinks = theDigi->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcHit"));
					for (Int_t j = 0; j < hitLinks.GetNLinks(); j++){
						PndEmcHit* hit = (PndEmcHit*)FairRootManager::Instance()->GetCloneOfLinkData(hitLinks.GetLink(j));
						//std::cout << "Hit: " << hit->GetDetectorID() << std::endl;
						if(hit) {
//							std::cout << "Cluster : " << clustmarker << " Hit to add: " << hitLinks.GetLink(j) << std::endl;
							if (cluster->GetLinks().count(hitLinks.GetLink(j)) == 0){
								cluster->AddTracksEnteringExiting(hit->GetTrackEntering(), hit->GetTrackExiting());
								cluster->AddLink(hitLinks.GetLink(j));
//								std::cout << "Links in Cluster: " << *cluster->GetPointerToLinks() << std::endl;
//								std::cout << "EnteringExiting for Hit: " << hitLinks.GetLink(j) << std::endl;
//								std::cout << "TrackEntering: " << hit->GetTrackEntering() << std::endl;
//								std::cout << "TrackExiting: " << hit->GetTrackExiting() << std::endl;
//								std::cout << "Resulting Enter: " << cluster->GetTrackEntering() << std::endl;
//								std::cout << "Resulting Exit: " << cluster->GetTrackExiting() << std::endl;
							} else {
								//std::cout << "Hit Already exists!" << std::endl;
							}
							delete(hit);

						} else {
							std::cout << "-E in PndEmcMakeCluster::Exec FairLink " << hitLinks.GetLink(j) << "to EmcHit delivers null" << std::endl;
						}
					}
				}
				else
				{
					PndEmcCluster* clust_clustmarker=(PndEmcCluster*) fClusterArray->At(clustmarker);
					PndEmcCluster* clust_i=(PndEmcCluster*) fClusterArray->At(i);
					clust_clustmarker->addCluster(clust_i, fDigiArray);
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
			newcluster->addDigi(fDigiArray, iDigi);

			FairMultiLinkedData hitLinks = theDigi->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcHit"));
			//std::cout << "HitLinks isNotAdded:  " << hitLinks << std::endl;
		//	theDigi->Print(); std::cout << std::endl;
			for (Int_t i = 0; i < hitLinks.GetNLinks(); i++){
				PndEmcHit* hit = (PndEmcHit*)FairRootManager::Instance()->GetCloneOfLinkData(hitLinks.GetLink(i));
				if(hit) {
//					std::cout << "NewCluster : " << clustLength << " Hit Added: " << hitLinks.GetLink(i) << std::endl;
//					std::cout << "TrackEntering: " << hit->GetTrackEntering() << std::endl;
//					std::cout << "TrackExiting: " << hit->GetTrackExiting() << std::endl;
					newcluster->SetTrackEntering(hit->GetTrackEntering());
					newcluster->SetTrackExiting(hit->GetTrackExiting());
					newcluster->SetInsertHistory(kFALSE);
					newcluster->AddLink(hitLinks.GetLink(i));
//					std::cout << "Resulting Enter: " << newcluster->GetTrackEntering() << std::endl;
//					std::cout << "Resulting Exit: " << newcluster->GetTrackExiting() << std::endl;
//					std::cout << "Links in Cluster: " << *newcluster->GetPointerToLinks() << std::endl;
					delete(hit);
				} else {
					std::cout << "-E in PndEmcMakeCluster::Exec FairLink " << hitLinks.GetLink(i) << "to EmcHit delivers null" << std::endl;
				}
			}
			//newcluster->SetLink(FairLink("EmcDigi", iDigi));
		}
	}
//	for (int k = 0; k < fClusterArray->GetEntriesFast(); k++){
//		PndEmcCluster* myCluster = (PndEmcCluster*)fClusterArray->At(k);
//		std::cout << k << " : Entering: " << myCluster->GetTrackEntering() << std::endl;
//		std::cout << k << " : Exiting: " << myCluster->GetTrackExiting() << std::endl;
//	}
//	std::cout << std::endl;

	fWriteOutArray->AbsorbObjects(fClusterArray);
}


/**
 * @brief Calculates properties of the cluster and sets them.
 * 
 * The calculated properties include the energy, position, timestamp and Zernike moments.
 * 
 * @param tmpclust The cluster to work on
 * @return void
 */
void PndEmcMakeCluster::FinishCluster(PndEmcCluster* tmpclust) 
{
	using namespace std;

	//cout << "\tfinalizing cluster: ";
	//cout << "\tmaxTime: " << tmpclust->GetTimeStamp() << endl;
	//cout << "\tcontains the following digis: " << endl;
	//for(unsigned int i=0; i<tmpclust->DigiList().size(); ++i) {
	//cout << "\t#:" << i << "\ttime: " << ((PndEmcDigi*) (fDigiArray->UncheckedAt(tmpclust->DigiList()[i])))->GetTimeStamp() << "\tenergy: " << ((PndEmcDigi*) (fDigiArray->UncheckedAt(tmpclust->DigiList()[i])))->GetEnergy() << "\tidx: " << ((PndEmcDigi*) (fDigiArray->UncheckedAt(tmpclust->DigiList()[i])))->GetDetectorId() << endl;
	//}

	PndEmcClusterProperties clustProperties(*tmpclust, fDigiArray);

	tmpclust->SetEnergy(clustProperties.Energy());
	tmpclust->SetTimeStamp(tmpclust->Maxima(fDigiArray)->GetTimeStamp());
	TVector3 tmpbumppos = clustProperties.Where(fRecoPar->GetEmcClusterPosMethod(), fClusterPosParam);
	tmpclust->SetPosition(tmpbumppos);
	PndEmcXClMoments xClMoments(*tmpclust, fDigiArray);
	tmpclust->SetZ20(xClMoments.AbsZernikeMoment(2, 0, 15));
	tmpclust->SetZ53(xClMoments.AbsZernikeMoment(5, 3, 15));
	tmpclust->SetLatMom(xClMoments.Lat());
	tmpclust->SetLinks(tmpclust->GetTrackEntering());
	//	std::cout << "ClusterOutput: " << std::endl;
	//	tmpclust->Print();
	//const std::vector<Int_t>& MCTruth = tmpclust->GetMcList();
	//std::cout<<"The cluster #"<<i<<" produced by MC Track #";
	//for(Int_t j=0;j<MCTruth.size();++j)
	//	cout<<MCTruth[j]<<' ';
	//cout<<endl;

	//tmpclust->fMcList.clear();
	//if(fHitArray && fMCTrackArray && !FairRunAna::Instance()->IsTimeStamp()){
	//	// BS: this is a first order approximation only !!!!

	//	std::vector <Int_t> newlist;
	//	newlist.clear();
	//	for(Int_t j=0; j<tmpclust->fDigiList.size(); j++){
	//		PndEmcDigi*m;
	//		m=(PndEmcDigi*)fDigiArray->At(tmpclust->fDigiList[j]);

	//		Int_t inx;
	//		inx=m->GetHitIndex();

	//		if(inx>=0){
		//			if(fHitArray->At(inx) != 0 ){//add by hujf
	//			const std::vector <Int_t> &tmplist=((PndEmcHit*) fHitArray->At(inx))->GetMcList();

	//			// I copy the complete list instead of only the highest energy particle
	//			// highest energy might be a problem for Hits which belong to two real clusters?
	//			for(Int_t k=0; k<tmplist.size(); k++){
	//				newlist.push_back(tmplist[k]);
	//			}
	//		}
	//	}
	//	// if( fMCTrackArray) checked above already
	//	cleansortmclist(newlist,fMCTrackArray);
	//	tmpclust->fMcList=newlist;
	//}
}


/**
 * @brief Calls FinishCluster() for each cluster
 * 
 * @return void
 */
void PndEmcMakeCluster::FinishClusters() 
{
	Int_t nCluster = fWriteOutArray->GetEntriesFast();
	for (Int_t i=0; i<nCluster; i++) {
		FinishCluster((PndEmcCluster*) (fWriteOutArray->At(i)));
	}

	fEventCounter++;
	if (fVerbose>0)
		cout<<"PndEmcMakeCluster, event: "<<fEventCounter<<endl;

	if (fVerbose>2){
		fTimer.Stop();
		Double_t rtime = fTimer.RealTime();
		Double_t ctime = fTimer.CpuTime();
		fTimer.Reset();
		cout << "PndEmcMakeCluster, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}	
}

/**
 * @brief Finishes clusters in timebased analysis
 * 
 * Checks if the timestamp of @p latestDigi is later than the active time of @p theCluster. If yes,
 * the cluster is written and true is returned.
 * 
 * @param latestDigi Current digi to check timestamp of.
 * @param theCluster Current cluster for which the expiration is checked.
 * @param clusterIdx Index of @p theCluster in the cluster TClonesArray.
 * @return bool
 * @retval true Cluster has expired and was written.
 * @retval false Cluster not expired.
 */
bool PndEmcMakeCluster::HasExpired(PndEmcDigi* latestDigi, PndEmcCluster* theCluster, Int_t clusterIdx) 
{
	if(!FairRunAna::Instance()->IsTimeStamp()) return false;

	if(latestDigi->GetTimeStamp() > theCluster->GetTimeStamp() + fClusterActiveTime) {
		fWriteOutArray->AbsorbObjects(fClusterArray, clusterIdx, clusterIdx);
		//cout << "cluster " << clusterIdx << "has been expired" << std::endl;
		return true;
	} else {
		return false;
	}	
}


/**
 * @brief Helper function, does not depend on class, identical to the one in PndEmcHitProducer
 * 
 * Currently not used.
 * 
 * @param newlist ...
 * @param mcTrackArray ...
 * @return void
 */
void PndEmcMakeCluster::cleansortmclist( std::vector <Int_t> &newlist,TClonesArray* mcTrackArray)
{
	std::vector <Int_t> tmplist;
	// Sort list...
	std::sort( newlist.begin(), newlist.end());
	// and copy every id only once (even though it might be in the list several times)
	std::unique_copy( newlist.begin(), newlist.end(), std::back_inserter( tmplist ) );

	// Now check if mother or (grand)^x-mother are already in the list
	// (which means i am a secondary)... if so, remove myself
	for(Int_t j=tmplist.size()-1; j>=0; j--){
		bool flag;
		PndMCTrack *pt;
		pt=((PndMCTrack*)mcTrackArray->At(tmplist[j]));
		if(pt->GetMotherID()<0) continue;
		flag=false;
		while(!flag){
			Int_t id;
			id=pt->GetMotherID();
			if(id<0) break;
			pt=(PndMCTrack*)mcTrackArray->At(id);

			for(Int_t k=j-1; k>=0; k--){
				if(tmplist[k]==id){
					tmplist.erase(tmplist.begin()+j);
					flag=true;
					break;
				}
			}
		}
	}
	newlist=tmplist;
}

void PndEmcMakeCluster::SetParContainers() {

	// Get run and runtime database
	FairRun* run = FairRun::Instance();
	if ( ! run ) Fatal("SetParContainers", "No analysis run");

	FairRuntimeDb* db = run->GetRuntimeDb();
	if ( ! db ) Fatal("SetParContainers", "No runtime database");

	// Get Emc geometry parameter container
	fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");

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
