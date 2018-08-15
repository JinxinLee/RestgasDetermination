// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector
// to run the macro:
// root  sim_complete.C  or in root session root>.x  sim_complete.C
// to run with different options:(e.g more events, different momentum, Geant4)
// root  sim_complete.C"(100, "TGeant4",2)"

int sim_complete(Int_t nEvents = 1000, TString  SimEngine ="TGeant3", Double_t BeamMomentum = 6.231552)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  // TString inputGenerator = 
  // EvtGen -> "xxxxxxxx.dec"
  // DPM    -> "dpm_xxxxx"
  // FTF    -> "ftf_xxxxx"
  TString  inputGenerator = "psi2s_Jpsi2pi_Jpsi_mumu.dec"; 
  TString  inputDir = gSystem->Getenv("VMCWORKDIR");
  inputDir += "/macro/QA/run3/";
  gRandom->SetSeed(1234);
  //-------------------------------------------------------------------------
  // -----   Create the Simulation run manager ------------------------------
  PndMasterRunSim *fRun = new PndMasterRunSim();
  fRun->SetInput(inputGenerator);
  fRun->SetInputDir(inputDir);
  fRun->SetName(SimEngine);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->SetNumberOfEvents(nEvents);
  fRun->SetBeamMom(BeamMomentum);
  // -----  Initialization   ------------------------------------------------
  fRun->Setup();
  // -----   Geometry   -----------------------------------------------------
  fRun->CreateGeometry();
  // -----   Event generator   ----------------------------------------------
  fRun->SetGenerator();
  // -----   Add tasks   ----------------------------------------------------
  fRun->AddSimTasks();
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(nEvents); 
  fRun->Finish();
  
  //return 0;  
  return 0;
};

