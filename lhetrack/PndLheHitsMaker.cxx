#include "PndLheHitsMaker.h"

#include "PndTpcLheHit.h"
#include "PndTpcLheTrack.h"

#include "FairRunAna.h"
#include "PndTpcPoint.h"
#include "PndTpcCluster.h"
#include "PndSttPoint.h"
#include "PndSttHit.h"
#include "PndSttHelixHit.h"
#include "PndMvdMCPoint.h"
#include "PndMvdHit.h"
#include "PndEmcCluster.h"
#include "PndEmcBump.h"
#include "PndMCTrack.h"
#include "FairMCApplication.h"
#include "FairRootManager.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "FairVolume.h"
#include "FairTask.h"
#include "TParticle.h"
#include "TRandom.h"
#include "Riostream.h"
#include "TH1F.h"

#include <iomanip>

//_________________________________________________________________
PndLheHitsMaker::PndLheHitsMaker() {
  //---

  fLheHits  = new TClonesArray("PndTpcLheHit");
  fGeantTracks = new TClonesArray("PndTpcLheTrack");
  fMvdMode       = 1; 
  fTpcMode       = 1; 
  fSttMode       = 0;
  fEmcMode       = 0;
  fSimulation    = kTRUE;
  fPersistence   = kTRUE;
  fTpcResolution = -1.;
  fMvdResolution = -1.;
  fSttResolution = -1.;
  fVerbose       = kFALSE;
}

//_________________________________________________________________
PndLheHitsMaker::PndLheHitsMaker(const char *name,
				       const char *title):FairTask(name) {
  //---
  fLheHits = new TClonesArray("PndTpcLheHit");
  fGeantTracks = new TClonesArray("PndTpcLheTrack");
  fMvdMode       = 1; 
  fTpcMode       = 1;
  fSttMode       = 0;
  fEmcMode       = 0;
  fSimulation    = kTRUE;
  fPersistence   = kTRUE;
  fTpcResolution = -1.;
  fMvdResolution = -1.;
  fSttResolution = -1.;
  fVerbose       = kFALSE;
}

//_________________________________________________________________
PndLheHitsMaker::~PndLheHitsMaker() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//_________________________________________________________________
InitStatus PndLheHitsMaker::ReInit() {
  // --- Update the pointers after reinitialisation

  return kSUCCESS;
}

