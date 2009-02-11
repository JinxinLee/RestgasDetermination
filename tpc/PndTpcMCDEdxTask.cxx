//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcMCDEdxTask
//      see PndTpcMCDEdxTask.h for details
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
#include "CbmMCTrack.h"
#include "PndTpcRawDEdxCollection.h"
#include "FairRootManager.h"
#include "PndTpcHitMerger.h"
#include "PndTpcDEDXStorageHelper.h"

// This Class' Header ------------------
#include "PndTpcMCDEdxTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include "assert.h"

// Collaborating Class Headers --------
#include "TMath.h"
#include "TH2F.h"
#include "TClonesArray.h"


using namespace std;


// Class Member definitions -----------

PndTpcMCDEdxTask::PndTpcMCDEdxTask()
  : FairTask("dEdx"), _persistence(kFALSE), _combineHitsLength(false),_combineHitsNumber(false), _catchRemaining(kFALSE),_combineLength(0.5),_combineNumber(10),_startHit(0), _pdgselect(false), _pdgId(0), _pmin(0.00001), _pmax(100.0), _thetamin(0), 
    _thetamax(TMath::TwoPi()), _minTpcHits(0), _maxTpcHits(100000)
{
	//these Branches store the inforamtion for calculating dedx
	_mcTrackBranchName = "MCTrack";
	_mcPointBranchName = "PndTpcPoint";
	
	_HistoDEdx=NULL;
	_HistoSumDESumdx=NULL;
	_HistoTruncSumDESumdx=NULL;
}

PndTpcMCDEdxTask::~PndTpcMCDEdxTask()
{
}


InitStatus
PndTpcMCDEdxTask::Init()
{
	//Get ROOT Manager
	FairRootManager* ioman= FairRootManager::Instance();
	if(ioman==0)
	{
		Error("TrackDEdxTask::Init","RootManager not instantiated!");
		return kERROR;
	}
		
	// Get input collection
	//Get  MonteCarlo Track
	 _mcTrackArray=(TClonesArray*) ioman->GetObject(_mcTrackBranchName);
	if(_mcTrackArray==0)   {
     	Error("PndTpcMCDEdxTask::Init","MC-Truth not found!");
      	return kERROR;
    	}
	
	//Get the MC-TpcHits
	_mcTpcHitArray=(TClonesArray*) ioman->GetObject(_mcPointBranchName);
  	if(_mcTpcHitArray==0)	{
     	Error("PndTpcMCDEdxTask::Init","Tpc MC-Hits not found!");
      	return kERROR;
    	}

 	 // create and register output array
	 // the calculated dedx values will be stored here
	_dEdxArray = new TClonesArray("PndTpcRawDEdxCollection");
	if(_dEdxArray==0)	{
		Error("PndTpcMCDEdxTask::Init", "dEdxArray==0");
		return kERROR;
	}
	ioman->Register("PndTpcRawDEdx", "PndTpc", _dEdxArray, _persistence);
	 
	  
	//_____________________________________
	//Some output histograms
	//_HistoDEdx=new TH2F("HistoDEdx","dE/dx",600,0,3, 10000, 0, 20000 );
	/*
	_HistoSumDESumdx=new TH2F("HistoSumDESumdx","sum_dE/sum_dx",300,0,3, 10000, 0, 20000);
	_HistoTruncSumDESumdx=new TH2F("HistoTruncSumDESumdx","Trunc(sum_dE/sum_dx)",300,0,3, 10000, 0, 20000);
	*/

	return kSUCCESS;
}

void PndTpcMCDEdxTask::ValidateArrays() const
{
	if(_dEdxArray==0) { 
		Fatal("PndTpcMCDEdxTask::Exec","No DEdx Output Array");
	}
	if(_mcTpcHitArray==0)	{
		Fatal("PndTpcMCDEdxTask::Exec","No Cluster Array");
	}
	if(_mcTrackArray==0)	{
		Fatal("PndTpcMCDEdxTask::Exec","No Track Array");
	}
}

