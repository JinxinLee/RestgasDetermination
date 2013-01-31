#include "PndFtsTrackerIdeal.h"

//#include "PndDetectorList.h"
#include "PndMCTrack.h"
#include "PndTrackCand.h"
#include "PndTrack.h"

#include "FairTrackParP.h"
#include "FairMCPoint.h"
#include "FairHit.h"
//#include "FairMCApplication.h"
#include "FairTask.h"
//#include "FairRunAna.h"
//#include "FairGeoNode.h"
//#include "FairGeoVector.h"
//#include "FairGeoMedium.h"
#include "FairRootManager.h"

#include "TObjectTable.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TRandom3.h"
#include <iostream>


//________________________________________________________________
PndFtsTrackerIdeal::PndFtsTrackerIdeal():
  FairTask("FTSTrackfinderIdeal"), fMCTracks(new TClonesArray()), fTrackCands(new TClonesArray()), fTracks(new TClonesArray()), 
  fTrackIds(new TClonesArray()), fMomSigma(0,0,0), fDPoP(0.), fRelative (kFALSE), fVtxSigma(0,0,0), fEfficiency(1.), 
  fTracksArrayName(""), pdg(0)  
{
  //---
  fTrackCands = new TClonesArray("PndTrackCand");
  fTracks = new TClonesArray("PndTrack");
  fVerbose = 0;
  fMomSigma.SetXYZ(0.,0.,0.);
  fVtxSigma.SetXYZ(0.,0.,0.);
  SetTrackOutput();
  for (int i=0;i<4;i++) fBranchActive[i]=kTRUE;
}

