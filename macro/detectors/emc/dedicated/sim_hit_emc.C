// Macro created 20/09/2006 by S.Spataro
// It creates a geant simulation file for emc
/**
 *  Modified 17.12.08 by M. Al-Turany
 *  The hit producing is added to the simulation session, so that the EmcHits are produced on the fly 
 *  during the simulation
 */

{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
//  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
//  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
//  rootlogon();
//  basiclibs();
  
  FairRunSim *fRun = new FairRunSim();
  
  TString digiFile = "emc.par";
  
  TString parFile = "simparams.root";
  
  // Set the number of events
  Int_t nEvents = 10; 
  
  // set the MC version used
  // ------------------------
  
  fRun->SetName("TGeant3");
  //fRun->SetName("TGeant4");
  
  fRun->SetOutputFile("sim_hit_emc.root");
  
  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid.root");
  fRun->AddModule(Magnet);

  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);

  FairModule *Pipe= new PndPipe("PIPE");
  fRun->AddModule(Pipe);

  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  fRun->AddModule(Stt);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);

  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  //new BwEndCap + FwEndCap
  Emc->SetGeometryFileNameTriple("emc_module125.dat","emc_module3new.root","emc_module4_StraightGeo24.4.root"); //MapperVersion: 6
  fRun->AddModule(Emc);

  FairDetector *Tof = new PndTof("TOF",kTRUE);
  Tof->SetGeometryFileName("tofbarrel.geo");
  fRun->AddModule(Tof);

  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetBarrel("torino");
  Muo->SetEndcap("torino");
  Muo->SetMuonFilter("torino");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);

  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
  fRun->AddModule(Drc);
	
  // Create and Set Event Generator
  //-------------------------------
	
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  // Box Generator: 
  FairBoxGenerator* boxGen = new FairBoxGenerator(22, 5); // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event
  
  boxGen->SetPRange(1.,1.); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
  primGen->AddGenerator(boxGen);  
  
  // fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
  
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
  
  	
  /**Set the digitization parameters */
  
  TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
  emcDigiFile += "/macro/params/";
  emcDigiFile += digiFile;
  /**Get the run time data base for this session and set the needed input*/
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(emcDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);        
  Bool_t kParameterMerged=kTRUE;
	
  /**Parameters created for this simulation goes to the out put*/
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("simparams.root");
  rtdb->setOutput(output);
  
  
  
  /**
     Add Hit producer task to the simulation 
  */
  PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  /* If you want the HitProducer to simulate a nonuniform lightoutput you have
   * to enable it by setting Use_nonuniformity in PndEmcDigiPar to 1.
   * The parameters for the nonuniformity will be read by default from: */
  //emcHitProd->SetNonuniformityFile("${VMCWORKDIR}/input/EmcDigiNoniformityPars.root");
  fRun->AddTask(emcHitProd);
  
  
  /**Initialize the session*/
  fRun->Init();
	
  /**After initialization now we can save the field parameters */
  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();
  
  /**All parameters are initialized and ready to be saved*/
  rtdb->saveOutput();
  //rtdb->print();
  
  // Transport nEvents
  // -----------------
  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}

