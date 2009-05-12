// C++ includes

#include <iostream>
/*
  #include <string>
  #include <vector>
*/
//ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TStopwatch.h"

void filterFile(const char* OutName)
{
  TStopwatch timer;
  timer.Start();
  
  const char* inFile = "/media/daq/babaiexp/VanniFiles/PidParTree.root";
  int Nent = 0;
  
  TFile inF(inFile,"READ");
  
  //Declaration of leaves types
  Float_t         Px;
  Float_t         Py;
  Float_t         Pz;
  Float_t         Pt;
  Float_t         p;
  Float_t         emc;
  Float_t         stt;
  Float_t         mvd;
  Float_t         tof;
  Float_t         thetaC;
  Float_t         delphi;
  Float_t         delz;
  Float_t         z00;
  Float_t         z11;
  Float_t         z22;
  Float_t         z20;
  Float_t         z40;
  Float_t         z42;
  Float_t         z31;  
  
  TTree *electron = (TTree*) inF.Get("electron");
  // Set branch addresses.
  electron->SetBranchAddress("Px",&Px);
  electron->SetBranchAddress("Py",&Py);
  electron->SetBranchAddress("Pz",&Pz);
  electron->SetBranchAddress("Pt",&Pt);
  electron->SetBranchAddress("p",&p);
  electron->SetBranchAddress("emc",&emc);
  electron->SetBranchAddress("stt",&stt);
  electron->SetBranchAddress("mvd",&mvd);
  electron->SetBranchAddress("tof",&tof);
  electron->SetBranchAddress("thetaC",&thetaC);
  electron->SetBranchAddress("delphi",&delphi);
  electron->SetBranchAddress("delz",&delz);
  electron->SetBranchAddress("z00",&z00);
  electron->SetBranchAddress("z11",&z11);
  electron->SetBranchAddress("z22",&z22);
  electron->SetBranchAddress("z20",&z20);
  electron->SetBranchAddress("z40",&z40);
  electron->SetBranchAddress("z42",&z42);
  electron->SetBranchAddress("z31",&z31);

  //Select branches
  electron->SetBranchStatus("*",0);
  electron->SetBranchStatus("p",1);
  electron->SetBranchStatus("emc",1);
  electron->SetBranchStatus("stt",1);
  electron->SetBranchStatus("mvd",1);
  electron->SetBranchStatus("tof",1);
  electron->SetBranchStatus("thetaC",1);

  TTree *pion     = (TTree*) inF.Get("pion");
  // Set branch addresses.
  pion->SetBranchAddress("Px",&Px);
  pion->SetBranchAddress("Py",&Py);
  pion->SetBranchAddress("Pz",&Pz);
  pion->SetBranchAddress("Pt",&Pt);
  pion->SetBranchAddress("p",&p);
  pion->SetBranchAddress("emc",&emc);
  pion->SetBranchAddress("stt",&stt);
  pion->SetBranchAddress("mvd",&mvd);
  pion->SetBranchAddress("tof",&tof);
  pion->SetBranchAddress("thetaC",&thetaC);
  pion->SetBranchAddress("delphi",&delphi);
  pion->SetBranchAddress("delz",&delz);
  pion->SetBranchAddress("z00",&z00);
  pion->SetBranchAddress("z11",&z11);
  pion->SetBranchAddress("z22",&z22);
  pion->SetBranchAddress("z20",&z20);
  pion->SetBranchAddress("z40",&z40);
  pion->SetBranchAddress("z42",&z42);
  pion->SetBranchAddress("z31",&z31);
  
  //Select branches
  pion->SetBranchStatus("*",0);
  pion->SetBranchStatus("p",1);
  pion->SetBranchStatus("emc",1);
  pion->SetBranchStatus("stt",1);
  pion->SetBranchStatus("mvd",1);
  pion->SetBranchStatus("tof",1);
  pion->SetBranchStatus("thetaC",1);
  
  TTree *muon     = (TTree*) inF.Get("muon");
  // Set branch addresses.
  muon->SetBranchAddress("Px",&Px);
  muon->SetBranchAddress("Py",&Py);
  muon->SetBranchAddress("Pz",&Pz);
  muon->SetBranchAddress("Pt",&Pt);
  muon->SetBranchAddress("p",&p);
  muon->SetBranchAddress("emc",&emc);
  muon->SetBranchAddress("stt",&stt);
  muon->SetBranchAddress("mvd",&mvd);
  muon->SetBranchAddress("tof",&tof);
  muon->SetBranchAddress("thetaC",&thetaC);
  muon->SetBranchAddress("delphi",&delphi);
  muon->SetBranchAddress("delz",&delz);
  muon->SetBranchAddress("z00",&z00);
  muon->SetBranchAddress("z11",&z11);
  muon->SetBranchAddress("z22",&z22);
  muon->SetBranchAddress("z20",&z20);
  muon->SetBranchAddress("z40",&z40);
  muon->SetBranchAddress("z42",&z42);
  muon->SetBranchAddress("z31",&z31);
  
  //Select branches
  muon->SetBranchStatus("*",0);
  muon->SetBranchStatus("p",1);
  muon->SetBranchStatus("emc",1);
  muon->SetBranchStatus("stt",1);
  muon->SetBranchStatus("mvd",1);
  muon->SetBranchStatus("tof",1);
  muon->SetBranchStatus("thetaC",1);

  TTree *kaon     = (TTree*) inF.Get("kaon");
  // Set branch addresses.
  kaon->SetBranchAddress("Px",&Px);
  kaon->SetBranchAddress("Py",&Py);
  kaon->SetBranchAddress("Pz",&Pz);
  kaon->SetBranchAddress("Pt",&Pt);
  kaon->SetBranchAddress("p",&p);
  kaon->SetBranchAddress("emc",&emc);
  kaon->SetBranchAddress("stt",&stt);
  kaon->SetBranchAddress("mvd",&mvd);
  kaon->SetBranchAddress("tof",&tof);
  kaon->SetBranchAddress("thetaC",&thetaC);
  kaon->SetBranchAddress("delphi",&delphi);
  kaon->SetBranchAddress("delz",&delz);
  kaon->SetBranchAddress("z00",&z00);
  kaon->SetBranchAddress("z11",&z11);
  kaon->SetBranchAddress("z22",&z22);
  kaon->SetBranchAddress("z20",&z20);
  kaon->SetBranchAddress("z40",&z40);
  kaon->SetBranchAddress("z42",&z42);
  kaon->SetBranchAddress("z31",&z31);
  
  //Select branches
  kaon->SetBranchStatus("*",0);
  kaon->SetBranchStatus("p",1);
  kaon->SetBranchStatus("emc",1);
  kaon->SetBranchStatus("stt",1);
  kaon->SetBranchStatus("mvd",1);
  kaon->SetBranchStatus("tof",1);
  kaon->SetBranchStatus("thetaC",1);

  TTree *proton   = (TTree*) inF.Get("proton");
  // Set branch addresses.
  proton->SetBranchAddress("Px",&Px);
  proton->SetBranchAddress("Py",&Py);
  proton->SetBranchAddress("Pz",&Pz);
  proton->SetBranchAddress("Pt",&Pt);
  proton->SetBranchAddress("p",&p);
  proton->SetBranchAddress("emc",&emc);
  proton->SetBranchAddress("stt",&stt);
  proton->SetBranchAddress("mvd",&mvd);
  proton->SetBranchAddress("tof",&tof);
  proton->SetBranchAddress("thetaC",&thetaC);
  proton->SetBranchAddress("delphi",&delphi);
  proton->SetBranchAddress("delz",&delz);
  proton->SetBranchAddress("z00",&z00);
  proton->SetBranchAddress("z11",&z11);
  proton->SetBranchAddress("z22",&z22);
  proton->SetBranchAddress("z20",&z20);
  proton->SetBranchAddress("z40",&z40);
  proton->SetBranchAddress("z42",&z42);
  proton->SetBranchAddress("z31",&z31);

  //Select branches
  proton->SetBranchStatus("*",0);
  proton->SetBranchStatus("p",1);
  proton->SetBranchStatus("emc",1);
  proton->SetBranchStatus("stt",1);
  proton->SetBranchStatus("mvd",1);
  proton->SetBranchStatus("tof",1);
  proton->SetBranchStatus("thetaC",1);
  
  Nent = electron->GetEntries();
  
  TFile OutFile (OutName,"RECREATE");
  
  TTree* el = new TTree("Elect","ElectDesc");
  TTree* pi = new TTree("Pion","PionDesc");
  TTree* mu = new TTree("Muon","MuonDesc");
  TTree* ka = new TTree("Kaon","KaonDesc");
  TTree* pr = new TTree("Proton","PionDesc");
  
  // E- Define the tree branches
  el->Branch("p"     , &p      ,"p/F");
  el->Branch("tof"   , &tof    ,"tof/F");
  el->Branch("emc"   , &emc    ,"emc/F");
  el->Branch("stt"   , &stt    ,"stt/F");
  el->Branch("mvd"   , &mvd    ,"mvd/F");
  el->Branch("thetaC", &thetaC ,"thetaC/F");
  // Fill the new tree
  for (int i = 0; i < Nent; i++) {
    electron->GetEntry(i);
    if (p>0 && p <= 20 && emc>0 && emc <= 2.0 && stt>0 && mvd>0 && tof>0 && thetaC>0){
      el->Fill();
    }
  }
  el->Print();
  el->Write();
  
  // Pi-
  Nent = pion->GetEntries();
  pi->Branch("p"     , &p      ,"p/F");
  pi->Branch("tof"   , &tof    ,"tof/F");
  pi->Branch("emc"   , &emc    ,"emc/F");
  pi->Branch("stt"   , &stt    ,"stt/F");
  pi->Branch("mvd"   , &mvd    ,"mvd/F");
  pi->Branch("thetaC", &thetaC ,"thetaC/F");
  // Fill the new tree
  for (int i = 0; i < Nent; i++) {
    pion->GetEntry(i);
    if (p>0 && p <= 20 && emc>0 && emc <= 2.0 && stt>0 && mvd>0 && tof>0 && thetaC>0){
      pi->Fill();
    }
  }
  pi->Print();
  pi->Write();

  // K-
  Nent = kaon->GetEntries();
  ka->Branch("p"     , &p      ,"p/F");
  ka->Branch("tof"   , &tof    ,"tof/F");
  ka->Branch("emc"   , &emc    ,"emc/F");
  ka->Branch("stt"   , &stt    ,"stt/F");
  ka->Branch("mvd"   , &mvd    ,"mvd/F");
  ka->Branch("thetaC", &thetaC ,"thetaC/F");
  
  // Fill the new tree
  for (int i = 0; i < Nent; i++){
    kaon->GetEntry(i);
    if( p>0 && p <= 20 && emc>=0 && emc <= 2.0 && stt>0 && mvd>0 && tof>0 && thetaC>0){
      ka->Fill();
    }
  }
  ka->Print();
  ka->Write();

  // Mu-
  Nent = muon->GetEntries();
  mu->Branch("p"     , &p      ,"p/F");
  mu->Branch("tof"   , &tof    ,"tof/F");
  mu->Branch("emc"   , &emc    ,"emc/F");
  mu->Branch("stt"   , &stt    ,"stt/F");
  mu->Branch("mvd"   , &mvd    ,"mvd/F");
  mu->Branch("thetaC", &thetaC ,"thetaC/F");
  // Fill the new tree
  for (int i = 0; i < Nent; i++) {
    muon->GetEntry(i);
    if (p>0 && p <= 20 && emc>=0 && emc <= 2.0 && stt>0 && mvd>0 && tof>0 && thetaC>0){
      mu->Fill();
    }
  }
  mu->Print();
  mu->Write();
  
  // Proton
  Nent = proton->GetEntries();
  pr->Branch("p"     , &p      ,"p/F");
  pr->Branch("tof"   , &tof    ,"tof/F");
  pr->Branch("emc"   , &emc    ,"emc/F");
  pr->Branch("stt"   , &stt    ,"stt/F");
  pr->Branch("mvd"   , &mvd    ,"mvd/F");
  pr->Branch("thetaC", &thetaC ,"thetaC/F");
  // Fill the new tree
  for (int i = 0; i < Nent; i++) {
    proton->GetEntry(i);
    if (p>0 && p <= 20 && emc>=0 && emc <= 2.0 &&stt>0 && mvd>0 && tof>0 && thetaC>0){
      pr->Fill();
    }
  }
  pr->Print();
  pr->Write();

  OutFile.Close();
  inF.Close();
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  std::cout << "time " << rtime/60.0 << " min." << std::endl;
  exit(0);
}
