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
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
//  gSystem->Load("libgenfit");
//  gSystem->Load("libtpc");
//  gSystem->Load("libtpcreco");
//  gSystem->Load("librecotasks");
  gSystem->Load("libGen");
  gSystem->Load("libPGen");
  gSystem->Load("libMvd");


  CbmRunSim *fRun = new CbmRunSim();
  
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

  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

 // CbmModule *Pipe= new PndPipe("PIPE");
 // Pipe->SetGeometryFileName("pipe.geo");
 // fRun->AddModule(Pipe);
  
  
  //CbmModule *Magnet= new PndMagnet("MAGNET");
  // 1- Active shielding Geometry
  //Magnet->SetGeometryFileName("magnet_active.geo");
  // 2- Iron Magnet
  // Magnet->SetGeometryFileName("magnet_iron.geo");
  //fRun->AddModule(Magnet);
  
  CbmDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD14.root");
  fRun->AddModule(Mvd);
  
 // CbmDetector *Sts= new CbmSts("STS", kTRUE);
 // Sts->SetGeometryFileName("sts_mvd.geo");
 // fRun->AddModule(Sts);

  
 // CbmModule *Target= new CbmTarget("Target");
 // Target->SetGeometryFileName("target_vacuum.geo");
 // fRun->AddModule(Target);		

 //CbmDetector *Tof= new CbmTof("TOF", kTRUE );
 //Tof->SetGeometryFileName("tof.geo");
 //fRun->AddModule(Tof);
	
 //CbmDetector *Trd= new CbmTrd("TRD",kTRUE );
 //Trd->SetGeometryFileName("trd_9.geo");
 //fRun->AddModule(Trd);

 // CbmDetector *Rich= new CbmRich("RICH", kTRUE);
 // Rich->SetGeometryFileName("rich.geo");
 // fRun->AddModule(Rich);

  //CbmDetector *Ecal= new CbmEcal("ECAL", kTRUE);
  //Ecal->SetGeometryFileName("ecal.geo");
  //fRun->AddModule(Ecal);


 // Create and Set Event Generator
 //-------------------------------

  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);

  //K+
  CbmBoxGenerator *fBox1 = new CbmBoxGenerator(321, 1);
     fBox1->SetPRange(0.01,1.);
     fBox1->SetThetaRange(5,150);
     fBox1->SetPhiRange(0.,360);
     fBox1->SetCosTheta();
     primGen->AddGenerator(fBox1);

  //P
  CbmBoxGenerator *fBox2 = new CbmBoxGenerator(2212, 1);
     fBox2->SetPRange(0.01,1.);
     fBox2->SetThetaRange(5,150);
     fBox2->SetPhiRange(0.,360);
     fBox2->SetCosTheta();
     primGen->AddGenerator(fBox2);


  //Pi+
  CbmBoxGenerator *fBox3 = new CbmBoxGenerator(211, 1);
     fBox3->SetPRange(0.1,1.);
     fBox3->SetThetaRange(5,150);
     fBox3->SetPhiRange(0.,360);
     fBox3->SetCosTheta();
     primGen->AddGenerator(fBox3);

  //Mu-
//   CbmBoxGenerator *fBox4 = new CbmBoxGenerator(13, 1);
//      fBox4->SetPRange(0.01,1);
//      fBox4->SetThetaRange(5,150);
//      fBox4->SetPhiRange(0.,360);
//      fBox4->SetCosTheta();
//      primGen->AddGenerator(fBox4);


  fRun->SetStoreTraj(kFALSE);
 

   fRun->Init();

  CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
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
  
