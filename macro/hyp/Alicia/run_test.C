// Macro created A.Sanchez
// It creates a geant simulation file for hyp
// Parameters
//1) Generator option 
// 1.a ) first step Xi m + Xib p --> Decfile, 
//
//       "box", Box generator
//       "hist", Th2D Distribution in P (thet)from GiBUU model
//       "Ascii", Ascii Gen, Intranuclear Cascade
//       "giBUU", Xi m from giBUU on 12C target
//       "param", Parametrization of a P(theta) dist from GiBUU
//       "giBUU+backg", p+pbar events from giBUU on 12C target
// 1.b ) second step , hypernuclei production "hypbup",
//2) detctor geo version
// 2.a) standard--> alicia version 
// 2.b) current --> sebastian version

void run_test(TString prefix = "",Int_t nEvents = 2000,TString Decfile = "box", TString vers = "current",Bool_t HYP_File= false,Bool_t gam = false){
  TStopwatch timer;
  timer.Start();
  gRandom->SetSeed(); 
  gDebug=0;

  //gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  
  gSystem->Load("libtrackrep");


  gSystem->Load("librazhyp");
  gSystem->Load("libHyp");
  
  FairRunSim *fRun = new FairRunSim();
  
  // --- first step ----
  TString inFile;
  //Background
  
  if(Decfile=="giBUU+backg")inFile= "/data/work/kpha4/lorente/sim_panda/carbon2_allParticles_1_32.root";

  // -- GiBUU Parametrization TH2F Corr. Dist P(theta)
  
  if(Decfile=="hist"||Decfile=="param")inFile= "XiGengiBUUSamp.root";
  
  //--Second xi minus momentum solution.
  
  //TString inFile= "data/xibximin2AStpRate.dat";//data/ximinAsciiStpRate.dat";
  //TString inFile= "data/ximinAsciiStpRate.dat";

   // -- UrqmdSmm/GiBUU SIGNAL/ background events 
  
  // Int_t nEvents =99480;
  
  if(Decfile=="giBUU")inFile= "../../../../../FairFeb14/pandaroot/macro/hyp/Alicia/carbon_onlyXi_2_5.root";//"/data/work/kpha4/lorente/XiProduction/UrQMDSmmXiXib/apC_potP1.root";

  //--- second step -- 
  //--- HF from vertexes ----
  
  if(Decfile=="hypbup"){
    
    if(gam){ 
      inFile= "/data/work/kpha4/lorente/sim_panda/hypBupV1T_Decay_gam_"+prefix+".root";
    }else{
      inFile= "hypBupV1T_Decay_3.root";
    }
  }
  

  // set the MC version used

 

  // ------------------------
  
  fRun->SetName("TGeant4");
  
 
  
  // ----------first step -------------

  // --------- background ------------

  if(Decfile=="giBUU+backg")fRun->SetOutputFile("/data/work/kpha4/lorente/sim_panda/Sim_hypFSG41TBg_SebGeo_"+prefix+".root");
  //----------------------------------

  // ++++++ Xi minus production ++++++
  if(Decfile=="box")fRun->SetOutputFile("sim_box.root");
  if(Decfile=="param")fRun->SetOutputFile("/data/work/kpha4/lorente/sim_panda/Sim_hypFSG41TXm_SebGeo_"+prefix+".root");
  if(Decfile=="giBUU") fRun->SetOutputFile("/data/work/kpha4/lorente/sim_panda/Sim_hypFSG41TXm_GB.root");
  // ----------first step XXb production -----------------------
  
  //fRun->SetOutputFile("Sim_hypFSG41TXXb_Geo2.root");

  // ---------- second step HF decay from secondary vertexes -------

  //fRun->SetOutputFile("Bup_hypFSG41TXXb.root");
  if(Decfile=="hypbup")fRun->SetOutputFile("Bup_hypFSG41TXm.root");
  
  
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
  // xxxxx Alicia's detector geo xxxx
  if(vers=="standard"){
    
    Hyp->SetAbsorberVol("stglAb"); // absorber layer
    Hyp->SetSensorVol("stglSi");   // silicon sensor
    Hyp->SetGeoVersion("List");
    Hyp->SetListMat("HYPdiamond");
    Hyp->SetListMat("HYPcarbon");
    //Hyp->SetGeometryFileName("HYPST_assexy3C5Lay.root");
    Hyp->SetGeometryFileName("HYPST_assexy3C5Lay_mvd.root");//HYPST_assexy3C5Lay_test.root");
    
  }
  
  // xxxxxxxx Sebastian asymmetric geo root xxxxxxxxxxx
  if(vers=="current"){
    Hyp->SetAbsorberVol("Absorber"); // absorber layer
    Hyp->SetSensorVol("Sensor");   // silicon sensor
    Hyp->SetGeoVersion("List");
    Hyp->SetListMat("HYPboron");
    Hyp->SetListMat("HYPaluminium");
    Hyp->SetListMat("Al97Mg3");
    Hyp->SetListMat("titanium");
    Hyp->SetListMat("HYPcarbon");
    Hyp->SetListMat("siliconinactive");
    Hyp->SetGeometryFileName("TargetSystem_Ti_TcT150um_filledCorners_addSens.root");
    //Hyp->SetGeometryFileName("TargetSystem_Ti_AbsWindow_addSens.root");//SekTarget_open_varAbs4Si5_3Q_HYPbe_1mm_MVD.root");
     }

  // ---------------------------------

  if(HYP_File){
   
    Hyp->SetHypSDtoFile(true,HYP_File);

    if(gam){
      Hyp->SetHypGamFEm(gam);
      Hyp->SetTreeFName("/data/work/kpha4/lorente/sim_panda/hypBupV1T_Decay_gam_"+prefix+".root");
    }else{
      Hyp->SetTreeFName("hypBupV1T_Decay_3.root");
    }
  }
  fRun->AddModule(Hyp);
  
  //gROOT->LoadMacro("$VMCWORKDIR/gconfig/SetFragments.C");
  //FragConfig(fRun);
  
  //fRun->SetUserDecay(kTRUE);

  // MVD outer structure
  //--------------------

  /* FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.2_Simplified_onlyStrip5_z-verschoben550.root"); // only sensors, update follows
  Mvd->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Mvd);*/
  
  // Create and Set Event Generator
  //-------------------------------
  
  
  
  if(Decfile=="giBUU+backg"){ 
    FairFilteredPrimaryGenerator* primGen = new FairFilteredPrimaryGenerator();
    fRun->SetGenerator(primGen);
    }else{
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);
    }

   // Box Generator: 
  if(Decfile=="box"){  
    PndBoxGenerator* boxGen = new PndBoxGenerator(3312, 1); // 13 = muon; 1 = multipl. // 211 = pi+
    // first number: PDG particle code: 2nd number: particle multiplicity per event
    
    boxGen->SetPRange(.1,.5); // GeV/c
    // boxGen->SetPtRange(1.,1.); // GeV/c
    boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree] 90
    boxGen->SetThetaRange(0., 180.); // Polar angle in lab system range [degree] 70
    
    boxGen->SetXYZ(0., 0., -55.0); // vertex coordinates [cm]
    primGen->AddGenerator(boxGen);
  }
  
  
  // *** Signal events GiBUU + TH2F(correlated dist in P (theta)) ***first step
  if(Decfile=="hist"){
    PndCorrDistGenerator* GiBGen = new PndCorrDistGenerator(inFile);
    
    primGen->SetTarget(-55.5,0.);
    primGen->AddGenerator(GiBGen);
    
  }
  // *** Signal events GiBUU + Param from TH2F(correlated dist in P (theta)) ***first step
  if(Decfile=="param"){
    PndCorrDistGenerator* GiBGen = new PndCorrDistGenerator(inFile);
    GiBGen->SetParam(); //setting parametrization to be be done.
    GiBGen->SetThetaRange(90.,120.); // low momentum Xi minus
    primGen->SetTarget(-55.5,0.);
    primGen->AddGenerator(GiBGen);
    
  }
  // *** SIGNAL events UrqmdSmm ***first step
  if(Decfile=="giBUU"){
    
    PndUrqmdSmmGenerator* AsciiGen = new PndUrqmdSmmGenerator(inFile);
    
    primGen->SetTarget(-55.5,0.);
    primGen->AddGenerator(AsciiGen);
  }

  if(Decfile=="giBUU+backg"){
    
    PndUrqmdSmmGenerator* AsciiGen = new PndUrqmdSmmGenerator(inFile);
    
    primGen->SetTarget(-55.5,0.);
    primGen->AddGenerator(AsciiGen);
    FairEvtFilterOnCounts* min1Lmd_1aLmd = new FairEvtFilterOnCounts();
    min1Lmd_1aLmd->AndMinMaxPdgCodes(1, 9999, 3312); // request at least 1 Xim
    min1Lmd_1aLmd->AndMinMaxPdgCodes(1, 9999, -3312); // request at least 1 anti-Xi plus
    primGen->AndNotFilter(min1Lmd_1aLmd);
  }
  
  // *** with Ascii inFile ***first step
  //FairAsciiGenerator* AsciiGen = new FairAsciiGenerator(inFile);
  // primGen->SetTarget(-55.5,0.);
  //primGen->AddGenerator(AsciiGen);
  
  // *** with root inFile ***second step
  if(Decfile=="hypbup"){
    
    PndHypBupGenerator* partGen = new PndHypBupGenerator(inFile);
    
    if(gam){
      partGen->GammaEmissPar(kTRUE);
      partGen->SetPRange(1.,1.);
      partGen->SetPhiRange(0,360);
      partGen->SetThetaRange(0,180);
    }
    primGen->AddGenerator(partGen);
  }
  
  
  

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
 if(Decfile=="param") output->open("/data/work/kpha4/lorente/sim_panda/Sim_hypFS1TG4Xm_SebGeo3params_"+prefix+".root");
 if(Decfile=="giBUU") output->open("/data/work/kpha4/lorente/sim_panda/Sim_hypFS1TG4Xm_GBparams.root");
 if(Decfile=="box") output->open("sim_boxparams.root");
  //output->open("Bup_hypFS1TG4XXbparams.root");
  if(Decfile=="hypbup")output->open("Bup_hypFS1TG4Xmparams.root");

  if(Decfile=="giBUU+backg")output->open("/data/work/kpha4/lorente/sim_panda/Sim_hypFS1TG4Bg_SebGeoparams_"+prefix+".root");
  
  rtdb->setOutput(output);
 
  /// rtdb->print();
  
  // Transport nEvents
  // -----------------
  // Set the number of events
  //Int_t nEvents =1000;//1002;//50505; 
  
  fRun->Run(nEvents);
  rtdb->saveOutput();
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  // delete fRun;
  //exit(0);
}  
  

