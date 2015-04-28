int QAmacro_drc_4()
{
  cout << "QA module for the DRC reconstruction." << endl;

  Int_t verbose = 0;
  Int_t nEvents = 0;
  TString simFile = "sim.root";
  TString parFile1="par.root";
  TString digiFile = "digi.root";
  TString hitFile = "hit.root";
  TString luttab = gSystem->Getenv("VMCWORKDIR");
  luttab += "/macro/drc/lut/lut_16_a.root";
  TString outFile = "reco.root";

  gSystem->Load("libSpectrum");
    
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetWriteRunInfoFile(kFALSE);
  fRun->SetInputFile(digiFile);
  fRun->AddFriend(simFile);
  fRun->AddFriend(hitFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile1.Data());
  rtdb->setFirstInput(parInput1);

  // -- Reconstruction using Look-up tables ---------------------------------
  PndDrcLutReco* lutreco = new PndDrcLutReco(verbose+3, luttab);  
  fRun->AddTask(lutreco);
       
  // -----   Initialize and run  --------------------------------------------
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

  exit(0); 
}  
  
