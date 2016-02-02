void digi_complete()
{
  // Macro created 20/09/2006 by S.Spataro
  // It loads a simulation file and digitize hits 

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // Input file (MC events)
  TString inFile = "sim_complete.root";
  
  // Parameter file
  TString parFile = "simparams.root"; // at the moment you do not need it
  
  // Digitisation file (ascii)
  TString digiFile = "all.par";
  
  // Output file
  TString outFile = "digi_complete.root";
  
  // -----   Reconstruction run   -------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->SetParamRootFile(parFile);
  fRun->SetParamAsciiFile(digiFile);
  fRun->Setup();
  
  fRun->AddDigiTasks();
  
  fRun->Init();
  fRun->Run();
  fRun->Finish();
 
  exit(0);
}
