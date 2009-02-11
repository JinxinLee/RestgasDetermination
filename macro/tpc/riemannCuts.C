#include "TFile.h"
#include "TTree.h"
#include "PndMCTrack.h"
#include "TClonesArray.h"
#include "PndTpcRiemannTrack.h"
#include <vector>
#include "PndTpcEvtTime.h"

void riemannCuts(){
  TFile* recofile=TFile::Open("$PANDAMC/FAIRRoot/DPM5/sigSlice1/evtmix3/reco1/test1.reco.root");
  TFile* mcfile=TFile::Open("$PANDAMC/FAIRRoot/DPM5/test1.mc.root");
  TFile* rawfile=TFile::Open("$PANDAMC/FAIRRoot/DPM5/sigSlice1/evtmix3/test1.raw.root");

  TTree* recotree=(TTree*)recofile->Get("cbmsim");
  TTree* mctree=(TTree*)mcfile->Get("cbmsim");
  TTree* rawtree=(TTree*)rawfile->Get("cbmsim");
  
  TClonesArray* evttA=new TClonesArray("PndTpcEvtTime");
  rawtree->SetBranchAddress("PndTpcEvtTime",&evttA);
  rawtree->GetEntry(0);
  int nevts=evttA->GetEntriesFast();

  // build up Riemann MC-Tracks
  std::vector<PndTpcRiemannTrack*> ritrk;
  TClonesArray* mctrkA=new TClonesArray("PndMCTrack");
  mctree->SetBranchAddress("MCTrack",&mctrkA);
  for(int imc=0;imc<nevts;++imc++){
    mctree->GetEntry(imc);
    int ntrk=mctrk->GetEntriesFast();
    for(int itrk=0;itrk<ntrk;++itrk){
      PndMCTrack* mctrk=(PndMCTrack*)mctrkA->At(itrk);
      
    }
      
  }


}
