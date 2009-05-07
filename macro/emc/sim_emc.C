// Macro created 20/09/2006 by S.Spataro
// It creates a geant simulation file for emc
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  rootlogon();
  basiclibs();
  
  FairRunSim *fRun = new FairRunSim();
  
  // Set the number of events
  Int_t nEvents = 10; 
  
  // set the MC version used
  // ------------------------
  
   fRun->SetName("TGeant3");
   //fRun->SetName("TGeant4");
  
   fRun->SetOutputFile("sim_emc.root");


   // Set Material file Name
  //-----------------------
   fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

   //FairModule *Magnet= new PndMagnet("MAGNET");
   //Magnet->SetGeometryFileName("magnet.geo");
   //fRun->AddModule(Magnet);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  fRun->AddModule(Pipe);

   //FairDetector *Emc = new PndEmc("EMC",kTRUE);
   PndEmc *Emc = new PndEmc("EMC",kTRUE);
   //Emc->SetGeometryFileName("emc_module12345.dat"); // if you want to use old geometry for FwEndCap & BwEndCap
   //Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root"); // obsolete!! if you want to use new geometry for FwEndCap
   Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root",0); //0 -means: *1245.dat + *.root -> new forward endcap, Mapper==2
   //Emc->SetGeometryFileNameDouble("emc_module1235.dat","emc_module4_StraightGeo26.root",1); //1 -means: *1235.dat + *.root -> new backward endcap, Mapper==7
   //
   // For testing a new version of Backward End-Cap (BwEndCap):
   // a) with the forward endcap geometry: "emc_module4_FwEndCapGeo.root"
   // b) with straight crystals 26mm x 26mm x 200mm: "emc_module4_StraightGeo26.root"
   // you can use SetGeometryFileName() or SetGeometryFileNameDouble() or SetGeometryFileNameTriple(), 
   // depends on the EMC geometry compination, like below:
   // 
   // Emc->SetGeometryFileName("emc_module4_StraightGeo26.root");    //only new BwEndCap, MapperVersion: 6
   // Emc->SetGeometryFileName("emc_module4_StraightGeo26_Al.root"); //only new BwEndCap with dead material in front, MapperVersion: 6
   // Emc->SetGeometryFileNameDouble("emc_module1235.dat","emc_module4_StraightGeo26.root",1); //*1235.dat + _new_ BwEndCap, MapperVersion: 7
   // Emc->SetGeometryFileNameTriple("emc_module125.dat","emc_module3new.root","emc_module4_StraightGeo26.root"); //*125.dat + _new_ FwEndCap + _new_ BwEndCap, MapperVersion: 6
   // 
   // MapperVersion you need to set in the "emc.par" file in ../trunk/macro/params/emc.par
   fRun->AddModule(Emc);
  

  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator: 
  FairBoxGenerator* boxGen = new FairBoxGenerator(22, 1); // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event
  
  boxGen->SetPRange(1.,1.); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
   boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
  primGen->AddGenerator(boxGen);  
  
  //fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
   fRun->SetBeamMom(15);
   PndMultiField *fField= new PndMultiField();

   PndTransMap *map_t= new PndTransMap("TransMap", "R");
   PndDipoleMap *map_d1= new PndDipoleMap("DipoleMap1", "R");
   PndDipoleMap *map_d2= new PndDipoleMap("DipoleMap2", "R");
   PndSolenoidMap *map_s1= new PndSolenoidMap("SolenoidMap1", "R");
   PndSolenoidMap *map_s2= new PndSolenoidMap("SolenoidMap2", "R");
   PndSolenoidMap *map_s3= new PndSolenoidMap("SolenoidMap3", "R");
   PndSolenoidMap *map_s4= new PndSolenoidMap("SolenoidMap4", "R");

   fField->AddField(map_t);
   fField->AddField(map_d1);
   fField->AddField(map_d2);
   fField->AddField(map_s1);
   fField->AddField(map_s2);
   fField->AddField(map_s3);
   fField->AddField(map_s4);


   fRun->SetField(fField);


   fRun->Init();
  
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
   FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
   Bool_t kParameterMerged=kTRUE;
  
  
    PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
    if (fField) {  Par->SetParameters(fField); }
    Par->setInputVersion(fRun->GetRunId(),1);
    Par->setChanged();
  
   FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
   output->open("simparams.root");
   
   rtdb->setOutput(output);
   rtdb->saveOutput();
   rtdb->print();


  // Transport nEvents
  // -----------------
   fRun->Run(nEvents);
  
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
}  
  
