// Macro created A.Sanchez
// It creates a geant simulation file for hyp
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  //gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  //basiclibs();
  
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  //rootlogon();
  
  gSystem->Load("librazhyp");
  gSystem->Load("libHyp");
  
  FairRunSim *fRun = new FairRunSim();
  
  
  
  //--Second xi minus momentum solution.
  
  //TString inFile= "data/xibximin2AStpRate.dat";//data/ximinAsciiStpRate.dat";
  //TString inFile= "data/ximinAsciiStpRate.dat";

  //--- second step -- HF from vertexes ----
  
  TString inFile= "/home/lorente/work/FairFeb14/pandaroot/macro/hyp/Alicia/hypBupV1T_Decay_gam.root";

//home/lorente/work/FairFeb14/pandaroot/macro/hyp/Alicia/hypBupV1T_Decay.root";//hypBupV1T_Geo2Decay.root";
  

  // set the MC version used

  // -- UrqmdSmm background events 
  // Int_t nEvents =99480;
  //TString inFile= "carbon_onlyXi_2_5.root";

  // ------------------------
  
  fRun->SetName("TGeant4");
  
  //fRun->SetOutputFile("./sim_hypgeantcheck.root");
  
  // ----------first step XXb production -----------------------
  //fRun->SetOutputFile("Sim_hypFSG41TXXb_Geo2.root");
  // ---------- second step HF decay from secondary vertexes -------
  fRun->SetOutputFile("bup_test.root");//Bup_hypFSG41TXm.root");
  
  
  // Set Material file Name
  //-----------------------
  
  //fRun->SetMaterials("media_pnd_hyp.geo");
  //new media for hyp
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------
  
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  /*
    FairModule *Magnet= new FairMagnet("MAGNET");
    Magnet->SetGeometryFileName("magnet.geo");
    fRun->AddModule(Magnet);
  */


  
  PndHyp *Hyp = new PndHyp("HYP",kTRUE);
  //FairDetector *Hyp = new PndHyp("HYP",kTRUE);
  
  //Hyp->SetGeometryFileName("HypST_block.geo");
  //fRun->AddModule(Hyp);
  //--blocks
  //---Layers (si+abs)
  //Hyp->SetGeometryFileName("HypST_prueba2.geo"); 
  //fRun->AddModule(Hyp);
  //--layers C+Si
  // Hyp->SetGeometryFileName("HypST_prueba23.geo"); 
  //fRun->AddModule(Hyp);
  //--layers C+si+hyppipe
  //Hyp->SetGeometryFileName("HypST_prueba24pipe.geo"); 
  //Hyp->SetGeometryFileName("HypST_newxy3C.geo");



  // --- root geometry ------
  /* Hyp->SetAbsorberVol("stglAb"); // absorber layer
  Hyp->SetSensorVol("stglSi");   // silicon sensor
  //Hyp->SetGeometryFileName("HYPST_assexy3C5Lay.root");
  Hyp->SetGeometryFileName("HYPST_assexy3C5Lay_mvd.root");//HYPST_assexy3C5Lay_test.root");
  */

  // xxxxxxxx Sebastian asymmetric geo root xxxxxxxxxxx
 
  Hyp->SetAbsorberVol("Absorber"); // absorber layer
  Hyp->SetSensorVol("Sensor");   // silicon sensor
  Hyp->SetGeoVersion("List");
  Hyp->SetListMat("HYPboron");
  Hyp->SetListMat("HYPaluminium");
  Hyp->SetListMat("Al97Mg3");
  Hyp->SetListMat("titanium");
  Hyp->SetListMat("HYPcarbon");
  Hyp->SetListMat("siliconinactive");
  Hyp->SetGeometryFileName("TargetSystem_Ti_AbsWindow_addSens.root");//SekTarget_open_varAbs4Si5_3Q_HYPbe_1mm_MVD.root");
  
  
  // ---------------------------------

  //Hyp->SetTreeFName("hypBupV1T_Geo2Decay.root");
  Hyp->SetHypSDtoFile(true,false);
  fRun->AddModule(Hyp);
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/SetFragments.C");
  FragConfig(fRun);
  
  //fRun->SetUserDecay(kTRUE);
  
  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  
  

  // Background events UrqmdSmm
  /* PndUrqmdSmmGenerator* AsciiGen = new PndUrqmdSmmGenerator(inFile);
  
  primGen->SetTarget(-55.5,0.);
  primGen->AddGenerator(AsciiGen);*/
 
  // *** with Ascii inFile ***first step
  //FairAsciiGenerator* AsciiGen = new FairAsciiGenerator(inFile);
 // primGen->SetTarget(-55.5,0.);
  //primGen->AddGenerator(AsciiGen);
  
  // *** with root inFile ***second step
  PndHypBupGenerator* partGen = new PndHypBupGenerator(inFile);
  partGen->GammaEmissPar(kTRUE);
  partGen->SetPRange(1.,1.);
  partGen->SetPhiRange(0,360);
  partGen->SetThetaRange(0,180);
  primGen->AddGenerator(partGen);
  
  
  

   PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0, 0 ,10. ); // values are in kG
  // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
  fMagField->SetFieldRegion(-50, 50,-50, 50, -200, 200);
  fRun->SetField(fMagField);
  

    /*PndMultiField *fField= new PndMultiField();
  PndTransMap *map= new PndTransMap("TransMap", "R");
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
  fField->AddField(map);
  fField->AddField(map1);
  fField->AddField(map2);
  fRun->SetField(fField);*/
  
  fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
  
  
  fRun->Init();
  

  // Fill the Parameter containers for this run
  //-------------------------------------------
  // cout<<" gGeoManager "<<gGeoManager<<endl;
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  
  // PndMultiFieldPar* fieldPar = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  // if ( fField ) { fieldPar->SetParameters(fField); }
  // fieldPar->setInputVersion(fRun->GetRunId(),1);
  // fieldPar->setChanged();

  PndConstPar* fieldPar = (PndConstPar*) rtdb->getContainer("PndConstPar");
  if ( fMagField ) { fieldPar->SetParameters(fMagField); }
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  fieldPar->setChanged();
  
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  //output->open("simparams.root");
  //output->open("Sim_hypFS1TG4XXb_Geo2params.root");
  output->open("test_params.root");//Bup_hypFS1TG4Xmparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
  
  // Transport nEvents
  // -----------------
  // Set the number of events
  Int_t nEvents =60;//50505; 
  
  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  // delete fRun;
  //exit(0);
}  
  

