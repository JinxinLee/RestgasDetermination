{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file

  TString MCFile = "sim_complete.root";

  // Number of events to process
  Int_t nEvents = 0;
 
  // ----  Load libraries   -------------------------------------------------
//  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
//  rootlogon();

  PndFileNameCreator creator(MCFile.Data());
  TString inFile = "2014-10-27-00-06-02_50_MHz_Filtered_PosCorrected_Event.root";
  TString outFile = "2014-10-27-00-06-02_50_MHz_Filtered_PosCorrected_TrackCorr.root";
  TString parFile = "simparams.root";

  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
  std::cout << "InFile: " << inFile.Data()<< std::endl;
  std::cout << "OutFile: " << outFile.Data()<< std::endl;
  std::cout << "ParFile: " << parFile.Data()<< std::endl;

  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  // ------------------------------------------------------------------------
  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
 // fRun->AddFriend(MCFile);
  fRun->SetOutputFile(outFile);
//  fRun->RunWithTimeStamps();
  fRun->SetUseFairLinks(kTRUE);
  FairGeane *Geane = new FairGeane();
//  fRun->AddTask(Geane);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
   TString allDigiFile = sysFile+"/macro/params/all.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ------------------------------------------------------------------------
 
  PndStraightLineTrackFinderTask* finder = new PndStraightLineTrackFinderTask(0, "MVDHitsPixelPosCorrected","","");
  finder->SetVerbose(0);
  fRun->AddTask(finder);

  PndLmdLinFitTask* lmdfit = new PndLmdLinFitTask("MVDTestBeamTrackCand","MVDHitsPixelPosCorrected", "MvdTrack", "Mvd");
  lmdfit->SetVerbose(0);
  fRun->AddTask(lmdfit);

 
  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);

  rtdb->saveOutput();
  rtdb->print();

  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
