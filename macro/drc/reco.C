void reco(Int_t nEvents = 0, TString simFile = "sim.root", TString parFile1="par.root", TString digiFile = "digi.root", TString hitFile = "hit.root", TString luttab = "lut/lut_e3_b5_l6_avr.root", TString outFile = "reco.root"){
  Int_t verbose = 0;
  gStyle->SetOptStat(0);
  
  gSystem->Load("libSpectrum");

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetGenerateRunInfo(kFALSE);
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
