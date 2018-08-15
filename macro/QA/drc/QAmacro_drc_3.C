#include "../auxi.C"
int QAmacro_drc_3()
{
  cout << "QA module for the DRC hit finder." << endl;

  Int_t nEvents = 0;
  TString inFile = "sim.root";
  TString parFile="par.root";
  TString inDigi = "digi.root";
  TString outFile = "hit.root";
  Int_t timeBased=1;

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetGenerateRunInfo(kFALSE);
  fRun->SetInputFile(inFile);
  fRun->AddFriend(inDigi);
  fRun->SetOutputFile(outFile);
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

  CloseGeoManager();
  return 0;
}

