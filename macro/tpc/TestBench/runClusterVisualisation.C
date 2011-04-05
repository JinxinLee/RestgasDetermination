void runClusterVisualisation(TString filename, TString digifile="") 
{
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TFile* file = new TFile(filename);
  TTree* tree = (TTree*)file->Get("cbmsim");
  
  if(digifile.Length()>1){
    tree->AddFriend("cbmsim",digifile);
  }

  TEveManager::Create();

  PndTpcClustVis* clustVis = PndTpcClustVis::getInstance();
  clustVis->reset();
  clustVis->setTree(tree);
  clustVis->setOptions("DCR");
  clustVis->gotoEvent(1);
  clustVis->open();
}
