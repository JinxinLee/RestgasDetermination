// Macro for running a test of the radiation length of PANDA detectors
// A single geantino particle is shot throught the detector. The length inside
// a geometry and the material properties are recorded
// to run the macro:
// root  sim_radlength_complete.C  or in root session root>.x  sim_radlength_complete.C

int sim_radlength_complete(TString  SimEngine ="TGeant3", Double_t BeamMomentum = 15)
{
  //-----User Settings:------------------------------------------------------
//  gDebug=5;
  TString parAsciiFile   = "all.par";

  TString prefix         = "radLength_geantinos";     // prefix string for output files
  Int_t nEvents = 0; //Automatically set by PndFixStepParticleGun
  TString inputGenerator = "PndFixStepParticleGun";
  

  //-------------------------------------------------------------------------
  // -----   Create the Simulation run manager ------------------------------
  PndMasterRunSim *fRun = new PndMasterRunSim();
  fRun->SetInput(inputGenerator);
  fRun->SetName(SimEngine);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->SetNumberOfEvents(nEvents);
  fRun->SetBeamMom(BeamMomentum);
  fRun->SetStoreTraj(kTRUE);
  fRun->SetRadLenRegister(kTRUE);
//  fRun->SetRadMapRegister(kTRUE);
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
   //-------------------------  STT       -----------------
   FairDetector *Stt= new PndStt("STT", kTRUE);
  // Stt->SetGeometryFileName("A_sttgeometry.root");
   Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
   fRun->AddModule(Stt);

   FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
   Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
   fRun->AddModule(Mvd);
//   //-------------------------  GEM       -----------------
   FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
   Gem->SetGeometryFileName("gem_3Stations_realistic_v2.root");
   fRun->AddModule(Gem);
//   //-------------------------  EMC       -----------------
   PndEmc *Emc = new PndEmc("EMC",kTRUE);
   Emc->SetGeometryVersion(1);
   Emc->SetStorageOfData(kFALSE);
   fRun->AddModule(Emc);
   //-------------------------  SCITIL    -----------------
   FairDetector *SciT = new PndSciT("SCIT",kTRUE);
   SciT->SetGeometryFileName("SciTil_201601.root");
   fRun->AddModule(SciT);
   //-------------------------  DRC       -----------------
   PndDrc *Drc = new PndDrc("DIRC", kTRUE);
   Drc->SetGeometryFileName("dirc_e3_b3_l6_m40.root");
   Drc->SetRunCherenkov(kFALSE);
   fRun->AddModule(Drc);
   //-------------------------  DISC      -----------------
   PndDsk* Dsk = new PndDsk("DSK", kTRUE);
   Dsk->SetStoreCerenkovs(kFALSE);
   Dsk->SetStoreTrackPoints(kFALSE);
   fRun->AddModule(Dsk);
   //-------------------------  MDT       -----------------
   PndMdt *Muo = new PndMdt("MDT",kTRUE);
   Muo->SetBarrel("fast");
   Muo->SetEndcap("fast");
   Muo->SetMuonFilter("fast");
   Muo->SetForward("fast");
   Muo->SetMdtMagnet(kTRUE);
   Muo->SetMdtCoil(kTRUE);
   Muo->SetMdtMFIron(kTRUE);
   fRun->AddModule(Muo);
   //-------------------------  FTS       -----------------
   FairDetector *Fts= new PndFts("FTS", kTRUE);
   Fts->SetGeometryFileName("fts.geo");
   fRun->AddModule(Fts);
   //-------------------------  FTOF      -----------------
   FairDetector *FTof = new PndFtof("FTOF",kTRUE);
   FTof->SetGeometryFileName("ftofwall.root");
   fRun->AddModule(FTof);
   //-------------------------  RICH       ----------------
   PndRich *Rich= new PndRich("RICH",kTRUE);
   Rich->SetGeometryFileName("rich_v313.root");
   fRun->AddModule(Rich);
  // -----   Event generator   ----------------------------------------------

   PndFixStepParticleGun* gen = new PndFixStepParticleGun();
   gen->SetDebug(kFALSE);
   gen->SetPDGType(0);
   gen->SetPRange(15,15,1);
   gen->SetThetaRange(1,179,1);
   gen->SetPhiRange(0,359,1);
   fRun->SetGenerator(gen);
   
   nEvents = gen->GetNEvents();
   std::cout << "******* NEvents() " << nEvents << " **********" << std::endl;

  // -----   Event filter setup   -------------------------------------------
  FairFilteredPrimaryGenerator *primGen = fRun->GetFilteredPrimaryGenerator();
  primGen->SetVerbose(0);

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddSimTasks();
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(nEvents);
  fRun->Finish();
  return 0;
}
