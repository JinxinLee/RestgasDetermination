// Macro for displaying the tracks for the DRC simulation
// only the DRC detector is ON
// input file testrun.root contains the MC information
//28/09/2006 Pablo Genova
//28/06/2007 Annalisa Cecchi - modified some visualisation properties

{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TFile* file = new TFile("./params_testrun1.root");
  file->Get("FairBaseParSet"); 
  
  gGeoManager->SetVisLevel(3);
  gGeoManager->GetMasterVolume()->Draw("ogl");


}