//_________________________________________________________________
InitStatus PndLheHitsMaker::Init() {

  //  cout << "InitStatus PndLheHitsMaker::Init\n\n";
  cout << "-I- PndLheHitsMaker::Init" << endl;
  
  fCuts = PndTpcLheTrackCuts::Instance();

  FairRootManager *fManager =FairRootManager::Instance();	
  
  switch (fTpcMode) 
    {
    case 0:
      cout << "-I- PndLheHitsMaker::Init: No TPC detector is used" << endl;
      break;
      
    case 1:
      fTpcInput   = (TClonesArray *)fManager->GetObject("PndTpcPoint");
      if ( ! fTpcInput ) 
	{
	  cout << "-W- PndLheHitsMaker::Init: No TpcPoint array! Switching TPC OFF" << endl;
	  fTpcMode = 0;
	}
      else
	{
	  if (fTpcResolution<0.) cout << "-I- PndLheHitsMaker::Init: Using PndTpcPoint, no position smearing" << endl;
	  else cout << "-I- PndLheHitsMaker::Init: Using PndTpcPoint, position smearing " << fTpcResolution << " [cm]" << endl;
	}
      break;
      
    case 2:
      fTpcInput   = (TClonesArray *)fManager->GetObject("PndTpcCluster");
      if ( ! fTpcInput ) 
	{
	  cout << "-W- PndLheHitsMaker::Init: No TpcCluster array! Switching TPC OFF" << endl;
	  fTpcMode = 0;
	}
      else
	{
	  cout << "-I- PndLheHitsMaker::Init: Using PndTpcCluster" << endl;
	}
      break;

    default:
      cout << "-E- PndLheHitsMaker::Init: Wrong TPC mode. Switching TPC OFF" << endl;
      fTpcMode = 0;
    }
  
 switch (fSttMode) 
    {
    case 0:
      cout << "-I- PndLheHitsMaker::Init: No STT detector is used" << endl;
      break;
      
    case 1:
      fSttInput   = (TClonesArray *)fManager->GetObject("STTPoint");
      if ( ! fSttInput ) 
	{
	  cout << "-W- PndLheHitsMaker::Init: No SttPoint array! Switching STT OFF" << endl;
	  fSttMode = 0;
	}
      else
	{
	  if (fSttResolution<0.) cout << "-I- PndLheHitsMaker::Init: Using PndSttPoint, no position smearing" << endl;
	  else cout << "-I- PndLheHitsMaker::Init: Using PndSttPoint, position smearing " << fSttResolution << " [cm]" << endl;
	}
      break;
      
    case 2:
      fSttInput   = (TClonesArray *)fManager->GetObject("STTHit");
      if ( ! fSttInput ) 
	{
	  cout << "-W- PndLheHitsMaker::Init: No STTHit array! Switching STT OFF" << endl;
	  fSttMode = 0;
	}
      else
	{
	  cout << "-I- PndLheHitsMaker::Init: Using PndSttHit" << endl;
	}
      break;
      
    case 3:
      fSttInput   = (TClonesArray *)fManager->GetObject("SttHelixHit");
      if ( ! fSttInput ) 
	{
	  cout << "-W- PndLheHitsMaker::Init: No SttHelixHit array! Switching STT OFF" << endl;
	  fSttMode = 0;
	}
      else
	{
	  cout << "-I- PndLheHitsMaker::Init: Using PndSttHelixHit" << endl;
	}
      break;
      
    default:
      cout << "-E- PndLheHitsMaker::Init: Wrong STT mode. Switching STT OFF" << endl;
      fSttMode = 0;
    }

 if (fSttMode>0 && fTpcMode > 0)
   {
     cout << "-E- PndLheHitsMaker::Init: Both STT and TPC mode. Too many tracking detectors for my tastes!!!" << endl;
     return kFATAL;
   }
 

  switch (fMvdMode) 
    {
    case 0:
      cout << "-I- PndLheHitsMaker::Init: No MVD detector is used" << endl;
      break;

    case 1:
      fMvdInput = (TClonesArray*) fManager->GetObject("MVDPoint");
      if ( ! fMvdInput ) 
	{
	  cout << "-W- PndLheHitsMaker::Init: No MVDPoint array! Switching MVDPoint OFF" << endl;
	  fMvdMode = 0;
	}
      else
	{
	  if (fMvdResolution<0.) cout << "-I- PndLheHitsMaker::Init: Using MVDPoint, no position smearing" << endl;
	  else cout << "-I- PndLheHitsMaker::Init: Using MVDPoint, position smearing " << fMvdResolution << " [cm]" << endl;
	}
      break;
      
    case 2: 
      fMvdInput = (TClonesArray*) fManager->GetObject("MVDHitsStrip");
      if ( ! fMvdInput ) 
	{
	  cout << "-W- PndLheHitsMaker::Init: No MVDHitsStrip array!" << endl;
	}
      fMvdInput2 = (TClonesArray*) fManager->GetObject("MVDHitsPixel");
      if ( ! fMvdInput2 ) 
	{
	  cout << "-W- PndLheHitsMaker::Init: No MVDHitsPixel array!" << endl;
	}
      if (( ! fMvdInput ) &&  ( ! fMvdInput2 ))
	{
	  cout << "-W- PndLheHitsMaker::Init: No MDC hits array! Switching MDC OFF" << endl;
	  fMvdMode = 0;
	}
      else
	{
	  cout << "-I- PndLheHitsMaker::Init: Using MVDHit" << endl;
	}
      break;
      
    default:
      cout << "-E- PndLheHitsMaker::Init: Wrong MVD mode. Switching MVD OFF" << endl;
      fMvdMode = 0;
    }

  switch (fEmcMode) 
    {
    case 0:
      cout << "-I- PndLheHitsMaker::Init: No EMC detector is used" << endl;
      break;
      
    case 2:
      fEmcInput = (TClonesArray*) fManager->GetObject("EmcCluster");
      if ( ! fEmcInput ) 
	{
	  cout << "-W- PndLheHitsMaker::Init: No EmcCluster array! Switching EmcCluster OFF" << endl;
	  fEmcMode = 0;
	}
      else
	{
	  cout << "-I- PndLheHitsMaker::Init: Using EmcCLuster" << endl;
	}
      break;
      
    case 3:
      fEmcInput = (TClonesArray*) fManager->GetObject("EmcBump");
      if ( ! fEmcInput ) 
	{
	  cout << "-W- PndLheHitsMaker::Init: No EmcBump array! Switching EmcBump OFF" << endl;
	  fEmcMode = 0;
	}
      else
	{
	  cout << "-I- PndLheHitsMaker::Init: Using EmcBump" << endl;
	}
      break;
      
    default:
      cout << "-E- PndLheHitsMaker::Init: Wrong EMC mode. Switching EMC OFF" << endl;
      fMvdMode = 0;
    }

  if ((fMvdMode+fTpcMode+fSttMode+fEmcMode)==0)
    {
      cout << "-E- PndLheHitsMaker::Init: No active detectors! Exit" << endl;
      return kFATAL;
    }
  
  if (fSimulation)
    { 
      fListMCtracks = (TClonesArray *)fManager->GetObject("MCTrack");
      if ( ! fListMCtracks ) {
	cout << "-W- PndLheHitsMaker::Init: No MCTrack array!" << "\t" << "Switching Simulation OFF" << endl;
	fSimulation = kFALSE;
      }
      else cout << "-I- PndLheHitsMaker::Init: Using MCTrack" << endl;
    }
  
  if (fVerbose)   cout << "-I- PndLheHitsMaker::Init: Verbose Mode ON" << endl;
  
  fNofEvents = 0;
  
  Register();
  
  return kSUCCESS;
  
}

