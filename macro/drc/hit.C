void hit(Int_t nEvents = 0, TString inFile = "sim.root",  TString parFile="par.root", TString inDigi = "digi.root", TString outFile = "hit.root", Int_t timeBased=0){
  Int_t verbose = 0;
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
 
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetGenerateRunInfo(kFALSE);
  fRun->SetInputFile(inFile);
  fRun->AddFriend(inDigi);
  fRun->SetOutputFile(outFile);
  fRun->SetUseFairLinks(kTRUE);
  if(timeBased) fRun->RunWithTimeStamps();

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput = new FairParRootFileIo();

  if(parFile=="batch"){
    TList* parlist = new TList();
    for(Int_t i=0; i<4; i++) parlist->Add(new TObjString(Form("par_b%d.root",i)));
    parInput->open(parlist);
  }else{
    parInput->open(parFile.Data());
  }
  rtdb->setFirstInput(parInput);
 
  // -----    DRC hit producer   -------------------------------------------- 
  PndDrcHitFinder* hitfind = new PndDrcHitFinder(0);
  fRun->AddTask(hitfind);
     
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
