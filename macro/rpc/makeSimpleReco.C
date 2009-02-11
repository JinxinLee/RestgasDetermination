{  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // 1 Input file after MC run
  TString MCFile = ("test.root");
 // 2 Input file after hit producer 
  TString HitFile = ("Hits.root");
 std::cout << "MCFile  : " << MCFile<< std::endl;  
 std::cout << "HitFile  : " << HitFile << std::endl;
  // Number of events to process
  Int_t nEvents = 0;  // if 0 all the events will be processed
  	
  TString outFile = ("rpcreco.root");
  
  // Loading libraries
  // If the macro gives error messages in loading libraries, 
  // please check the path of the libs and put it by hands
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");  
  gSystem->Load("libRpc"); 
  
  // -----   Timer 
  TStopwatch timer;
  timer.Start();
    
  // -----   Reconstruction run
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->AddFriend(HitFile);
  fRun->SetOutputFile(outFile);
  
  // -----   Add tasks
  PndRpcSimpleReco* reco= new PndRpcSimpleReco();
  fRun->AddTask(reco);
  
  // -----   Intialise and run 
  cout << "fRun->Init()" << endl;
  fRun->Init();
  fRun->Run(0,nEvents);

// Output file ("rpcreco.root") is created.   

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
  
}
