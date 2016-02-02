// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector
// to run the macro:
// root  sim_complete.C  or in root session root>.x  sim_complete.C
// to run with different options:(e.g more events, different momentum, Geant4)
// root  sim_complete.C"(100, "TGeant4",2)"

sim_complete(Int_t nEvents = 100, TString  SimEngine ="TGeant3", Double_t BeamMomentum = 6.231552)
{
  //-----User Settings:------------------------------------------------------
  TString  outputFile     = "sim_complete.root";
  TString  parRootFile    = "simparams.root";
  TString  parAsciiFile   = "all.par";
  TString  evtgenFile     = "psi2s_Jpsi2pi_Jpsi_mumu.dec"; 
  //-------------------------------------------------------------------------
  // -----   Create the Simulation run manager ------------------------------
  PndMasterRunSim *fRun = new PndMasterRunSim();
  fRun->SetName(SimEngine);
  fRun->SetOutputFile(outputFile);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->SetParamRootFile(parRootFile);
  fRun->SetBeamMom(BeamMomentum);
  fRun->Setup();
  fRun->CreateGeometry();
  // -----   Add tasks   ----------------------------------------------------
  fRun->AddSimTasks();
  
  // -----   Set the Event Generator ----------------------------------------
  //fRun->UseDpmGenerator();
  //fRun->UseFtfGenerator();
  fRun->UseEvtGenGenerator(evtgenFile);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(nEvents); 
  fRun->Finish();
  
  //exit(0);  
};

