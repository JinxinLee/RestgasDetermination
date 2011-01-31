//pi+ = 211, pi- = -211, mu+ = -13, mu- = 13, K+ = 321, K- = -321, K0L = 130
//pi0 = 111, gamma = 22, e- = 11, e+ = -11, proton = 2212, protonMin = -2212

void sim_sttcombi_pgun( int seed = 32579, int nEvents = 10, int pid  = 11,
			float p1 = 1.0, float p2 = 1.5,
			Char_t paramFile [] = "params_sttcombi.root", 
			Char_t outFile   [] = "points_sttcombi.root"
			)
{
  gRandom->SetSeed((seed + 10));
  
  std::cerr << "\nSeed is " << (seed + 10)
	    << "\npdg = "   << pid
	    << "\nnEvt = "  << nEvents
	    << "\n"         << std::endl;
  
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TString digiFile = "all.par";
  TString parFile = paramFile;
  
  FairRunSim* fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------
  fRun->SetName("TGeant3");
  //fRun->SetName("TGeant4");
  
  fRun->SetOutputFile(outFile);
  
  // Set the parameters
  //-------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);        
  Bool_t kParameterMerged = kTRUE;
  
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parFile);
  rtdb->setOutput(output);
  
  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------
  FairModule* Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  
  FairModule* Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  fRun->AddModule(Magnet);
  
  FairModule* Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);
  
  //FairModule* Pipe= new PndPipe("PIPE");
  //fRun->AddModule(Pipe);
  
  FairDetector* Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  fRun->AddModule(Stt);
  
  FairDetector* Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  fRun->AddModule(Mvd);

  PndEmc* Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryVersion(15);
  Emc->SetStorageOfData(kTRUE);
  fRun->AddModule(Emc);

  /*
    FairDetector* Tof = new PndTof("TOF",kTRUE);
    Tof->SetGeometryFileName("tofbarrel.geo");
    fRun->AddModule(Tof);
  */

  PndMdt* Muo = new PndMdt("MDT",kTRUE);
  Muo->SetBarrel("torino");
  Muo->SetEndcap("torino");
  Muo->SetMuonFilter("torino");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);
  
  FairDetector* Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations.root");
  fRun->AddModule(Gem);
  
  PndDsk* Dsk = new PndDsk("DSK", kTRUE);
  Dsk->SetGeometryFileName("dsk.root");
  Dsk->SetStoreCerenkovs(kFALSE);
  Dsk->SetStoreTrackPoints(kFALSE);
  fRun->AddModule(Dsk);
  
  PndDrc* Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
  fRun->AddModule(Drc);
  
  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  // Box Generator
  FairBoxGenerator* boxGen = new FairBoxGenerator(pid, 1); // 13 = muon; 1 = multipl.
  
  if (p2 < p1){
    p2 = p1;
  }  
  
  boxGen->SetPRange(p1, p2); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(5., 140.); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
  primGen->AddGenerator(boxGen); 

  // Set beam properties
  //-------------------------------
  //fRun->SetStoreTraj(kTRUE);
  fRun->SetStoreTraj(kFALSE);
  fRun->SetBeamMom(15);

  // Create and Set Magnetic Field
  //-------------------------------
  PndMultiField* fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  
  /**Initialize the session*/
  fRun->Init();
  
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
  
  // Transport nEvents
  // -----------------
  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  std::cout << "RealTime = " << rtime
	    << "seconds, CpuTime = " << ctime
	    << "seconds\n";
  exit(0);
}
