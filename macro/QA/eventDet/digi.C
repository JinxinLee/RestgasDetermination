void digi()
{
  // -----------   User Settings: -------------------------------

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  Bool_t timebased = kTRUE;
  Double_t eventrate = 20.0; // in MHz
  Int_t nEvents = 0;

  // ------------------------

  TString inFile = "sim.root";
  TString parFile ="sim_params.root";
  TString digiFile = "all.par";
  TString outFile = "digi.root";
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->SetUseFairLinks(kTRUE);
  fRun->SetWriteRunInfoFile(kFALSE);
  //fRun->SetEventMeanTime(50); //in ns
  if (timebased) fRun->SetEventMeanTime(1/(eventrate * 10e6)*10e9); //in n
 
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

  // -----   MDV digi producers   ---------------------------------
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->SetVerbose(iVerbose);
  fRun->AddTask(mvddigi);
 
  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);

  // -----   SciT hit producers   ---------------------------
  PndSciTDigiTask* SciTDigi = new PndSciTDigiTask();
  SciTDigi->SetVerbose(iVerbose);
  if(timebased) SciTDigi->RunTimeBased();
  fRun->AddTask(SciTDigi);

  // -----   Ftof hit producers   ---------------------------
  PndFtofHitProducerIdeal* ftofhit = new PndFtofHitProducerIdeal();
  ftofhit->SetVerbose(iVerbose);
  fRun->AddTask(ftofhit);
 
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  timer.Start();
  fRun->Run(0, nEvents);

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

  return 0;
}
