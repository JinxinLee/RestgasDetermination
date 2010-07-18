

drawGeo()
{
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TFile* file = new TFile("params.root");
  file->Get("FairBaseParSet"); 
  
  gGeoManager->SetVisLevel(3); //defoult 3
  gGeoManager->GetMasterVolume()->Draw("ogl");


}