void
PndTpcMCDEdxTask::Exec(Option_t* opt)
{					
	cout << "PndTpcMCDEdxTask::Exec..." << endl;
	ValidateArrays();
	
	// clear output
  	_dEdxArray->Delete();

	// loop over mc-tracks in this event
	int nmctrks=_mcTrackArray->GetEntriesFast();
	cout << "PndTpcMCDEdxTask::Exec: " << "Number of Tracks: " << nmctrks << endl;
	vector<PndTpcPoint *> HitsInTrack;
	vector<PndTpcDEDXStorageHelper> DEDXPoints;
	for(int imc=0; imc<nmctrks;++imc) 
	{
		//cout << "TrackNr: " << imc << endl;
		CbmMCTrack *mc=(CbmMCTrack*)_mcTrackArray->At(imc);

		Int_t nPDG = mc->GetPdgCode();
		if(!CheckPDG(nPDG))					{ continue;}
		
		Double_t TrackMometum = mc->GetMomentum().Mag();
		if( !CheckMomentum(TrackMometum))		{ continue;}		

		GetHitsInTrack(imc, HitsInTrack);
		if( !CheckHits(HitsInTrack.size()))	{continue;}
		
		PndTpcHitMerger HitMerger;
		vector <PndTpcDEDXStorageHelper> DEDXInTrack;
		HitMerger.ConvertHits(HitsInTrack, DEDXInTrack);
		HitMerger.CatchRemaining(_catchRemaining);
		if(_combineHitsLength)	{
			DEDXPoints.clear();
			HitMerger.MaxDxMerging( _combineLength, DEDXInTrack,DEDXPoints );
			cout << "Merging Hits by Length active!" << endl;
		}
		else if(_combineHitsNumber)	{
			HitMerger.CombineHits( _combineNumber, DEDXInTrack,DEDXPoints );
			cout << "Merging Hits by Number active!" << endl;		
		}
		else	{
			DEDXPoints=DEDXInTrack;
			cout << "just assigned DEDXPoints." << endl;
		}
		
		cout << "Starting real work..." << endl;
		Int_t size=_dEdxArray->GetEntriesFast();
		//PndTpcRawDEdxCollection* dedxinf=new PndTpcRawDEdxCollection;
		std::sort(DEDXPoints.begin(), DEDXPoints.end());
	
		vector<PndTpcDEDXStorageHelper>::const_iterator cit;
		
		for(cit=DEDXPoints.begin(); cit!=DEDXPoints.end(); cit++)
		{
			cout << "DE: " << cit->GetEnergyLoss() << " dx: " << cit->GetLength();
			if(cit->GetLength())	{
				cout << " DE/dx: " << cit->GetEnergyLoss()/cit->GetLength(); 
			}		
			cout << endl;
		}
		
		if(DEDXPoints.size())	{
		PndTpcRawDEdxCollection* dedxinf=new ((*_dEdxArray)[size]) PndTpcRawDEdxCollection;
		if(dedxinf)	{
			dedxinf->SetTrackNr(imc);
			dedxinf->SetPDG(nPDG);
			dedxinf->SetP(TrackMometum);
			//cout << "try setting dedx... ";
			vector<PndTpcDEDXStorageHelper>::const_iterator cit;
			for(cit=DEDXPoints.begin(); cit!=DEDXPoints.end(); cit++)
			{
				PndTpcDEDXStorageHelper Point=*cit;
				dedxinf->SetRawDEdx(Point.GetEnergyLoss(), Point.GetLength() );
				//cout << "DE: " << pPoint->GetEnergyLoss() << "DX: " << pPoint->GetLength() << endl;
			}
			cout << "Closed" << endl;
		}
		else	{
			cout << "DedxInf does not exist!" << endl;
		}
		}
		DEDXPoints.clear();
		HitsInTrack.clear();
		//PndTpcRawDEdxCollection* dedxinf2=new ((*_dEdxArray)[size]) PndTpcRawDEdxCollection;
		//(*dedxinf2)=(*dedxinf);
		//delete dedxinf;
		cout << "...Done real work" << endl;
	} //end over tracks
	
	cout << "PndTpcMCDEdxTask::Exec ...has finished" << endl;
	return;
}

void PndTpcMCDEdxTask::GetHitsInTrack(int nTrackNr, std::vector<PndTpcPoint*> &Hits) const
{
	int ntpcMChits=_mcTpcHitArray->GetEntriesFast();
	int startHit=_startHit;
	for(int jmc=startHit; jmc<ntpcMChits; ++jmc)
	{
		PndTpcPoint *pPoint=(PndTpcPoint*)_mcTpcHitArray->At(jmc);
		assert(pPoint);
		Int_t nID=pPoint->GetTrackID();
		if( nID == nTrackNr )	{	
			Hits.push_back(pPoint);
		}	//end over hits of track
	} //end over hits
}

bool PndTpcMCDEdxTask::CheckPDG(Int_t nPDG) const
{
	if( !( _pdgId == nPDG || !_pdgselect ) )	{	//if pdgselect is deactivated, always false
		cout <<  "PndTpcMCDEdxTask::Exec: " << "Info: Track " << " will be skipped..." << endl;
		cout <<  "...Reason: PDG: " << nPDG << " not equal to " << _pdgId << endl;
		return false;	
	}
	return true;
}

bool PndTpcMCDEdxTask::CheckMomentum(Double_t P) const
{
	//cout << "P: - " << P << endl;	
	if( P >  _pmax || P < _pmin )	{
		cout <<  "PndTpcMCDEdxTask::Exec: " <<  "Skipping Track" << "!" << endl;
		cout <<  "...Reason: Momentum P: " << P << " out of Range" << endl;
		return false;
	}	
	return true;
}

bool PndTpcMCDEdxTask::CheckHits(unsigned int nHits) const
{
	//cout << "nHits: " << nHits << endl;
	if(nHits < _minTpcHits || nHits > _maxTpcHits)	{	
		//cout <<  "PndTpcMCDEdxTask::Exec: " <<  "Skipping Track" << "!" << endl;
		//cout <<  "...Reason: Number of TpcHits: " << nHits << " out of Range" << endl;			
		return false;
	}
	return true;
}

void 
PndTpcMCDEdxTask::WriteHistograms(const TString& filename){

     TFile* file = new TFile(filename, "RECREATE" );
	if(!file)	{
		Warning("PndTpcMCDEdxTask::WriteHistograms","An Error occured - no histograms saved");
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


ClassImp(PndTpcMCDEdxTask)
