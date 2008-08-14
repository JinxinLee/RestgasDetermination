#include "PndTpcTestChamberOutputTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmMCTrack.h"
#include "CbmRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "TClonesArray.h"
#include "PndTpcSample.h"

PndTpcTestChamberOutputTask::PndTpcTestChamberOutputTask()
  : CbmTask("TPC Cluster Finder"), outtree(NULL)
{
  _sampleBranchName = "PndTpcSample";
  _filename = "dreggn.root";
}


PndTpcTestChamberOutputTask::~PndTpcTestChamberOutputTask()
{}


void
PndTpcTestChamberOutputTask::SetParContainers() {
  std::cout<<"PndTpcTestChamberOutputTask::SetParContainers"<<std::endl;
}


InitStatus
PndTpcTestChamberOutputTask::Init()
{
  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcTestChamberOutputTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _sampleArray=(TClonesArray*) ioman->GetObject(_sampleBranchName);
  
  if(_sampleArray==0)
    {
      Error("PndTpcTestChamberOutputTask::Init","sample-array not found!");
      return kERROR;
    }

  // Get MCT collection
  MCTarray=(TClonesArray*) ioman->GetObject("MCTrack");
  
  if(MCTarray==0)
    {
      Error("PndTpcTestChamberOutputTask::Init","MCT-array not found!");
      return kERROR;
    }



  outfile = TFile::Open(_filename,"RECREATE");
  
  outtree = new TTree("datatree_m","MC tree from PandaROOT");
  outtree->Branch("trigger",&outstruct.trigger,"trigger/i");
  //outree->Branch("chan",outstruct.chan,"chan[128]/I");
  //outree->Branch("nslice",outstruct.nslice,"nslice[128]/I");
  outtree->Branch("charge",&outstruct.charge[0][0],"charge[128][150]/I");
  //outtree->Branch("time",outstruct.time,"time[128][150]/I");
  outtree->Branch("MCTax",&outstruct.MCTax,"MCTax/D");
  outtree->Branch("MCTbx",&outstruct.MCTbx,"MCTbx/D");
  outtree->Branch("MCTay",&outstruct.MCTay,"MCTay/D");
  outtree->Branch("MCTby",&outstruct.MCTby,"MCTby/D");

  return kSUCCESS;
}


void
PndTpcTestChamberOutputTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcTestChamberOutputTask::Exec"<<std::endl;

  int nMCT = MCTarray->GetEntriesFast();
  CbmMCTrack* MCT = (CbmMCTrack*) MCTarray->At(0);
  assert(MCT->GetPdgCode()==13);
  assert(MCT->GetMotherID()==-1);

  TVector3 startVertex = MCT->GetStartVertex();
  TVector3 momentum = MCT->GetMomentum();

  double t = -1.*startVertex.Z()/momentum.Z();
  outstruct.MCTax=momentum.X()/momentum.Z();
  outstruct.MCTbx=(startVertex+t*momentum).X();
  outstruct.MCTay=momentum.Y()/momentum.Z();
  outstruct.MCTby=(startVertex+t*momentum).Y();

  static unsigned int nev(0);
  outstruct.trigger = ++nev;

  for(int i=0;i<128;++i){
    for(int j=0;j<150;++j){
      outstruct.charge[i][j]=-1;
    }
  }

    

  Int_t nsamp=_sampleArray->GetEntriesFast();
  for(Int_t isamp=0;isamp<nsamp;++isamp){
    PndTpcSample *s=(PndTpcSample*)_sampleArray->At(isamp);
    unsigned int padId = s->padId();
    int time = s->t();
    assert(time>=0);
    if(time<150){
      outstruct.charge[padId][time]=s->amp();
    }
  }
   
  if(nsamp>0){
    std::cout << "fill @ nev " << nev << std::endl;
    outtree->Fill();
  }
  else{
    std::cout << "NOTfill @ nev " << nev << std::endl;
  }
  return;
}

void PndTpcTestChamberOutputTask::writeToFile(){
  outfile->cd();
  outtree->Write();
  outfile->Close();
}

ClassImp(PndTpcTestChamberOutputTask)
