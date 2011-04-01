void runClusterVisualisation(TString filename)
{
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TFile* file = new TFile(filename);
  TTree* tree = (TTree*)file->Get("cbmsim");
  
  TEveManager::Create();

  PndTpcClustVis* clustVis = PndTpcClustVis::getInstance();
  clustVis->reset();
  clustVis->setTree(tree);
  clustVis->setOptions("DCR");
  clustVis->gotoEvent(1);
  clustVis->open();
}