//_________________________________________________________________
PndFtsTrackerIdeal::~PndFtsTrackerIdeal() 
{  
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//_________________________________________________________________
void PndFtsTrackerIdeal::Register() 
{
  //---
  FairRootManager::Instance()->Register(fTracksArrayName,"FTSTrk", fTracks, kTRUE);
  FairRootManager::Instance()->Register(fTracksArrayName+"Cand","FTSTrk", fTrackCands, kTRUE);
  if(fVerbose>3) Info("Register","Done.");
}

//________________________________________________________________
InitStatus PndFtsTrackerIdeal::Init() {
  // ---
  if(fVerbose>3) Info("Init","Start initialisation.");
  
  FairRootManager *fManager = FairRootManager::Instance();
  
  // Get MC arrays
  fMCTracks = (TClonesArray*)fManager->GetObject("MCTrack");
  if ( ! fMCTracks ) {
    std::cout << "-W-  PndFtsTrackerIdeal::Init: No MCTrack array! Needed for MC Truth" << std::endl;
    return kERROR;
  }
  
  //FTS
  fMCPoints[0] = (TClonesArray*)fManager->GetObject("FTSPoint");
  if ( ! fMCPoints[0] ) {
    std::cout << "-W-  PndFtsTrackerIdeal::Init: No FTSPoint array!" << std::endl;
    return kERROR;
  }
  fHits[0] = (TClonesArray *)fManager->GetObject("FTSHit");
  if ( ! fHits[0] ) {
    std::cout << "-W-  PndFtsTrackerIdeal::Init: No FTSHit array!" << std::endl;
    return kERROR;
  }
  fBranchIDs[0] = 	FairRootManager::Instance()->GetBranchId("FTSHit");
  
  
  //GEM
  fMCPoints[1] = (TClonesArray*)fManager->GetObject("GEMPoint");
  if ( ! fMCPoints[1] ) {
    std::cout << "-W-  PndFtsTrackerIdeal::Init: No GEMPoint array!" << std::endl;
    fMCPoints[1]=new TClonesArray("FairMCPoint");
  }
  fHits[1] = (TClonesArray *)fManager->GetObject("GEMHit");
  if ( ! fHits[1] ) {
    std::cout << "-W-  PndFtsTrackerIdeal::Init: No GEMHit array!" << std::endl;
    fHits[1]=new TClonesArray("FairHit");
  }
  fBranchIDs[1] = 	FairRootManager::Instance()->GetBranchId("GEMHit");
  
  //MVD Pixel
  fMCPoints[2] = (TClonesArray*)fManager->GetObject("MVDPoint");
  if ( ! fMCPoints[2] ) {
    std::cout << "-W-  PndFtsTrackerIdeal::Init: No MVDPoint array!" << std::endl;
    fMCPoints[1]=new TClonesArray("FairMCPoint");
  }
  fHits[2] = (TClonesArray *)fManager->GetObject("MVDHitsPixel");
  if ( ! fHits[2] ) {
    std::cout << "-W-  PndFtsTrackerIdeal::Init: No MVDHitsPixel array!" << std::endl;
    fHits[2]=new TClonesArray("FairHit");
  }
  fBranchIDs[2] = 	FairRootManager::Instance()->GetBranchId("MVDHitsPixel");
  
  fMCPoints[3] = fMCPoints[2];
  fHits[3] = (TClonesArray *)fManager->GetObject("MVDHitsStrip");
  if ( ! fHits[3] ) {
    std::cout << "-W-  PndFtsTrackerIdeal::Init: No MVDHitsStrip array!" << std::endl;
    fMCPoints[3]=new TClonesArray("FairHit");
  }
  fBranchIDs[3] = 	FairRootManager::Instance()->GetBranchId("MVDHitsStrip");
  
  if(fVerbose>3) Info("Init","Fetched all arrays.");
  
  Register();
  
  pdg = new TDatabasePDG();
  if(fVerbose>3) Info("Init","End initialisation.");
  
  return kSUCCESS;
}

//_________________________________________________________________
void PndFtsTrackerIdeal::Exec(Option_t * option) 
{
  Reset();
  if(fVerbose>3) Info("Exec","Start eventloop.");
  if(fVerbose>4){
    Info("Exec","Print some array properties");
    for(int iii=0;iii<4;iii++){
      std::cout<<"fHits["<<iii<<"] is branchID "<<fBranchIDs[iii]<<" with the name "<<fHits[iii]->GetName()<<" and contains "<<fHits[iii]->GetEntriesFast()<<" entries."<<std::endl;
      std::cout<<"fMCPoints["<<iii<<"] with the name "<<fMCPoints[iii]->GetName()<<" and contains "<<fMCPoints[iii]->GetEntriesFast()<<" entries."<<std::endl;
    }
  }
  if(fHits[0]->GetEntriesFast() == 0) {
    if(fVerbose>3) Info("Exec","Skip the event, since we have no hits in FTS");
    return;
  }
  FairHit* ghit = NULL;
  std::map<Int_t, FairHit*> firstHit;
  std::map<Int_t, FairHit*> lastHit;
  FairMCPoint* myPoint=NULL;
  std::map<Int_t, FairMCPoint*> firstPoint;
  std::map<Int_t, FairMCPoint*> lastPoint;
  std::map<Int_t, PndTrackCand*> candlist;
  for(Int_t iDet=0;iDet<4;iDet++){
    if (kFALSE == fBranchActive[iDet]) continue; //skip manually switched off detector
    if(fVerbose>4) Info("Exec","Use detector %i",iDet);
    for (Int_t ih = 0; ih < fHits[iDet]->GetEntriesFast(); ih++) {
      ghit = (FairHit*) fHits[iDet]->At(ih);
      if(!ghit) {
        if(fVerbose>3) Error("Exec","Have no ghit %i, array size: %i",ih,fHits[iDet]->GetEntriesFast());
        continue;
      }
      Int_t mchitid=ghit->GetRefIndex();
      if(mchitid<0) {
        if(fVerbose>3) Error("Exec","Have a mcHit %i",mchitid);
        continue;
      }
      myPoint = (FairMCPoint*)(fMCPoints[iDet]->At(mchitid));
      if(!myPoint) continue;
      Int_t trackID = myPoint->GetTrackID();
      if(trackID<0) continue;
      
      if(fVerbose>5) Info("Exec","Have a Hit %i at Track index %i",ih,trackID);
      
      PndTrackCand* cand=candlist[trackID];
      if(NULL==cand){
        if(0!=iDet){
          if(fVerbose>5) Info("Exec","Skip Hit %i, it's not connected to a Track in FTS",ih);
          continue; // skip Tracks in MVD/GEM not going to FTS
        }
        if(fVerbose>5) Info("Exec","Create new PndTrack object %i",trackID);
        cand=new PndTrackCand();
        cand->setMcTrackId(trackID);
        if(fVerbose>5) Info("Exec","Create new PndTrack object finished %i",trackID);
      }
      if(fVerbose>5) Info("Exec","add the hit %i to trackcand %i",ih,trackID);
      cand->AddHit(fBranchIDs[iDet],ih,ghit->GetZ());
      if(!firstHit[trackID] || firstHit[trackID]->GetZ() > ghit->GetZ()) {
        firstHit[trackID]=ghit;
        firstPoint[trackID]=myPoint;
      }
      if(!lastHit[trackID] || lastHit[trackID]->GetZ() < ghit->GetZ()) {
        lastHit[trackID]=ghit;
        lastPoint[trackID]=myPoint;
      }
      
      candlist[trackID] = cand; // set
    }
  }
  if(fVerbose>3) Info("Exec","Insert to TCA (depending on efficiency)");
  
  // re-iterate and select by efficiency & number of hits
  std::map<Int_t, PndTrackCand*>::iterator  candit;
  //FairTrackParP* firstPar=NULL;
  //FairTrackParP* lastPar=NULL;
  PndMCTrack *mc=NULL;
  TVector3 svtx, smom;
  Int_t charge=0, trackID=-1;
  
  for(candit=candlist.begin(); candit!=candlist.end(); ++candit) {
    PndTrackCand* tcand=candit->second; 
    trackID=candit->first;
    if(!tcand) {
      if(fVerbose>3) Warning("Exec","Have no candidate at %i",trackID);
      continue;
    }
    if( tcand->GetNHits() < 3 ) continue;
    if(0 < fEfficiency && fEfficiency < 1){
      if(gRandom->Rndm() > fEfficiency) continue;
    }
    tcand->Sort();
    mc = (PndMCTrack*)fMCTracks->At(trackID);
    if (mc->GetPdgCode()<100000000) charge = (Int_t)TMath::Sign(1.0, ((TParticlePDG*)pdg->GetParticle(mc->GetPdgCode()))->Charge());
    else charge = 1;
    tcand->setMcTrackId(trackID);
    // prepare track parameters
    firstPoint[trackID]->Position(svtx); // set position to first hit
    SmearFWD(svtx, fVtxSigma);
    firstPoint[trackID]->Momentum(smom);
    if (fRelative) fMomSigma=fDPoP*smom;
    SmearFWD(smom, fMomSigma);
    FairTrackParP* firstPar=new FairTrackParP(svtx, smom,
                                              fVtxSigma, fMomSigma,
                                              charge, svtx,
                                              TVector3(1.,0.,0.), TVector3(0.,1.,0.));					 
        
    lastPoint[trackID]->Position(svtx);
    SmearFWD(svtx, fVtxSigma);
    lastPoint[trackID]->Momentum(smom);
    SmearFWD(smom, fMomSigma);
    FairTrackParP* lastPar=new FairTrackParP(svtx, smom,
                                             fVtxSigma, fMomSigma,
                                             charge, svtx,
                                             TVector3(1.,0.,0.), TVector3(0.,1.,0.));						 
    
    if(fVerbose>3) Info("Exec","Store candidate at %i",trackID);
    // Creates a new hit in the TClonesArray.
    if(fVerbose>3) Info("AddTrack","Adding a Track.");
    TClonesArray &pndtracks = *fTracks;
    TClonesArray &pndtrackcands = *fTrackCands;
    //  TClonesArray &pndtrackids = *fTrackIds;
    Int_t size = pndtrackcands.GetEntriesFast();
    if(pndtracks.GetEntriesFast() != size) {
      Error("Exec","Arrays out of synchronisation: %i tracks, %i cands. Abort event."
            ,pndtracks.GetEntriesFast(),pndtrackcands.GetEntriesFast());
      return;
    }
    
    PndTrackCand* pndTrackCand = new(pndtrackcands[size]) PndTrackCand(*tcand);
    PndTrack* pndTrack = new(pndtracks[size]) PndTrack(*firstPar, *lastPar, *tcand,0,0,1,mc->GetPdgCode(),trackID,FairRootManager::Instance()->GetBranchId("MCTrack"));
  }
  
  if(fVerbose>3) Info("Exec","End eventloop.");
}

//_________________________________________________________________
void PndFtsTrackerIdeal::Finish() 
{
  std::cout << " Found  "<< fTracks->GetEntriesFast() << " tracks\n";
}

//________________________________________________________________
void PndFtsTrackerIdeal::Reset() {
  //---
  if (fTracks->GetEntriesFast() != 0)  fTracks->Clear();
  if (fTrackCands->GetEntriesFast() != 0)  fTrackCands->Clear();
}

//_________________________________________________________________
void PndFtsTrackerIdeal::SmearFWD(TVector3 &vec, const TVector3 &sigma)
{
  // gaussian smearing
  Double_t rannn=0.;
  rannn = gRandom->Gaus(vec.X(),sigma.X());
  vec.SetX(rannn);
  
  rannn = gRandom->Gaus(vec.Y(),sigma.Y());
  vec.SetY(rannn);
  
  rannn = gRandom->Gaus(vec.Z(),sigma.Z());
  vec.SetZ(rannn);
  
  return;
}

ClassImp(PndFtsTrackerIdeal);

