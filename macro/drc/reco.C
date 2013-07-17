void reco(Int_t nEvents = 0, TString inFile = "digi.root", TString outFile = "reco.root", TString parFile1="par.root"){
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  
  TString luttab = "luttab.root";
  
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend("sim.root");
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile1.Data());
  rtdb->setFirstInput(parInput1);
 
  // -- Reconstruction using Look-up tables ---------------------------------
  PndDrcLutReco* lutreco = new PndDrcLutReco(iVerbose, luttab);  
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
