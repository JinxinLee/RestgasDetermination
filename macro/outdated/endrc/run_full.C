// Macro for geant simulation of forward disk DIRC

// at the moment we use particles generated with FairBoxGenerator
// Vertex is set to (0,0,0)

{
  gROOT->Reset();

  // control the script
  TString  fOutFile      = "sim_endrcP.root";         // save data in workdir
  TString  fMediaFile    = "media_pnd.geo";        // media_pnd.geo
  TString  fGeoFile      = "endrc.root";              // geometry definition
  // TString  fGeoFile      = "endrc.geo";              // geometry definition
  TString  fGeantVersion = "TGeant3";              // simulation to run
  UInt_t   fSeed         = 0;                      // set seed to systemtime
  Double_t fPMin         = 4.;                     // momentum in Gev/c for the created particle
//   Double_t fPMax        = 9;
  Double_t fPMax         = fPMin;
  Double_t fThetaMin     = 20.;                    // between 10. and 22. degree will hit for sure
//   Double_t fThetaMax    = 22.;
  Double_t fThetaMax     = fThetaMin;
  Int_t    fEvents       = 1;                      // number of events, usually 1 at the moment to analyse
  Int_t    fParticles    = 1;                      // number of particles created in vertex
  Int_t   *fParticle     = new Int_t [fParticles]; // the particles we want to be generated
  Int_t    i=0;
//     fParticle[i++] = -211;   // pi -
//     fParticle[i++] =  111;   // pi 0
//     fParticle[i++] =  211;   // pi +
//     fParticle[i++] = -321;   // K-
//     fParticle[i++] =  311;   // K 0
//    fParticle[i++] =  321;   // K+
         fParticle[i++] = 2212;   // p

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  // random seed. Set to 0 for a random seed.
  gRandom->SetSeed(fSeed);

  // to measure runtime
  TStopwatch timer;
  timer.Start();
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libEnDrc");          // our library

  // stopp if there is already an instance
  // i cant find out how to use this one, so
  // we better restart root.
  // If we will go on, root will crash.
  FairRunSim* fRun = 0;
  if (0 != (FairRunSim::Instance())) {
    cout << "[ ** ] FairRunSim instance found. Please restart root." << endl;
    return;
  }
  else
    fRun = new FairRunSim();

  // setup versions and files
//   fRun->SetOutputFile(fOutFile.Data());
  fRun->SetOutputFile(fOutFile);
  fRun->SetName(fGeantVersion);
  fRun->SetMaterials(fMediaFile);

  // modules
  // the cave
  FairModule* Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);
  // the disk dirc
  FairDetector* Dsk = new PndEnDrc("EDRC", kTRUE);
  Dsk->SetGeometryFileName(fGeoFile);
  fRun->AddModule(Dsk);

  // create and set event generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  Double_t randx, randy;
  for (Int_t n =0; n<10; n++){
	randx= gRandom->Gaus(0,1);
	randy= gRandom->Gaus(0,1);
	FairParticleGenerator* partGen = new FairParticleGenerator(2212, 5, 0.3*randx, 0.3*randy, 0.3);
  	primGen->AddGenerator(partGen);
  }

 
//   for (Int_t p=0; p<fParticles; p++) {
//     FairBoxGenerator* boxGen = new FairBoxGenerator(fParticle[p], 1);
//     boxGen->SetBoxXYZ(0.,0.,0.,0.,0.); // create in vertex, no smearing
//     boxGen->SetPRange(fPMin, fPMax); // momentum GeV/c
//     boxGen->SetPhiRange(0., 0.); // Azimuth angle range [degree]
//     boxGen->SetThetaRange(fThetaMin, fThetaMax); // Polar angle in lab system range [degree]    
//     primGen->AddGenerator(boxGen);
//   }
//   // store the trajectories
  fRun->SetStoreTraj(kTRUE);

  // init the simulation
  fRun->Init();

  primGen->DoTracking(kTRUE);
  FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  // Set cuts for storing the trajectpries
  trajFilter->SetStepSizeCut(0.01); // 1 cm
//   trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
//   trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//   trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
  trajFilter->SetStorePrimaries(kTRUE);
  trajFilter->SetStoreSecondaries(kTRUE);

  // Fill the Parameter containers for this run
  //-------------------------------------------
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("simparams.root");

  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // run
  fRun->Run(fEvents);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}