//_________________________________________________________________
void PndLheHitsMaker::GetMvdPoints() {
  // Taking points from PndMvdMCPoint
  
  if (fVerbose)
    cout << " -I- PndLheHitsMaker::GetMvdPoints(): MVD points entries " << fMvdInput->GetEntriesFast() <<endl;
  
  for (int j=0; j < fMvdInput->GetEntriesFast(); j++ ) 
    {
      PndMvdMCPoint* point = (PndMvdMCPoint*) fMvdInput->At(j);
      
      if (fVerbose) point->Print(" "); //PR(point->GetTrackID());
      
      PndTpcLheHit* hit = AddHit();
      hit->SetHitNumber(fNHit++);
      
      if (fTpcResolution<=0.)
	{
	  hit->SetX(0.5*(point->GetX()+point->GetXOut()));
	  hit->SetY(0.5*(point->GetY()+point->GetYOut()));
	  hit->SetZ(0.5*(point->GetZ()+point->GetZOut()));
	  
	  if (fVerbose) cout << "MVD POINT " 
			     << hit->GetX() << " " 
			     << hit->GetY() << " " 
			     << hit->GetZ() << " RADIUS " 
			     << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";	
	  
	  hit->SetXerr(0.005);
	  hit->SetYerr(0.005);
	  hit->SetZerr(0.005);
	}
      else
	{
	  hit->SetX(gRandom->Gaus(point->GetX(),fMvdResolution));
	  hit->SetY(gRandom->Gaus(point->GetY(),fMvdResolution));
	  hit->SetZ(gRandom->Gaus(point->GetZ(),fMvdResolution));
	  
	  if (fVerbose) cout << "MVD POINT SMEARED " 
			     << hit->GetX() << " " 
			     << hit->GetY() << " " 
			     << hit->GetZ() << " RADIUS " 
			     << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";
	  
	  hit->SetXerr(fMvdResolution);
	  hit->SetYerr(fMvdResolution);
	  hit->SetZerr(fMvdResolution);
	}
      
      hit->SetDetectorId(kMVDPoint);
      hit->SetTrackID(point->GetTrackID());
      hit->SetRefIndex(j);
      
      if (fVerbose)  hit->Print();
    } // end of MVDPoint loop  
}

