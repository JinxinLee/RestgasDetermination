// -------------------------------------------------------------------------
// -----                PndSttTrackFitterQATask source file             -----
// -------------------------------------------------------------------------

#include "PndSttTrackFitterQATask.h"

#include "PndSttHit.h"
#include "PndSttHitInfo.h"
#include "PndSttPoint.h"
//#include "PndSttSingleStraw.h"
#include "PndSttMapCreator.h"
#include "PndSttTube.h"
#include "PndSttTrack.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndSttHelixHit.h"
#include "PndMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TRandom.h"
#include "TH1F.h"
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::sqrt;

// -----   Default constructor   -------------------------------------------
PndSttTrackFitterQATask::PndSttTrackFitterQATask() :
  FairTask("") 
{ 
 
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttTrackFitterQATask::~PndSttTrackFitterQATask() 
{ 
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndSttTrackFitterQATask::Init() 
{

  hptfit   = new TH1F("hptfit", "pt fit: mc - reco", 100, -0.3, 0.3);
  hplfit   = new TH1F("hplfit", "pl fit: mc - reco", 100, -0.4, 0.4);
  hptotfit = new TH1F("hptotfit", "ptot fit: mc - reco", 100, -0.3, 0.3);

  hDist = new TH1F("hDist", "Dist fit: mc - reco", 100, -1., 1.);
  hRad  = new TH1F("hRad", "Rad fit: mc - reco", 100, -15., 15.);
  hPhi  = new TH1F("hPhi", "Phi fit: mc - reco", 100, -0.1, 0.1);
  hTanL = new TH1F("hTanL", "TanL fit: mc - reco", 100, -0.2, 0.2);
  hZ    = new TH1F("hZ", "Z fit: mc - reco", 100, -0.3, 0.3);
 
  hptfound = new TH1F("hptfound", "pt found: mc - reco", 100, -0.3, 0.3);
  hplfound = new TH1F("hplfound", "pl found: mc - reco", 100, -0.4, 0.4);
  hptotfound = new TH1F("hptotfound", "ptot found: mc - reco", 100, -0.3, 0.3);
 
  hresx = new TH1F("hresx", "x: mc - reco", 100, -0.2, 0.2);
  hresy = new TH1F("hresy", "y: mc - reco", 100, -0.2, 0.2);
  hresz = new TH1F("hresz", "z: mc - reco", 100, -3, 3);
  
  hx = new TH1F("hx", "x no skewed: mc - reco", 100, -0.2, 0.2);
  hy = new TH1F("hy", "y no skewed: mc - reco", 100, -0.2, 0.2);
  hz = new TH1F("hz", "z no skewed: mc - reco", 100, -3, 3);
  
  hxs = new TH1F("hxs", "x skewed: mc - reco", 100, -0.2, 0.2);
  hys = new TH1F("hys", "y skewed: mc - reco", 100, -0.2, 0.2);
  hzs = new TH1F("hzs", "z skewed: mc - reco", 100, -3, 3);

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndSttHelixHitProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*)  ioman->GetObject("MCTrack"); 
  if ( ! fMCTrackArray) 
    {
      cout << "-E- PndSttTrackFitterQATask::Init: No MCTrack array!"
	   << endl;
      return kERROR;
    }

  // Get SttTrack array
  fTrackArray  = (TClonesArray*) ioman->GetObject("STTTrack"); 
  if ( ! fTrackArray) 
    {
      cout << "-E- PndSttTrackFitterQATask::Init: No SttTrack array!"
	   << endl;
      return kERROR;
    }

 // Get SttTrackCand array
  fTrackCandArray  = (TClonesArray*) ioman->GetObject("STTTrackCand"); 
  if ( ! fTrackCandArray) 
    {
      cout << "-E- PndSttTrackFitterQATask::Init: No SttTrack Cand  array!"
	   << endl;
      return kERROR;
    }
	
  // MC points
  fPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndSttTrackFitterQATask::Init: "
	 << "No STTPoint array!" << endl;
    return kERROR;
  }

  // hits
  fHitArray = (TClonesArray*) ioman->GetObject("STTHit");
  if ( ! fHitArray ) {
    cout << "-W- PndSttTrackFitterQATask::Init: "
	 << "No STTHit array!" << endl;
    return kERROR;
  }

  // helix hits
  fHelixHitArray = (TClonesArray*) ioman->GetObject("SttHelixHit");
  if ( ! fHelixHitArray ) {
    cout << "-W- PndSttTrackFitterQATask::Init: "
	 << "No SttHelixHit array!" << endl;
    return kERROR;
  }

  // CHECK added 
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();

  cout << "-I- PndSttTrackFitterQATask: Intialization successfull" << endl;
  
  return kSUCCESS;
 
}
// -------------------------------------------------------------------------

