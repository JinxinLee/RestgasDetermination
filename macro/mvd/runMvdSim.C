// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 0;
  Int_t nEvents = 10;
  
  //FileNames
  TString simOutput="Mvd_Test.root";

//  TString simOutput="Mvd_DPMfixed_4GeV_10000.root";
  TString parOutput="MvdParams.root";

  // Load basic libraries
//   gROOT->Macro("Libs.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  CbmRunSim *fRun = new CbmRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant4");
  // Choose the Geant Navigation System

  fRun->SetOutputFile(simOutput);

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
//      CbmParticleGenerator* partGenY   = new CbmParticleGenerator(211,1, 0.,1.,0., 0.,0.,0.);
//      CbmParticleGenerator* partGenZ   = new CbmParticleGenerator(211,1, 0.,0.1,1., 0.,0.,0.);
//      CbmParticleGenerator* partGenXYZ = new CbmParticleGenerator(211,1, 1.,1.,5., 0.,0.,0.);
//      primGen->AddGenerator(partGenX);
//      primGen->AddGenerator(partGenY);
//      primGen->AddGenerator(partGenZ);
//      primGen->AddGenerator(partGenXYZ);

 // Ion Generator
 //    CbmIonGenerator *fIongen= new CbmIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
 //    primGen->AddGenerator(fIongen);

  // Box Generator
  //Pions
 CbmBoxGenerator *fBox = new CbmBoxGenerator(211, 1);
    fBox->SetPRange(0.1,2.5);
    fBox->SetThetaRange(5,150);
    fBox->SetPhiRange(0.,360);
    fBox->SetCosTheta();
    primGen->AddGenerator(fBox);

  //EvtGen Generator
//    CbmEvtGenGenerator* evtGen = new
//      CbmEvtGenGenerator("/home/ralfk/Pandaroot/pandaroot/macro/mvd/output.evt");
//    primGen->AddGenerator(evtGen);  

  // Urqmd  Generator
  //    CbmUrqmdGenerator* urqmdGen = new CbmUrqmdGenerator("../../input/00-03fm.100ev.f14");
  //    primGen->AddGenerator(urqmdGen);

  // DPM Generator
  //PndDpmGenerator* dpmGen = new PndDpmGenerator("/home/kph/stockman/fairroot/cbmroot/pandaroot/pgenerators/DpmEvtGen/Background-micro.root");
  //primGen->AddGenerator(dpmGen);

  // Field Map Definition
  // --------------------
  // 1- Reading the new field map in the old format

   PndMultiField *fField= new PndMultiField();

   PndTransMap *map= new PndTransMap("TransMap", "R");
   PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
   PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
   fField->AddField(map);
   fField->AddField(map1);
   fField->AddField(map2);

   fRun->SetField(fField);
   
   //fRun->SetStoreTraj(kTRUE); // toggle this for use with EVE
   fRun->SetStoreTraj(kFALSE);


   fRun->Init();

 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------

 // Set cuts for storing the trajectpries
//   CbmTrajFilter* trajFilter = CbmTrajFilter::Instance();
//     trajFilter->SetStepSizeCut(0.01); // 1 cm
//     trajFilter->SetVertexCut(-200., -200., -200, 200., 200., 200.);
//     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
//     trajFilter->SetStorePrimaries(kTRUE);
//     trajFilter->SetStoreSecondaries(kTRUE);
   

  // Fill the Parameter containers for this run
  //-------------------------------------------
  CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
  output->open(parOutput.Data(),"RECREATE");
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
  
