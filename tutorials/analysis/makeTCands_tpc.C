// *******
// Macro for running analysis in a Task
// Take a look to tutorials/anatask/PndAnalysis.cxx
// Add the line add_subdirectory(tutorials/anatask)
// to pandaroot/CMakeLists.txt
// and do a 'make install' in build directory first
// *******


void makeTCands_tpc(int nevts=10)
{
  ;
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  TString sysFile = gSystem->Getenv("VMCWORKDIR");

  TString parFile = "data/params_tpccombi.root";
  TString inSimuFile = "data/points_tpccombi.root";
  TString inDigiFile = "data/digi_tpccombi.root";
  TString inRecoFile = "data/reco_tpccombi.root";
  TString inKalmFile = "data/fit_tpccombi.root";
  TString outFile = "data/tcands.root";
	
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inSimuFile);
  fRun->AddFriend(inDigiFile);
  fRun->AddFriend(inRecoFile);
  fRun->AddFriend(inKalmFile);
  
  fRun->SetOutputFile(outFile);
  FairGeane *Geane = new FairGeane(inSimuFile);
  PndEmcMapper *emcMap = PndEmcMapper::Instance(2,inSimuFile);

  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/all.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());

  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  fRun->LoadGeometry();

  // ------------------------------------------------------------------------
  PndPidCorrelator* corr = new PndPidCorrelator();
  corr->SetInputBranch("LheGenTrack");
  corr->SetInputIDBranch("LheTrackID");
  fRun->AddTask(corr);

  PndPidIdealAssociatorTask* pidcls = new PndPidIdealAssociatorTask();
  pidcls->SetVerbose(3);
  fRun->AddTask(pidcls);
  
  //append the analysis task
  //PndMicroWriter* miwri = new PndMicroWriter("LheGenTrack");
  //fRun->AddTask(miwri);
  //  PndStoreTCands *tcandTask=new PndStoreTCands();
  //  fRunA->AddTask(tcandTask);

  //PndSimpleAnalysis *anaTask=new PndSimpleAnalysis(cfgfile.Data());
  //fRunA->AddTask(anaTask);

  fRun->Init();
  //  if (nevts==0) nevts=10;
  fRun->Run(0,nevts);


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}

