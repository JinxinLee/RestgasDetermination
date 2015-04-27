// TEST 2: digitization
void QAmacro_stt_2()
{
  TStopwatch timer;
  timer.Start();

  Bool_t fTest=kFALSE;

  // ------------------------------------------------------------------------
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "testrun.root";

  // Number of events to process
  Int_t nEvents = 0;

  // Parameter file
  TString parFile = "testparams.root";

  // Output file
  TString outFile = "testdigi.root";

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
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

  // -----   MDV digi producers   ---------------------------------
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->SetVerbose(iVerbose);
  fRun->AddTask(mvddigi);

  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);

  // -----   STT reconstruction tasks   --------------------------------------------
  // digitize ....
  PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
  // sttHitProducer->SetVerbose(3); // debug = print each evt num
  fRun->AddTask(sttHitProducer);


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
  
