{
  // Loads a filewith points, makes digitization, reconstruction and Kalman filtering
  // producing objects of Track type
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; 
  
   // Base File name
  TString base="tst";
  // Input file (MC events)
  TString inFile = base+".root";
  //Parameter file
  TString parFile = base+".param.root";
  // Output file
  TString outFile = base+".recoReal.root";
  
  // Number of events to process
  Int_t nEvents = 1;  // if 0 all the events will be processed
  	
  // Loading libraries
  // If the macro gives error messages in loading libraries, 
  // please check the path of the libs and put it by hands
  
  std::cout<<"libgeant321\t"<<gSystem->Load("libgeant321")<<endl;
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  //gSystem->Load("libDrcProp");
  //gSystem->Load("libDrc");
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
  fRun->SetOutputFile(outFile)
;
  // -----  Parameter database
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  rtdb->Print();

  // -----   Add tasks
   PndDchDigiProducer* digiProducer= new PndDchDigiProducer();
   //digiProducer->SetVerbose(2);
  fRun->AddTask(digiProducer);
  
  PndDchCylinderHitProducer* cylHitProducer= new PndDchCylinderHitProducer();
  cylHitProducer->SetVerbose(2);
  fRun->AddTask(cylHitProducer);

  //------ Ideal DCH track finder --------------------
  PndDchFindTracks* finderTask = new PndDchFindTracks("dchFindTracks");
  finderTask->SetUseHitOrDigi("chit");
  finderTask->SetVerbose(3);
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
  PndDchPrepareKalmanTracks2 *prepareKalmanTracks = new PndDchPrepareKalmanTracks2();
  prepareKalmanTracks->UseGeane(kTRUE);
  prepareKalmanTracks->SetPersistence();
  fRun->AddTask(prepareKalmanTracks);
  // ------------------------------------------------- 
  PndDchKalmanTask2* dchKalman = new PndDchKalmanTask2();
  dchKalman->SetVerbose(1);
  dchKalman->SetNumIterations(1);
  dchKalman->SetSmooth(kFALSE);
  fRun->AddTask(dchKalman);
  // ------------------------------------------------- 

   
  // -----   Intialise and run 
  cout << "fRun->Init()" << endl;
  fRun->Init();
  fRun->Run(0,nEvents);

  //dchKalman->WriteHistograms();
  dchKalman->PlotHistograms();
  
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
  
  //exit(0);
}
