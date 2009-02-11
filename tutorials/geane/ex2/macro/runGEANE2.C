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
  
  FairRunAna *fRun = new FairRunAna();

  fRun->SetInputFile("ex2.root");
  fRun->AddFriend("ex2d.root");
  fRun->AddFriend("ex2r.root");
  fRun->SetOutputFile("ex2g.root");

   //FairGeane *Geane = new FairGeane();
   FairGeane *Geane = new FairGeane("ex2.root");

//   FairGeaneTr *tr= new FairGeaneTr();
//   FairGeaneTrC *tr= new FairGeaneTrC();
   FairGeaneTrT *tr= new FairGeaneTrT();
//   FairGeaneTrP *tr= new FairGeaneTrP();
//   FairGeaneTrK *tr= new FairGeaneTrK();
  fRun->AddTask(tr);

 //  FairGeaneTrP *trP= new FairGeaneTrP();
 //  fRun->AddTask(trP);
 // -----  Parameter database   --------------------------------------------

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open("ex2params.root"); 
  rtdb->setFirstInput(parInput1);

  // ------------------------------------------------------------------------

  fRun->Init();

  rtdb->print();
 // Set the field(if any) to Geane
  Geane->SetField(fRun->GetField());
     
  // Transport nEvents
  // -----------------
  TStopwatch timer;
  timer.Start();
  
    
  Int_t nEvents = 1000;
  fRun->Run(0,nEvents);
 // fRun->Run();   
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
