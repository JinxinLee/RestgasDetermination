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
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");

  gSystem->Load("libMvd");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libGen");
  gSystem->Load("libgenfit");
  gSystem->Load("libtpc");
  
  gSystem->Load("pgenerators/DpmEvtGen/lib/libDpmEvtGen");
  gSystem->Load("libPGen");

  CbmRunSim *fRun = new CbmRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  Int_t nEvents = 100;

  TString PANDAMC=gSystem->Getenv("PANDAMC");

  TString jobname="4PART";
  TString jobdir="../data/";
  jobdir.ReplaceAll("$PANDAMC",PANDAMC);
  jobdir+=jobname;
  jobdir+="/";
  TString cmd="mkdir ";
  cmd+=jobdir;
  if(gSystem->Exec(cmd)) {
    std::cout<<"Could not create Job-Directory "<<jobdir<<". Aborting."<<std::endl;
    return;
  }

  TString base=jobdir;base+="test1";
  TString outfile=base+".mc.root";
  TString dbfile=base+".param.root";

  fRun->SetOutputFile(outfile);
  

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  


  // Create and add detectors
  //-------------------------

  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  //CbmModule *Pipe= new CbmPipe("PIPE");
  //Pipe->SetGeometryFileName("pipe.geo");
  //fRun->AddModule(Pipe);
  
  
  CbmModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);
  
  CbmDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetGeometryFileName("tpc.geo");
  fRun->AddModule(PndTpc);
  
  //CbmDetector *Sts= new CbmTst("TST", kTRUE);
  //Sts->SetGeometryFileName("tst_mvd.geo");
  //fRun->AddModule(Sts);


  CbmDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD14.root");
  fRun->AddModule(Mvd);
  
  //CbmDetector *Emc = new CbmEmc("EMC",kTRUE);
  //Emc->SetGeometryFileName("emc_module1234.dat");
  //fRun->AddModule(Emc);

  //CbmDetector *Drc = new CbmDrc("DIRC", kTRUE);
  //Drc->SetGeometryFileName("dirc.geo");
  //fRun->AddModule(Drc);

 //CbmModule *Target= new CbmTarget("Target");
 //Target->SetGeometryFileName("target_vacuum.geo");
 //fRun->AddModule(Target);		

 //CbmDetector *Tof= new CbmTof("TOF", kTRUE );
 //Tof->SetGeometryFileName("tof.geo");
 //fRun->AddModule(Tof);
	
 //CbmDetector *Trd= new CbmTrd("TRD",kTRUE );
 //Trd->SetGeometryFileName("trd_9.geo");
 //fRun->AddModule(Trd);

 // CbmDetector *Rich= new CbmRich("RICH", kTRUE);
 // Rich->SetGeometryFileName("rich.geo");
 // fRun->AddModule(Rich);

  //CbmDetector *Ecal= new CbmEcal("ECAL", kTRUE);
  //Ecal->SetGeometryFileName("ecal.geo");
  //fRun->AddModule(Ecal);

  
 // Create and Set Event Generator
 //-------------------------------
 std::cout<<"Setup EvtGens"<<std::endl;std::cout.flush();
 CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
 fRun->SetGenerator(primGen);

 
 // Box Generator

  // pdgs 211=pion 13=muon 11=electron
  CbmBoxGenerator* boxGen = new CbmBoxGenerator(211, 4); // 13 = muon; 1 = multipl.
   boxGen->SetPRange(.5,1.5); // GeV/c //setPRange vs setPtRange
   boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
   boxGen->SetThetaRange(25, 50); // Polar angle in lab system range [degree]
   boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
   primGen->AddGenerator(boxGen);

  //CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  //fRun->SetGenerator(primGen);

   // DPM
   //PndDpmGenerator* dpmGen = new PndDpmGenerator("data/dpm2GeV.root");
   //primGen->AddGenerator(dpmGen);  
   
   
   //CbmEvtGenGenerator* evtGen = new CbmEvtGenGenerator("../data/evtgen.y4260.jpsipipi.vvpipi.dat");
   //primGen->AddGenerator(evtGen);  

  //CbmEvtGenGenerator* evtGen = new CbmEvtGenGenerator("pgenerators/EvtGen/etacphiphi100.evt");
  //primGen->AddGenerator(evtGen);  

 // Field Map Definition
 // --------------------
  // 1- Reading the new field map in the old format

     // CbmFieldMap *fMagField= new CbmFieldMap("FIELD.v04_pavel.map");
   // Constant Field
      PndConstField *fMagField=new PndConstField();
      fMagField->SetField(0, 0 ,20. ); // values are in kG
     // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
  fMagField->SetFieldRegion(-50, 50,-50, 50, -2000, 2000);

  // 2- Reading the new field map in the new format

//  CbmField *fMagField= new CbmFieldMapSym3("FieldActive");
  // Active Shielding

  fRun->SetField(fMagField);
   
   fRun->SetStoreTraj(kTRUE);
   //fRun->SetStoreTraj(kFALSE);
    
   std::cout<<"Starting INIT"<<std::endl;
   fRun->Init();
   std::cout<<"Ending INIT"<<std::endl;
   std::cout.flush();

 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------

;
 // Set cuts for storing the trajectpries
//   CbmTrajFilter* trajFilter = CbmTrajFilter::Instance();
//   trajFilter->SetStepSizeCut(0.01); // 1 cm
//   trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
//   trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//   trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
//   trajFilter->SetStorePrimaries(kTRUE);
//   trajFilter->SetStoreSecondaries(kTRUE);
   

  // Fill the Parameter containers for this run
  //-------------------------------------------

  CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
  output->open(dbfile.Data());
  rtdb->setOutput(output);

  PndConstPar* fieldPar = (PndConstPar*) rtdb->getContainer("PndConstPar");
  if ( fMagField ) {  fieldPar->SetParameters(fMagField); }
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  fieldPar->setChanged(kTRUE);

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
  
