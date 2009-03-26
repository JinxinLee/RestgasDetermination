void extract(const char* OutName)
{

  char* elecFile = "ElectOutTree.root";
  char* pionFile = "PionOutTree.root";
  char* kaonFile = "KaonOutTree.root";
  char* muonFile = "MuonOutTree.root";
  char* gammaFile = "GammaOutTree.root";
  char* protonFile = "ProtonOutTree.root";
  

  TFile elec (elecFile,"READ");
  TFile pion (pionFile,"READ");
  TFile kaon (kaonFile,"READ");
  TFile muon (muonFile,"READ");
  TFile gamma (gammaFile,"READ");
  TFile proton (protonFile,"READ");

  TFile OutFile (OutName,"RECREATE");

  TTree *el = (TTree*) elec.Get("Elect");
  TTree *pi = (TTree*) pion.Get("Pion");
  TTree *ka = (TTree*) kaon.Get("Kaon");
  TTree *mu = (TTree*) muon.Get("Muon");
  TTree *ga = (TTree*) gamma.Get("Gamma");
  TTree *pr = (TTree*) proton.Get("Proton");
  
  el->Write();
  pi->Write();
  ka->Write();
  mu->Write();
  ga->Write();
  pr->Write();

  OutFile.Close();
  
  elec.Close();
  pion.Close();
  kaon.Close();
  muon.Close();
  gamma.Close();
  proton.Close();
}
