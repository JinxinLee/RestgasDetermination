//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRecoDEdxTask
//      see PndTpcRecoDEdxTask.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Viola Michael    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------
#include "PndTpcPoint.h"
#include "PndTpcCluster.h"
#include "Track.h"
#include "PndTpcPlanarRecoHit.h"
#include "CbmMCTrack.h"
#include "CbmRootManager.h"
#include "DetPlane.h"
#include "TrackCand.h"
#include "TrackFitStat.h"
#include "PndTpcDEDXFits.h"
#include "PndTpcDEDXStorageHelper.h"
#include "PndTpcClusterTrack.h"
//#include "dedxStoreage.h"

// This Class' Header ------------------
#include "PndTpcRecoDEdxTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <vector>
#include <functional>
#include <assert.h>
#include <algorithm>

// Collaborating Class Headers --------
#include "TMath.h"
#include "TH2F.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TStopwatch.h"

//Other Class Headers
#include "PndTpcRawDEdxCollection.h"
#include "PndTpcDXCalculator.h"
#include "PndTpcDEDXDiagnostics.h"
#include "PndTpcDEDXDiagnosticPoint.h"

using namespace std;

struct TFSInfo
{
	TFSInfo(TClonesArray *trackArray, TClonesArray *trackFitStatArray):_trackArray(trackArray), _trackFitStatArray(trackFitStatArray), bTrackFitStatInfoAvailable(false), _pdgselect(false) {}
	void LoadTrackFitStatInfo();	
	bool CheckPdg(int TrackNr) const;
	int GetPdg(int TrackNr) const	{ return ( isPDGokay.size()>TrackNr ? isPDGokay[TrackNr] : 0 ); }
	int GetMotherID(int TrackNr) const	{ return ( MotherIds.size()>TrackNr ? MotherIds[TrackNr] : -2 ); }
	double GetMCP(int TrackNr) const	{ return ( mcps.size()>TrackNr ? mcps[TrackNr] : -100. ); }
private:
     TClonesArray *_trackArray;
	TClonesArray *_trackFitStatArray;
	bool bTrackFitStatInfoAvailable;
	bool _pdgselect;
	vector<int> isPDGokay;
	vector<int> MotherIds;
	vector<double> mcps; 	
};

void TFSInfo::LoadTrackFitStatInfo()
{
	//Get inforamtion out of TrackFitStat
	int nTrack=_trackArray->GetEntriesFast();
	if(_trackFitStatArray != 0 )	{
		int nStat=_trackFitStatArray->GetEntriesFast();
		cout << "PndTpcRecoDEdxTask::Exec: " << "_trackFitStatArray - Number of Tracks: " << nStat << endl;
		for(int i=0; i<nStat; i++)
		{
			TrackFitStat *pFitStat=(TrackFitStat*)_trackFitStatArray->At(i);
			isPDGokay.push_back(pFitStat->GetPdgCode());
			MotherIds.push_back(pFitStat->GetMotherID());
			mcps.push_back(pFitStat->GetMCP());
		}
		bTrackFitStatInfoAvailable = true;
	}
	else	 {
		cout << "PndTpcRecoDEdxTask::Exec: " << "TrackFitStat not available!" << endl;
		bTrackFitStatInfoAvailable = false;
	}
	if(!(isPDGokay.size()==nTrack))	{
		cout << "PndTpcRecoDEdxTask::Exec: " << "Error: Number of tracks doesn't match number of FitStats" << endl;
		if(_pdgselect)	{
			cout << "PndTpcRecoDEdxTask::Exec: " << "No PDG Selection will be performed!" << endl;
		}
		bTrackFitStatInfoAvailable = false;
	}
}

bool TFSInfo::CheckPdg(int TrackNr) const
{
	if(bTrackFitStatInfoAvailable)	{
		assert(isPDGokay.size()>TrackNr);
		if(!isPDGokay[TrackNr] && _pdgselect )	{
			return false;
		}
	}
	return true;
}

