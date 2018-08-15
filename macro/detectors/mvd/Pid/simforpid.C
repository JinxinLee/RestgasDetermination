// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 0;
  Int_t nEvents = 10000;

  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
//  gSystem->Load("libgenfit");
//  gSystem->Load("libtpc");
//  gSystem->Load("libtpcreco");
//  gSystem->Load("librecotasks");
  gSystem->Load("libGen");
  gSystem->Load("libPGen");
  gSystem->Load("libMvd");


  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
//   fRun->SetOutputFile("testMCPoints.root");
  fRun->SetOutputFile("data/MvdMC_Pid_10k.root");

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

 // FairModule *Pipe= new PndPipe("PIPE");
 // Pipe->SetGeometryFileName("pipe.geo");
 // fRun->AddModule(Pipe);
  
  
  //FairModule *Magnet= new PndMagnet("MAGNET");
  // 1- Active shielding Geometry
  //Magnet->SetGeometryFileName("magnet_active.geo");
  // 2- Iron Magnet
  // Magnet->SetGeometryFileName("magnet_iron.geo");
  //fRun->AddModule(Magnet);
  
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD14.root");
  fRun->AddModule(Mvd);
  
 // FairDetector *Sts= new CbmSts("STS", kTRUE);
 // Sts->SetGeometryFileName("sts_mvd.geo");
 // fRun->AddModule(Sts);

  
 // FairModule *Target= new CbmTarget("Target");
 // Target->SetGeometryFileName("target_vacuum.geo");
 // fRun->AddModule(Target);		

 //FairDetector *Tof= new CbmTof("TOF", kTRUE );
 //Tof->SetGeometryFileName("tof.geo");
 //fRun->AddModule(Tof);
	
 //FairDetector *Trd= new CbmTrd("TRD",kTRUE );
 //Trd->SetGeometryFileName("trd_9.geo");
 //fRun->AddModule(Trd);

 // FairDetector *Rich= new CbmRich("RICH", kTRUE);
 // Rich->SetGeometryFileName("rich.geo");
 // fRun->AddModule(Rich);

  //FairDetector *Ecal= new CbmEcal("ECAL", kTRUE);
  //Ecal->SetGeometryFileName("ecal.geo");
  //fRun->AddModule(Ecal);


 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  //K+
  FairBoxGenerator *fBox1 = new FairBoxGenerator(321, 1);
     fBox1->SetPRange(0.01,1.);
     fBox1->SetThetaRange(5,150);
     fBox1->SetPhiRange(0.,360);
     fBox1->SetCosTheta();
     primGen->AddGenerator(fBox1);

  //P
  FairBoxGenerator *fBox2 = new FairBoxGenerator(2212, 1);
     fBox2->SetPRange(0.01,1.);
     fBox2->SetThetaRange(5,150);
     fBox2->SetPhiRange(0.,360);
     fBox2->SetCosTheta();
     primGen->AddGenerator(fBox2);


  //Pi+
  FairBoxGenerator *fBox3 = new FairBoxGenerator(211, 1);
     fBox3->SetPRange(0.1,1.);
     fBox3->SetThetaRange(5,150);
     fBox3->SetPhiRange(0.,360);
     fBox3->SetCosTheta();
     primGen->AddGenerator(fBox3);

  //Mu-
//   FairBoxGenerator *fBox4 = new FairBoxGenerator(13, 1);
//      fBox4->SetPRange(0.01,1);
//      fBox4->SetThetaRange(5,150);
//      fBox4->SetPhiRange(0.,360);
//      fBox4->SetCosTheta();
//      primGen->AddGenerator(fBox4);


  fRun->SetStoreTraj(kFALSE);
 

   fRun->Init();

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("testparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();


  fRun->Run(nEvents);



  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
