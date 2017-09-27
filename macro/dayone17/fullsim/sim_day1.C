// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector
// to run the macro:
// root  sim_complete.C  or in root session root>.x  sim_complete.C
// to run with different options:(e.g more events, different momentum, Geant4)
// root  sim_day1.C"(100, "TGeant4",2)"

int sim_day1(Int_t nEvents = 100, TString  SimEngine ="TGeant3", Double_t BeamMomentum = 6.231552)
{
  //-----User Settings:------------------------------------------------------
  TString parAsciiFile   = "all.par";
  TString options        = "day1+gem2+strip"; //with fts1234
  //TString options        = "day1+gem2+strip+fts1256";
  TString prefix         = "evtday1";     // prefix string for output files

  // TString inputGenerator =
  // EvtGen -> "xxxxxxxx.dec" (parses dec-file for initial particle) or "xxxxxxx.dec:initial_particle"
  // DPM    -> "dpm_xxxxx"
  // FTF    -> "ftf_xxxxx"
  // BOX    -> "box:type(pdgcode,mult):p(min,max):tht(min,max):phi(min,max)"

  TString inputGenerator = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  //TString inputGenerator = "dpm";
  //TString inputGenerator = "ftf"; //caution, works only with Geant3 rihgt now (02/2017)
  //TString inputGenerator = "box:type(211,1):p(1,1):tht(10,120):phi(0,360)";

  //-------------------------------------------------------------------------
  // -----   Create the Simulation run manager ------------------------------
  PndMasterRunSim *fRun = new PndMasterRunSim();
  fRun->SetOptions(options);
  fRun->SetInput(inputGenerator);
  fRun->SetName(SimEngine);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->SetNumberOfEvents(nEvents);
  fRun->SetBeamMom(BeamMomentum);
  // -----  Initialization   ------------------------------------------------
  fRun->Setup(prefix);
   // -----   Geometry   -----------------------------------------------------
  fRun->CreateGeometry();
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
  if (gROOT->GetVersionInt() >= 60602) {
    gGeoManager->GetListOfVolumes()->Delete();
    gGeoManager->GetListOfShapes()->Delete();
    delete gGeoManager;
  }
  return 0;
}