//_________________________________________________________________
void PndLheHitsMaker::GetMvdHits() {
  // Taking points from PndMvdHits
  
  for (int j2=0; j2 < fMvdInput2->GetEntriesFast(); j2++ ) 
    {
      PndMvdHit* point = (PndMvdHit*) fMvdInput2->At(j2);
      
      PndTpcLheHit* hit = AddHit();
      hit->SetHitNumber(fNHit++);
      
      hit->SetX(point->GetX());
      hit->SetY(point->GetY());
      hit->SetZ(point->GetZ());
      
      if (fVerbose) cout << "MVD PIXEL HIT " 
			 << hit->GetX() << " " 
			 << hit->GetY() << " " 
			 << hit->GetZ() << " RADIUS " 
			 << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";	
      
      // hit->SetXerr(point->GetDx());
      // hit->SetYerr(point->GetDy());
      // hit->SetZerr(point->GetDz());
     
      hit->SetXerr(0.005);
      hit->SetYerr(0.005);
      hit->SetZerr(0.005);

      hit->SetDetectorId(kMVDHitsPixel);
      hit->SetTrackID(hit->GetRefIndex());
      hit->SetRefIndex(j2);
      
      if (fVerbose)  hit->Print();
    } // end of MVDHitsPixel loop  
  

 
  for (int j=0; j < fMvdInput->GetEntriesFast(); j++ ) 
    {
      PndMvdHit* point = (PndMvdHit*) fMvdInput->At(j);
      
      PndTpcLheHit* hit = AddHit();
      hit->SetHitNumber(fNHit++);
      
      hit->SetX(point->GetX());
      hit->SetY(point->GetY());
      hit->SetZ(point->GetZ());
      
      if (fVerbose) cout << "MVD STRIP HIT " 
			 << hit->GetX() << " " 
			 << hit->GetY() << " " 
			 << hit->GetZ() << " RADIUS " 
			 << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";	
      
      // hit->SetXerr(point->GetDx());
      // hit->SetYerr(point->GetDy());
      // hit->SetZerr(point->GetDz());
       
      hit->SetXerr(0.005);
      hit->SetYerr(0.005);
      hit->SetZerr(0.005);

      hit->SetDetectorId(kMVDHitsStrip);
      hit->SetTrackID(hit->GetRefIndex());
      hit->SetRefIndex(j);
      
      if (fVerbose)  hit->Print();
    } // end of MVDHitsStrip loop  

}
  
//_________________________________________________________________
void PndLheHitsMaker::GetTpcPoints() {
   // Taking points from PndTpcPoint
  
  if (fVerbose)
    cout << " PndLheHitsMaker::GetTpcHits(): Tpc points entries " << fTpcInput->GetEntriesFast() <<endl;
  
  
  for (int j=0; j < fTpcInput->GetEntriesFast(); j++ ) {
    PndTpcPoint* point = (PndTpcPoint*) fTpcInput->At(j);
    
    if (fVerbose) point->Print(" "); //PR(point->GetTrackID());

    PndTpcLheHit* hit = AddHit();
    hit->SetHitNumber(fNHit++);
    
    if (fTpcResolution<=0.)
      {
	hit->SetX(point->GetX());
	hit->SetY(point->GetY());
	hit->SetZ(point->GetZ());
	
	if (fVerbose) cout << "TPC POINT " 
			   << hit->GetX() << " " 
			   << hit->GetY() << " " 
			   << hit->GetZ() << " RADIUS " 
			   << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";
	
	hit->SetXerr(0.03);
	hit->SetYerr(0.03);
	hit->SetZerr(0.03);
      }
    else
      {
	hit->SetX(gRandom->Gaus(point->GetX(),fTpcResolution));
	hit->SetY(gRandom->Gaus(point->GetY(),fTpcResolution));
	hit->SetZ(gRandom->Gaus(point->GetZ(),2*fTpcResolution));
	
	if (fVerbose) cout << "TPC POINT SMEARED " 
			   << hit->GetX() << " " 
			   << hit->GetY() << " " 
			   << hit->GetZ() << " RADIUS " 
			   << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";
	
	hit->SetXerr(fTpcResolution);
	hit->SetYerr(fTpcResolution);
	hit->SetZerr(2*fTpcResolution);
      }
    
    hit->SetDetectorId(kTpcPoint);
    hit->SetTrackID(point->GetTrackID());
    hit->SetRefIndex(j);
    
    if (fVerbose)  hit->Print();
  }  // end of TpcPoints loop
  
} 

