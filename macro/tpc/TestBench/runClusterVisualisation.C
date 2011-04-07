void runClusterVisualisation(TString filename, double DriftField, int paramSet=0, TString digifile="") 
{
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  // load TpcEve Library
  if (TString(gSystem->DynamicPathName("libTpcEve", kTRUE)) != TString(""))
    gSystem->Load("libTpcEve");
  else{
    std::cerr<<"Library TpcEve not found. Make sure your ROOT is compiled with EVE"<<std::endl;
    return;
  }
  
  TFile* file = new TFile(filename);
  TTree* tree = (TTree*)file->Get("cbmsim");
  
  if(digifile.Length()>1){
    tree->AddFriend("cbmsim",digifile);
  }

  TEveManager::Create();

  PndTpcClustVis* clustVis = PndTpcClustVis::getInstance();
  clustVis->reset();
  clustVis->setTree(tree);

  TString gasfile,padplanefile,padshapefile;
  double gain,spread,zGem,samplingFreq,wallclock;
  TString basedir = gSystem->Getenv("VMCWORKDIR");
  if(paramSet==0){ // standard testBench Settings
    gasfile=basedir+"/tpc/TestBench/ARGON-89.635_CO2-10.365_B0.3_PRES1013.asc";
    padplanefile=basedir+"/tpc/TestBench/padPlane_FOPI.dat";
    padshapefile=basedir+"/tpc/TestBench/TBhexa_pads.dat";
    gain=4000;
    spread=0.02;
    zGem=-0.2.;
    samplingFreq=20;
    wallclock=-6000;
  }
   if(paramSet==1){ // standard PANDA SIM Settings
    gasfile=basedir+"/tpc/NEON-90_CO2-10_B2_PRES1013.asc";
    padplanefile=basedir+"/tpc/pndhexplane0.15.dat";
    padshapefile=basedir+"/tpc/Hexagons0.15.dat";
    gain=4000;
    spread=0.02;
    zGem=-39.5;
    samplingFreq=40;
    wallclock=-200000;
    DriftField=400;
  }


   clustVis->initDigimapper(DriftField,gain,spread,zGem,samplingFreq,wallclock,
			    gasfile.Data(),padplanefile.Data(),padshapefile.Data());
  clustVis->setOptions("DCR");
  clustVis->gotoEvent(1);
  clustVis->open();
}
