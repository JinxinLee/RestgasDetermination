void digi(Int_t nEvents = 10, TString inFile = "sim.root", TString parFile="par.root", TString outFile = "digi.root", Int_t timeBased=1, Double_t eventRate=50, Double_t deadTime=5){
  Int_t verbose = 0;
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
 
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetWriteRunInfoFile(kFALSE);
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  if(timeBased) fRun->SetEventMeanTime(eventRate);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput = new FairParRootFileIo();
  parInput->open(parFile.Data());
  rtdb->setFirstInput(parInput);

  if(parFile=="batch"){
    TList* parlist = new TList();
    for(Int_t i=0; i<4; i++) parlist->Add(new TObjString(Form("par_b%d.root",i)));
    parInput->open(parlist);
  }else{
    parInput->open(parFile.Data());
  }
  rtdb->setFirstInput(parInput);
 
  // -----    DRC Digitization stage ----------------------------------------
  PndDrcDigiTask* drcdigi = new PndDrcDigiTask(0);
  drcdigi->SetChargeSharing(kFALSE);
  drcdigi->SetDeadTime(deadTime); //5 ns
  drcdigi->SetTimeResolution(0.4); //0.4 ns
  if(timeBased) drcdigi->RunTimeBased();
  fRun->AddTask(drcdigi);

  // PndDrcDigiSorterTask* digiSorter = new PndDrcDigiSorterTask(1000, 0.4, "DrcDigi", "DrcSortedDigi", "PndDrc");
  // digiSorter->SetVerbose(3);
  // fRun->AddTask(digiSorter);

  // // -----    DRC hit producer   ------------------------------------------ 
  // PndDrcHitFinder* hitfind = new PndDrcHitFinder(0);
  // fRun->AddTask(hitfind);
     
  // -----   Initialize and run   -------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);

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
}