//_________________________________________________________________
void PndLheHitsMaker::GetTpcClusters() {
   // Taking points from PndTpcCluster

  for (int j=0; j < fTpcInput->GetEntriesFast(); j++ ) {
    PndTpcCluster* clu = (PndTpcCluster*) fTpcInput->At(j);
    
    PndTpcLheHit* hit = AddHit();
    hit->SetHitNumber(fNHit++);
    
    hit->SetX(clu->pos().X());  
    hit->SetY(clu->pos().Y());
    hit->SetZ(clu->pos().Z());
  
    
    if (fVerbose) cout << "TPC CLU " 
		       << hit->GetX() << " " 
		       << hit->GetY() << " " 
		       << hit->GetZ() << " RADIUS " 
		       << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";
    
    hit->SetXerr(clu->sig().X());  
    hit->SetYerr(clu->sig().Y());
    hit->SetZerr(clu->sig().Z());

    hit->SetDetectorId(kTpcCluster);
    hit->SetTrackID(clu->mcId().DominantID().mctrackID());
    hit->SetRefIndex(j);
    
    if (fVerbose)  hit->Print();
    
  }  // end of TpcCluster loop  
} 
//_________________________________________________________________
void PndLheHitsMaker::GetSttPoints() {
   // Taking points from PndSttPoint
  
  if (fVerbose)
    cout << " PndLheHitsMaker::GetSttHits(): Stt points entries " << fSttInput->GetEntriesFast() <<endl;
  
  
  for (int j=0; j < fSttInput->GetEntriesFast(); j++ ) {
    PndSttPoint* point = (PndSttPoint*) fSttInput->At(j);
    
    if (fVerbose) point->Print(" "); //PR(point->GetTrackID());

    PndTpcLheHit* hit = AddHit();
    hit->SetHitNumber(fNHit++);
    
    if (fSttResolution<=0.)
      {
	hit->SetX(point->GetX());
	hit->SetY(point->GetY());
	hit->SetZ(point->GetZ());
	
	if (fVerbose) cout << "STT POINT " 
			   << hit->GetX() << " " 
			   << hit->GetY() << " " 
			   << hit->GetZ() << " RADIUS " 
			   << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";
	
	hit->SetXerr(.5);
	hit->SetYerr(.5);
	hit->SetZerr(.5);
      }
    else
      {
	hit->SetX(gRandom->Gaus(point->GetX(),fSttResolution));
	hit->SetY(gRandom->Gaus(point->GetY(),fSttResolution));
	hit->SetZ(gRandom->Gaus(point->GetZ(),2*fSttResolution));
	
	if (fVerbose) cout << "STT POINT SMEARED " 
			   << hit->GetX() << " " 
			   << hit->GetY() << " " 
			   << hit->GetZ() << " RADIUS " 
			   << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";
	
	hit->SetXerr(fSttResolution);
	hit->SetYerr(fSttResolution);
	hit->SetZerr(2*fSttResolution);
      }
    
    hit->SetDetectorId(kSttPoint);
    hit->SetTrackID(point->GetTrackID());
    hit->SetRefIndex(j);
    
    if (fVerbose)  hit->Print();
  }  // end of SttPoints loop
  
}


//_________________________________________________________________
void PndLheHitsMaker::GetSttHit() {
   // Taking points from PndSttHit

  for (int j=0; j < fSttInput->GetEntriesFast(); j++ ) {
    PndSttHit* sttHit = (PndSttHit*) fSttInput->At(j);
    
    PndTpcLheHit* hit = AddHit();
    hit->SetHitNumber(fNHit++);
    
    hit->SetX(sttHit->GetX());  
    hit->SetY(sttHit->GetY());
    hit->SetZ(sttHit->GetZ());
  
    
    if (fVerbose) cout << "STT HIT " 
		       << hit->GetX() << " " 
		       << hit->GetY() << " " 
		       << hit->GetZ() << " RADIUS " 
		       << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";
    
    hit->SetXerr(.5);  
    hit->SetYerr(.5);
    hit->SetZerr(.5);

    hit->SetDetectorId(kSttHit);
    hit->SetTrackID(hit->GetRefIndex());
    hit->SetRefIndex(j);
    
    if (fVerbose)  hit->Print();
    
  }  // end of SttHit loop  
} 

