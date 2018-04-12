//-----------------------------------------------------------
//-----------------------------------------------------------

// Panda Headers ----------------------
#include "PndKFParticleFinderPID.h"
#include "PndKFParticleFinder.h"
#include "PndPidCandidate.h"
#include "PndEmcBump.h"
#include "FairRunAna.h"
#include "FairField.h"

//KF Particle headers
#include "KFParticleTopoReconstructor.h"
#include "KFPTrackVector.h"
#include "PndEmcBump.h"

//ROOT headers
#include "TClonesArray.h"
#include "TStopwatch.h" //to measure the time
#include "TMath.h" //to calculate Prob function

//c++ and std headers
#include <iostream>
#include <cmath>
#include <vector>
using std::vector;

PndKFParticleFinder::PndKFParticleFinder(const char* name, Int_t iVerbose):
  FairTask(name, iVerbose), fChargedTrackBranchName(""), fNeutralTrackBranchName(""), fChargedTrackArray(0), fNeutralTrackArray(0), fEmcBumps(0),
  fTopoReconstructor(0), fPVFindMode(0), fPID(0)
{
  fTopoReconstructor = new KFParticleTopoReconstructor;
  // set default cuts
  SetPrimaryProbCut(0.0001); // 0.01% to consider primary track as a secondary;
}

PndKFParticleFinder::~PndKFParticleFinder()
{
  if(fTopoReconstructor) delete fTopoReconstructor;
}

InitStatus PndKFParticleFinder::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
  {
    Error("PndKFParticleFinder::Init","RootManager not instantiated!");
    return kERROR;
  }
  
  // Get input collection
  fChargedTrackArray=(TClonesArray*) ioman->GetObject(fChargedTrackBranchName);
  if(fChargedTrackArray==0)
  {
    Error("PndKFParticleFinder::Init","track-array not found!");
    return kERROR;
  }

  if(!(fNeutralTrackBranchName.IsNull()))
  {
    fNeutralTrackArray = (TClonesArray*) ioman->GetObject(fNeutralTrackBranchName);
  }
  fEmcBumps = (TClonesArray*) ioman->GetObject("EmcBump");
  
  //set magnetic field for KF Particle
  FairField* MF=FairRunAna::Instance()->GetField();
  Double_t xyz[3]={0,0,0};
  Double_t fieldValue[3];
  MF->Field(xyz,fieldValue);
  double Bz = fieldValue[2];
  fTopoReconstructor->SetField(-Bz);
  
  return kSUCCESS;
}

