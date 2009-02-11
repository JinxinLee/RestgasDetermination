/**

  sim_dsk.C

  @brief Macro for geant simulation of forward disk DIRC

  Calculates refraction index and absorption lengths

  @author Peter Koch
  @date   2008-03-26
  @since  2007-11-26

**/

{
  gROOT->Reset();

  // control the script
  TString  fOutFile      = "sim_dsk.root";         // save data in workdir
  TString  fPathToGeo    = "../macro/dsk/";
  TString  fMediaFile    = TString(fPathToGeo).Append("dsk_media.geo");        // dsk_media.geo
//   TString  fGeoFile      = TString(fPathToGeo).Append("dsk.geo");              // geometry definition
//   TString  fGeoFile      = TString(fPathToGeo).Append("dsk_simple.geo");              // geometry definition
  TString  fGeoFile      = TString(fPathToGeo).Append("dsk_simple_working.geo");              // geometry definition
  TString  fGeantVersion = "TGeant4";              // simulation to run
  UInt_t   fSeed         = 1;                      // set seed to systemtime
  Double_t fPMin         = 4.;                     // momentum in Gev/c for the created particle
  Double_t fPMax         = fPMin;
  Double_t fThetaMin     = 15.;                    // between 10. and 22. degree will hit for sure
  Double_t fThetaMax     = fThetaMin;
  Double_t fPhiMin       = 270.;                   // its in degree!
  Double_t fPhiMax       = fPhiMin;
  Double_t fVertexSize   = 0.;                    // semearing for vertex -fVertexSize ... +fVertexSize
  Int_t    fEvents       = 1;                      // number of events, usually 1 at the moment to analyse
  Int_t    fParticles    = 1;                      // number of particles created in vertex
  Int_t   *fParticle     = new Int_t [fParticles]; // the particles we want to be generated
  Int_t    i=0;
    fParticle[i++] = -211;   // pi -
//     fParticle[i++] =  111;   // pi 0
//     fParticle[i++] =  211;   // pi +
//     fParticle[i++] = -321;   // K-
//     fParticle[i++] =  311;   // K 0
//     fParticle[i++] =  321;   // K+
//     fParticle[i++] = 2212;   // p

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  // random seed. Set to 0 for a random seed.
  gRandom->SetSeed(fSeed);

  // to measure runtime
  TStopwatch timer;
  timer.Start();

  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load the Disk-DIRC libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndBase");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
//   gSystem->Load("libStt");
  gSystem->Load("libDsk");          // our library

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
  // the stt for testing with an other module
//   FairDetector *Stt= new PndStt("STT", kTRUE);
//   Stt->SetGeometryFileName("straws_skewed_blocks.geo");
//   fRun->AddModule(Stt);
  // the disk dirc
  PndDsk* Dsk = new PndDsk("DSK", kTRUE);
  Dsk->SetGeometryFileName(fGeoFile);
  // 3 diffrent mirror types, 120 mirrors per edge
  Dsk->SetMirrors(3, 12);
  fRun->AddModule(Dsk);


  // create and set event generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  for (Int_t p=0; p<fParticles; p++) {
    FairBoxGenerator* boxGen = new FairBoxGenerator(fParticle[p], 1);
    boxGen->SetBoxXYZ(-fVertexSize,+fVertexSize,-fVertexSize,+fVertexSize,0.);
    boxGen->SetPRange(fPMin, fPMax); // momentum GeV/c
    boxGen->SetPhiRange(fPhiMin, fPhiMax); // Azimuth angle range [degree]
    boxGen->SetThetaRange(fThetaMin, fThetaMax); // Polar angle in lab system range [degree]
    primGen->AddGenerator(boxGen);
  }

  // store the trajectories
  fRun->SetStoreTraj(kTRUE);

  // init the simulation
  fRun->Init();

  primGen->DoTracking(kTRUE);
  FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  // Set cuts for storing the trajectpries
  trajFilter->SetStepSizeCut(0.01); // 1 cm
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