//_________________________________________________________________
void PndLheHitsMaker::GetSttHelixHit() {
   // Taking points from PndSttHelixHit

  for (int j=0; j < fSttInput->GetEntriesFast(); j++ ) {
    PndSttHelixHit* sttHit = (PndSttHelixHit*) fSttInput->At(j);
    if (isnan(sttHit->GetX()))
      {
        sttHit->Print();
        cout << "-W- PndLheHitsMaker::GetSttHelixHit: SttHelixHit with NaN - skipped (requires STT bug fix!!)" << endl;
        continue;
      }
      if (sttHit->GetZ()==0)
      {
        sttHit->Print();
        cout << "-W- PndLheHitsMaker::GetSttHelixHit: SttHelixHit with GetZ() == 0 - skipped (requires STT bug fix!!)" << endl;
        continue;
      }

    PndTpcLheHit* hit = AddHit();
    hit->SetHitNumber(fNHit++);
    
    hit->SetX(sttHit->GetX());  
    hit->SetY(sttHit->GetY());
    hit->SetZ(sttHit->GetZ());
  
    
    if (fVerbose) cout << "STT HELIX HIT " 
		       << hit->GetX() << " " 
		       << hit->GetY() << " " 
		       << hit->GetZ() << " RADIUS " 
		       << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";
    
    hit->SetXerr(.5);  
    hit->SetYerr(.5);
    hit->SetZerr(.5);

    hit->SetDetectorId(kSttHelixHit);
    hit->SetTrackID(hit->GetRefIndex());
    hit->SetRefIndex(j);
    
    if (fVerbose)  hit->Print();
    
  }  // end of SttHelixHit loop  
} 

//_________________________________________________________________
void PndLheHitsMaker::GetEmcClusters() {
   // Taking points from PndEmcCluster

  for (int j=0; j < fEmcInput->GetEntriesFast(); j++ ) {
    PndEmcCluster* clu = (PndEmcCluster*) fEmcInput->At(j);
    
    PndTpcLheHit* hit = AddHit();
    hit->SetHitNumber(fNHit++);
    
    hit->SetX(clu->x());  
    hit->SetY(clu->y());
    hit->SetZ(clu->z());
  
    
    if (fVerbose) cout << "EMC CLU " 
		       << hit->GetX() << " " 
		       << hit->GetY() << " " 
		       << hit->GetZ() << " RADIUS " 
		       << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";
    
    hit->SetXerr(1.);  
    hit->SetYerr(1.);
    hit->SetZerr(1.);

    hit->SetDetectorId(kEmcCluster);
    //hit->SetTrackID(point->GetTrackID());
    hit->SetTrackID(-1);
    hit->SetRefIndex(j);
    
    if (fVerbose)  hit->Print();
    
  }  // end of EmcCluster loop  
} 
//_________________________________________________________________
void PndLheHitsMaker::GetEmcBumps() {
   // Taking points from PndEmcBump

  for (int j=0; j < fEmcInput->GetEntriesFast(); j++ ) {
    PndEmcBump* clu = (PndEmcBump*) fEmcInput->At(j);
    
    PndTpcLheHit* hit = AddHit();
    hit->SetHitNumber(fNHit++);
    
    hit->SetX(clu->x());  
    hit->SetY(clu->y());
    hit->SetZ(clu->z());
  
    
    if (fVerbose) cout << "EMC BUMP " 
		       << hit->GetX() << " " 
		       << hit->GetY() << " " 
		       << hit->GetZ() << " RADIUS " 
		       << sqrt((hit->GetX()*hit->GetX())+(hit->GetY()*hit->GetY())) << "\n";
    
    hit->SetXerr(1.);  
    hit->SetYerr(1.);
    hit->SetZerr(1.);

    hit->SetDetectorId(kEmcBump);
    //hit->SetTrackID(point->GetTrackID());
    hit->SetTrackID(-1);
    hit->SetRefIndex(j);
    
    if (fVerbose)  hit->Print();
    
  }  // end of TpcCluster loop  
} 

