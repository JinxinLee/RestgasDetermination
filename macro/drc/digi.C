void digi(Int_t nEvents = 0, TString inFile = "sim.root", TString outFile = "digi.root", TString parFile1="par.root"){
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 3;

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  basiclibs();

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile1.Data());
  rtdb->setFirstInput(parInput1);
 
  // -----    DRC Digitization stage ----------------------------------------
  PndDrcDigiTask* drcdigi = new PndDrcDigiTask();  
  drcdigi->SetIsDetEfficiency(0); 
  drcdigi->SetTransportEfficiency(0);
  drcdigi->SetChargeSharing(kTRUE); 
  fRun->AddTask(drcdigi);

  // -----    DRC hit producer   -------------------------------------------- 
  PndDrcHitFinder* hitfind = new PndDrcHitFinder();
  fRun->AddTask(hitfind);
     
  // -----   Initialize and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile1 << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
}