// Class Member definitions -----------
PndTpcRecoDEdxTask::PndTpcRecoDEdxTask()
  : CbmTask("dEdx"), _persistence(kFALSE), _spatialSorting(kFALSE), _diagnosticOutput(kFALSE),  _pdgselect(false), _pdgId(0), _mcIDselect(false), _mcID(0), _pmin(0.0001), _pmax(100.0), _thetamin(0), _thetamax(TMath::TwoPi()), _minTpcHits(0), _maxTpcHits(100000), _pid(NULL), nEvent(0)
{
	//these Branches store the inforamtion for calculating dedx
	_trackBranchName = "TrackPreFit";
	_clusterBranchName = "PndTpcCluster";
	
	//these contain additional information (MC)
	_mcBranchName = "MCTrack";
	_mcTpcBranchName = "PndTpcPoint";
	_trackFitStatBranchName="TrackFitStat";
	
	_HistoDEdx=NULL;
	_HistoSumDESumdx=NULL;
	_HistoTruncSumDESumdx=NULL;
	ResetOutputInfo();
}

PndTpcRecoDEdxTask::~PndTpcRecoDEdxTask()
{
}

InitStatus
PndTpcRecoDEdxTask::Init()
{
	//Get ROOT Manager
	CbmRootManager* ioman= CbmRootManager::Instance();
	if(ioman==0)
	{
		Error("TrackDEdxTask::Init","RootManager not instantiated!");
		return kERROR;
	}
		
	// Get input collection
	// Get the clusters
	_clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);
	if(_clusterArray==0){
		Error("TRackDEdxTask::Init","TpcCluster array not found");
		return kERROR;
	}
	
	// Get input collection
	// Get the reconstructed track
	_trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);	
	if(_trackArray==0)	{
		Error("PndTpcRecoDEdxTask::Init","Track-array not found!");
		return kERROR;
	}	
	
	//Get test data
	//Get the MonteCarlo Track
	 _mcTrackArray=(TClonesArray*) ioman->GetObject(_mcBranchName);
	if(_mcTrackArray==0)   {
     	Error("PndTpcRecoDEdxTask::Init","MC-Truth not found!");
      	return kERROR;
    	}
	
	//Get test data
	//Get the MC-TpcHits
	_mcTpcHitArray=(TClonesArray*) ioman->GetObject(_mcTpcBranchName);
  	if(_mcTpcHitArray==0)	{
     	Error("PndTpcRecoDEdxTask::Init","Tpc MC-Hits not found!");
      	return kERROR;
    	}

	//Get test data
	_trackFitStatArray=(TClonesArray*) ioman->GetObject(_trackFitStatBranchName);
  	if(_trackFitStatArray==0)	{
      	Error("PndTpcRecoDEdxTask::Init","TrackFitStat not found!");
      	return kERROR;
    }
	
 	 // create and register output array
	 // the calculated dedx values will be stored here
	_dEdxArray = new TClonesArray("PndTpcRawDEdxCollection");
	if(_dEdxArray==0)	{
		Error("PndTpcRecoDEdxTask::Init", "dEdxArray==0");
		return kERROR;
	}
	ioman->Register("PndTpcRawDEdxReco", "PndTpc", _dEdxArray, _persistence);
	
	if(_diagnosticOutput)	{
		_diaArray = new TClonesArray("PndTpcDEDXDiagnostics");
		if(_diaArray==0)	{
			Error("PndTpcRecoDEdxTask::Init", "diaArray==0");
			return kERROR;
		}
		ioman->Register("PndTpcDEDXDiag", "PndTpc", _diaArray, _persistence);		
	}	
	
	//Load PID Info
	_pid = new PndTpcDEDXFits;
	if(_pid==0)	{
      	Error("PndTpcRecoDEdxTask::Init","PndTpcDEDXFits could not be created");
      	return kERROR;
     }	
	_pid->LoadFile(BBFitFile.Data());
	_pid->LoadResolution(ResoFile.Data());
	//_____________________________________
	//Some output histograms
	/*
	_HistoDEdx=new TH2F("HistoDEdx","dE/dx",600,0,3, 10000, 0, 20000 );
	_HistoSumDESumdx=new TH2F("HistoSumDESumdx","sum_dE/sum_dx",300,0,3, 10000, 0, 20000);
	_HistoTruncSumDESumdx=new TH2F("HistoTruncSumDESumdx","Trunc(sum_dE/sum_dx)",300,0,3, 10000, 0, 20000);
	*/

	return kSUCCESS;
}