//_________________________________________________________________
void PndLheHitsMaker::Exec(Option_t * option) {
    
  cout << "\n\n  ***  Event # " << ++fNofEvents << endl;

  cout << " =====   PndLheHitsMaker   =====\n";

  Reset();

  if ((fMvdMode==1) && (fMvdInput->GetEntriesFast()>0))  GetMvdPoints();
  if ((fMvdMode==2) && ((fMvdInput->GetEntriesFast()+fMvdInput2->GetEntriesFast())>0))  GetMvdHits();
  
  if ((fTpcMode==1) && (fTpcInput->GetEntriesFast()>0))  GetTpcPoints();
  if ((fTpcMode==2) && (fTpcInput->GetEntriesFast()>0))  GetTpcClusters();

  if ((fSttMode==1) && (fSttInput->GetEntriesFast()>0))  GetSttPoints();
  if ((fSttMode==2) && (fSttInput->GetEntriesFast()>0))  GetSttHit();
  if ((fSttMode==3) && (fSttInput->GetEntriesFast()>0))  GetSttHelixHit();
  
  if ((fEmcMode==2) && (fEmcInput->GetEntriesFast()>0))  GetEmcClusters();
  if ((fEmcMode==3) && (fEmcInput->GetEntriesFast()>0))  GetEmcBumps();
  
  cout << "  Total number of hits for tracking: " <<
    setw(5) << fLheHits->GetEntriesFast() << endl;

  if (fSimulation)  CheckTracks();
  if (fVerbose) PrintTracks(0);
  
  //  fTpcInput->Clear("C");
  //  fMvdInput->Clear("C");

}

//_________________________________________________________________
PndTpcLheHit * PndLheHitsMaker::AddHit() {
  // Creates a new hit in the TClonesArray.

  TClonesArray& hitRef = *fLheHits;
  Int_t size = hitRef.GetEntriesFast();
  return new(hitRef[size]) PndTpcLheHit();
}

//_________________________________________________________________
void PndLheHitsMaker::Register() {
  //---

  FairRootManager::Instance()->
    Register("LheGeantTrack","Lhe", fGeantTracks, fPersistence);
  //    Register("LheGeantTrack","Lhe", fGeantTracks, kTRUE);

  FairRootManager::Instance()->
    Register("LheHit","Lhe", fLheHits, fPersistence);

}

//_________________________________________________________________
void PndLheHitsMaker::Finish() {

//   FairRootManager *fManager =FairRootManager::Instance();	
//   fManager->Fill();

  cout << " PndLheHitsMaker::Finish : Geant tracks " <<
    fGeantTracks->GetEntriesFast() << endl;

  fGeantTracks->Delete();
  fLheHits->Delete();

}

//_________________________________________________________________
void PndLheHitsMaker::Reset() {
  //---
  //  cout << " PndLheHitsMaker::Reset  " << endl;

  fNHit = 0;
  fNTrack = 0;	

  if (fLheHits->GetEntriesFast() != 0)  fLheHits->Clear("C");
  if (fGeantTracks->GetEntriesFast() != 0)fGeantTracks->Clear("C");

}

