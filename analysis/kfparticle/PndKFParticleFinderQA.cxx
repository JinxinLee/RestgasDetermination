//-----------------------------------------------------------
//-----------------------------------------------------------

// Panda Headers ----------------------
#include "PndKFParticleFinderQA.h"
#include "PndTrack.h"
#include "PndMCTrack.h"
#include "PndPidCandidate.h"
#include "PndEmcBump.h"

#include "FairRunAna.h"

//KF Particle headers
#include "KFParticleTopoReconstructor.h"
#include "KFTopoPerformance.h"
#include "KFMCTrack.h"
#include "KFParticleMatch.h"

//ROOT headers
#include "TClonesArray.h"
#include "TFile.h"
#include "TObject.h"
#include "TMath.h"
#include "TDatabasePDG.h"

//c++ and std headers
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
using std::vector;

PndKFParticleFinderQA::PndKFParticleFinderQA(const char* name, Int_t iVerbose, KFParticleTopoReconstructor* tr, TString outFileName):
  FairTask(name, iVerbose), fMCTracksBranchName(""), fChargedTrackBranchName(""), fNeutralTrackBranchName(""),
  fMCTrackArray(0), fChargedTrackArray(0), fNeutralTrackArray(0), fEmcBumps(0),
  fRecParticles(0), fMCParticles(0), fMatchParticles(0), fSaveParticles(0), fSaveMCParticles(0),
  fOutFileName(outFileName), fOutFile(0), fEfffileName("Efficiency.txt"), fTopoPerformance(0), fPrintFrequency(100), fNEvents(0)
{
  for(Int_t i=0; i<5; i++)
    fTime[i] = 0;
  
  fTopoPerformance = new KFTopoPerformance;
  fTopoPerformance->SetTopoReconstructor(tr);

  TFile* curFile = gFile;
  TDirectory* curDirectory = gDirectory;
  
  if(!(fOutFileName == ""))
    fOutFile = new TFile(fOutFileName.Data(),"RECREATE");
  else
    fOutFile = gFile;
  fTopoPerformance->CreateHistos("KFTopoReconstructor",fOutFile);
  
  gFile = curFile;
  gDirectory = curDirectory;
  
  //register MC particles from the KFPartEfficiency in the TDatabasePDG
  KFPartEfficiencies eff;
  int index[4] = {48, 49, 50, 63};
  {
    for(int iPall=0; iPall<4; iPall++)
    {
      int iParticle = index[iPall];
      Double_t lifetime = eff.partLifeTime[iParticle]; // lifetime
      Double_t mass = eff.partMass[iParticle];
      Int_t PDG = eff.partPDG[iParticle];
      Double_t charge = eff.partCharge[iParticle];
      Double_t width = 6.58211928e-22/lifetime;
      
      TDatabasePDG::Instance()->AddParticle(eff.partTitle[iParticle], eff.partTitle[iParticle], 
                                            mass, kFALSE, width, charge, "hadron", PDG);
    }
  }
}

PndKFParticleFinderQA::~PndKFParticleFinderQA()
{
  if(fTopoPerformance) delete fTopoPerformance;  
  
  if(fSaveParticles)
    fRecParticles->Delete();
  if(fSaveMCParticles)
  {
    fMCParticles->Delete();
    fMatchParticles->Delete();
  }
}

InitStatus PndKFParticleFinderQA::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
  {
    Error("PndKFParticleFinderQA::Init","RootManager not instantiated!");
    return kERROR;
  }
  
  //MC Tracks
  fMCTrackArray=(TClonesArray*) ioman->GetObject(fMCTracksBranchName);
  if(fMCTrackArray==0)
  {
    Error("StandaloneHitGenerator::Init","mc track array not found!");
    return kERROR;
  }
  
  //Charged Tracks
  fChargedTrackArray=(TClonesArray*) ioman->GetObject(fChargedTrackBranchName);
  if(fChargedTrackArray==0)
  {
    Error("StandaloneHitGenerator::Init","Charged tracks array not found!");
    return kERROR;
  }
  
  //Neutral Tracks
  fNeutralTrackArray=(TClonesArray*) ioman->GetObject(fNeutralTrackBranchName);
  if(fNeutralTrackArray==0)
  {
    Error("StandaloneHitGenerator::Init","Neutral tracks match array not found! Continue without gammas.");
  }
  fEmcBumps = (TClonesArray*) ioman->GetObject("EmcBump");

  if(fSaveParticles)
  {
    // create and register TClonesArray with output reco particles
    fRecParticles = new TClonesArray("KFParticle",100);
    ioman->Register("RecoParticles", "KFParticle", fRecParticles, kTRUE);
  }

  if(fSaveMCParticles)
  {
    // create and register TClonesArray with output MC particles
    fMCParticles = new TClonesArray("KFMCParticle",100);
    ioman->Register("KFMCParticles", "KFParticle", fMCParticles, kTRUE);

    // create and register TClonesArray with matching between reco and MC particles
    fMatchParticles = new TClonesArray("KFParticleMatch",100);
    ioman->Register("KFParticleMatch", "KFParticle", fMatchParticles, kTRUE);
  }
  return kSUCCESS;
}

