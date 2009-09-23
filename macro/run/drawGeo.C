{
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  

  TFile* file = new TFile("testparams.root");
  file->Get("FairBaseParSet"); 
  
//  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 1100, 1100);
//  c1->SetFillColor(10);

  gGeoManager->SetVisLevel(4);
  gGeoManager->GetMasterVolume()->Draw("ogl");


}

