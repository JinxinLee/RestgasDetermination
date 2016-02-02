// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector
// to run the macro:
// root  sim_complete.C  or in root session root>.x  sim_complete_stt.C
// to run with different options:(e.g more events, different momentum, Geant4)
// root  sim_complete.C"(100, "TGeant4",2)"

sim_complete(Int_t nEvents = 100, TString  SimEngine ="TGeant3", Float_t mom = 6.231552)
{
  //-----User Settings:-----------------------------------------------
  TString  OutputFile     ="sim_complete.root";
  TString  ParOutputfile  ="simparams.root";
  TString  MediaFile      ="media_pnd.geo";
  gDebug                  = 0;
  TString digiFile        = "all.par"; //The emc run the hit producer directly
                                       // choose your event generator
  Bool_t UseEvtGenDirect      =kTRUE;
  Bool_t UseDpm 	      =kFALSE;
  Bool_t UseFtf 	      =kFALSE;
  Bool_t UseBoxGenerator      =kFALSE;
  
  Double_t BeamMomentum = 0.; // beam momentum ONLY for the scaling of the dipole field.
  if (UseBoxGenerator)
  {
    BeamMomentum   =15.0; // ** change HERE if you run Box generator
  }
  else
  {
    BeamMomentum = mom;  // for DPM/EvtGen BeamMomentum is always = mom
  }
  //------------------------------------------------------------------
  TLorentzVector fIni(0, 0, mom, sqrt(mom*mom+9.3827203e-01*9.3827203e-01)+9.3827203e-01);
  TDatabasePDG::Instance()->AddParticle("pbarpSystem","pbarpSystem",fIni.M(),kFALSE,0.1,0, "",88888);
  //------------------------------------------------------------------
  // Create the Simulation run manager--------------------------------
  PndMasterRunSim *fRun = new PndMasterRunSim();
  fRun->SetName(SimEngine.Data() );
  fRun->SetOutputFile(OutputFile.Data());
  fRun->SetBeamMom(BeamMomentum);
  fRun->Setup();
  fRun->CreateGeometry();
  
  // Create and Set Event Generator
  //-------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
	 
  if(UseBoxGenerator){	// Box Generator
    FairBoxGenerator* boxGen = new FairBoxGenerator(22, 5); // 13 = muon; 1 = multipl.
    boxGen->SetPRange(mom,mom); // GeV/c
    boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
    boxGen->SetThetaRange(0., 90.); // Polar angle in lab system range [degree]
    boxGen->SetXYZ(0., 0., 0.); // cm
    primGen->AddGenerator(boxGen);
  }
  if(UseDpm){
    PndDpmDirect *Dpm= new PndDpmDirect(mom,1);
    primGen->AddGenerator(Dpm);
  }
  if(UseFtf){
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
  
  // EMC Hit producer
  //-------------------------------
  PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  fRun->AddTask(emcHitProd);
  
  //-------------------------  Initialize the RUN  -----------------
  fRun->Init();
  //-------------------------  Run the Simulation  -----------------
  fRun->Run(nEvents);
  
  fRun->Finish(); 
  //exit(0);
  
};