//_______________________________________________________________
void PndLheHitsMaker::CheckTracks() {

  using namespace std;

  Int_t  parent, tr_num, tr_pdg;
  Int_t nhit = 0;

  TObjArray dtracks;
  TObjArray  *chits;

  PndMCTrack *gtrack = 0;

  Int_t nMCtracks = fListMCtracks->GetEntriesFast();

  Int_t selected_tracks = 0;

  for (Int_t ih = 0; ih < fLheHits->GetEntriesFast(); ih++) {
    PndTpcLheHit *hit = (PndTpcLheHit *)fLheHits->UncheckedAt(ih);
    SetTrack(hit);
    //    hit->Print();
  }

  if (fVerbose) {
    cout << " MC tracks in event: " <<fListMCtracks->GetEntriesFast();
    cout << " tracks in TPC " << fNTrack << endl;
  }

  for (Int_t itrack = 0; itrack < fNTrack; itrack++) {

    PndTpcLheTrack *track = (PndTpcLheTrack*)fGeantTracks->UncheckedAt(itrack);
    tr_num = track->GetTrackNumber();

    if(tr_num >= 0 && tr_num < nMCtracks) {
      tr_pdg = 0;
      parent = 99999;

      gtrack = (PndMCTrack *) fListMCtracks->At(tr_num);

      tr_pdg = gtrack->GetPdgCode();
      track->SetPid(tr_pdg);
      track->SetCharge(TMath::Sign(1,tr_pdg));
      parent = gtrack->GetMotherID();
      if(parent == -1) {
	track->ComesFromMainVertex(kTRUE);
	track->SetGood(kTRUE);
      }

      TVector3 vertex = gtrack->GetStartVertex();
      track->SetVertex(vertex.X(), vertex.Y(), vertex.Z());

      TVector3 moment = gtrack->GetMomentum();
      track->SetPx(moment.X());
      track->SetPy(moment.Y());
      track->SetPz(moment.Z());

      nhit = track->GetNumberOfHits();
      chits = track->GetRHits();
      // print some info about track
      
      if (fVerbose>2) {
	cout << "\n *** track *** " << track->GetTrackNumber() <<
	  " pdg " << track->GetPid() <<
	  " nhits " <<nhit << " p_t ";
	printf("%f", track->GetPt());
	cout << "\n";
      }
      
      if (fVerbose) track->Print();
      PndTpcLheHit *hit = (PndTpcLheHit *)chits->First();
      if (fVerbose) hit->Print();

      if (fCuts->IsGoodGeantTrack(track)) {
	selected_tracks++;
      }

      if (parent == 99999 ) cout << "  track parent unknown";

    }
  }

#if 0
  for (Int_t ih = 0; ih < fLheHits->GetEntriesFast(); ih++) {
    PndTpcLheHit *hit = (PndTpcLheHit *)fLheHits->UncheckedAt(ih);
    Int_t mTrackNumber = hit->GetTrackID();
    for (Int_t itrack = 0; itrack < fNTrack; itrack++) {
      PndTpcLheTrack *track = (PndTpcLheTrack*)fGeantTracks->UncheckedAt(itrack);
      Int_t tr_num = track->GetTrackNumber();
      if (mTrackNumber ==  tr_num && !track->IsGood()) {
	hit->SetUsage(kTRUE);
      }
    }
  }
#endif

  cout << "Total number of tracks in TPC: " <<
    setw(5) << fGeantTracks->GetEntriesFast() << endl;
  cout << "           Good tracks in TPC: " <<
    setw(5) << selected_tracks << endl;
     
}

//________________________________________________________________
void PndLheHitsMaker::SetTrack(PndTpcLheHit *hit) {

  Int_t mTrackNumber = hit->GetTrackID();
  Int_t mTN;

#ifdef HITS
     cout << " fNHit = " << hit->GetHitNumber();
     cout << " fNTrack = " << fNTrack;
     cout << " mTrackNumber = " << mTrackNumber;
     cout << endl;
#endif

  PndTpcLheTrack *track = NULL;

  Bool_t newtrack = kTRUE;
  
  if (fNTrack > 0) {
    for (Int_t i = 0; i < fGeantTracks->GetEntriesFast(); i++) {
      PndTpcLheTrack *currtrk = ((PndTpcLheTrack*) fGeantTracks->At(i));
      mTN = currtrk->GetTrackNumber();
      //      PR( currtrk->GetTrackNumber());
      if (mTN == mTrackNumber) {
	track = currtrk;
	newtrack = kFALSE;
	break;
      }
    }
  }
  if (newtrack)  track = AddTrack(mTrackNumber);

  track->AddHit(hit);
  
}

//________________________________________________________________
PndTpcLheTrack *PndLheHitsMaker::AddTrack(Int_t mTrackNumber) {
// Add a new track to the list of tracks for this event.

  TClonesArray &tracks = *fGeantTracks;
  PndTpcLheTrack *track = new(tracks[fNTrack++]) PndTpcLheTrack(mTrackNumber);
  return track;
}

//_________________________________________________________________
void PndLheHitsMaker::PrintTracks(Int_t ntr) {


  //  cout << " " << fNTrack << " tracks  \n\n ";
  //  cout << "Station X Y Z Px Py Pz \n\n";

  Int_t  ptracks;

  if(ntr != 0 ) 
    ptracks = ntr;
  else
    ptracks = fGeantTracks->GetEntriesFast();

  for (Int_t itrack = 0; itrack < ptracks; itrack++) {
	
    PndTpcLheTrack *track = (PndTpcLheTrack *)fGeantTracks->UncheckedAt(itrack);

    track->Print(); //

  }

}

ClassImp(PndLheHitsMaker)

