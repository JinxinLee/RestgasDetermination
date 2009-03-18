#include "fillTree.h"

void mcToTree(char *outPutF="EventOutPut.root")
{
  int nthreads, tid;
  TDatabasePDG *pdg = new TDatabasePDG();

  //Electron data
  TString const pointsEl = "../SimFiles/points_tpcmvdemcEl09.root";
  TString const trakEl   = "../SimFiles/traks_tpcmvdemcEl09.root"; 
  Int_t Epid = pdg->GetParticle("e-")->PdgCode();
  
  //Pion- Data
  TString const pointsPiM = "../SimFiles/points_tpcmvdemcPiMin09.root";
  TString const trakPiM   = "../SimFiles/traks_tpcmvdemcPiMin09.root"; 
  Int_t PiMpid = pdg->GetParticle("pi-")->PdgCode();
  
  //Kaon- Data
  TString const pointsKMin = "../SimFiles/points_tpcmvdemcKMin09.root";
  TString const trakKMin   = "../SimFiles/traks_tpcmvdemcKMin09.root";
  Int_t KMinpid = pdg->GetParticle("K-")->PdgCode();

  //Muon- Data
  TString const pointsMuMin = "../SimFiles/points_tpcmvdemcMuMin09.root";
  TString const trakMuMin   = "../SimFiles/traks_tpcmvdemcMuMin09.root";
  Int_t MuMinpid = pdg->GetParticle("mu-")->PdgCode();
  
  //Create the output file 
  TFile f(outPutF ,"RECREATE");

  TTree *el   = new TTree("El", "Electron event parameters");
  TTree *pion = new TTree("Pion", "Pion- event parameters");
  TTree *kaon = new TTree("Kaon", "Kaon- event parameters");
  TTree *muon = new TTree("Muon", "Muon- event parameters");

  fillTree(pointsEl,  trakEl, Epid, *el);
  fillTree(pointsPiM, trakPiM, PiMpid, *pion);
  fillTree(pointsKMin,trakKMin,KMinpid,*kaon);
  fillTree(pointsMuMin,trakMuMin,MuMinpid,*muon);
  
  el->Print();
  pion->Print();
  kaon->Print();
  muon->Print();
  
  // Write the file to output file
  f.Write();
  
  //Clean-up the pointers
  
  delete el;
  delete pion;
  delete kaon;
  delete muon;
  f.Close();

  exit(1);
}
