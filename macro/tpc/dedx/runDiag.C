{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;  
  

 // TString recoFile="/nfs/data/panda-mc/E2Dir_oR/t40/t40/t40P321_0.3-0.3_40-40_40-40_0";
  //TString digiInput = "/nfs/data/panda-mc/test/test/testP11_0.5-0.5_40-40_40-40_0";
  //TString mcInput = "/nfs/data/panda-mc/test/testP11_0.5-0.5_40-40_40-40_0";
  //TString parFile = "/nfs/data/panda-mc/test/test/testP11_0.5-0.5_40-40_40-40_0.param.root";
  
  /*TString input="/nfs/data/panda-mc/test/test/test/testP11_0.5-0.5_40-40_40-40_0";*/
  //TString digiFile = digiInput+".raw.root";
  //TString mcFile = mcInput+".mc.root";
 // TString parFile = input + ".param.root";
 // cout << "Using parFile: " << parFile.Data();

  TString directory="/nfs/data/panda-mc/test/test/test/";
  TString filename="test2P211_0.5-0.5_40-40_40-40_0";
  //TString filename="testP11_0.5-0.5_40-40_40-40_0";
  TString input=directory + filename;
  TString output=directory+ filename; 
 
  TString recoFile = input + ".reco.root";
  TString outFile = output+".diag.root";
  
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libgenfit");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("libtrackrep");
  gSystem->Load("librecotasks");


  // ------------------------------------------------------------------------


  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(recoFile);
  //fRun->AddFriend(digiFile);
  //fRun->AddFriend(mcFile);
  fRun->SetOutputFile(outFile);
  
 
  /* 
    // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString tpcDigiFile = gSystem->Getenv("VMCWORKDIR");
  tpcDigiFile +=  "/tpc/tpcHighADC.par";
  parInput2->open(tpcDigiFile.Data(),"in");
  
  rtdb->setFirstInput(parInput2);
  rtdb->setSecondInput(parInput1);
  
  fRun->LoadGeometry();
 */
  // ------------------------------------------------------------------------
  PndTpcDEDXDiagnosticsTask* diaTask=new PndTpcDEDXDiagnosticsTask;
  fRun->AddTask(diaTask);	

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,0);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
