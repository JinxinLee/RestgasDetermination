int runFit(TString parFile, TString DigiFile, TString RecoFile, TString outFile, TString path)
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  parFile = path+parFile;
  DigiFile = path+DigiFile;
  RecoFile = path+RecoFile;
  outFile = path+outFile;


  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");  
  // ------------------------------------------------------------------------
  // Output file

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  //  fRun->SetInputFile(MCFile);
  fRun->SetInputFile(DigiFile);
  fRun->AddFriend(RecoFile);

  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);

  // -----    Default MVD hit producer   --------------------------------------------
  
  TtCracowTask *fit = new TtCracowTask();
  fRun->AddTask(fit);

  rtdb->setOutput(parInput1);
  rtdb->print();
    
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,0);
  // ------------------------------------------------------------------------

  rtdb->saveOutput();
  rtdb->print();

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
  // ------------------------------------------------------------------------

  return 0;

}
