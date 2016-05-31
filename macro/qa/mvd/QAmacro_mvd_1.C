// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector
// to run the macro:
// root  sim_complete.C  or in root session root>.x  sim_complete_stt.C
// to run with different options:(e.g more events, different momentum, Geant4)
// root  sim_complete.C"(100, "TGeant4",2)"

void QAmacro_mvd_1(Int_t nEvents = 10, TString  SimEngine ="TGeant3", Float_t mom = 6.231552)
{
  //-----User Settings:-----------------------------------------------
  TString  OutputFile     ="mvdqasim.root";
  TString  ParOutputfile  ="mvdqapar.root";
  TString  MediaFile      ="media_pnd.geo";
  gDebug                  = 0;
  TString digiFile        = "all.par"; //The emc run the hit producer directly
                                       // choose your event generator
    BeamMomentum   =15.0; // ** change HERE if you run Box generator
  //------------------------------------------------------------------
  TLorentzVector fIni(0, 0, mom, sqrt(mom*mom+9.3827203e-01*9.3827203e-01)+9.3827203e-01);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem","pbarpSystem",fIni.M(),kFALSE,0.1,0, "",88888);
  //------------------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  gRandom->SetSeed();
  
  // Create the Simulation run manager--------------------------------
  FairRunSim *fRun = new FairRunSim();
  fRun->SetName(SimEngine.Data() );
  fRun->SetOutputFile(OutputFile.Data());
  fRun->SetGenerateRunInfo(kFALSE);
  fRun->SetBeamMom(BeamMomentum);
  fRun->SetMaterials(MediaFile.Data());
  fRun->SetUseFairLinks(kTRUE);
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  
  // Set the parameters
  //-------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;
  
  
  //-------Set the parameter output --------------------
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);
  
  //---------------------Set Parameter output      ----------
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(ParOutputfile.Data());
  rtdb->setOutput(output);
  
  // Create and add detectors
  
  //-------------------------  CAVE      -----------------
  
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);
  //-------------------------  Pipe     -----------------
  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201309.root");
  fRun->AddModule(Pipe);
  //-------------------------  MVD       -----------------
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  fRun->AddModule(Mvd);
  

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
	 
    FairBoxGenerator* boxGen = new FairBoxGenerator(211, 2); // 13 = muon; 1 = multipl.
    boxGen->SetPRange(0.5,2.5); // GeV/c
    boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
    boxGen->SetThetaRange(5., 150.); // Polar angle in lab system range [degree]
    boxGen->SetXYZ(0., 0., 0.); // cm
    primGen->AddGenerator(boxGen);

    FairBoxGenerator* boxGen2 = new FairBoxGenerator(-211, 2);
    boxGen2->SetPRange(0.5,2.5);
    boxGen2->SetPhiRange(0.,360.);
    boxGen2->SetThetaRange(5., 150.);
    primGen->AddGenerator(boxGen2);
  
PndMultiField *fField= new PndMultiField("AUTO");
  fRun->SetField(fField);
  
  // EMC Hit producer
  //-------------------------------
  
  //-------------------------  Initialize the RUN  -----------------
  fRun->Init();
  //-------------------------  Run the Simulation  -----------------
  fRun->Run(nEvents);
  //-------------------------  Save the parameters -----------------
  rtdb->saveOutput();
  //------------------------Print some info and exit----------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  delete fRun;  
  exit(0);
  
};

