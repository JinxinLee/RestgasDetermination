// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector
// to run the macro:
// root  sim_complete.C  or in root session root>.x  sim_complete.C
// to run with different options:(e.g more events, different momentum, Geant4)
// root  sim_complete.C"(100, "TGeant4",2)"

int sim_complete(Int_t nEvents = 100, TString  SimEngine ="TGeant3", Double_t BeamMomentum = 1.5)
{
  //-----User Settings:------------------------------------------------------
//  gDebug=5;
  TString parAsciiFile   = "all.par";

  TString prefix         = "evtcomplete";     // prefix string for output files

  // TString inputGenerator =
  // EvtGen -> "xxxxxxxx.dec" (parses dec-file for initial particle) or "xxxxxxx.dec:initial_particle"
  // DPM    -> "dpm_xxxxx"
  // FTF    -> "ftf_xxxxx"
  // BOX    -> "box:type(pdgcode,mult):p(min,max):tht(min,max):phi(min,max)"
  // PIPI   -> "pipi:cosTheta(min,max)"
  // LEP    -> "leplep:pid(value):gegm(value):cosTheta(min,max)"

  //TString inputGenerator = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  //TString inputGenerator = "dpm";
  TString inputGenerator = "ftf";
  //TString inputGenerator = "box:type(211,1):p(1,1):tht(10,120):phi(0,360)";

  //-------------------------------------------------------------------------
  // -----   Create the Simulation run manager ------------------------------
  PndMasterRunSim *fRun = new PndMasterRunSim();
  fRun->SetInput(inputGenerator);
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
   //-------------------------  STT       -----------------
   FairDetector *Stt= new PndStt("STT", kTRUE);
   Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
   fRun->AddModule(Stt);
   //-------------------------  MVD       -----------------
   FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
   Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
   fRun->AddModule(Mvd);
   //-------------------------  GEM       -----------------
   FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
   Gem->SetGeometryFileName("gem_3Stations_realistic_v2.root");
   fRun->AddModule(Gem);
   //-------------------------  EMC       -----------------
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
  fRun->SetGenerator();

  // -----   Event filter setup   -------------------------------------------
  FairFilteredPrimaryGenerator *primGen = fRun->GetFilteredPrimaryGenerator();
  primGen->SetVerbose(0);
  // ---- Example configuration for the event filter ------------------------
  //FairEvtFilterOnSingleParticleCounts* chrgFilter = new FairEvtFilterOnSingleParticleCounts("chrgFilter");
  //chrgFilter->AndMinCharge(4, FairEvtFilter::kCharged);
  //primGen->AndFilter(chrgFilter);

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddSimTasks();
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(nEvents);
  fRun->Finish();
  return 0;
}
