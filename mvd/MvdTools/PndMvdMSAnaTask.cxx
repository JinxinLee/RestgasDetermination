// -------------------------------------------------------------------------
// -----                PndMvdMSAnaTask source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TVector3.h"


// framework includes
#include "CbmRootManager.h"
#include "PndMvdMSAnaTask.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmHit.h"
#include "CbmMCTrack.h"
// PndMvd includes
#include "PndMvdMCPoint.h"
#include "CbmTrackParH.h"
#include "CbmTrackParP.h"

#include "TDatabasePDG.h"

#include <vector>
#include <map>


// -----   Default constructor   -------------------------------------------
PndMvdMSAnaTask::PndMvdMSAnaTask() : CbmTask("Geane Task for PANDA PndMvd"), fEventNr(0)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMvdMSAnaTask::~PndMvdMSAnaTask()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdMSAnaTask::Init()
{
  // Get RootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( !ioman){
		std::cout << "-E- PndMvdMSAnaTask::Init: "<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	// Get input arrays
  	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  	if (!fMCTracks){
  		std::cout << "-W- PndMvdMSAnaTask::Init: "<< "No MCTrack" << " array!" << std::endl;
  		return kERROR;
  	}
  
	fMCHits = (TClonesArray*) ioman->GetObject("MVDPoint");
	if ( !fMCHits)	{
		std::cout << "-W- PndMvdMSAnaTask::Init: "<< "No MVDPoint"<<" array!" << std::endl;
		return kERROR;
	}
	
	  fTrackParGeane = new TClonesArray("CbmTrackParP");
	  ioman->Register("GeaneTrackPar","Geane", fTrackParGeane, kTRUE);
	  
	  fTrackParIni = new TClonesArray("CbmTrackParP");
	  ioman->Register("GeaneTrackIni","Geane", fTrackParIni, kTRUE);
	  
	  fTrackParFinal = new TClonesArray("CbmTrackParP");
	  ioman->Register("GeaneTrackFinal","Geane", fTrackParFinal, kTRUE);
	  
	  fDetName = new TClonesArray("TObjString");
	  ioman->Register("DetName", "Geane", fDetName, kTRUE);
	  
	  fPro = new CbmGeanePro();
	  fGeoH = new PndMvdGeoHandling(gGeoManager);

  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndMvdMSAnaTask::SetParContainers()
{
  // Get Base Container
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndMvdMSAnaTask::Exec(Option_t* opt)
{																	
	std::map<int, std::vector<int> > mcHitMap;						//Track ->  MCHits
	fTrackParGeane->Delete();
	fTrackParIni->Delete();
	fTrackParFinal->Delete();
	fDetName->Delete();
	
	mcHitMap = AssignHitsToTracks();
	
	std::cout << "------------Event " << fEventNr << "-------------" << std::endl;
	fEventNr++;
			
	for (std::map<int, std::vector<int> >::const_iterator it = mcHitMap.begin(); it!= mcHitMap.end(); it++){  //go through all tracks
		CbmMCTrack* myTrack = (CbmMCTrack*)(fMCTracks->At(it->first));
		std::vector<int> MChits = it->second;
		
		if (MChits.size() > 1 && myTrack->GetMotherID() < 0){
			PndMvdMCPoint* startPoint = (PndMvdMCPoint*)(fMCHits->At(0));
			std::cout << "StartPoint: " << *startPoint << std::endl;
			TVector3 StartPos(startPoint->GetPosition());
			TVector3 StartPosErr(0.0, 0.0, 0.0);
			TVector3 StartMom(startPoint->GetPx(), startPoint->GetPy(), startPoint->GetPz());
			TVector3 StartMomErr(0.0, 0.0, 0.0);
			TVector3 startO, startU, startV;
			
			fGeoH->GetOUVId(startPoint->GetDetName(), startO, startU, startV);
			
			fPro->PropagateFromPlane(startU, startV);
			
			Int_t PDGCode = myTrack->GetPdgCode();
			TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
			TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
			Double_t  fCharge= fParticle->Charge();
			
			TClonesArray& clref1 = *fTrackParIni;
			Int_t size1 = clref1.GetEntriesFast();
			CbmTrackParP *fStart= new (clref1[size1]) CbmTrackParP(StartPos, StartMom, StartPosErr, StartMomErr, fCharge, startO, startU, startV);
			
			for (int p = 1; p < MChits.size(); p++){											//go through all hits in track
				PndMvdMCPoint* myPoint = (PndMvdMCPoint*)(fMCHits->At(MChits[p]));
				std::cout << "PropagationPoint: " << *myPoint << std::endl;
				
				TVector3 StopPos(myPoint->GetPosition());
				TVector3 StopPosErr(0.0, 0.0, 0.0);
				TVector3 StopMom(myPoint->GetPx(), myPoint->GetPy(), myPoint->GetPz());
				TVector3 StopMomErr(0.0, 0.0, 0.0);
				
				TVector3 o, u, v;
				fGeoH->GetOUVId(myPoint->GetDetName(), o, u, v);
				
				TClonesArray& clref2 = *fTrackParFinal;
				Int_t size2 = clref2.GetEntriesFast();
				CbmTrackParP *fStop= new (clref2[size2]) CbmTrackParP(StopPos, StopMom, StopPosErr, StopMomErr, fCharge, o, u, v);
				
				TClonesArray& clref = *fTrackParGeane;
				Int_t size = clref.GetEntriesFast();
				CbmTrackParP *fRes=	new(clref[size]) CbmTrackParP();
				
				//std::cout << "DetName: " << fGeoH->GetPath(myPoint->GetDetName()) << std::endl;
				TClonesArray& cDetRef = *fDetName;
				Int_t size3 = cDetRef.GetEntriesFast();
				new (cDetRef[size3]) TObjString(fGeoH->GetPath(myPoint->GetDetName()));
				
				std::cout << "Propagation Plane:" << std::endl;
				std::cout << "o: " << o[0] << " " << o[1] << " " << o[2] << std::endl;
				std::cout << "u: " << u[0] << " " << u[1] << " " << u[2] << std::endl;
				std::cout << "v: " << v[0] << " " << v[1] << " " << v[2] << std::endl;
				
				TVector3 endPoint(myPoint->GetPosition());
				//fPro->SetPoint(endPoint);
				//fPro->PropagateToPCA(1);
				fPro->PropagateToPlane(o, u, v);
				fPro->Propagate(fStart, fRes, PDGCode);
				
				std::cout << std::endl;
				std::cout << "Propagation Points: " << std::endl;
				std::cout << fRes->GetX() << " +/- " << fRes->GetDX() << std::endl;
				std::cout << fRes->GetY() << " +/- " << fRes->GetDY() << std::endl;
				std::cout << fRes->GetZ() << " +/- " << fRes->GetDZ() << std::endl;
				std::cout << std::endl;
				
				TVector3 global(fRes->GetX(), fRes->GetY(), fRes->GetZ());
				TVector3 local = fGeoH->MasterToLocalId(global, myPoint->GetDetName());
				
				std::cout << "Propagation Point local: " << std::endl;
				std::cout << local[0] << " " << local[1] << " " << local[2] << std::endl;
				std::cout << std::endl;
			}
		}
	}
	fMCTracks->Delete();
	fMCHits->Delete();
}

void PndMvdMSAnaTask::Finish()
{
}

std::map<int, std::vector<int> > PndMvdMSAnaTask::AssignHitsToTracks()
{
	std::map<int, std::vector<int> > result;
	for (int i = 0; i < fMCHits->GetEntriesFast(); i++){											//get all MC Hits
		PndMvdMCPoint* myPoint = (PndMvdMCPoint*)(fMCHits->At(i));									//sort MCHits with Tracks
		CbmMCTrack* myTrack = (CbmMCTrack*)(fMCTracks->At(myPoint->GetTrackID()));
		result[myPoint->GetTrackID()].push_back(i);
	
	}
	return result;
}

ClassImp(PndMvdMSAnaTask);
