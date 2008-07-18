{
  // Loads a filewith points, digitizes them and reconstructs spatial cylinder hits
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; 
  
  // Input file (MC events)
  TString inFile = "tst.root";
  
  
  // Number of events to process
  Int_t nEvents = 0;  // if 0 all the events will be processed
  	
  // Output file
  TString outFile = inFile;
  outFile.ReplaceAll(".root","_Tracks.root");
  
  // Loading libraries
  // If the macro gives error messages in loading libraries, 
  // please check the path of the libs and put it by hands
  
  gSystem->Load("libgeant321");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libGen");
  gSystem->Load("libTrkBase");
  gSystem->Load("libGeane");
  gSystem->Load("libgenfit");
  gSystem->Load("libtrackrep");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");
  gSystem->Load("libDch");

  // -----   Timer 
  TStopwatch timer;
  timer.Start();
    
  // -----   Reconstruction run
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  
  // -----   Add tasks
   PndDchDigiProducer* digiProducer= new PndDchDigiProducer();
   //digiProducer->SetVerbose(2);
  fRun->AddTask(digiProducer);
  
  PndDchCylinderHitProducer* cylHitProducer= new PndDchCylinderHitProducer();
  cylHitProducer->SetVerbose(0);
  fRun->AddTask(cylHitProducer);
  
  //------ Ideal DCH track finder --------------------
  PndDchFindTracks* finderTask = new PndDchFindTracks("dchFindTracks");
  finderTask->SetUseHitOrDigi("chit");
  fRun->AddTask(finderTask);
  
  PndDchTrackFinderIdealCylHit* mcTrackFinder = new  PndDchTrackFinderIdealCylHit();
  mcTrackFinder->SetVerbose(0);  // verbosity level
  mcTrackFinder->SetPrimary(1);  // 1 = Only primary tracks are processed, 0 = all (default)
  finderTask->UseFinder(mcTrackFinder);
  //--------------------------------------------------
  
  
  //------ Match PndDchTracks and MCTracks tracks ----
  PndDchMatchTracks *matchTask = new PndDchMatchTracks();
  matchTask->SetUseHitOrDigi("chit");
  matchTask->SetVerbose(0);
  fRun->AddTask(matchTask);
  // -------------------------------------------------

 // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  CbmGeane *Geane = new CbmGeane(inFile);
  // Set the field(if any) to Geane
  Geane->SetField(fRun->GetField());

  //------ Prepare Kalman Tracks ---------------------
  PndDchPrepareKalmanTracks *prepareKalmanTracks = new PndDchPrepareKalmanTracks();
  prepareKalmanTracks->UseGeane();
  //prepareKalmanTracks->SetPersistence();
  fRun->AddTask(prepareKalmanTracks);
  // ------------------------------------------------- 
  PndDchKalmanTask* dchKalman = new PndDchKalmanTask();
  dchKalman->SetVerbose(1);
  fRun->AddTask(dchKalman);
  // ------------------------------------------------- 

  
   
  // -----   Intialise and run 
  cout << "fRun->Init()" << endl;
  fRun->Init();
  fRun->Run(0,nEvents);

  // -----   Finish
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  
  exit(0);
}
