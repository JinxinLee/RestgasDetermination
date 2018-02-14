// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector
// to run the macro:
// root  sim_complete.C  or in root session root>.x  sim_complete_stt.C
// to run with different options:(e.g more events, different momentum, Geant4)
// root  sim_complete.C"(100, "TGeant4",2)"

sim(Int_t nEvents = 1, TString  SimEngine ="TGeant4", Float_t mom = 6.231552)
{

  //-----User Settings:-----------------------------------------------
  TString  OutputFile     ="sim.root";
  TString  ParOutputfile  ="simparams.root";
  TString  MediaFile      ="media_pnd.geo";
  gDebug                  = 0;
  TString digiFile        = "all.par"; //The emc run the hit producer directly
                                       // choose your event generator
  Bool_t UseEvtGenDirect      =kFALSE;
  Bool_t UseDpm 	      =kFALSE;
  Bool_t UseFtf 	      =kFALSE;
  Bool_t UseBoxGenerator      =kTRUE;
  
  Double_t BeamMomentum = 0.; // beam momentum ONLY for the scaling of the dipole field.
  if (UseBoxGenerator)
  {
    BeamMomentum   =4.0; // ** change HERE if you run Box generator
    mom = 4.;
  }
  else
  {
    BeamMomentum = mom;  // for DPM/EvtGen BeamMomentum is always = mom
  }
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
  //-------------------------  Magnet   -----------------
  //FairModule *Magnet= new PndMagnet("MAGNET");
  //Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  //Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  //fRun->AddModule(Magnet);

  //------------------------- Disc DIRC       ------------
  PndDisc* Disc = new PndDisc("DISC", kTRUE);
  Disc->SetVerboseLevel(2);
  Disc->SetGeometryFileName("DIRC_GEO_SIO2.root");
  Disc->SetFilterInterval(200., 800.);
  fRun->AddModule(Disc);
  
  // Create and Set Event Generator
  //-------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
	 
  if(UseBoxGenerator){	// Box Generator
    FairBoxGenerator* boxGen = new FairBoxGenerator(211, 1); //211 = pion, 2212 = proton 321 = kaon
    boxGen->SetPRange(mom,mom); // GeV/c
    double deg = TMath::Pi()/180.;
    double theta = 15.;
    double phi = 45.;
    double r = 193.9/cos(theta*deg);
    boxGen->SetPhiRange(phi, phi); // Azimuth angle range [degree]
    boxGen->SetThetaRange(theta, theta); // Polar angle in lab system range [degree]
    theta = theta*deg;
    phi = phi*deg;
    //boxGen->SetXYZ(r*sin(theta)*cos(phi), r*sin(theta)*sin(phi), r*cos(theta)); // cm
    //boxGen->SetXYZ(29.04, -42.71, 193.9); // cm
    boxGen->SetXYZ(0, 0, 0); // cm
    primGen->AddGenerator(boxGen);

    //FairBoxGenerator* boxGen2 = new FairBoxGenerator(321, 1);
    //boxGen2->SetPRange(mom,mom); // GeV/c
    //boxGen2->SetPhiRange(45., 45.); //Azimuth angle range [degree]
    //boxGen2->SetThetaRange(15., 15.); // Polar angle in lab system range [degree]
    //boxGen2->SetXYZ(0., 0., 0.); // cm
    //primGen->AddGenerator(boxGen2);

    fRun->SetStoreTraj(kTRUE); // to store particle trajectories 

    //Cherenkov photons for testing purpose
    /*FairBoxGenerator* boxGen = new FairBoxGenerator(50000050, 100); //50000050 pid code for cherenov photons
    boxGen->SetPRange(2.258e-9, 2.258e-9); // GeV/c
    boxGen->SetPhiRange(75., 75.); //Azimuth angle range [degree]
    boxGen->SetThetaRange(90.-32.5, 90.-32.5); // Polar angle in lab system range [degree]
    boxGen->SetXYZ(3.200016, 104.742558, 195.); // cm
    primGen->AddGenerator(boxGen);*/
  }
  if(UseDpm){
    PndDpmDirect *Dpm= new PndDpmDirect(mom,1);
    primGen->AddGenerator(Dpm);
  }
  if(UseFtf){
    //          TString macfile = gSystem->Getenv("VMCWORKDIR");
    //	  macfile += "/pgenerators/FtfEvtGen/PbarP.mac";
    //	  PndFtfDirect *Ftf = new PndFtfDirect(macfile.Data());
    PndFtfDirect *Ftf = new PndFtfDirect("anti_proton", "G4_H", 1, "ftfp", mom, 123456);
    primGen->AddGenerator(Ftf);
  }
  if(UseEvtGenDirect){
    TString  EvtInput =gSystem->Getenv("VMCWORKDIR");
    EvtInput+="/macro/run/psi2s_Jpsi2pi_Jpsi_mumu.dec";
    PndEvtGenDirect *EvtGen = new PndEvtGenDirect("pbarpSystem", EvtInput.Data(), mom);
    EvtGen->SetStoreTree(kTRUE);
    primGen->AddGenerator(EvtGen);
  }
  
  //---------------------Create and Set the Field(s)----------
  PndMultiField *fField= new PndMultiField("AUTO");
  fRun->SetField(fField);
  
  // EMC Hit producer
  //-------------------------------
  PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  fRun->AddTask(emcHitProd);
  
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
  
  //exit(0);
  
};

