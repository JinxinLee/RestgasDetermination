void digi_complete()
{
  // Macro created 03/02/2016 by S.Spataro
  // It loads a simulation file and digitize hits 

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment

  // Number of events to process
  Int_t nEvents = 0;  // if 0 all the events will be processed
  
  // Input file (MC events)
  TString simFile = "sim_complete.root";
  
  // Parameter file (root)
  TString parRootFile = "simparams.root"; // at the moment you do not need it
  
  // Parameter file (ascii)
  TString parAsciiFile = "all.par";
  
  // Output file
  TString outFile = "digi_complete.root";

  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInputFile(simFile);
  fRun->SetOutputFile(outFile);
  fRun->SetParamRootFile(parRootFile);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup();

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddDigiTasks();

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
 
  exit(0);
}