void PndKFParticleFinderQA::Exec(Option_t*)
{ 
  if(fSaveParticles)
    fRecParticles->Delete();
  if(fSaveMCParticles)
  {
    fMCParticles->Delete();
    fMatchParticles->Delete();
  }
  
  static int nEvents = 0;
  nEvents++;
  
  Int_t nMCTracks = fMCTrackArray->GetEntriesFast();
  vector<KFMCTrack> mcTracks(nMCTracks);
  vector< vector<int> > mcDaughters(nMCTracks);
  for(Int_t iMC=0; iMC<nMCTracks; iMC++)
  {
    PndMCTrack *pndMCTrack = (PndMCTrack*)fMCTrackArray->At(iMC);

    mcTracks[iMC].SetX ( pndMCTrack->GetStartVertex().X() );
    mcTracks[iMC].SetY ( pndMCTrack->GetStartVertex().Y() );
    mcTracks[iMC].SetZ ( pndMCTrack->GetStartVertex().Z() );
    mcTracks[iMC].SetPx( pndMCTrack->GetMomentum().X() );
    mcTracks[iMC].SetPy( pndMCTrack->GetMomentum().Y() );
    mcTracks[iMC].SetPz( pndMCTrack->GetMomentum().Z() );
    
    Int_t pdg = pndMCTrack->GetPdgCode();
    Double_t q=1;
    if ( pdg < 20000 )
      q = TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.0;
    else    q = 0;    
    Double_t p = pndMCTrack->GetMomentum().Mag();
    
    mcTracks[iMC].SetMotherId(pndMCTrack->GetMotherID());
    mcTracks[iMC].SetQP(q/p);
    mcTracks[iMC].SetPDG(pdg);
    mcTracks[iMC].SetNMCPoints(0);
    
    if( (mcTracks[iMC].R() > 10) && (pdg == 22) )
      mcTracks[iMC].SetOutOfDetector();
    
    if(pndMCTrack->GetMotherID() > -1)
      mcDaughters[pndMCTrack->GetMotherID()].push_back(iMC);
  }
  
//   for(Int_t iMC=0; iMC<nMCTracks; iMC++)
//   {
//     PndMCTrack *pndMCTrack = (PndMCTrack*)fMCTrackArray->At(iMC);
//     Int_t pdg = pndMCTrack->GetPdgCode();
// 
//     if(  (mcTracks[iMC].R() < 10) && (pdg == 22) )
//     {
//       PndMCTrack *pndMCTrack1 = (PndMCTrack*)fMCTrackArray->At(mcDaughters[iMC][0]);
//       std::cout << "imc " << iMC << " r " << pndMCTrack1->GetStartVertex().X() << " " << pndMCTrack1->GetStartVertex().Y() << " " << pndMCTrack1->GetStartVertex().Z() << " pdg " << pdg <<std::endl;
//     }
//   }
  
  Int_t ntrackMatches=fChargedTrackArray->GetEntriesFast();
  if(fNeutralTrackArray)
    ntrackMatches += fNeutralTrackArray->GetEntriesFast();
  
  vector<int> trackMatch(ntrackMatches, -1);
static int Nghosts = 0;
  for(int iTr=0; iTr<ntrackMatches; iTr++)
  {
    PndPidCandidate *inTrack;
    if(iTr<fChargedTrackArray->GetEntriesFast())
      inTrack = (PndPidCandidate*)fChargedTrackArray->At(iTr);
    else
      inTrack = (PndPidCandidate*)fNeutralTrackArray->At(iTr - fChargedTrackArray->GetEntriesFast());
    if(!inTrack) continue;
    Int_t trId = inTrack->GetMcIndex();
    
    if(trId >= 0)
    {
      PndMCTrack *pndMCTrack = (PndMCTrack*)fMCTrackArray->At(trId);
      if(iTr<fChargedTrackArray->GetEntriesFast() && TMath::Abs(pndMCTrack->GetPdgCode()) > 2500)
        continue;
      //Correct linking between EMC clusters and MC tracks
      float rMC[3] = { static_cast<float>(pndMCTrack->GetStartVertex().X()), 
                       static_cast<float>(pndMCTrack->GetStartVertex().Y()), 
                       static_cast<float>(pndMCTrack->GetStartVertex().Z()) };
      float R = TMath::Sqrt(rMC[0]*rMC[0] + rMC[1]*rMC[1] + rMC[2]*rMC[2]);
  
      //if cluster points to some mother or grandmother of the gamma
      if(TMath::Abs(pndMCTrack->GetPdgCode()) != 22 && iTr>=fChargedTrackArray->GetEntriesFast())
      {        
        float drMin = 1.e9f;
        int closestTrack = -1;
        int iMother = -1;
        
        int iBump = inTrack->GetEmcIndex();
        PndEmcBump *inBump = (PndEmcBump*) fEmcBumps->At(iBump);
        float rReco[3] = { static_cast<float>(inBump->x()), 
                           static_cast<float>(inBump->y()), 
                           static_cast<float>(inBump->z()) };
        FindClosestMCTrackToBump(trId, closestTrack, drMin, rReco, mcDaughters);
        FindEmcClusterMother(closestTrack, iMother);
        
        if(iMother < 0)
          trId = -1;
        else
        {
          PndMCTrack* mother = (PndMCTrack*)fMCTrackArray->At( iMother );
          if(mother->GetPdgCode() == 22)
            trId = iMother;
          else
            trId = -1;
        }
      }
    }
    
    if(trId < 0) continue;
    mcTracks[trId].SetReconstructed();
    trackMatch[iTr] = trId;
  }
  
  fTopoPerformance->SetMCTracks(mcTracks);
  fTopoPerformance->SetTrackMatch(trackMatch);
  
  fTopoPerformance->CheckMCTracks();
  fTopoPerformance->MatchTracks();
  fTopoPerformance->FillHistos();
  
  fNEvents++;
  fTime[4] += fTopoPerformance->GetTopoReconstructor()->Time();
  for(int iT=0; iT<4; iT++)
    fTime[iT] += fTopoPerformance->GetTopoReconstructor()->StatTime(iT);
  if(fNEvents%fPrintFrequency == 0)
  {
    std::cout << "Topo reconstruction time"
      << " Real = " << std::setw( 10 ) << fTime[4]/fNEvents * 1.e3 << " ms" << std::endl;
    std::cout << "    Init                " << fTime[0]/fNEvents * 1.e3 << " ms" << std::endl;
    std::cout << "    PV Finder           " << fTime[1]/fNEvents * 1.e3 << " ms" << std::endl;
    std::cout << "    Sort Tracks         " << fTime[2]/fNEvents * 1.e3 << " ms" << std::endl;
    std::cout << "    KF Particle Finder  " << fTime[3]/fNEvents * 1.e3 << " ms" << std::endl;
  }
  
  // save particles to a ROOT file
  if(fSaveParticles)
  {
    for(unsigned int iP=0; iP < fTopoPerformance->GetTopoReconstructor()->GetParticles().size(); iP++)
    {
      new((*fRecParticles)[iP]) KFParticle(fTopoPerformance->GetTopoReconstructor()->GetParticles()[iP]);
    }
  }

  if(fSaveMCParticles)
  {
    for(unsigned int iP=0; iP < fTopoPerformance->GetTopoReconstructor()->GetParticles().size(); iP++)
    {
      new((*fMatchParticles)[iP]) KFParticleMatch();
      KFParticleMatch *p = (KFParticleMatch*)( fMatchParticles->At(iP) );

      Short_t matchType = 0;
      int iMCPart = -1;
      if(!(fTopoPerformance->ParticlesMatch()[iP].IsMatchedWithPdg())) //background
      {
        if(fTopoPerformance->ParticlesMatch()[iP].IsMatched())
        {
          iMCPart = fTopoPerformance->ParticlesMatch()[iP].GetBestMatchWithPdg();
          matchType = 1;
        }
      }
      else
      {
        iMCPart = fTopoPerformance->ParticlesMatch()[iP].GetBestMatchWithPdg();
        matchType = 2;
      }

      p->SetMatch(iMCPart);
      p->SetMatchType(matchType);
    }

    for(unsigned int iP=0; iP < fTopoPerformance->MCParticles().size(); iP++)
    {
      new((*fMCParticles)[iP]) KFMCParticle(fTopoPerformance->MCParticles()[iP]);
    }
  }
}

