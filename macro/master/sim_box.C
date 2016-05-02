// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector
// to run the macro:
// root  sim_box.C  or in root session root>.x  sim_box.C
// to run with different options:(e.g more events, Geant4, different momentum)
// root  sim_box.C"(100, "TGeant4",2)"

sim_box(Int_t nEvents = 100, TString  SimEngine ="TGeant3", Double_t BeamMomentum = 6.231552)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  inputGenerator = "box_1pi_1GeV_theta10-120"; 
  //-------------------------------------------------------------------------
  // -----   Create the Simulation run manager ------------------------------
  PndMasterRunSim *fRun = new PndMasterRunSim();
  fRun->SetInput(inputGenerator);
  fRun->SetName(SimEngine);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->SetNumberOfEvents(nEvents);
  fRun->SetBeamMom(BeamMomentum);
  // -----  Initialization   ------------------------------------------------
  fRun->Setup();
  // -----   Geometry   -----------------------------------------------------
  fRun->CreateGeometry();
  // -----   Event generator   ----------------------------------------------
  FairBoxGenerator* boxGen = new FairBoxGenerator(13, 1); // 13 = muon; 1 = multipl.
  boxGen->SetPRange(1.,1.); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(10., 120.); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // cm
  fRun->SetGenerator(boxGen);
  // -----   Add tasks   ----------------------------------------------------
  fRun->AddSimTasks();
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(nEvents); 
  fRun->Finish();
  
  //exit(0);  
};

