void extract()
{
  //char* elecFile = "";
  char* pionFile = "PionOutTree.root";
  char* kaonFile = "KaonOutTree.root";
  char* muonFile = "MuonOutTree.root";
  

  //TFile* elec = new TFile(elecFile,"READ");
  TFile pion (pionFile,"READ");
  TFile kaon (kaonFile,"READ");
  TFile muon (muonFile,"READ");

  TFile OutFile ("EventFeatures.root","RECREATE");

  //TTree *el = (TTree*) muon->Get("Electron");
  TTree *pi = (TTree*) pion.Get("Pion");
  TTree *ka = (TTree*) kaon.Get("Kaon");
  TTree *mu = (TTree*) muon.Get("Muon");
  
  //el->Write();
  pi->Write();
  ka->Write();
  mu->Write();

  OutFile.Close();
  //elec.Close();
  pion.Close();
  kaon.Close();
  muon.Close();
}
