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
#include "PndMCTrack.h"
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
  : FairTask("dEdx"), fpersistence(kFALSE), fcombineHitsLength(false),fcombineHitsNumber(false), fcatchRemaining(kFALSE),fcombineLength(0.5),fcombineNumber(10),fstartHit(0), fpdgselect(false), fpdgId(0), fpmin(0.00001), fpmax(100.0), fthetamin(0), 
    fthetamax(TMath::TwoPi()), fminTpcHits(0), fmaxTpcHits(100000)
{
	//these Branches store the inforamtion for calculating dedx
	fmcTrackBranchName = "MCTrack";
	fmcPointBranchName = "PndTpcPoint";
	
	fHistoDEdx=NULL;
	fHistoSumDESumdx=NULL;
	fHistoTruncSumDESumdx=NULL;
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
	 fmcTrackArray=(TClonesArray*) ioman->GetObject(fmcTrackBranchName);
	if(fmcTrackArray==0)   {
     	Error("PndTpcMCDEdxTask::Init","MC-Truth not found!");
      	return kERROR;
    	}
	
	//Get the MC-TpcHits
	fmcTpcHitArray=(TClonesArray*) ioman->GetObject(fmcPointBranchName);
  	if(fmcTpcHitArray==0)	{
     	Error("PndTpcMCDEdxTask::Init","Tpc MC-Hits not found!");
      	return kERROR;
    	}

 	 // create and register output array
	 // the calculated dedx values will be stored here
	fdEdxArray = new TClonesArray("PndTpcRawDEdxCollection");
	if(fdEdxArray==0)	{
		Error("PndTpcMCDEdxTask::Init", "dEdxArray==0");
		return kERROR;
	}
	ioman->Register("PndTpcRawDEdx", "PndTpc", fdEdxArray, fpersistence);
	 
	  
	//f____________________________________
	//Some output histograms
	//fHistoDEdx=new TH2F("HistoDEdx","dE/dx",600,0,3, 10000, 0, 20000 );
	/*
	fHistoSumDESumdx=new TH2F("HistoSumDESumdx","sum_dE/sum_dx",300,0,3, 10000, 0, 20000);
	fHistoTruncSumDESumdx=new TH2F("HistoTruncSumDESumdx","Trunc(sum_dE/sum_dx)",300,0,3, 10000, 0, 20000);
	*/

	return kSUCCESS;
}

void PndTpcMCDEdxTask::ValidateArrays() const
{
	if(fdEdxArray==0) { 
		Fatal("PndTpcMCDEdxTask::Exec","No DEdx Output Array");
	}
	if(fmcTpcHitArray==0)	{
		Fatal("PndTpcMCDEdxTask::Exec","No Cluster Array");
	}
	if(fmcTrackArray==0)	{
		Fatal("PndTpcMCDEdxTask::Exec","No Track Array");
	}
}

