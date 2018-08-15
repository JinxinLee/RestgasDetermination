sim_dsk(
  TString  the_RunName = "test"
)
{
  TString  fGeantVersion = "TGeant4";              // simulation to run

  TString  fOutFile      = TString("./sim_dsk.").Append(the_RunName).Append(".root");
  TString  fMediaFile    = "media_pnd.geo";
  TString  fGeoFile      = "dsk.root";
  Int_t    fParticle     = 211; // pi+

  Double_t fPMin         = 2.;  // momentum in Gev/c for the created particle
  Double_t fPMax         = fPMin;

  Bool_t   fStoreGeoTraj   = kTRUE;
  Bool_t   fStoreDskTraj   = kTRUE;
  Bool_t   fStoreCerenkovs = kTRUE;
  Int_t    fNofEvents      = 1;

  // set coords
  Double_t fDiskDistanzZ = 196.;
  Double_t fHitX; Double_t fHitY; Double_t fHitZ; // [cm]
  Double_t fHitTheta; Double_t fHitPhi;           // [degree]
  Double_t fThetaMin; Double_t fThetaMax; Double_t fPhiMin; Double_t fPhiMax;
  TVector3 fVertexPos;
  // by angle
  fHitTheta = 15.;
  fHitPhi = 0.;
  fHitX = fDiskDistanzZ*TMath::Tan(fHitTheta*TMath::DegToRad())*TMath::Cos(fHitPhi*TMath::DegToRad());
  fHitY = fDiskDistanzZ*TMath::Tan(fHitTheta*TMath::DegToRad())*TMath::Sin(fHitPhi*TMath::DegToRad());
  fHitZ = fDiskDistanzZ;
  fVertexPos.SetXYZ(fHitX,fHitY,fHitZ);
  fVertexPos *= (fDiskDistanzZ-1.)/fDiskDistanzZ; // one cm before disk
  fThetaMin = fHitTheta;
  fThetaMax = fThetaMin;
  fPhiMin = fHitPhi;
  fPhiMax = fPhiMin;
  // by position
//   fHitX = 0.;
//   fHitY = 60.;
//   fHitZ = fDiskDistanzZ;
//   fVertexPos.SetXYZ(fHitX,fHitY,fHitZ);
//   fVertexPos *= (fDiskDistanzZ-1.)/fDiskDistanzZ; // one cm before disk
//   fHitTheta = fVertexPos.Theta()*TMath::RadToDeg();
//   fHitPhi = fVertexPos.Phi()*TMath::RadToDeg();
//   fThetaMin = fHitTheta;
//   fThetaMax = fThetaMin;
//   fPhiMin = fHitPhi;
//   fPhiMax = fPhiMin;
  // by range
//   fVertexPos.SetXYZ(0.,0.,0.);
//   fThetaMin = 15.;
//   fThetaMax = fThetaMin;
//   fPhiMin = 0.;
//   fPhiMax = fPhiMin;
  
  
  


  



  TStopwatch timer;
  timer.Start();
  gDebug=0;

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  rootlogon();
  basiclibs();

  gSystem->Load("libDsk");
  
  FairRunSim* fRun = 0;
  if (0 != (FairRunSim::Instance())) {
    cout << "[ ** ] FairRunSim instance found. Please restart root." << endl;
    return;
  } else {
    fRun = new FairRunSim();
  }

  fRun->SetName(fGeantVersion);
  fRun->SetOutputFile(fOutFile);
  fRun->SetMaterials(fMediaFile);

  // --- Modules ---------------------------------------------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  PndDsk* Dsk = new PndDsk("DSK", kTRUE);
  Dsk->SetGeometryFileName(fGeoFile);
  Dsk->SetStoreCerenkovs(fStoreCerenkovs);
  Dsk->SetStoreTrackPoints(fStoreDskTraj);
  fRun->AddModule(Dsk);

//   FairModule *Pipe= new PndPipe("PIPE");
//   Pipe->SetGeometryFileName("pipe.geo");
//   fRun->AddModule(Pipe);  
  
//   PndDrc *Drc = new PndDrc("DIRC", kTRUE);
//   Drc->SetRunCherenkov(kTRUE);
//   fRun->AddModule(Drc);
  
  
  
  // --- Generators ------------------------------------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  FairBoxGenerator* boxGen = new FairBoxGenerator(fParticle, 1);
  boxGen->SetXYZ(fVertexPos.X(),fVertexPos.Y(),fVertexPos.Z());
  boxGen->SetPRange(fPMin, fPMax); // momentum GeV/c
  boxGen->SetPhiRange(fPhiMin, fPhiMax); // Azimuth angle range [degree]
  boxGen->SetThetaRange(fThetaMin, fThetaMax); // Polar angle in lab system range [degree]
  primGen->AddGenerator(boxGen);

  // --- Trajectories ----------------------------------------------------------

  fRun->SetStoreTraj(fStoreGeoTraj);

  // --- Fields ----------------------------------------------------------------

  // --- Init ------------------------------------------------------------------

  fRun->Init();

  // --- RuntimeDB -------------------------------------------------------------

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
  output->open(TString("./simparams.").Append(the_RunName).Append(".root").Data());
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // --- Run -------------------------------------------------------------------

  fRun->Run(fNofEvents);

  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
