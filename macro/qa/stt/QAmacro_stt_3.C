// TEST 3: track finding
void QAmacro_stt_3()
{
  TStopwatch timer;
  timer.Start();

  //   gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  //   rootlogon();
 
  Bool_t fTest=kFALSE;

  // ------------------------------------------------------------------------
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "testrun.root";
  TString inFile2 = "testdigi.root";

  // Number of events to process
  Int_t nEvents = 0;

  // Parameter file
  TString parFile = "testparams.root";

  // Output file
  TString outFile = "testreco.root";

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(inFile2);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------
 

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  
  // Digitisation file (ascii)
  TString digiFile = "all.par";
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  rtdb->setSecondInput(parIo1);

  // -----   STT reconstruction -----------------------------------
  // trackfinding ....
  PndTrkTracking2* tracking = new PndTrkTracking2(0,false,false,false);
  tracking->SetInputBranchName("STTHit","MVDHitsPixel","MVDHitsStrip");
  tracking->NoMvdAloneTracking();
  fRun->AddTask(tracking);
  
  PndMCTrackAssociator* trackMC = new PndMCTrackAssociator();
  trackMC->SetTrackInBranchName("SttMvdTrack");
  trackMC->SetTrackOutBranchName("SttMvdTrackID");
  fRun->AddTask(trackMC);

 
  // -----   Initialize and run   --------------------------------------------
  fRun->Init();
  fRun->Run();
  // ------------------------------------------------------------------------

  fTest = kTRUE;

  if (fTest){
    cout << " Test Passed" << endl;
    cout << " All Ok " << endl;  
  }else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;         
  }

  delete fRun;
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  exit(0); 
}  
  
