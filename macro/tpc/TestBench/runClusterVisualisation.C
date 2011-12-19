void runClusterVisualisation(TString filename,
                             double DriftField=400,
                             int paramSet=0,
                             TString digifile="",
                             TString mcfilename="")
{
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  char * clustvis;
  clustvis = getenv ("CLUSTVIS");
  if (clustvis==NULL){
    std::cerr<<"WARNING: environment variable CLUSTVIS is not set, PndTpcClustVis is not built!"<<std::endl;
    return;
  }

  // load TpcEve Library
  if (TString(gSystem->DynamicPathName("libTpcEve", kTRUE)) != TString(""))
    gSystem->Load("libTpcEve");
  else{
    std::cerr<<"Library TpcEve not found. Make sure your ROOT is compiled with EVE"<<std::endl;
    return;
  }
  
  TFile* file = new TFile(filename);
  TTree* tree = (TTree*)file->Get("cbmsim");
  
  if(digifile.Length()>5){
    tree->AddFriend("cbmsim",digifile);
  }

  TEveManager::Create();


  PndTpcClustVis* clustVis = PndTpcClustVis::getInstance();
  clustVis->reset();
  clustVis->setTree(tree);

  if(mcfilename.Length()>1){
    TFile* mcfile = new TFile(mcfilename);
    TTree* mctree = (TTree*)mcfile->Get("cbmsim");
    clustVis->setMCTree(mctree);
  }



  TString gasfile,padplanefile,padshapefile,geoFile;
  double gain,spread,zGem,samplingFreq,wallclock, MagField;
  TString basedir = gSystem->Getenv("VMCWORKDIR");
  if(paramSet==0){ // standard testBench Settings
    gasfile=basedir+"/tpc/TestBench/NEON-90_CO2-10_B0.6_PRES1013.asc";
    padplanefile=basedir+"/tpc/TestBench/padPlane_FOPI.dat";
    padshapefile=basedir+"/tpc/TestBench/TBhexa_pads.dat";
    gain=4000;
    spread=0.02;
    zGem=-62.5;
    samplingFreq=20;
    wallclock=1000;

    MagField=6.;
    geoFile="$VMCWORKDIR/geometry/tpc_prototype.root"; // todo: kalman crashes
    geoFile="$VMCWORKDIR/tpc/TestBench/FOPIGeo.root"; // this works

    clustVis->setRiemannscale(8.6);
  }
  else if(paramSet==1){ // standard PANDA SIM Settings
    gasfile=basedir+"/tpc/NEON-90_CO2-10_B2_PRES1013.asc";
    padplanefile=basedir+"/tpc/pndhexplane0.15.16s.dat";
    //padplanefile=basedir+"/tpc/pndhexplane0.15.dat";
    padshapefile=basedir+"/tpc/Hexagons0.15.dat";
    gain=4000;
    spread=0.02;
    zGem=-39.5;
    samplingFreq=40;
    wallclock=-200000;
    DriftField=400;

    MagField=20.;
    geoFile="$VMCWORKDIR/geometry/TPC_V1.1.root"; // todo: kalman crashes
    geoFile="$VMCWORKDIR/tpc/TestBench/FOPIGeo.root"; // this works

    clustVis->setRiemannscale(24.6);
  }
  else{
    cout<<"use paramSet 0 or 1"<<endl;
    return;
  }

  clustVis->initDigimapper(DriftField,gain,spread,zGem,samplingFreq,wallclock,
                           gasfile.Data(),padplanefile.Data(),padshapefile.Data());

  clustVis->setFieldZ(MagField);

  TGeoManager* geom = new TGeoManager("Geometry", "Geane geometry");
  TGeoManager::Import(geoFile);

  clustVis->open();
}
