

#include "PndLambdaIM.h"

// pandaroot
#include "PndSttMapCreator.h"
#include "PndSttTube.h"
#include "PndTrack.h"
#include "PndTrackID.h"
#include "PndTrackCand.h"
#include "PndGemHit.h"
#include "PndGemMCPoint.h"
#include "PndMCTrack.h"
#include "PndSttHit.h"
#include "PndSttPoint.h"

// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeanePro.h"
#include "FairGeaneUtil.h"
#include "FairTrackParP.h"
#include "FairTrackParH.h"

// ROOT
#include "TDatabasePDG.h"
#include "TArc.h"
#include "TMarker.h"
#include "TF1.h"
#include "TClonesArray.h"

// general
#include <iostream>
#include <cmath>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndLambdaIM::PndLambdaIM() :
  FairTask("LAM tracking") { 
  fDisplayOn = kFALSE;
}
// -------------------------------------------------------------------------

PndLambdaIM::PndLambdaIM(Int_t verbose) :
  FairTask("LAM tracking") { 
  fVerbose = verbose;
  fDisplayOn = kFALSE;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndLambdaIM::~PndLambdaIM() { 
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndLambdaIM::Init() {
  if(fDisplayOn) {
    display = new TCanvas("display", "display", 0, 0, 800, 800);
  } 

  evt = 0;

  hIM =  new TH1F("hIM", "lam/lambar IM", 150, 1., 1.3);
  hIMpeak =  new TH1F("hIMpeak", "lam/lambar IM in peak", 150, 1., 1.3);

  hpionmomentum =  new TH1F("hpionmomentum", "pion momentum", 100, 0., 1.5);
  hprotmomentum =  new TH1F("hprotmomentum", "proton momentum", 100, 0., 4.);
  hMCpionmomentum =  new TH1F("hMCpionmomentum", "MC pion momentum", 100, 0., 1.5);
  hMCprotmomentum =  new TH1F("hMCprotmomentum", "MC proton momentum", 100, 0., 4.);

 

  hPiVertexDistanceXY =  new TH1F("hPiVertexDistanceXY", "#pi - vertex distance xy", 100, 0, 20);
  hPiVertexDistanceZ =  new TH1F("hPiVertexDistanceZ", "#pi - vertex distance z", 200, -20, 20);
  hPVertexDistanceXY =  new TH1F("hPVertexDistanceXY", "p - vertex distance xy", 100, 0, 20);
  hPVertexDistanceZ =  new TH1F("hPVertexDistanceZ", "p - vertex distance z", 200, -20, 20);

  hPideltapoverp1 = new TH1F("hPideltapoverp1", "delta p/p pi-/pi+", 100, -2., 2.);
  hPideltaptoverpt1 = new TH1F("hPideltaptoverpt1", "delta pt/pt pi-/pi+", 100, -2., 2.);
  hPideltaploverpl1 = new TH1F("hPideltaploverpl1", "delta pl/pl pi-/pi+", 100, -2., 2.);
  hPideltapoverp2 = new TH1F("hPideltapoverp2", "delta p/p pi-/pi+", 100, -2., 2.);
  hPideltaptoverpt2 = new TH1F("hPideltaptoverpt2", "delta pt/pt pi-/pi+", 100, -2., 2.);
  hPideltaploverpl2 = new TH1F("hPideltaploverpl2", "delta pl/pl pi-/pi+", 100, -2., 2.);

  hPdeltapoverp1 = new TH1F("hPdeltapoverp1", "delta p/p pbar/p", 100, -2., 2.);
  hPdeltaptoverpt1 = new TH1F("hPdeltaptoverpt1", "delta pt/pt pbar/p", 100, -2., 2.);
  hPdeltaploverpl1 = new TH1F("hPdeltaploverpl1", "delta pl/pl pbar/p", 100, -2., 2.);
  hPdeltapoverp2 = new TH1F("hPdeltapoverp2", "delta p/p pbar/p", 100, -2., 2.);
  hPdeltaptoverpt2 = new TH1F("hPdeltaptoverpt2", "delta pt/pt pbar/p", 100, -2., 2.);
  hPdeltaploverpl2 = new TH1F("hPdeltaploverpl2", "delta pl/pl p-/p+", 100, -2., 2.);


  FairRootManager *ioman = FairRootManager::Instance();


  
  if (!ioman) 
    {
      cout << "-E- PndLambdaIM: "
	   << "RootManager not instantised!" << endl;
      return kFATAL;
    }
  

  fSecTrackArray = (TClonesArray*) ioman->GetObject("SttMvdSecTrack");
  if(!fSecTrackArray) {
    Error("PndLambdaIM:Init","track - array not found!");
    return kERROR;
  }
  
  
  fSecTrackIDArray = (TClonesArray*) ioman->GetObject("SttMvdSecTrackID");
  if(!fSecTrackIDArray) {
    Error("PndLambdaIM:Init","trackID - array not found!");
    return kERROR;
  }
  
  
  // open MC track array CHECK to be deleted for real data
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if(!fMCTrackArray) {
    Error("PndLambdaIM:Init","mctrack - array not found!");
    return kERROR;
  }

  // open STT hit array 
  fSttHitArray = (TClonesArray*) ioman->GetObject("STTHit");
  if(!fSttHitArray) {
    Error("PndLambdaIM:Init","stt hit - array not found!");
    return kERROR;
  }

  // open STT point array 
  fSttPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
  if(!fSttPointArray) {
    Error("PndLambdaIM:Init","stt point - array not found!");
    return kERROR;
  }

  
  // GEANE propagation to volume
  fPro = new FairGeanePro();


  // STT mapper
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();

  mcpipid = 1, mcpimid = 3, mcppid = 2, mcpmid = 0;


  cout << "-I- PndLambdaIM: Intialisation successfull" << endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndLambdaIM::SetParContainers() {

  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();

  // get STT parameters
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");

}





// -----   Public method Exec   --------------------------------------------
void PndLambdaIM::Exec(Option_t* opt) {

  cout << "EVENT " << evt << endl;
  evt++;
 
  if(fDisplayOn) {
    char goOnChar;
    cout << "press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
   
    DrawGeometry();
    DrawHits();
  }



  // COUNT MC POINTS FOR PRIMARIES
  int mcpointsintrack[4] = {0, 0, 0, 0};
  for(int ipnt = 0; ipnt < fSttPointArray->GetEntriesFast(); ipnt++) {
    PndSttPoint *pnt = (PndSttPoint*) fSttPointArray->At(ipnt);
    int trkID = pnt->GetTrackID();
    if(trkID > 3) continue;
    mcpointsintrack[trkID]++;
  }

//   for(int itrk = 0; itrk < 4; itrk++)  cout << "mc track " << itrk << " has " << mcpointsintrack[itrk] << " points" << endl;


  // GET MC VERTICES only pion plus and minus
  PndMCTrack *mcpiptrk = (PndMCTrack*) fMCTrackArray->At(mcpipid);
  TVector3 lambdabarVtx = mcpiptrk->GetStartVertex();
  PndMCTrack *mcpimtrk = (PndMCTrack*) fMCTrackArray->At(mcpimid);
  TVector3 lambdaVtx = mcpimtrk->GetStartVertex();

  // ----------------------
  TVector3 mcpipmom = mcpiptrk->GetMomentum();
  TVector3 mcpimmom = mcpimtrk->GetMomentum();
  hMCpionmomentum->Fill(mcpipmom.Mag());
  hMCpionmomentum->Fill(mcpimmom.Mag());

  PndMCTrack *mcpptrk = (PndMCTrack*) fMCTrackArray->At(mcppid);
  PndMCTrack *mcpmtrk = (PndMCTrack*) fMCTrackArray->At(mcpmid);
  TVector3 mcppmom = mcpptrk->GetMomentum();
  TVector3 mcpmmom = mcpmtrk->GetMomentum();
  hMCprotmomentum->Fill(mcppmom.Mag());
  hMCprotmomentum->Fill(mcpmmom.Mag());
  // -----------------------


  // TRACK SELECTION:
  // the kalman has been performed ONLY of primary tracks (0-4)
  int ntracks = fSecTrackArray->GetEntriesFast();

  int npip = 0, npim = 0, npp = 0, npm = 0;


  // each entry in the list is: track ID - nrighthits - nwronghits
  std::vector< std::vector<int> > piplist, pimlist, pplist, pmlist, otherslist;


  for(int itrk = 0; itrk < ntracks; itrk++) {

    PndTrack *trk = (PndTrack*) fSecTrackArray->At(itrk);
    if(!trk) { 
      cout << "ERROR track " << itrk << " does not exist" << endl; 
      continue;
    }
    
//     if(trk->GetFlag() == -1 || trk->GetFlag() == -2 || trk->GetFlag() == -5) { 
//       //	  cout << "FLAG " << trk->GetFlag() << endl; 
//       continue; 
//     }

    if(trk->GetFlag() < 0) {
      //	  cout << "FLAG " << trk->GetFlag() << endl; 
      continue; 
    }

    PndTrackID *trkid = (PndTrackID*) fSecTrackIDArray->At(itrk);
    if(!trkid) { 
      cout << "ERROR trackID " << itrk << " does not exist" << endl; 
      continue;
    }

    if(trkid->GetNCorrTrackId() == 0) continue;

    PndTrackCand *cand = trk->GetTrackCandPtr();
    if(!cand) 
      { 
	cout << "ERROR track " << itrk << " has no candidate association" << endl; 
	continue;
      }

    // which TRACK ID is this?
    Int_t mcid = trkid->GetCorrTrackID();
    int nrightpoints = 0, nwrongpoints = 0;

    for (Int_t ihit = 0; ihit < cand->GetNHits(); ihit++) {
      PndTrackCandHit candhit = cand->GetSortedHit(ihit);
      
      Int_t hitId = candhit.GetHitId();
      Int_t detId = candhit.GetDetId();
      PndSttHit *hit = NULL;
      
      if(detId == FairRootManager::Instance()->GetBranchId("STTHit")) {
	hit = (PndSttHit*) fSttHitArray->At(hitId);
	if(!hit) cout << "ERROR stt " << hitId << " does not exist" << endl;
	int refidx = hit->GetRefIndex();
	if(refidx == -1) continue;
	PndSttPoint *pnt = (PndSttPoint*) fSttPointArray->At(refidx);
	// the track is associated to mcid track
	// if trackid == mcid GOOD 
	// else bad
	int trkID = pnt->GetTrackID();
	if(trkID == mcid) nrightpoints++;
	else nwrongpoints++;
      }
    }

 //    // COUNT IF MORE THAN 70% OF MC HITS HAVE BEEN ASSOCIATED, OTHERWISE DISCARD THE TRACK
//      if((((double) nrightpoints)/mcpointsintrack[mcid]) < 0.7) continue;




 //  //# @#@#@#@#@#@#@#@#@# @
  //  fDisplayOn = kTRUE;
  if(fDisplayOn) 
    {
      
  	char goOnChar;
	cout << "GO ON AND SEE THE RESULT" << endl;
	cin >> goOnChar;
	
	for (Int_t ihit = 0; ihit < cand->GetNHits(); ihit++) {
	  PndTrackCandHit candhit = cand->GetSortedHit(ihit);
      
	  Int_t hitId = candhit.GetHitId();
	  Int_t detId = candhit.GetDetId();
	  PndSttHit *hit = NULL;
      
	  if(detId == FairRootManager::Instance()->GetBranchId("STTHit")) {
	    hit = (PndSttHit*) fSttHitArray->At(hitId);
	    if(!hit) cout << "ERROR stt " << hitId << " does not exist" << endl;
	    int refidx = hit->GetRefIndex();
	    if(refidx == -1) continue;
	    PndSttPoint *pnt = (PndSttPoint*) fSttPointArray->At(refidx);
	    // the track is associated to mcid track
	    // if trackid == mcid GOOD 
	    // else bad
	    TArc *barc = new TArc(hit->GetX(), hit->GetY(), hit->GetIsochrone());
	    int trkID = pnt->GetTrackID();
	    if(trkID == mcid) barc->SetLineColor(3);
	    else barc->SetLineColor(2);
	    barc->SetFillStyle(0);

	    cout << hitId << endl;

	    barc->Draw("SAME");
	    display->Update();
	    display->Modified();  
	  }
	}



	Double_t xc, yc, radius, fitm, fitp;
	Int_t charge = trk->GetParamFirst().GetQ();
	trk->GetParamFirst().GetPosition().Print();
	trk->GetParamFirst().GetMomentum().Print();
	GetInitialParams(charge, trk->GetParamFirst().GetPosition(), trk->GetParamFirst().GetMomentum(), xc, yc, radius, fitm, fitp);
	TArc *arc = new TArc(xc, yc, radius);
	arc->SetFillStyle(0);
	arc->Draw("SAME");
	display->Update();
	display->Modified();
	cin >> goOnChar;
    }
  
  //# @#@#@#@#@#@#@#@#@# @



    std::vector<int> entry;
    entry.push_back(itrk);
    entry.push_back(nrightpoints);
    entry.push_back(nwrongpoints);

    if(mcid == mcpipid) piplist.push_back(entry);
    else if(mcid == mcpimid) pimlist.push_back(entry);
    else if(mcid == mcppid) pplist.push_back(entry);
    else if(mcid == mcpmid) pmlist.push_back(entry);
    else  otherslist.push_back(entry);
    
  }
  
  npip = piplist.size();
  npim = pimlist.size();
  npp = pplist.size();
  npm = pmlist.size();
  //  cout << npm << " " << npip << " " << npp << " " << npim << endl;
  // ------------ clear pip ----------------------------
  int tmpright = 0, tmpwrong = 1000, tmptrackid = -1;
  if(npip > 1) {
    for(int itrk = 0; itrk < npip; itrk++) {
      std::vector<int> entry = piplist[itrk];
      int trackid = entry[0];
      int nrightpoints = entry[1];
      int nwrongpoints = entry[2];
      
      if(nrightpoints > tmpright) {
	tmptrackid = trackid;
	tmpright = nrightpoints;
	tmpwrong = nwrongpoints;
      }
      else if(nrightpoints == tmpright) {
	if(nwrongpoints < tmpwrong) {
	  tmptrackid = trackid;
	  tmpright = nrightpoints;
	  tmpwrong = nwrongpoints;
	}
      }
    }
  
    piplist.clear();
    std::vector<int> newentry;
    newentry.push_back(tmptrackid);
    newentry.push_back(tmpright);
    newentry.push_back(tmpwrong);
    piplist.push_back(newentry);
    // cout << "final pip " << tmptrackid << endl;
    npip = piplist.size();
  }

  // ------------ clear pim ----------------------------
  tmpright = 0, tmpwrong = 1000, tmptrackid = -1;
  if(npim > 1) {
    for(int itrk = 0; itrk < npim; itrk++) {
      std::vector<int> entry = pimlist[itrk];
      int trackid = entry[0];
      int nrightpoints = entry[1];
      int nwrongpoints = entry[2];
      
      if(nrightpoints > tmpright) {
	tmptrackid = trackid;
	tmpright = nrightpoints;
	tmpwrong = nwrongpoints;
      }
      else if(nrightpoints == tmpright) {
	if(nwrongpoints < tmpwrong) {
	  tmptrackid = trackid;
	  tmpright = nrightpoints;
	  tmpwrong = nwrongpoints;
	}
      }
    }
    
    pimlist.clear();
    std::vector<int> newentry;
    newentry.push_back(tmptrackid);
    newentry.push_back(tmpright);
    newentry.push_back(tmpwrong);
    pimlist.push_back(newentry);
    // cout << "final pim " << tmptrackid << endl;
    npim = pimlist.size();
  }
  
  // ------------ clear pp ----------------------------
  tmpright = 0, tmpwrong = 1000, tmptrackid = -1;
  if(npp > 1) {
    for(int itrk = 0; itrk < npp; itrk++) {
      std::vector<int> entry = pplist[itrk];
      int trackid = entry[0];
      int nrightpoints = entry[1];
      int nwrongpoints = entry[2];
      
      if(nrightpoints > tmpright) {
	tmptrackid = trackid;
	tmpright = nrightpoints;
	tmpwrong = nwrongpoints;
      }
      else if(nrightpoints == tmpright) {
	if(nwrongpoints < tmpwrong) {
	  tmptrackid = trackid;
	  tmpright = nrightpoints;
	  tmpwrong = nwrongpoints;
	}
      }
    }
  
    pplist.clear();
    std::vector<int> newentry;
    newentry.push_back(tmptrackid);
    newentry.push_back(tmpright);
    newentry.push_back(tmpwrong);
    pplist.push_back(newentry);
    // cout << "final pp " << tmptrackid << endl;
    npp = pplist.size();
  }

  // ------------ clear pm ----------------------------
  tmpright = 0, tmpwrong = 1000, tmptrackid = -1;
  if(npm > 1) {
    for(int itrk = 0; itrk < npm; itrk++) {
      std::vector<int> entry = pmlist[itrk];
      int trackid = entry[0];
      int nrightpoints = entry[1];
      int nwrongpoints = entry[2];
      
      if(nrightpoints > tmpright) {
	tmptrackid = trackid;
	tmpright = nrightpoints;
	tmpwrong = nwrongpoints;
      }
      else if(nrightpoints == tmpright) {
	if(nwrongpoints < tmpwrong) {
	  tmptrackid = trackid;
	  tmpright = nrightpoints;
	  tmpwrong = nwrongpoints;
	}
      }
    }
 
    pmlist.clear();
    std::vector<int> newentry;
    newentry.push_back(tmptrackid);
    newentry.push_back(tmpright);
    newentry.push_back(tmpwrong);
    pmlist.push_back(newentry);
    // cout << "final pm " << tmptrackid << endl;
    npm = pmlist.size();
  }

  // ----------------------- lists cleared:
  // piplist, pimlist, pplist, pmlist have just one entry each at most, check it
  if(npip > 1 || npim > 1 || npp > 1 || npm > 1) cout << "ERROR a LIST has more than one entry!" << endl;

  //  cout << "reco track " << npm << " " << npip << " " << npp <<  " " << npim << endl;

  // LAMBDA ***********************************************
  if(npim == 1 && npp == 1) {

    std::vector<int> pimentry = pimlist[0];
    int pimtrackid = pimentry[0];
    
    std::vector<int> ppentry = pplist[0];
    int pptrackid = ppentry[0];
    //    cout << "pimid " << pimtrackid << " ppid " << pptrackid << endl;
    BuildInvariantMass(pimtrackid, pptrackid, lambdaVtx, 0);
  }
  // LAMBDA BARv***********************************************
  if(npip == 1 && npm == 1) {

    std::vector<int> pipentry = piplist[0];
    int piptrackid = pipentry[0];
    
    std::vector<int> pmentry = pmlist[0];
    int pmtrackid = pmentry[0];
    //   cout << "pipid " << piptrackid << " pmid " << pmtrackid << endl;
    
    BuildInvariantMass(piptrackid, pmtrackid, lambdabarVtx, 1);
  }


  
}



Bool_t PndLambdaIM::BuildInvariantMass(Int_t pitrkid, Int_t ptrkid, TVector3 lambdavertex, Int_t lamlambar) {



  // (ANTI)PION  
  TVector3 piPos(0, 0, 0), piMom(0, 0, 0);
  PndTrack *pitrk = (PndTrack*) fSecTrackArray->At(pitrkid);
  if(!pitrk) { 
    cout << "!!ERROR track " << pitrkid << " does not exist" << endl; 
   return kFALSE;
  }
 cout << "pi flag " << pitrk->GetFlag() << endl;
 
  Int_t piCharge = pitrk->GetParamFirst().GetQ();
  Int_t piCode = 211 * piCharge;
  // Calculating params at PCA to Origin
  FairTrackParP pipar = pitrk->GetParamFirst();
  Int_t ierr = 0;
  FairTrackParH *pihelix = new FairTrackParH(&pipar, ierr);
  FairTrackParH *fResPi= new FairTrackParH();
  fPro->SetPoint(lambdavertex);
  fPro->PropagateToPCA(1, -1);
 Bool_t rc =  fPro->Propagate(pihelix, fResPi, piCode);
  if (rc)
    {
      piPos.SetXYZ(fResPi->GetX(), fResPi->GetY(), fResPi->GetZ());
      piMom.SetXYZ(fResPi->GetPx(), fResPi->GetPy(), fResPi->GetPz());
    }
  else return kFALSE;


  // (ANTI)PROTON
  TVector3 pPos(0, 0, 0), pMom(0, 0, 0);
  PndTrack *ptrk = (PndTrack*) fSecTrackArray->At(ptrkid);
  if(!ptrk) { 
    cout << "!!ERROR track " << ptrkid << " does not exist" << endl; 
    return kFALSE;
  }

  cout << "p flag " << ptrk->GetFlag() << endl;

  Int_t pCharge = ptrk->GetParamFirst().GetQ();
  Int_t pCode = 2212 * pCharge;
  // Calculating params at PCA to Origin
  FairTrackParP ppar = ptrk->GetParamFirst();
  ierr = 0;
  FairTrackParH *phelix = new FairTrackParH(&ppar, ierr);
  FairTrackParH *fResP= new FairTrackParH();
  fPro->SetPoint(lambdavertex);
  fPro->PropagateToPCA(1, -1);
  rc =  fPro->Propagate(phelix, fResP, pCode);
  if (rc)
    {
      pPos.SetXYZ(fResP->GetX(), fResP->GetY(), fResP->GetZ());
      pMom.SetXYZ(fResP->GetPx(), fResP->GetPy(), fResP->GetPz());
    }
  else return kFALSE;


  // ----------------- to TLorentzVector
  Double_t pimass = 0.139570, pmass = 0.9382723;

  double epi = TMath::Sqrt(pimass * pimass + piMom.Mag() * piMom.Mag());
  double ep  = TMath::Sqrt(pmass * pmass + pMom.Mag() * pMom.Mag());
  double imass = TMath::Sqrt(pimass * pimass + pmass * pmass + 2 * (epi * ep - piMom.Dot(pMom)));

  cout << "pi " << piMom.Mag() << " " << piMom.Perp() << " " << piMom.Z() << endl;
   piMom.Print();
   cout << "p " << pMom.Mag() << " " << pMom.Perp() << " " << pMom.Z() << endl;
   pMom.Print();
   cout << "IM " << imass << endl;


  hIM->Fill(imass);
  hPiVertexDistanceXY->Fill((piPos - lambdavertex).Perp());
  hPiVertexDistanceZ->Fill((piPos - lambdavertex).Z());
  hPVertexDistanceXY->Fill((pPos - lambdavertex).Perp());
  hPVertexDistanceZ->Fill((pPos - lambdavertex).Z());

  // lamlambar: 0 lambda, 1 lambdabar

  // pion
  PndMCTrack *mcpi, *mcp;

  if(lamlambar == 0) {  // pim pp
    mcpi = (PndMCTrack*) fMCTrackArray->At(mcpimid);
    mcp =  (PndMCTrack*) fMCTrackArray->At(mcppid);
  }
  else  {  // pip pm
    mcpi = (PndMCTrack*) fMCTrackArray->At(mcpipid);
    mcp =  (PndMCTrack*) fMCTrackArray->At(mcpmid);
  }
  
  TVector3 mcpimom = mcpi->GetMomentum();
  TVector3 mcpmom = mcp->GetMomentum();

  cout << "MCpi " << mcpimom.Mag() << " " << mcpimom.Perp() << " " << mcpimom.Z() << endl;
   mcpimom.Print();
   cout << "MCp " << mcpmom.Mag() << " " << mcpmom.Perp() << " " << mcpmom.Z() << endl;
   mcpmom.Print();


  hpionmomentum->Fill(piMom.Mag());
  hprotmomentum->Fill(pMom.Mag());
//   hMCpionmomentum->Fill(mcpimom.Mag());
//   hMCprotmomentum->Fill(mcpmom.Mag());

  if(mcpimom.Mag() < 0.45) {
    hPideltapoverp1->Fill((piMom.Mag() - mcpimom.Mag())/mcpimom.Mag());
    hPideltaptoverpt1->Fill((piMom.Perp() - mcpimom.Perp())/mcpimom.Perp());
    hPideltaploverpl1->Fill((piMom.Z() - mcpimom.Z())/mcpimom.Z());
  }
  else {
    hPideltapoverp2->Fill((piMom.Mag() - mcpimom.Mag())/mcpimom.Mag());
    hPideltaptoverpt2->Fill((piMom.Perp() - mcpimom.Perp())/mcpimom.Perp());
    hPideltaploverpl2->Fill((piMom.Z() - mcpimom.Z())/mcpimom.Z());
  }

  if(mcpmom.Mag() < 1.75) {
    hPdeltapoverp1->Fill((pMom.Mag() - mcpmom.Mag())/mcpmom.Mag());
    hPdeltaptoverpt1->Fill((pMom.Perp() - mcpmom.Perp())/mcpmom.Perp());
    hPdeltaploverpl1->Fill((pMom.Z() - mcpmom.Z())/mcpmom.Z());
  }
  else {
    hPdeltapoverp2->Fill((pMom.Mag() - mcpmom.Mag())/mcpmom.Mag());
    hPdeltaptoverpt2->Fill((pMom.Perp() - mcpmom.Perp())/mcpmom.Perp());
    hPdeltaploverpl2->Fill((pMom.Z() - mcpmom.Z())/mcpmom.Z());
  }
  


  if(fabs((piMom.Mag() - mcpimom.Mag())/mcpimom.Mag()) < 0.15 && fabs((pMom.Mag() - mcpmom.Mag())/mcpmom.Mag()) < 0.15) hIMpeak->Fill(imass);


  return kTRUE;
}




void PndLambdaIM::WriteHistograms(){
  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("Lambda");
  file->cd("Lambda");
  
  hIM->Write();
  delete hIM;
  hIMpeak->Write();
  delete hIMpeak;
  hPiVertexDistanceXY->Write();
  delete hPiVertexDistanceXY;
  hPiVertexDistanceZ->Write();
  delete hPiVertexDistanceZ;
  hPVertexDistanceXY->Write();
  delete hPVertexDistanceXY;
  hPVertexDistanceZ->Write();
  delete hPVertexDistanceZ;

  hPdeltapoverp1->Write();
  delete hPdeltapoverp1;
  hPdeltaptoverpt1->Write();
  delete hPdeltaptoverpt1;
  hPdeltaploverpl1->Write();
  delete hPdeltaploverpl1;
  hPdeltapoverp2->Write();
  delete hPdeltapoverp2;
  hPdeltaptoverpt2->Write();
  delete hPdeltaptoverpt2;
  hPdeltaploverpl2->Write();
  delete hPdeltaploverpl2;

  hPideltapoverp1->Write();
  delete hPideltapoverp1;
  hPideltaptoverpt1->Write();
  delete hPideltaptoverpt1;
  hPideltaploverpl1->Write();
  delete hPideltaploverpl1;
  hPideltapoverp2->Write();
  delete hPideltapoverp2;
  hPideltaptoverpt2->Write();
  delete hPideltaptoverpt2;
  hPideltaploverpl2->Write();
  delete hPideltaploverpl2;

  hpionmomentum->Write();
  delete hpionmomentum;
  hprotmomentum->Write();
  delete hprotmomentum;
  hMCpionmomentum->Write();
  delete hMCpionmomentum;
  hMCprotmomentum->Write();
  delete hMCprotmomentum;
 

 
}

void PndLambdaIM::GetInitialParams(Int_t charge, TVector3 pos, TVector3 mom, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &fitm, Double_t &fitp)
{

  radius = mom.Perp()/0.006;
  Double_t beta;
  
  if(fabs(mom.X()) >  1e-10) {
    // track from tangent ---------------------
    //     double _m1 = mom.Y() / mom.X();
    //     double _q1 = pos.Y() - pos.X() * _m1;
    //     double _m2 = -1./_m1;
    //     double _q2 = pos.Y() - pos.X() * _m2;
    beta = TMath::ATan2(mom.X(), mom.Y());
  }
  else beta = TMath::Sign(1., mom.Y()) * TMath::Pi(); 
  double X0, Y0;
  if(charge > 0) { 
    xc = pos.X() + radius * TMath::Cos(beta);
    yc = pos.Y() - radius * TMath::Sin(beta);
  }
  else {
    xc = pos.X() - radius * TMath::Cos(beta);
    yc = pos.Y() + radius * TMath::Sin(beta);
  }

  fitm = 0; // CHECK
  fitp = 0; // CHECK 
}

void PndLambdaIM::DrawHits() {
  
    int nhits = fSttHitArray->GetEntriesFast();

    for(int ihit = 0; ihit < nhits; ihit++) {
      PndSttHit *hit = (PndSttHit*) fSttHitArray->At(ihit);
      if(!hit) continue; 
      
      TVector3 position;
      hit->Position(position);
      TMarker *mrk = new TMarker(position.X(), position.Y(), 3);

      Int_t tubeID = hit->GetTubeID();
      PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
      
      TVector3 wireDirection = tube->GetWireDirection();
      
      if(wireDirection != TVector3(0., 0., 1.))  {
	mrk->Draw("SAME");
	}
	else {

	  TArc *arc = new TArc(position.X(), position.Y(), tube->GetRadIn()); // hit->GetIsochrone());
	  arc->SetFillStyle(0);
	  arc->Draw("SAME");

	}
      
    }
  display->Update();
  display->Modified();  
}



void PndLambdaIM::DrawGeometry() {

  h2 = new TH2F("h2", "", 100, -43, 43, 100, -43, 43);
  display->cd();
  h2->Draw();
  display->Update();
  display->Modified();  
 
}






ClassImp(PndLambdaIM)
