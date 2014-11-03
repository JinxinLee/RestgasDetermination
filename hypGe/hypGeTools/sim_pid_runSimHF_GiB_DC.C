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
//       "hypbup", second hyp step,  breakUp hyp/ pions from seq.decay emitted 

//        HYP_File== " true "//first step -->bup / pionic decay products -->root output
//
// 1.b ) second step , hypernuclei production "hypbup",
//2) detctor geo version
// 2.a) standard--> alicia version 
// 2.b) current --> sebastian version

void sim_pid_runSimHF_GiB_DC(int nEvents=10, int seed=17, int runNr=0, TString name="sim_pidC", 
TString path="data", int geoID=201922, TString Decfile = "param", TString vers = "current",Bool_t HYP_File= true){
  TStopwatch timer;
  timer.Start();
  gRandom->SetSeed(seed); 
  gDebug=0;

  // Load basic libraries
 
  
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  //rootlogon();
  
//  gSystem->Load("librazhyp");
  gSystem->Load("libHyp");
  
  FairRunSim *fRun = new FairRunSim();
  
  // --- first step ----

  // -- GiBUU Parametrization TH2F Corr. Dist P(theta)
  
  if(Decfile=="hist"||Decfile=="param") TString inFile= "XiGenGiBUUSamp.root";
  
  //--Second xi minus momentum solution.
  
  //TString inFile= "data/xibximin2AStpRate.dat";//data/ximinAsciiStpRate.dat";
  //TString inFile= "data/ximinAsciiStpRate.dat";

   // -- UrqmdSmm/GiBUU SIGNAL/ background events 
  
  if(Decfile=="giBUU") TString inFile= "carbon_onlyXi_2_5.root";

  //--- second step -- 
  //--- HF from vertexes ----
  
  if(Decfile=="hypbup"){
    TString inFile= "hypBupV1T_Decay.root";
  }
  

  // set the MC version used

 

  // ------------------------
  
  //fRun->SetName("TGeant4");
  fRun->SetName("TGeant3");
  
  TString outFile=path+"/"+name+".root";
   
  cout << "Datei: " << outFile << endl;
  fRun->SetOutputFile(outFile);
  
  
  // ----------first step -------------
  // ++++++ Xi minus production ++++++
  
  //if(Decfile=="param")fRun->SetOutputFile(outFile);

  // ----------first step XXb production -----------------------
  
  //fRun->SetOutputFile("Sim_hypFSG41TXXb_Geo2.root");

  // ---------- second step HF decay from secondary vertexes -------

  //fRun->SetOutputFile("Bup_hypFSG41TXXb.root");
  //if(Decfile=="hypbup")fRun->SetOutputFile("Bup_hypFSG41TXm.root");
  
  
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

  TString geoname;
  switch(geoID){
  case 904:
    geoname="SekTarget_open_varAbs4_3Q_HYPbe_1mm.root";
    break;
  case 9047:
    geoname="SekTarget_open_varAbs4_3Q_StartAbs_HYPbe_1mm.root";
    break;
  case 904745:
    geoname="SekTarget_open_varAbs4_3Q_StartAbs_HYPbe_1mm_thick5mm.root";
    break;
  case 9047410:
    geoname="SekTarget_open_varAbs4_3Q_StartAbs_HYPbe_1mm_thick10mm.root";
    break;
  case 905:
    geoname="SekTarget_open_varAbs4Si5_3Q_HYPbe_1mm.root";
    break;
  case 201922:  // T=20, S=19, Z(Ti)=22, former 2019
    geoname="TargetSystem_Ti.root";
    break;
  case 2019223:  // T=20, S=19, Z(Ti)=22, Corners=3,  former 20193
    geoname="TargetSystem_Ti_filledCorners.root";
    break;
  case 2019223765:  // T=20, S=19, Z(Ti)=22, Corners=3, Vertexposition -76.5
    geoname="TargetSystem_Ti_filledCorners_Vertex-765.root";
    break;
  case 20192212:  // T=20, S=19, Z(Ti)=22, long version +20.0: l = 12
    geoname="TargetSystem_Ti_z_elongated.root";
    break;
  case 201922701:  // T=20, S=19, Z(Ti)=22, Getauschte Lagen G=7
    geoname="TargetSystem_Ti_StartAbs_1mm.root";
    break;
  case 201922702:  // T=20, S=19, Z(Ti)=22, Getauschte Lagen G=7
    geoname="TargetSystem_Ti_StartAbs_2mm.root";
    break;
  case 201922705:  // T=20, S=19, Z(Ti)=22, Getauschte Lagen G=7
    geoname="TargetSystem_Ti_StartAbs_5mm.root";
    break;
  case 2019223701:  // T=20, S=19, Z(Ti)=22, Getauschte Lagen G=7
    geoname="TargetSystem_Ti_StartAbs_1mm_filledCorners.root";
    break;
  case 2019223702:  // T=20, S=19, Z(Ti)=22, Getauschte Lagen G=7
    geoname="TargetSystem_Ti_StartAbs_2mm_filledCorners.root";
    break;
  case 2019223705:  // T=20, S=19, Z(Ti)=22, Getauschte Lagen G=7
    geoname="TargetSystem_Ti_StartAbs_5mm_filledCorners.root";
    break;
  case 16100:  // P=16
    geoname="TargetSystem_Ti_pureAbsorberblock_100.root";
    break;
  case 16500:  // P=16
    geoname="TargetSystem_Ti_pureAbsorberblock_500.root";
    break;
  case 201925:  // T=20, S=19, Z(AlMg)=13+12=25
    geoname="TargetSystem_AlMg200.root";
    break;
  case 2019253:  // T=20, S=19, Z(AlMg)=13+12=25, Corners=3
    geoname="TargetSystem_AlMg200_filledCorners.root";
    break;
  case 201925100:  // T=20, S=19, Z(AlMg)=13+12=25, +100µm to default thickness
    geoname="TargetSystem_AlMg300.root";
    break;
  }

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
    Hyp->SetGeometryFileName(geoname);
     }

  // ---------------------------------

  if(HYP_File){ //first step -->bup / pionic decay products -->root output
    Hyp->SetTreeFName(path+"/hypBupV1T_Decay.root");
    Hyp->SetHypSDtoFile(true,HYP_File);
  }
  
    Hyp->SetStartEvID(runNr);
  
  
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
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  
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
    primGen->SetTarget(-55., 0.);
    primGen->AddGenerator(GiBGen);
    
  }
  // *** SIGNAL events UrqmdSmm ***first step
  if(Decfile=="giBUU"){
    
    PndUrqmdSmmGenerator* AsciiGen = new PndUrqmdSmmGenerator(inFile);
    
    primGen->SetTarget(-55.5,0.);
    primGen->AddGenerator(AsciiGen);
  }
  
  // *** with Ascii inFile ***first step
  //FairAsciiGenerator* AsciiGen = new FairAsciiGenerator(inFile);
  // primGen->SetTarget(-55.5,0.);
  //primGen->AddGenerator(AsciiGen);
  
  // *** with root inFile ***second step
  if(Decfile=="hypbup"){
    PndHypBupGenerator* partGen = new PndHypBupGenerator(inFile);
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
  if(nEvents<101)
  fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
  else
  fRun->SetStoreTraj(kFALSE);
  
  fRun->Init();
  
  /*FairTrajFilter* trajFilter = FairTrajFilter::Instance();
    trajFilter->SetStepSizeCut(0.001); // 1 cm
    //  trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
    // trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
    //  trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
    trajFilter->SetStorePrimaries(kTRUE);
    trajFilter->SetStoreSecondaries(kTRUE);*/ // not used for the others.????
  
  
  
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
  if(Decfile=="param") output->open(path+"/pidparams.root");//Sim_hypFS1TG4Xm_SebGeoparams.root");
  //output->open("Bup_hypFS1TG4XXbparams.root");
  if(Decfile=="hypbup") output->open(path+"/Bup_hypFS1TG4Xmparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
  
  // Transport nEvents
  // -----------------
  // Set the number of events
  //Int_t nEvents =31;//50505; 
  
  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  delete fRun;
  //exit(0);
}  
  
