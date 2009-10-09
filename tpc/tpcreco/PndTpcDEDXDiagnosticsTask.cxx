#include "PndTpcDEDXDiagnosticsTask.h"
#include "PndTpcDEDXDiagnostics.h"

#include <iostream>

#include "TVector3.h"
#include "TClonesArray.h"

#include "FairRootManager.h"

#include "PndTpcCluster.h"
#include "GFTrack.h"
#include "GFTrackCand.h"
#include "GFRecoHitFactory.h"
#include "PndTpcPlanarRecoHit.h"
#include "LSLTrackRep.h"

using namespace std;

PndTpcDEDXDiagnosticsTask::PndTpcDEDXDiagnosticsTask(): _persistence(true)	
{
	_trackBranchName = "TrackPreFit";
	_clusterBranchName = "PndTpcCluster";

}

InitStatus PndTpcDEDXDiagnosticsTask::Init()
{
	//Get ROOT Manager
	FairRootManager* ioman= FairRootManager::Instance();
	if(ioman==0)
	{
		Error("TrackDEdxTask::Init","RootManager not instantiated!");
		return kERROR;
	}

	_clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);
	if(_clusterArray==0){
		Error("TRackDEdxTask::Init","TpcCluster array not found");
		return kERROR;
	}
	
	_trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);	
	if(_trackArray==0)	{
		Error("PndTpcRecoDEdxTask::Init","Track-array not found!");
		return kERROR;
	}	
		
	// create and register output array
	_dEdxDiagnostics = new TClonesArray("PndTpcDEDXDiagnostics");
	if(_dEdxDiagnostics==0)	{
		Error("PndTpcDEDXDiagnosticsTask::Init", "_dEdxDiagnostics==0");
		return kERROR;
	}
	ioman->Register("PndTpcDEDXDiag", "PndTpc", _dEdxDiagnostics, _persistence);
	
	return kSUCCESS;
}

void PndTpcDEDXDiagnosticsTask::Exec(Option_t* opt)
{
	if(_clusterArray==0)	{
		Fatal("PndTpcDEDXDiagnosticsTask::Exec","No Cluster Array");
	}
	if(_trackArray==0)	{
		Fatal("PndTpcDEDXDiagnosticsTask::Exec","No Track Array");
	}
	if(_dEdxDiagnostics==0) {
		Fatal("PndTpcDEDXDiagnosticsTask::Exec","No Output Array");
	}
	
	// clear output
  	_dEdxDiagnostics->Delete();
	
	//loop over tracks
	int nTrack=_trackArray->GetEntriesFast();
	int nTpcCluster = _clusterArray->GetEntriesFast();

	PndTpcDEDXDiagnosticPoint point;
	cout << "Number of tracks: " << nTrack << endl;
	for(Int_t i=0; i<nTrack; i++)	
	{
		//Create output object, store inforamtion
		Int_t size=_dEdxDiagnostics->GetEntriesFast();
		PndTpcDEDXDiagnostics* dedxdia=new ((*_dEdxDiagnostics)[size]) PndTpcDEDXDiagnostics();
		GFTrack* track=(GFTrack*)_trackArray->At(i);
		GFTrackCand cand=track->getCand();		//get list of hits
		unsigned int nHits=cand.getNHits();
		cout << "Size of cand list: " << nHits << endl;
		for(unsigned int ih=0; ih<nHits; ++ih)
		{ 
			unsigned int DetId=0;
			unsigned int HitId=0;
			cand.getHit(ih, DetId, HitId);
			if(DetId != 2)	{ cout << "detId!=2" << endl; continue;}	
			PndTpcCluster *cl=(PndTpcCluster*)_clusterArray->At(HitId);	//get Cluster
			Double_t amp=cl->amp();
			TVector3 pos=cl->pos();
			Double_t x=pos.x();
			Double_t y=pos.y();
			Double_t z=pos.z();

			point.SetCoordinates(x,y,z,amp);
			point.SetHitNumber(ih);
			dedxdia->SetPoint(point);
		}	
		track->reset();
	}
}
