int QAmacro_drc_2()
{
  cout << "QA module for the DRC digitization." << endl;

  Int_t nEvents = 10;
  TString inFile = "sim.root";
  TString parFile="par.root";
  TString outFile = "digi.root";
  Int_t timeBased=1;
  Double_t eventRate=50;
  Double_t deadTime=5;
  
  Int_t verbose = 0;
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
 
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetGenerateRunInfo(kFALSE);
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
     
  // -----   Initialize and run   -------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
  
  cout << " Test passed" << endl;
  cout << " All ok " << endl;  
  
  // Bool_t fTest=kFALSE;
  // if (fTest){
  //   cout << " Test passed" << endl;
  //   cout << " All ok " << endl;  
  // }else{
  //   cout << " Test Failed" << endl;
  //   cout << " Not Ok " << endl;         
  // }

  return 0; 
}  
  