void PndKFParticleFinder::Exec(Option_t*)
{
  Int_t ntracks=0;//fChargedTrackArray->GetEntriesFast();

  vector<PndPidCandidate> vRTracks(fChargedTrackArray->GetEntriesFast());
  vector<int> pdg(fChargedTrackArray->GetEntriesFast(), -1);
  vector<int> trackId(fChargedTrackArray->GetEntriesFast(), -1);
    
  for(int iTr=0; iTr<fChargedTrackArray->GetEntriesFast(); iTr++)
  {
    PndPidCandidate *inTrack = (PndPidCandidate*)fChargedTrackArray->At(iTr);
  
    bool ok = 1;
    ok = ok && std::isfinite(inTrack->GetPosition().x());
    ok = ok && std::isfinite(inTrack->GetPosition().y());
    ok = ok && std::isfinite(inTrack->GetPosition().z());
    ok = ok && std::isfinite(inTrack->GetMomentum().x());
    ok = ok && std::isfinite(inTrack->GetMomentum().y());
    ok = ok && std::isfinite(inTrack->GetMomentum().z());
    
    float cov[21];
    cov[ 0] = inTrack->GetErrorP7()[0];
    cov[ 1] = inTrack->GetErrorP7()[1];
    cov[ 2] = inTrack->GetErrorP7()[2];
    cov[ 3] = inTrack->GetErrorP7()[3];
    cov[ 4] = inTrack->GetErrorP7()[4];
    cov[ 5] = inTrack->GetErrorP7()[5];
    cov[ 6] = inTrack->GetErrorP7()[6];
    cov[ 7] = inTrack->GetErrorP7()[7];
    cov[ 8] = inTrack->GetErrorP7()[8];
    cov[ 9] = inTrack->GetErrorP7()[18];
    cov[10] = inTrack->GetErrorP7()[9];
    cov[11] = inTrack->GetErrorP7()[10];
    cov[12] = inTrack->GetErrorP7()[11];
    cov[13] = inTrack->GetErrorP7()[19];
    cov[14] = inTrack->GetErrorP7()[20];
    cov[15] = inTrack->GetErrorP7()[12];
    cov[16] = inTrack->GetErrorP7()[13];
    cov[17] = inTrack->GetErrorP7()[14];
    cov[18] = inTrack->GetErrorP7()[21];
    cov[19] = inTrack->GetErrorP7()[22];
    cov[20] = inTrack->GetErrorP7()[23];
    
      
    for(unsigned short iC=0; iC<21; iC++)
      ok = ok && std::isfinite(cov[iC]);
//     ok = ok && (cov[0] < 100. && cov[0] > 0.)
//             && (cov[2] < 100. && cov[2] > 0.)
//             && (cov[5] < 100. && cov[5] > 0.)
//             && (cov[9] < 100. && cov[9] > 0.)
//             && (cov[14] < 100. && cov[14] > 0.)
//             && (cov[20] < 100. && cov[20] > 0.);
    ok = ok && (cov[0] > 0.)
            && (cov[2] > 0.)
            && (cov[5] > 0.)
            && (cov[9] > 0.)
            && (cov[14] > 0.)
            && (cov[20] > 0.);
            
    ok = ok && inTrack->GetChiSquared() < 20*inTrack->GetDegreesOfFreedom();
    if(!ok) continue;
    
    if(fPID)
    {
      if(fPID->GetPID()[iTr] == -2) continue; 
      pdg[ntracks] = fPID->GetPID()[iTr];
    }
    vRTracks[ntracks] = *inTrack;
    trackId[ntracks] = iTr;
    
    ntracks++;
  }
  
  vRTracks.resize(ntracks);
  pdg.resize(ntracks);
  trackId.resize(ntracks);
  
  KFPTrackVector tracks;
  tracks.Resize(ntracks);
  //fill vector with tracks
  for(Int_t iTr=0; iTr<ntracks; iTr++)
  {
    PndPidCandidate *inTrack = (PndPidCandidate*)fChargedTrackArray->At(trackId[iTr]);
    
    float par[6] = {0.f};
    par[0] = inTrack->GetPosition().x();
    par[1] = inTrack->GetPosition().y();
    par[2] = inTrack->GetPosition().z();
    par[3] = inTrack->GetMomentum().x();
    par[4] = inTrack->GetMomentum().y();
    par[5] = inTrack->GetMomentum().z();
    
    Int_t q = inTrack->GetCharge();
    
    float cov[21];
    cov[ 0] = inTrack->GetErrorP7()[0];
    cov[ 1] = inTrack->GetErrorP7()[1];
    cov[ 2] = inTrack->GetErrorP7()[2];
    cov[ 3] = inTrack->GetErrorP7()[3];
    cov[ 4] = inTrack->GetErrorP7()[4];
    cov[ 5] = inTrack->GetErrorP7()[5];
    cov[ 6] = inTrack->GetErrorP7()[6];
    cov[ 7] = inTrack->GetErrorP7()[7];
    cov[ 8] = inTrack->GetErrorP7()[8];
    cov[ 9] = inTrack->GetErrorP7()[18];
    cov[10] = inTrack->GetErrorP7()[9];
    cov[11] = inTrack->GetErrorP7()[10];
    cov[12] = inTrack->GetErrorP7()[11];
    cov[13] = inTrack->GetErrorP7()[19];
    cov[14] = inTrack->GetErrorP7()[20];
    cov[15] = inTrack->GetErrorP7()[12];
    cov[16] = inTrack->GetErrorP7()[13];
    cov[17] = inTrack->GetErrorP7()[14];
    cov[18] = inTrack->GetErrorP7()[21];
    cov[19] = inTrack->GetErrorP7()[22];
    cov[20] = inTrack->GetErrorP7()[23];
    
    for(Int_t iP=0; iP<6; iP++)
      tracks.SetParameter(par[iP], iP, iTr);    
    for(Int_t iC=0; iC<21; iC++)
      tracks.SetCovariance(cov[iC], iC, iTr);
    tracks.SetId(trackId[iTr], iTr);
    if(fPID)
      tracks.SetPDG(fPID->GetPID()[iTr], iTr);
    else
      tracks.SetPDG(-1, iTr);
    tracks.SetQ(q, iTr);

    if(fPVFindMode == 0)
    {    
      tracks.SetPVIndex(-1, iTr);
    }
    else
      tracks.SetPVIndex(-1, iTr);
  }

    
  //fill vector with Emc clusters
  KFPEmcCluster emcClusters;
  if(fNeutralTrackArray)
  {
    int nClusters = 0;
    vector<int> clusterId(fNeutralTrackArray->GetEntriesFast());
    for(int iClust=0; iClust<fNeutralTrackArray->GetEntriesFast(); iClust++)
    {
      PndPidCandidate *inTrack = (PndPidCandidate*)fNeutralTrackArray->At(iClust);
      int iBump = inTrack->GetEmcIndex();
      PndEmcBump *inBump = (PndEmcBump*) fEmcBumps->At(iBump);
      
      if(inTrack->GetEmcQuality()<400) continue;
      if(inBump->energy() < 0.02) continue;
      
      clusterId[nClusters] = iClust;
      nClusters++;
    }
    
    emcClusters.Resize(nClusters);
    for(int iClust2=0; iClust2<nClusters; iClust2++)
    {
      int iClust = clusterId[iClust2];
      PndPidCandidate *inTrack = (PndPidCandidate*)fNeutralTrackArray->At(iClust);
      int iBump = inTrack->GetEmcIndex();
      PndEmcBump *inBump = (PndEmcBump*) fEmcBumps->At(iBump);
      
      float par[4] = {0.f};
      par[0] = inTrack->GetLastHit().x();
      par[1] = inTrack->GetLastHit().y();
      par[2] = inTrack->GetLastHit().z();
      par[3] = inBump->energy();
      float cov[10] = {0.f};
      cov[ 0] = inTrack->GetErrorP7()[0];
      cov[ 1] = inTrack->GetErrorP7()[1];
      cov[ 2] = inTrack->GetErrorP7()[2];
      cov[ 3] = inTrack->GetErrorP7()[3];
      cov[ 4] = inTrack->GetErrorP7()[4];
      cov[ 5] = inTrack->GetErrorP7()[5];
      cov[ 6] = inTrack->GetErrorP7()[15];
      cov[ 7] = inTrack->GetErrorP7()[16];
      cov[ 8] = inTrack->GetErrorP7()[17];
      cov[ 9] = inTrack->GetErrorP7()[27];
            
      for(Int_t iP=0; iP<4; iP++)
        emcClusters.SetParameter(par[iP], iP, iClust2);    
      for(Int_t iC=0; iC<10; iC++)
        emcClusters.SetCovariance(cov[iC], iC, iClust2);
      emcClusters.SetId(iClust+fChargedTrackArray->GetEntriesFast(), iClust2);
    }
  }
  
  TStopwatch timer;
  timer.Start();
  
  fTopoReconstructor->Init(tracks);
  if(fNeutralTrackArray)
  {
    fTopoReconstructor->SetEmcClusters(&emcClusters);
  }
  
  if(fPVFindMode == 0)
  {
    KFPVertex primVtx_tmp;
    primVtx_tmp.SetXYZ(0,0,0);
    primVtx_tmp.SetCovarianceMatrix( 0,0,0,0,0,0 );
    primVtx_tmp.SetNContributors( 0 );
    primVtx_tmp.SetChi2( -100 );

    vector<int> pvTrackIds;
    KFVertex pv(primVtx_tmp);
    fTopoReconstructor->AddPV(pv, pvTrackIds);
  }
  else if(fPVFindMode == 1)
    fTopoReconstructor->ReconstructPrimVertex();
  else if(fPVFindMode == 2)
    fTopoReconstructor->ReconstructPrimVertex(0);
  
  fTopoReconstructor->SortTracks();
  fTopoReconstructor->ReconstructParticles();
  
  timer.Stop();
  fTopoReconstructor->SetTime(timer.RealTime());
}

double PndKFParticleFinder::InversedChi2Prob(double p, int ndf) const
{
  double epsilon = 1.e-14;
  double chi2Left = 0.f;
  double chi2Right = 10000.f;
  
  double probLeft = p - TMath::Prob(chi2Left, ndf);
  
  double chi2Centr = (chi2Left+chi2Right)/2.f;
  double probCentr = p - TMath::Prob( chi2Centr, ndf);
  
  while( TMath::Abs(chi2Right-chi2Centr)/chi2Centr > epsilon )
  {
    if(probCentr * probLeft > 0.f)
    {
      chi2Left = chi2Centr;
      probLeft = probCentr;
    }
    else
    {
      chi2Right = chi2Centr;
    }
    
    chi2Centr = (chi2Left+chi2Right)/2.f;
    probCentr = p - TMath::Prob( chi2Centr, ndf);
  }
  
  return chi2Centr;
}

void PndKFParticleFinder::SetPrimaryProbCut(float prob)
{ 
  fTopoReconstructor->SetChi2PrimaryCut( InversedChi2Prob(prob, 2) );
}

ClassImp(PndKFParticleFinder);
