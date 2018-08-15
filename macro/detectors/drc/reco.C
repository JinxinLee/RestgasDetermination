int reco(Int_t nEvents = 0, TString simFile = "sim.root", TString parFile1="par.root", TString digiFile = "digi.root", TString hitFile = "hit.root", TString lut = "lut/lut_e3_b3_l6_m40_avr.root",TString pdf = "pdf/pdf_3.5.root", TString outFile = "reco.root", Double_t r1 = 0, Double_t r2 =0){
  Int_t verbose = 3;
  gStyle->SetOptStat(0);
  
  gSystem->Load("libSpectrum");

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetGenerateRunInfo(kFALSE);

  // //fRun->SetInputFile(digiFile);
  fRun->SetInputFile(simFile);
  fRun->AddFriend(hitFile);
  
  // fRun->SetInputFile("sim_321.root");
  // fRun->AddFriend("sim_211.root");
  // fRun->AddFriend("hit_321.root");
  // fRun->AddFriend("hit_211.root");sim

  // FairFileSource* source = new FairFileSource(simFile);
  // source->AddFriend(hitFile);

  // fRun->SetSource(source);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile1.Data());
  rtdb->setFirstInput(parInput1);

  // -- Reconstruction using Look-up tables ---------------------------------
  //PndDrcLutReco* lutreco = new PndDrcLutReco(verbose+0, luttab);
  PndDrcReco* lutreco = new PndDrcReco(outFile,lut,pdf,1,r1,r2);  
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

  return 0;
}