void
PndTpcMCDEdxTask::Exec(Option_t* opt)
{					
	cout << "PndTpcMCDEdxTask::Exec..." << endl;
	ValidateArrays();
	
	// clear output
  	fdEdxArray->Delete();

	// loop over mc-tracks in this event
	int nmctrks=fmcTrackArray->GetEntriesFast();
	cout << "PndTpcMCDEdxTask::Exec: " << "Number of Tracks: " << nmctrks << endl;
	vector<PndTpcPoint *> HitsInTrack;
	vector<PndTpcDEDXStorageHelper> DEDXPoints;
	for(int imc=0; imc<nmctrks;++imc) 
	{
		//cout << "TrackNr: " << imc << endl;
		PndMCTrack *mc=(PndMCTrack*)fmcTrackArray->At(imc);

		Int_t nPDG = mc->GetPdgCode();
		if(!CheckPDG(nPDG))					{ continue;}
		
		Double_t TrackMometum = mc->GetMomentum().Mag();
		if( !CheckMomentum(TrackMometum))		{ continue;}		

		GetHitsInTrack(imc, HitsInTrack);
		if( !CheckHits(HitsInTrack.size()))	{continue;}
		
		PndTpcHitMerger HitMerger;
		vector <PndTpcDEDXStorageHelper> DEDXInTrack;
		HitMerger.ConvertHits(HitsInTrack, DEDXInTrack);
		HitMerger.CatchRemaining(fcatchRemaining);
		if(fcombineHitsLength)	{
			DEDXPoints.clear();
			HitMerger.MaxDxMerging( fcombineLength, DEDXInTrack,DEDXPoints );
			cout << "Merging Hits by Length active!" << endl;
		}
		else if(fcombineHitsNumber)	{
			HitMerger.CombineHits( fcombineNumber, DEDXInTrack,DEDXPoints );
			cout << "Merging Hits by Number active!" << endl;		
		}
		else	{
			DEDXPoints=DEDXInTrack;
			cout << "just assigned DEDXPoints." << endl;
		}
		
		cout << "Starting real work..." << endl;
		Int_t size=fdEdxArray->GetEntriesFast();
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
		PndTpcRawDEdxCollection* dedxinf=new ((*fdEdxArray)[size]) PndTpcRawDEdxCollection;
		if(dedxinf)	{
			dedxinf->SetTrackNr(imc);
			dedxinf->SetPDG(nPDG);
			dedxinf->SetP(TrackMometum);
			//cout << "try setting dedx... ";
			vector<PndTpcDEDXStorageHelper>::const_iterator cIt;
			for(cIt=DEDXPoints.begin(); cIt!=DEDXPoints.end(); cIt++)
			{
				PndTpcDEDXStorageHelper Point=*cIt;
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
		//PndTpcRawDEdxCollection* dedxinf2=new ((*fdEdxArray)[size]) PndTpcRawDEdxCollection;
		//(*dedxinf2)=(*dedxinf);
		//delete dedxinf;
		cout << "...Done real work" << endl;
	} //end over tracks
	
	cout << "PndTpcMCDEdxTask::Exec ...has finished" << endl;
	return;
}

void PndTpcMCDEdxTask::GetHitsInTrack(int nTrackNr, std::vector<PndTpcPoint*> &Hits) const
{
	int ntpcMChits=fmcTpcHitArray->GetEntriesFast();
	int startHit=fstartHit;
	for(int jmc=startHit; jmc<ntpcMChits; ++jmc)
	{
		PndTpcPoint *pPoint=(PndTpcPoint*)fmcTpcHitArray->At(jmc);
		assert(pPoint);
		Int_t nID=pPoint->GetTrackID();
		if( nID == nTrackNr )	{	
			Hits.push_back(pPoint);
		}	//end over hits of track
	} //end over hits
}

bool PndTpcMCDEdxTask::CheckPDG(Int_t nPDG) const
{
	if( !( fpdgId == nPDG || !fpdgselect ) )	{	//if pdgselect is deactivated, always false
		cout <<  "PndTpcMCDEdxTask::Exec: " << "Info: Track " << " will be skipped..." << endl;
		cout <<  "...Reason: PDG: " << nPDG << " not equal to " << fpdgId << endl;
		return false;	
	}
	return true;
}

bool PndTpcMCDEdxTask::CheckMomentum(Double_t P) const
{
	//cout << "P: - " << P << endl;	
	if( P >  fpmax || P < fpmin )	{
		cout <<  "PndTpcMCDEdxTask::Exec: " <<  "Skipping Track" << "!" << endl;
		cout <<  "...Reason: Momentum P: " << P << " out of Range" << endl;
		return false;
	}	
	return true;
}

bool PndTpcMCDEdxTask::CheckHits(unsigned int nHits) const
{
	//cout << "nHits: " << nHits << endl;
	if(nHits < fminTpcHits || nHits > fmaxTpcHits)	{	
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
     
	if(fHistoDEdx)	{
		fHistoDEdx->Write();
		delete fHistoDEdx;
		fHistoDEdx=NULL;
	}
	
	if(fHistoSumDESumdx)	{
		fHistoSumDESumdx->Write();
		delete fHistoSumDESumdx;
		fHistoSumDESumdx=NULL;
	}
	if(fHistoTruncSumDESumdx)	{
		fHistoTruncSumDESumdx->Write();
		delete fHistoTruncSumDESumdx;
		fHistoTruncSumDESumdx=NULL;
     }
	
     file->Close();
     delete file;
	file=NULL;
}


ClassImp(PndTpcMCDEdxTask)