// CHECK added 
void PndSttTrackFitterQATask::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

// -----   Public method Exec   --------------------------------------------
void PndSttTrackFitterQATask::Exec(Option_t* opt) 
{
  // Declare some variables
  PndSttTrack*  pTrack  = NULL;
  PndTrackCand* pTrackCand  = NULL;
  PndMCTrack * mcTrack = NULL;

  if ( ! fTrackArray ) Fatal("Exec", "No fTrackArray");
   
  // Loop over fitted tracks
  Int_t nTracks = fTrackArray->GetEntriesFast();

  for (Int_t j = 0; j < nTracks; j++) {
    pTrack = (PndSttTrack *) fTrackArray->At(j);
    if(!pTrack) continue;
    Int_t trackCandID = pTrack->GetTrackCandIndex();
    pTrackCand = (PndTrackCand *) fTrackCandArray->At(trackCandID);
    if(!pTrackCand) continue;

    // ================= momentum residual ========================
    if(pTrack->GetFlag() < 3) continue; // only prefit-fit-zfit CHECK
    // -------------------------------------------------------------------
    // FITTED track
    // xy
    Int_t h = -(Int_t) pTrack->GetCharge();
    Double_t d0 = pTrack->GetDist();
    Double_t phi0 = pTrack->GetPhi();
    Double_t Rad =  pTrack->GetRad();
    // z    
    Double_t z0 = pTrack->GetZ();
    Double_t zslope = pTrack->GetTanL();
    // center of curvature of helix
    TVector2 vec((d0+Rad)*cos(phi0), (d0+Rad)*sin(phi0));
  
    Double_t ptran = 0.003 * 2 * Rad;
    Double_t plong = ptran * zslope;
    Double_t ptot = sqrt(plong*plong + ptran*ptran);

    // -------------------------------------------------------------------
    // FOUND track
    TVector3 foundMom = pTrackCand->getDirSeed();
    Double_t momMag = fabs(1./pTrackCand->getQoverPseed());
    foundMom.SetMag(momMag);

    // -------------------------------------------------------------------
    // MC track
    Int_t mcIndex = pTrackCand->getMcTrackId();
    mcTrack = (PndMCTrack *) fMCTrackArray->At(mcIndex);
    if(!mcTrack) continue;
    TVector3 mcmom = mcTrack->GetMomentum();       
    TVector3 vertex = mcTrack->GetStartVertex();       
 
    hptfit->Fill(ptran - mcmom.Perp());
    hplfit->Fill(plong - mcmom.Z());
    hptotfit->Fill(ptot - mcmom.Mag());

    hptfound->Fill(foundMom.Perp() - mcmom.Perp());
    hplfound->Fill(foundMom.Z() - mcmom.Z());
    hptotfound->Fill(foundMom.Mag() - mcmom.Mag());


    // ================= parameters residual ========================
    double mcRad = mcmom.Perp()/0.006;

    // track from tangent ---------------------
    double mc_m1 = mcmom.Y() / mcmom.X();
    double mc_q1 = vertex.Y() - vertex.X() * mc_m1;
    double mc_m2 = -1./mc_m1;
    double mc_q2 = vertex.Y() - vertex.X() * mc_m2;

    double alpha = TMath::ATan2(mcmom.X(), mcmom.Y());
    double mcX0, mcY0;
    if(-h > 0) { 
      mcX0 = vertex.X() + mcRad * TMath::Cos(alpha);
      mcY0 = vertex.Y() - mcRad * TMath::Sin(alpha);
    }
    else {
      mcX0 = vertex.X() - mcRad * TMath::Cos(alpha);
      mcY0 = vertex.Y() + mcRad * TMath::Sin(alpha);
    }

    Double_t mcDist, mcPhi;

    mcDist = TMath::Sqrt(mcX0 * mcX0 + mcY0 * mcY0) - mcRad;
    mcPhi = atan2(mcY0, mcX0);

    Double_t mcTanL, mcZ;
    mcTanL = mcmom.Z()/mcmom.Perp();

    // mcZ = ??

    if(pTrack->GetFlag() >= 2) hDist->Fill(d0 - mcDist);
    if(pTrack->GetFlag() >= 2) hRad->Fill(Rad - mcRad);
    if(pTrack->GetFlag() >= 2) hPhi->Fill(phi0 - mcPhi);
    if(pTrack->GetFlag() >= 3) hTanL->Fill(zslope - mcTanL);
    //    if(pTrack->GetFlag() >= 3)  hZ->Fill(z0 - mcZ); // ??
  

    // ========================= helix hits ==============================
    Int_t hitcounter = pTrack->GetNofHelixHits();
    TVector2 point; // point

    for (Int_t k = 0; k < hitcounter; k++) {
      Int_t iHHit = pTrack->GetHelixHitIndex(k);
      PndSttHelixHit *helixhit = (PndSttHelixHit*) fHelixHitArray->At(iHHit);
      Int_t hitindex = helixhit->GetHitIndex();
      PndSttHit* hit = (PndSttHit*) fHitArray->At(hitindex);
     
      // tubeID  CHECK added
      Int_t tubeID = hit->GetTubeID();
      PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
      
      PndSttPoint *mcpoint = (PndSttPoint*) fPointArray->At(hit->GetRefIndex());

      hresx->Fill(helixhit->GetX() - mcpoint->GetX());
      hresy->Fill(helixhit->GetY() - mcpoint->GetY());
      hresz->Fill(helixhit->GetZ() - mcpoint->GetZ());

      if(tube->GetWireDirection() == TVector3(0, 0, 1)) {
	hx->Fill(helixhit->GetX() - mcpoint->GetX());
	hy->Fill(helixhit->GetY() - mcpoint->GetY());
	hz->Fill(helixhit->GetZ() - mcpoint->GetZ());
      }
      else {
	hxs->Fill(helixhit->GetX() - mcpoint->GetX());
	hys->Fill(helixhit->GetY() - mcpoint->GetY());
	hzs->Fill(helixhit->GetZ() - mcpoint->GetZ());
      }
      
    }
  }
}
// -------------------------------------------------------------------------
 
 
void PndSttTrackFitterQATask::WriteHistograms()
{
  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("SttFitterQATask");
  file->cd("SttFitterQATask");
  
  hptfit->Write();
  delete hptfit;
  hplfit->Write();
  delete hplfit;
  hptotfit->Write();
  delete hptotfit;

  hDist->Write();
  delete hDist;
  hRad->Write();
  delete hRad;
  hPhi->Write();
  delete hPhi;
  hTanL->Write();
  delete hTanL;
  hZ->Write();
  delete hZ;


  hptfound->Write();
  delete hptfound;
  hplfound->Write();
  delete hplfound;  
  hptotfound->Write();
  delete hptotfound;


  hresx->Write();
  delete hresx;
  hresy->Write();
  delete hresy;
  hresz->Write();
  delete hresz;

  hx->Write();
  delete hx;
  hy->Write();
  delete hy;
  hz->Write();
  delete hz;

  hxs->Write();
  delete hxs;
  hys->Write();
  delete hys;
  hzs->Write();
  delete hzs;

}



ClassImp(PndSttTrackFitterQATask)
