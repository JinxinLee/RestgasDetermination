{
  // Loads a file with points, makes ideal hit reconstruction
  // and attempts track fitting assuming a single track in dch;
  // produces a file *_hots.root with ideal hits and fitted hots (hit-on-track)
  // and another file containing pull distos. for fitted momentum components
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // Input file (MC events)
  TString inFile = "tst.root";
  
  
  // Number of events to process
  Int_t nEvents = 10;  // if 0 all the events will be processed
  	
  // Output file
  TString outFile = inFile;
  outFile.ReplaceAll(".root","_hots.root");
  
  // Loading libraries
  // If the macro gives error messages in loading libraries, 
  // please check the path of the libs and put it by hands
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libgenfit");
  gSystem->Load("libDch"); 
  
  // -----   Timer 
  TStopwatch timer;
  timer.Start();
    
  // -----   Reconstruction run
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  
  // -----   Add tasks 
  PndDchIdealHitProducer* hitProducer= new PndDchIdealHitProducer();
  fRun->AddTask(hitProducer);
  
  PndDchPreFitter* fitter= new PndDchPreFitter();
  fRun->AddTask(fitter);
  
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
  
}
