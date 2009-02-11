// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndBase");
  gSystem->Load("libField");
  gSystem->Load("libPassive");

  gSystem->Load("libMvd");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libGen");
  gSystem->Load("libgenfit");
  gSystem->Load("libtpc");
  gSystem->Load("librecotasks");


  gSystem->Load("libPGen");

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant4");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  

  //fRun->SetOutputFile("data/pion.15deg.mc.root");
  //fRun->SetOutputFile("data/lambdas.mc.root");
  //fRun->SetOutputFile("2pions.1GeV.mc.root");
  //fRun->SetOutputFile("data/pions.mc.root");
  fRun->SetOutputFile("data/testChamber.mc.root");

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  

  std::cout<< "Materials set" << std::endl;
  

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new CbmCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  //FairModule *Pipe= new CbmPipe("PIPE");
  //Pipe->SetGeometryFileName("pipe.geo");
  //fRun->AddModule(Pipe);
  
  
  //FairModule *Magnet= new CbmMagnet("MAGNET");
  //Magnet->SetGeometryFileName("magnet.geo");
  //fRun->AddModule(Magnet);
  
  FairDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetGeometryFileName("tpcTestChamber.geo");
  fRun->AddModule(PndTpc);
  
  //FairDetector *Sts= new CbmTst("TST", kTRUE);
  //Sts->SetGeometryFileName("tst_mvd.geo");
  //fRun->AddModule(Sts);


  //FairDetector *Mvd = new MvdMCDetector("MVD", kTRUE);
  //Mvd->SetGeometryFileName("MVD_Rev14b_Corr+Dead.geo");
  //fRun->AddModule(Mvd);
  
  //FairDetector *Emc = new CbmEmc("EMC",kTRUE);
  //Emc->SetGeometryFileName("emc_module1234.dat");
  //fRun->AddModule(Emc);

  //FairDetector *Drc = new CbmDrc("DIRC", kTRUE);
  //Drc->SetGeometryFileName("dirc.geo");
  //fRun->AddModule(Drc);

 //FairModule *Target= new CbmTarget("Target");
 //Target->SetGeometryFileName("target_vacuum.geo");
 //fRun->AddModule(Target);		

 //FairDetector *Tof= new CbmTof("TOF", kTRUE );
 //Tof->SetGeometryFileName("tof.geo");
 //fRun->AddModule(Tof);
	
 //FairDetector *Trd= new CbmTrd("TRD",kTRUE );
 //Trd->SetGeometryFileName("trd_9.geo");
 //fRun->AddModule(Trd);

 // FairDetector *Rich= new CbmRich("RICH", kTRUE);
 // Rich->SetGeometryFileName("rich.geo");
 // fRun->AddModule(Rich);

  //FairDetector *Ecal= new CbmEcal("ECAL", kTRUE);
  //Ecal->SetGeometryFileName("ecal.geo");
  //fRun->AddModule(Ecal);


 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

 
 // Box Generator

  // pdgs 211=pion 13=muon 11=electron
  FairBoxGenerator* boxGen = new FairBoxGenerator(13, 1); // 13 = muon; 1 = multipl.
   boxGen->SetPRange(0.2,1.5); // GeV/c //setPRange vs setPtRange
   boxGen->SetPhiRange(0,360); // Azimuth angle range [degree]
   boxGen->SetThetaRange(0, 5); // Polar angle in lab system range [degree]
   boxGen->SetXYZ(0., 0., -100.); // mm o cm ??
   primGen->AddGenerator(boxGen);

  //FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  //fRun->SetGenerator(primGen);


  FairEvtGenGenerator* evtGen = new FairEvtGenGenerator("data/lambdas.evt");
  //primGen->AddGenerator(evtGen);  

  //FairEvtGenGenerator* evtGen = new FairEvtGenGenerator("pgenerators/EvtGen/etacphiphi100.evt");
  //primGen->AddGenerator(evtGen);  

 // Field Map Definition
 // --------------------
  // 1- Reading the new field map in the old format

     // FairFieldMap *fMagField= new FairFieldMap("FIELD.v04_pavel.map");
   // Constant Field
      PndConstField *fMagField=new PndConstField();
      fMagField->SetField(0, 0 ,20. ); // values are in kG
     // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
  fMagField->SetFieldRegion(-50, 50,-50, 50, -200, 200);

  // 2- Reading the new field map in the new format

//  FairField *fMagField= new FairFieldMapSym3("FieldActive");
  // Active Shielding

  //fRun->SetField(fMagField);
   
   fRun->SetStoreTraj(kTRUE);
   //fRun->SetStoreTraj(kFALSE);
    

   fRun->Init();


 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------

;
 // Set cuts for storing the trajectpries
//   FairTrajFilter* trajFilter = FairTrajFilter::Instance();
//   trajFilter->SetStepSizeCut(0.01); // 1 cm
//   trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
//   trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//   trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
//   trajFilter->SetStorePrimaries(kTRUE);
//   trajFilter->SetStoreSecondaries(kTRUE);
   

  // Fill the Parameter containers for this run
  //-------------------------------------------

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("demoparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------

  Int_t nEvents = 100;
  fRun->Run(nEvents);



  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
 printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
