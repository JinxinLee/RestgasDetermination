{
  gDebug=0;
  // Load basic libraries
  TStopwatch timer;
  timer.Start();

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  Int_t nEvents  = 69000;

//   gROOT->Macro("Libs.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  TString inFile = "./data/Mvd_FlatG4Special_0G5_70k.root";
  // Parameter file
  TString parFile = "./data/Mvd_FlatG4Special_0G5_70k.param.root";

  CbmRunAna *fRun = new CbmRunAna();

  fRun->SetInputFile(inFile);
 // fRun->AddFile("Mvd_FlatG4_30k_1.root");
//mv  fRun->AddFile("Mvd_FlatG4_30k_2.root");
  fRun->SetOutputFile("./data/Mvd_FlatG4Special_0G5_70k_Geane.root");

   //CbmGeane *Geane = new CbmGeane();
   CbmGeane *Geane = new CbmGeane(inFile);

   PndMvdMSAnaTask *tr= new PndMvdMSAnaTask();
  fRun->AddTask(tr);

  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile);
  rtdb->setFirstInput(parInput1);

  PndMultiField *fField= new PndMultiField();

   PndTransMap *map= new PndTransMap("TransMap", "R");
   PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
   PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
   fField->AddField(map);
   fField->AddField(map1);
   fField->AddField(map2);


   fRun->SetField(fField);

  fRun->Init();

  rtdb->print();
 // Set the field(if any) to Geane
  Geane->SetField(fRun->GetField());

  // Transport nEvents
  // -----------------
  TStopwatch timer;
  timer.Start();

  fRun->Run(0,nEvents);
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}

