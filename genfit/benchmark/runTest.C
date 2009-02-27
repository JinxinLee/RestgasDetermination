// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  gROOT->Reset();

  TString inFile  = "tpcMC.root";
  TString outFile = "geane.root";
  TString parFile = "tpcMC.param.root"; 

  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  // gSystem->Load("libBField");
  gSystem->Load("libPndData");  
  gSystem->Load("libPassive");
  //  gSystem->Load("libPlane");
  gSystem->Load("libGen");
  gSystem->Load("libTrkBase");  
  gSystem->Load("libGeane");
  //  gSystem->Load("libGeanepca");
  gSystem->Load("libtpc");
  gSystem->Load("libgenfit");
  gSystem->Load("libtrackrep");
  gSystem->Load("libbenchmark");
  

  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile); 
  fRun->SetOutputFile(outFile);


  FairGeane *Geane = new FairGeane(inFile);
  //  POCAtestTask4 *poca= new POCAtestTask4();
  //  fRun->AddTask(poca);
  SPtestTask *SPT = new SPtestTask();

  SPT->_nEv=10000;
  SPT->_mom=.5;
  double theta=25.;
  SPT->_th=TMath::Pi()*(theta/180.);
  SPT->_res=0.025;
  char buf[200];
  sprintf(buf,"%.2fGeV_%.3fcmRes_%.2fdeg_%dev.root",SPT->_mom,SPT->_res,theta,SPT->_nEv);
  std::string fileName(buf);
  SPT->setFileName(fileName);
  fRun->AddTask(SPT);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  // -----  Parameter database   --------------------------------------------


  fRun->Init();
  rtdb->print();
  // Set the field(if any) to Geane
  Geane->SetField(fRun->GetField());


  // Transport nEvents
  // -----------------
  TStopwatch timer;
  timer.Start();

    
  Int_t nEvents = 1;
  fRun->Run(0,nEvents);
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  //  poca->WriteToFile();

}  
  
