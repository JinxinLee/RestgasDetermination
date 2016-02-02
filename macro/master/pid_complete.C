void pid_complete()
{
  // Macro created 03/02/2016 by S.Spataro
  // It loads a reconstruction file and compute PID informations

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // Number of events to process
  Int_t nEvents = 0;  // if 0 all the vents will be processed
  
  // Parameter file (root)
  TString parRootFile = "simparams.root"; // at the moment you do not need it
  
  // Paramter file (ascii)
  TString parAsciiFile = "all.par";

  // MC file
  TString simFile  = "sim_complete.root";
  
  // Digi file
  TString digiFile = "digi_complete.root";
  
  // Reco file
  TString recoFile = "reco_complete.root";
  
  // Output file
  TString outFile  = "pid_complete.root";
  
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInputFile(simFile);
  fRun->AddFriend(digiFile);
  fRun->AddFriend(recoFile);
  fRun->SetOutputFile(outFile);
  fRun->SetParamRootFile(parRootFile);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup();
  
  // -----   Add tasks   ----------------------------------------------------
  fRun->AddPidTasks();
  
  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();

  exit(0); 
}
