void readInTopix4TBData(TString inFileName, Double_t clockFrequency = 50.)
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
  
  TString tempOutFile = inFileName;
  tempOutFile.Append("_");
  tempOutFile += clockFrequency;
  tempOutFile.Append("_MHz.root");

  // Output file
  TString outFile = tempOutFile;
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
 // fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->SetWriteRunInfoFile(kFALSE);  
  
  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  

  PndMvdReadInTBDataTask* readTask = new PndMvdReadInTBDataTask();
//  readTask->AddFile(inFileName);
  TString tempFileName = inFileName;
  tempFileName.Append("--101-data--1.txt");
  readTask->AddFile(tempFileName);

  tempFileName = inFileName;
  tempFileName.Append("--102-data--1.txt");
  readTask->AddFile(tempFileName);

  tempFileName = inFileName;
  tempFileName.Append("--103-data--1.txt");
  readTask->AddFile(tempFileName);

  tempFileName = inFileName;
  tempFileName.Append("--102-data--1.txt");
  readTask->AddFile(tempFileName);

  readTask->SetFrequency(clockFrequency);
  fRun->AddTask(readTask);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  timer.Start();
  fRun->RunOnTBData();

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  cout << " Test passed" << endl;
  cout << " All ok " << endl;

  exit(0);
}
