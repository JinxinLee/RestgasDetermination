int fillLut(Int_t nEvents = 0, TString inFile = "simlut.root", TString parFile1="parlut.root", TString lutFile = "luttab.root"){
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  basiclibs();

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(inFile+"_out");

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile1.Data());
  rtdb->setFirstInput(parInput1);

  // -----    DRC Digitization stage ----------------------------------------
  PndDrcDigiTask* drcdigi = new PndDrcDigiTask();  
  drcdigi->SetIsDetEfficiency(0); 
  drcdigi->SetTransportEfficiency(0);
  drcdigi->SetChargeSharing(kFALSE); 
  fRun->AddTask(drcdigi);

  // -----    Hit finder ---------------------------------------------------- 
  PndDrcHitFinder* hitfind = new PndDrcHitFinder(iVerbose);
  fRun->AddTask(hitfind);

  // -----    Fill look-up table --------------------------------------------
  PndDrcLutFill* filllut = new PndDrcLutFill(iVerbose,lutFile);
  fRun->AddTask(filllut);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << inFile+"_out" << endl;
  cout << "Parameter file is " << parFile1 << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;

  return 0;
}
