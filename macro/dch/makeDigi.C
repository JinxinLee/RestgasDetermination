{
  // Loads a filewith hits and makes digitization for EMC
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // Input file (MC events)
  TString inFile = "tst.root";
  
  
  // Number of events to process
  Int_t nEvents = 0;  // if 0 all the events will be processed
  	
  // Output file
  TString outFile = inFile;
  outFile.ReplaceAll(".root","_digis.root");
  
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
  gSystem->Load("libgenfit");
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
  fRun->AddTask(digiProducer);
  
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
