{
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libPndBase");
  gSystem->Load("libPassive");
  gSystem->Load("libTrkBase");  
  gSystem->Load("libGeane");
  gSystem->Load("libGen");
  gSystem->Load("libGeaneExEmc");

  gSystem->Load("libStt");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libtpc");
  gSystem->Load("libMvd");
  
  FairRunAna *fRun = new FairRunAna();

  fRun->SetInputFile("mc.root");
  fRun->SetOutputFile("geane.root");

   //FairGeane *Geane = new FairGeane();
   FairGeane *Geane = new FairGeane("mc.root");

   FairGeaneTrEmc *tr= new FairGeaneTrEmc();
   fRun->AddTask(tr);

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open("params.root"); 
  rtdb->setFirstInput(parInput1);
  new FairTrajFilter();

  fRun->Init();

  rtdb->print();
 // Set the field(if any) to Geane
  Geane->SetField(fRun->GetField());
     
  // Transport nEvents
  // -----------------
  TStopwatch timer;
  timer.Start();
      
  Int_t nEvents = 0;
  fRun->Run(0,nEvents);
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
