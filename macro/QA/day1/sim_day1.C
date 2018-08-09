// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector
// to run the macro:
// root  sim_complete.C  or in root session root>.x  sim_complete.C
// to run with different options:(e.g more events, different momentum, Geant4)
// root  sim_day1.C"(100, "TGeant4",2)"

int sim_day1(Int_t nEvents = 100, TString  SimEngine ="TGeant3", Double_t BeamMomentum = 6.231552)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  // TString inputGenerator =
  // EvtGen -> "xxxxxxxx.dec"
  // DPM    -> "dpm_xxxxx"
  // FTF    -> "ftf_xxxxx"
  TString  inputGenerator = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  inputDir = gSystem->Getenv("VMCWORKDIR");
  inputDir += "/macro/QA/day1/";
  gRandom->SetSeed(1234);
  //-------------------------------------------------------------------------
  // -----   Create the Simulation run manager ------------------------------
  PndMasterRunSim *fRun = new PndMasterRunSim();
  fRun->SetOptions("day1+fakeonline"); // day1 phase1 gem3 nogem fakeonline
  fRun->SetInput(inputGenerator);
  fRun->SetInputDir(inputDir);
  fRun->SetName(SimEngine);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->SetNumberOfEvents(nEvents);
  fRun->SetBeamMom(BeamMomentum);
  // -----  Initialization   ------------------------------------------------
  fRun->Setup("fakeonline");
  // -----   Geometry   -----------------------------------------------------
  fRun->CreateGeometry();
  // -----   Event generator   ----------------------------------------------
  fRun->SetGenerator();
  // -----   Add tasks   ----------------------------------------------------
  fRun->AddSimTasks();
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  //gROOT->GetListOfFiles()->ls();
  //TString text=((TObjString*)gDirectory->GetObjectUnchecked("TaskList"))->String();
  //IterateTasksList(fRun->GetMainTask(), text);
  //cout<<text.Data()<<endl;
  //IterateTasksList(fRun->GetMainTask(), text);
  //TObjString otext(text);
  //otext.Write("TaskList");
  // fetch text from root file:  cout << TaskList->String().Data() <<endl;
  // --- now run!
  fRun->Run(nEvents);
  fRun->Finish();

  return 0;
};