void PndKFParticleFinderQA::Finish()
{
  TDirectory *curr = gDirectory;
  TFile *currentFile = gFile;
  // Open output file and write histograms

  fOutFile->cd();
  WriteHistosCurFile(fTopoPerformance->GetHistosDirectory());
//         if (histodirmod!=NULL) WriteHistos(histodirmod);
        //WriteHistos(gDirectory);
  if(!(fOutFileName == ""))
  {   
    fOutFile->Close();
    fOutFile->Delete();
  }
  gFile = currentFile;
  gDirectory = curr;

  std::fstream eff(fEfffileName.Data(),fstream::out);
  eff << fTopoPerformance->fParteff;
  eff.close();
}

void PndKFParticleFinderQA::FindClosestMCTrackToBump(const int trackId, int& closestTrack, float& drMin, const float* rReco, 
                                                     const vector< vector<int> >& mcDaughters)
{
  if( (trackId<0) || (trackId>=mcDaughters.size()) ) return;

  for(int iDaughter=0; iDaughter<mcDaughters[trackId].size(); iDaughter++)
  {
    int daughterIndex = mcDaughters[trackId][iDaughter];
    PndMCTrack* daughter = (PndMCTrack*)fMCTrackArray->At( daughterIndex );
    float rMC[3] = { static_cast<float>(daughter->GetStartVertex().X()), 
                     static_cast<float>(daughter->GetStartVertex().Y()), 
                     static_cast<float>(daughter->GetStartVertex().Z()) };
    
    float dx = rReco[0] - rMC[0];
    float dy = rReco[1] - rMC[1];
    float dz = rReco[2] - rMC[2];
    float dr = sqrt(dx*dx + dy*dy + dz*dz);
    if(dr < drMin)
    {
      drMin = dr;
      closestTrack = daughterIndex;
    }

    if(mcDaughters[daughterIndex].size() > 0)
      FindClosestMCTrackToBump( daughterIndex, closestTrack, drMin, rReco, mcDaughters);
  }
}

