// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 0;
  Int_t nEvents = 1000;
  
  // Load basic libraries
  gROOT->Macro("../Libs.C");

  TString parFile = "../data/MvdTrackingParams.root";
  PndMvdFileNameCreator namecreator("../data/MvdTracking.root");
  TString MCFile = namecreator.GetSimFileName();

  CbmRunSim *fRun = new CbmRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System

  fRun->SetOutputFile(MCFile);

  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  CbmModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);

  CbmModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);

  CbmDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
//   Mvd->SetGeometryFileName("MVD14.root");
  Mvd->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Mvd);

//  CbmDetector *Stt= new CbmStt("STT", kTRUE);
//  Stt->SetGeometryFileName("straws_axial.geo");
//  fRun->AddModule(Stt);

//  CbmDetector *Emc = new CbmEmc("EMC",kTRUE);
//  Emc->SetGeometryFileName("emc_module12345.dat"); 
//  fRun->AddModule(Emc);

//  CbmDetector *Drc = new CbmDrc("DIRC", kTRUE);
//  Drc->SetGeometryFileName("dirc.geo"); 
//  fRun->AddModule(Drc);


  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);

 // Particle Generator (pdgid,mult, px,py,pz, vx,vy,vz)

     // single pions for testing
//      CbmParticleGenerator* partGenX   = new CbmParticleGenerator(211,1, 1.,0.,0., 0.,0.,0.);
//      primGen->AddGenerator(partGenX);
//      primGen->AddGenerator(partGenY);
//      primGen->AddGenerator(partGenXYZ);

 // Ion Generator
 //    CbmIonGenerator *fIongen= new CbmIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
 //    primGen->AddGenerator(fIongen);

 // Box Generator
//     PndBoxGenerator *fBox = new PndBoxGenerator(211, 3); //pi+
//     fBox->SetPRange(0.1,10);
//      fBox->SetThetaRange(60,120); //barrel
//     fBox->SetThetaRange(0,30); //forward
//     fBox->SetThetaRange(0,180); //all
//     fBox->SetYRange(-100,100);
//     fBox->SetCosTheta();
//     primGen->AddGenerator(fBox);

  //P
  CbmBoxGenerator *fBox2 = new CbmBoxGenerator(2212, 1);
     fBox2->SetPRange(0.9,1.1);
     fBox2->SetThetaRange(5,150);
     fBox2->SetPhiRange(0.,360.);
     fBox2->SetCosTheta();
     primGen->AddGenerator(fBox2);

  //EvtGen Generator
// CbmEvtGenGenerator* evtGen = new
//   CbmEvtGenGenerator("/home/ralfk/Pandaroot/pandaroot/pgenerators/EvtGen/lambda.evt");
// primGen->AddGenerator(evtGen);  
 // Urqmd  Generator
 //    CbmUrqmdGenerator* urqmdGen = new CbmUrqmdGenerator("../../input/00-03fm.100ev.f14");
 //    primGen->AddGenerator(urqmdGen);
 // DPM Generator
 //PndDpmGenerator* dpmGen = new PndDpmGenerator("/home/kph/stockman/fairroot/cbmroot/pandaroot/pgenerators/DpmEvtGen/Background-micro.root");
 //primGen->AddGenerator(dpmGen);

  // Field Map Definition
  // --------------------
  // 1- Reading the new field map in the old format
//    PndMultiField *fField= new PndMultiField();
//    PndTransMap *map= new PndTransMap("TransMap", "R");
//    PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
//    PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
//    fField->AddField(map);
//    fField->AddField(map1);
//    fField->AddField(map2);
//    fRun->SetField(fField);

      PndConstField *fMagField=new PndConstField();
      fMagField->SetField(0, 0 ,20. ); // values are in kG
      // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
      fMagField->SetFieldRegion(-500, 500,-500, 500, -200, 200);
      fRun->SetField(fMagField);

   // support event display!
   fRun->SetStoreTraj(kTRUE);

   fRun->Init();
  

  // Fill the Parameter containers for this run
  //-------------------------------------------
  CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
  output->open(parFile.Data());
  rtdb->setOutput(output);
  
  // Transport nEvents
  // -----------------

  fRun->Run(nEvents);

  rtdb->saveOutput();
  rtdb->print();


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