void PndTpcRecoDEdxTask::ValidateArrays() const
{
	if(_clusterArray==0)	{
		Fatal("PndTpcRecoDEdxTask::Exec","No Cluster Array");
	}
	if(_trackArray==0)	{
		Fatal("PndTpcRecoDEdxTask::Exec","No Track Array");
	}
	if(_mcTrackArray==0)	{
		Fatal("PndTpcRecoDEdxTask::Exec","No MC-Track Array");
	}
	if(_dEdxArray==0) {
		Fatal("PndTpcRecoDEdxTask::Exec","No DEdx Output Array");
	}
	if(_diagnosticOutput)	{
		if(_diaArray==0) {
			Fatal("PndTpcRecoDEdxTask::Exec","No dia Output Array");
		}		
	}
	/*
	if(_dEdxAlt==0)	{
		Fatal("PndTpcRecoDEdxTask::Exec","No Alternative DEdx Output Array");
	}
	*/
}

double CalculateLengthAlongTrack(Track *track, int nHit1, int nHit2)
{
	AbsRecoHit* previousHit=track->getHit(nHit1);
	AbsRecoHit* thisHit=track->getHit(nHit2);
	AbsTrackRep* rep=track->getCardinalRep()->clone();
	rep->extrapolate(previousHit->getDetPlane(rep));
	double length=rep->extrapolate(thisHit->getDetPlane(rep));
	cout << "Hit1: " << nHit1 << " Hit2: " << nHit2 << " Length along Track: " << length << endl;
	delete rep;
	return length;
}


void PndTpcRecoDEdxTask::PrepareClusterList(vector<PndTpcCluster *> &PreparedClusterList, Track *track ) const
{
	unsigned int nh=track->getNumHits();
	TrackCand cand=track->getCand();		//get list of hits
	int nTpcCluster = _clusterArray->GetEntriesFast();
	for(unsigned int ih=0; ih<nh; ++ih)
	{ 
		unsigned int DetId=0;
		unsigned int HitId=0;
		cand.getHit(ih, DetId, HitId);	//get Detector, get Cluster Index for this hit
		if(HitId>nTpcCluster)	{		//check if everything is as expected
			Fatal("PndTpcRecoDEdxTask::Exec","Request for non existing Cluster!");
		}
		if(!CheckDetector(DetId))	{ continue;	}
		PndTpcCluster *cl=(PndTpcCluster*)_clusterArray->At(HitId);	//get Cluster
		cl->SetIndexInTrack(ih);	//needed for sorting and dx calculation
		if(!CheckClusterMCID(*cl))	{ continue;	}
		PreparedClusterList.push_back(cl);
	}
	if(_spatialSorting)	{
		cout << "PndTpcRecoDEdxTask::PrepareClusterList: try spatial sorting of clusters along track" << endl;
		sort(PreparedClusterList.begin(),PreparedClusterList.end(), PndTpcClusterTrack(track) );
	}
}