void PndKFParticleFinderQA::FindEmcClusterMother(const int iDaughter, int& iMother)
{
  PndMCTrack* daughter = (PndMCTrack*)fMCTrackArray->At( iDaughter );
  float rMC[3] = { static_cast<float>(daughter->GetStartVertex().X()), 
                   static_cast<float>(daughter->GetStartVertex().Y()), 
                   static_cast<float>(daughter->GetStartVertex().Z()) };
  float R = TMath::Sqrt(rMC[0]*rMC[0] + rMC[1]*rMC[1] + rMC[2]*rMC[2]);
  
//   while( !(R<80. && daughter->GetPdgCode() == 22) )
  while( R > 10. )
  {
    iMother = daughter->GetMotherID();
    if(iMother < 0) break;
    
    daughter = (PndMCTrack*)fMCTrackArray->At( iMother );
    rMC[0] = daughter->GetStartVertex().X();
    rMC[1] = daughter->GetStartVertex().Y();
    rMC[2] = daughter->GetStartVertex().Z();
    
    R = TMath::Sqrt(rMC[0]*rMC[0] + rMC[1]*rMC[1] + rMC[2]*rMC[2]);
  }
}

void PndKFParticleFinderQA::WriteHistosCurFile( TObject *obj ){
  if( !obj->IsFolder() ) obj->Write();
  else{
    TDirectory *cur = gDirectory;
    TFile *currentFile = gFile;

    TDirectory *sub = cur->GetDirectory(obj->GetName());
    sub->cd();
    TList *listSub = (static_cast<TDirectory*>(obj))->GetList();
    TIter it(listSub);
    while( TObject *obj1=it() ) WriteHistosCurFile(obj1);
    cur->cd();
    gFile = currentFile;
    gDirectory = cur;
  }
}

void PndKFParticleFinderQA::SetPrintEffFrequency(Int_t n)
{ 
  fTopoPerformance->SetPrintEffFrequency(n); 
  fPrintFrequency = n;
}

ClassImp(PndKFParticleFinderQA);
