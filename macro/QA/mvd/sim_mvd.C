// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector
// to run the macro:
// root  sim_complete.C  or in root session root>.x  sim_complete.C
// to run with different options:(e.g more events, different momentum, Geant4)
// root  sim_complete.C"(100, "TGeant4",2)"

int sim_mvd(Int_t nEvents = 1000, TString  SimEngine ="TGeant3", Double_t BeamMomentum = 1.5)
{
  //-----User Settings:------------------------------------------------------
//  gDebug=5;
  TString parAsciiFile   = "all.par";

  TString prefix         = "mvdqa";     // prefix string for output files

  // TString inputGenerator =
  // EvtGen -> "xxxxxxxx.dec" (parses dec-file for initial particle) or "xxxxxxx.dec:initial_particle"
  // DPM    -> "dpm_xxxxx"
  // FTF    -> "ftf_xxxxx"
  // BOX    -> "box:type(pdgcode,mult):p(min,max):tht(min,max):phi(min,max)"
  // PIPI   -> "pipi:cosTheta(min,max)"
  // LEP    -> "leplep:pid(value):gegm(value):cosTheta(min,max)"

  //TString inputGenerator = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  //TString inputGenerator = "dpm";
  //TString inputGenerator = "ftf";
  TString inputGenerator = "box:type(211,1):p(1,1):tht(10,120):phi(0,360)";

  //-------------------------------------------------------------------------
  // -----   Create the Simulation run manager ------------------------------
  PndMasterRunSim *fRun = new PndMasterRunSim();
//  fRun->SetInput(inputGenerator);
  fRun->SetName(SimEngine);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->SetNumberOfEvents(nEvents);
  fRun->SetBeamMom(BeamMomentum);
  fRun->SetStoreTraj(kFALSE);
  // -----  Initialization   ------------------------------------------------
  fRun->Setup(prefix);
  // -----   Geometry   -----------------------------------------------------
  FairModule *Cave= new PndCave("CAVE");
   Cave->SetGeometryFileName("pndcave.geo");
   fRun->AddModule(Cave);
   //-------------------------  Magnet   -----------------
   // This part is commented because the MDT geometry contains the magnet now
   //FairModule *Magnet= new PndMagnet("MAGNET");
   //Magnet->SetGeometryFileName("FullSolenoid_V842.root");
   //Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
   //fRun->AddModule(Magnet);
   FairModule *Dipole= new PndMagnet("MAGNET");
   Dipole->SetGeometryFileName("dipole.geo");
   fRun->AddModule(Dipole);
   //-------------------------  Pipe     -----------------
   FairModule *Pipe= new PndPipe("PIPE");
   Pipe->SetGeometryFileName("beampipe_201309.root");
   fRun->AddModule(Pipe);

   //-------------------------  MVD       -----------------
   FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
   Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
   fRun->AddModule(Mvd);
  // -----   Event generator   ----------------------------------------------
   fRun->SetGenerator();

   FairPrimaryGenerator* primGen = fRun->GetPrimaryGenerator();

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

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddSimTasks();
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(nEvents);
  fRun->Finish();
  return 0;
}