void
PndTpcRecoDEdxTask::Exec(Option_t* opt)
{					
	cout << "PndTpcRecoDEdxTask::Exec..." << endl;
	TStopwatch timer;
	timer.Start();
	ValidateArrays();
	
	// clear output
  	_dEdxArray->Delete();
	if(_diagnosticOutput)	{
		_diaArray->Delete();
	}
	ResetOutputInfo();
	
	//Get inforamtion out of TrackFitStat
	TFSInfo TFS(_trackArray, _trackFitStatArray);
	TFS.LoadTrackFitStatInfo(); 
		
	//loop over tracks
	int nTrack=_trackArray->GetEntriesFast();
	int nTpcCluster = _clusterArray->GetEntriesFast();
	cout << "PndTpcRecoDEdxTask::Exec: " << "Number of Tracks: " << nTrack << endl;
	PndTpcDEDXDiagnosticPoint point;
	++nEvent;
	cout << " -------- ******** nEvent: " << nEvent << endl;
	for(Int_t i=0; i<nTrack; i++)	
	{
		cout << "TrackNr.: " << i << endl; 
		Track* track=(Track*)_trackArray->At(i);
		
		PndTpcDEDXDiagnostics* dedxdia=NULL;
		if(_diagnosticOutput)	{
			Int_t size=_diaArray->GetEntriesFast();
			dedxdia=new ((*_diaArray)[size]) PndTpcDEDXDiagnostics();
			assert(dedxdia);
		}		
		
		Double_t TrackMometum = track->getMom().Mag();
		if(!CheckRecoMomentum(TrackMometum))	{	continue;}
		unsigned int nh=track->getNumHits();
		if(!CheckHits(nh))	{continue;}
		
		PDG=TFS.GetPdg(i);	
		MotherID=TFS.GetMotherID(i);
		MCP=TFS.GetMCP(i);
		TrackNR=i;
		P=TrackMometum;

		//loop over hits
		Double_t last_x=0;
		Double_t last_y=0;
		Double_t last_z=0;
		
		//vector<dedxStoreage> data;
		vector <PndTpcDEDXStorageHelper> data;
		
		vector<Double_t> vzs;
		
		PndTpcDXCalculator Calx(track);
		
		vector<PndTpcCluster *> SortedClusterList;
		PrepareClusterList(SortedClusterList, track);
		
		vector<PndTpcCluster *>::const_iterator cit;
		vector<PndTpcCluster *>::const_iterator end=SortedClusterList.end();
		unsigned int ih=0;
		unsigned int prevIndex=0;
		cout << "PndTpcRecoDEdxTask::Exec: " << "Starting real work... " << endl;
		for(cit=SortedClusterList.begin();cit!=end; ++cit)
		{
			PndTpcCluster *cl=*cit;
			assert(cl);
			Double_t amp=cl->amp();
			TVector3 pos=cl->pos();
			Double_t x=pos.x(); Double_t y=pos.y();Double_t z=pos.z();
			vzs.push_back(z);
			
			unsigned int currentIndex=cl->GetIndexInTrack();
			
			Calx.SetNextPoint(x,y,z,amp,currentIndex);
			double de;
			double dx;
			Calx.GetDEDX(de,dx);

			data.push_back( PndTpcDEDXStorageHelper(de,dx) );
			SetMCTrackNR(GetClusterMCID(*cl)); 
	
			prevIndex=cl->GetIndexInTrack();
			
			if(_diagnosticOutput)	{
				point.SetCoordinates(x,y,z,amp);
				point.SetHitNumber(ih);
				assert(dedxdia);
				dedxdia->SetPoint(point);
			}
			++ih;
		}//end over hits of track
		cout << "PndTpcRecoDEdxTask::Exec: " << "Ending real work... " << endl;
		
		bSorted=CheckOrder(vzs);
		if(!CheckHits(data.size()))	{continue;}	//hits in the track could have been skipped

		//double ret= CalculateLengthAlongTrack(track,0, nh-1);	
		
		cout << "PndTpcRecoDEdxTask::Exec: " << "storing work... " << endl;
		//Create output object, store inforamtion
		Int_t size=_dEdxArray->GetEntriesFast();
		PndTpcRawDEdxCollection* dedxinf=new ((*_dEdxArray)[size]) PndTpcRawDEdxCollection();
		FillData(data, dedxinf);
		ResetOutputInfo();
	}	//end over tracks
	
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << "PndTpcRecoDEdxTask::Exec ...has finished in RealTime=" << rtime << " seconds, CpuTime="<< ctime << " seconds" << endl;
	cout << endl;
	return;
}


void PndTpcRecoDEdxTask::FillData(const vector<PndTpcDEDXStorageHelper> &data, PndTpcRawDEdxCollection* Col) const
{
	if(Col)	{
		Col->SetTrackNr(TrackNR);
		Col->SetP(P);
		Col->SetMCP(MCP);
		Col->SetPDG(PDG);
		Col->SetMotherId(MotherID);
		Col->SetSorted(bSorted);
		Col->SetMCTrackId(MCTrackNR);
	//	Col->MakeTruncatedMeans();
		vector<PndTpcDEDXStorageHelper>::const_iterator cit;
		for(cit=data.begin(); cit!=data.end(); cit++)
		{
			//cout << "in: " << (*cit).de << " " << (*cit).dx << endl;
			Col->SetRawDEdx(cit->GetEnergyLoss(), cit->GetLength());
		}
		Col->Close();
		double measuredDEDX=Col->TruncateAndMean(0.,0.6);
		double probability=-1.;
		if(_pid->GetInfo("e+", P, measuredDEDX, probability))	{
			Col->SetP_e(probability);
		}	
		if(_pid->GetInfo("p+", P, measuredDEDX, probability))	{
			Col->SetP_p(probability);
		}
		if(_pid->GetInfo("Muon-", P, measuredDEDX, probability))	{
			Col->SetP_Mu(probability);
		}
		if(_pid->GetInfo("K+", P, measuredDEDX, probability))	{
			Col->SetP_K(probability);
		}
		if(_pid->GetInfo("Pi+", P, measuredDEDX, probability))	{
			Col->SetP_Pi(probability);
		}
	}
	else	{
		cout << "PndTpcRecoDEdxTask::Exec: DEDXCollection does not exist!" << endl;
	}	

}

