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
  gSystem->Load("libMCStack");
  gSystem->Load("libPassive");
  gSystem->Load("libTrkBase");  
  gSystem->Load("libGeane");
  gSystem->Load("libStt");
  gSystem->Load("libPlane");
  gSystem->Load("libGen");
  gSystem->Load("libGeaneEx");
  
  CbmRunAna *fRun = new CbmRunAna();

  fRun->SetInputFile("ex1.root");
  fRun->SetOutputFile("ex1g.root");

   //CbmGeane *Geane = new CbmGeane();
   CbmGeane *Geane = new CbmGeane("ex1.root");

   CbmGeaneTr *tr= new CbmGeaneTr();
  fRun->AddTask(tr);

  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open("ex1params.root"); 
  rtdb->setFirstInput(parInput1);

  fRun->Init();

  rtdb->print();
 // Set the field(if any) to Geane
  Geane->SetField(fRun->GetField());
     
  // Transport nEvents
  // -----------------
  TStopwatch timer;
  timer.Start();
      
  Int_t nEvents = 10000;
  fRun->Run(0,nEvents);
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