void PndTpcRecoDEdxTask::ResetOutputInfo()
{
	P=-1.;
	MCP=-1.;
	PDG=0;
	TrackNR=-100;
	MCTrackNR.clear();
	MotherID=-100;
	bSorted=false;
}

void PndTpcRecoDEdxTask::SetMCTrackNR(int nMCTrackNR)
{
	if(find(MCTrackNR.begin(),MCTrackNR.end(),nMCTrackNR)==MCTrackNR.end())	{
		MCTrackNR.push_back(nMCTrackNR);
	}
}

bool PndTpcRecoDEdxTask::CheckClusterMCID(const PndTpcCluster &cl) const
{
	if(!_mcIDselect)	{
		return true;
	}
	Int_t id=GetClusterMCID(cl);
	if( id==_mcID )	{
		return true;
	}
	else	{
		cout << "PndTpcRecoDEdxTask::Exec: Skipping hit" << endl;
		cout <<  "...Reason: MCID " << id << " does not match " << _mcID << "!" << endl;
		return false;
	}
}

Int_t PndTpcRecoDEdxTask::GetClusterMCID(const PndTpcCluster &cl) const
{
	Int_t MCTrackID=(cl.mcId()).DominantID().mctrackID();	
	double mrw=(cl.mcId()).MaxRelWeight();
	if(mrw>0.99)	{
		return MCTrackID;
	}
	else	{
		return -100;
	}
}

bool PndTpcRecoDEdxTask::CheckRecoMomentum(Double_t P) const
{
	cout << "P: - " << P << endl;
	if( P >  _pmax || P < _pmin )	{
		cout <<  "PndTpcRecoDEdxTask::Exec: " <<  "Skipping Track" << "!" << endl;
		cout <<  "...Reason: Momentum P: " << P << " out of Range" << endl;
		return false;
	}	
	return true;
}

bool PndTpcRecoDEdxTask::CheckHits(unsigned int nHits) const
{
	if(nHits < _minTpcHits || nHits > _maxTpcHits)	{	
		cout <<  "PndTpcRecoDEdxTask::Exec: " <<  "Skipping Track " << "!" << endl;
		cout <<  "...Reason: Number of TpcHits: " << nHits << " out of Range" << endl;			
		return false;
	} 
	return true;
}

bool PndTpcRecoDEdxTask::CheckDetector(unsigned int DetId) const
{
	if(DetId!=2)	{	//is Hit in TPC?
		cout <<  "PndTpcRecoDEdxTask::Exec: " << "Skipping Hit "
			<<  "... Reason: Not in TPC" << endl;
		return false;
	}
	return true;
}

bool PndTpcRecoDEdxTask::CheckOrder(const std::vector<Double_t> &check) const
{
	vector<Double_t> cvzs(check);
	std::sort(cvzs.begin(), cvzs.end());
	
	/*
	vector<Double_t>::const_iterator cit;
	for(cit=cvzs.begin();cit!=cvzs.end();cit++)
	{
		cout << *cit << " - ";
	}
	cout << endl;
	for(cit=check.begin();cit!=check.end();cit++)
	{
		cout << *cit << " - ";
	}	
	*/
	
	cout << endl;
	if( cvzs != check )	{
		cout << "PndTpcRecoDEdxTask::Exec: not sorted in z" << endl;
		return false;
	}
	return true;
}

void 
PndTpcRecoDEdxTask::WriteHistograms(const TString& filename){

     TFile* file = new TFile(filename, "RECREATE" );
	if(!file)	{
		Warning("PndTpcRecoDEdxTask::WriteHistograms","An Error occured - no histograms saved");
		return;
	}
     file->mkdir("dEdx");
     file->cd("dEdx");
     
	if(_HistoDEdx)	{
		_HistoDEdx->Write();
		delete _HistoDEdx;
		_HistoDEdx=NULL;
	}
	
	if(_HistoSumDESumdx)	{
		_HistoSumDESumdx->Write();
		delete _HistoSumDESumdx;
		_HistoSumDESumdx=NULL;
	}
	if(_HistoTruncSumDESumdx)	{
		_HistoTruncSumDESumdx->Write();
		delete _HistoTruncSumDESumdx;
		_HistoTruncSumDESumdx=NULL;
     }
	
     file->Close();
     delete file;
	file=NULL;
}



ClassImp(PndTpcRecoDEdxTask)
