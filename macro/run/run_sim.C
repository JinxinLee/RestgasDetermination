// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  fRun->SetOutputFile("testsimu10.root");


  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  
  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid.root");
  fRun->AddModule(Magnet);

  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);

  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks.geo");
  fRun->AddModule(Stt);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);
  
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root"); // if you want to use new geometry for FwEndCap
  fRun->AddModule(Emc);
  
  FairDetector *Tof = new PndTof("TOF",kTRUE);
  Tof->SetGeometryFileName("tofbarrel.geo");
  fRun->AddModule(Tof);

  FairDetector *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetGeometryFileName("dirc.geo"); 
  fRun->AddModule(Drc); 

  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetGeometryFileName("muopars.root");
  Muo->SetMdtVersion("torino");
  fRun->AddModule(Muo);
/*
  FairDetector *Dch = new PndDchDetector("DCH", kTRUE);
  Dch->SetGeometryFileName("dch.root"); 
  fRun->AddModule(Dch);

*/

    // Create and Set Event Generator
    //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Urqmd  Generator
  // FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator("../../input/00-03fm.100ev.f14");
  // primGen->AddGenerator(urqmdGen);

  // Particle Generator
  //FairParticleGenerator* partGen = new FairParticleGenerator(13, 1, 0.5, 0., 0.);
  //primGen->AddGenerator(partGen);

/*  // Box Generator
  FairBoxGenerator* boxGen = new FairBoxGenerator(13, 200); // 13 = muon; 1 = multipl.
  //  boxGen->SetPRange(1.,1.1); // GeV/c
  boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0., 90.); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
  primGen->AddGenerator(boxGen);
 */
  // proton 2212 pi+ 211 pi- -211 
  Double_t randx, randy;
  for (Int_t n =0; n<5; n++){
	randx= gRandom->Gaus(0,1);
	randy= gRandom->Gaus(0,1);
	FairParticleGenerator* partGen = new FairParticleGenerator(2212, 1, 0.3*randx, 0.3*randy, 1.0);
  	primGen->AddGenerator(partGen);
  }

 
  // Ion Generator
  //FairIonGenerator *fIongen= new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
  //  primGen->AddGenerator(fIongen);

  
  // 
  //fRun->SetStoreTraj(kTRUE);
   fRun->SetBeamMom(15);
   PndMultiField *fField= new PndMultiField();

   PndTransMap *map_t= new PndTransMap("TransMap", "R");
   PndDipoleMap *map_d1= new PndDipoleMap("DipoleMap1", "R");
   PndDipoleMap *map_d2= new PndDipoleMap("DipoleMap2", "R");
   PndSolenoidMap *map_s1= new PndSolenoidMap("SolenoidMap1", "R");
   PndSolenoidMap *map_s2= new PndSolenoidMap("SolenoidMap2", "R");
   PndSolenoidMap *map_s3= new PndSolenoidMap("SolenoidMap3", "R");
   PndSolenoidMap *map_s4= new PndSolenoidMap("SolenoidMap4", "R");

   fField->AddField(map_t);
   fField->AddField(map_d1);
   fField->AddField(map_d2);
   fField->AddField(map_s1);
   fField->AddField(map_s2);
   fField->AddField(map_s3);
   fField->AddField(map_s4);


   fRun->SetField(fField);
  


  fRun->Init();

 // -Trajectories Visualization
 // ----------------------------
  /*   FairTrajFilter* trajFilter = FairTrajFilter::Instance();
 // Set cuts for storing the trajectpries
     trajFilter->SetStepSizeCut(0.01); // 1 cm
//     trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
//     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
     trajFilter->SetStorePrimaries(kTRUE);
     trajFilter->SetStoreSecondaries(kTRUE);
*/
// 
//   // Fill the Parameter containers for this run
//   //-------------------------------------------
//      
   FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
   Bool_t kParameterMerged=kTRUE;
   FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
   output->open("testparams.root");
   rtdb->setOutput(output);
   
       
  //if a field is used save the parameters in the RTDB
/*    
    PndSolenoidPar* Par1 = (PndSolenoidPar*) rtdb->getContainer("PndSolenoidPar");
    if ( map2 ) {  Par1->SetParameters(map2); }
    Par1->setChanged();
    Par1->setInputVersion(fRun->GetRunId(),1);
 
    PndDipolePar* Par2 = (PndDipolePar*) rtdb->getContainer("PndDipolePar");
    if (map1 ) {  Par2->SetParameters(map1); }
    Par2->setInputVersion(fRun->GetRunId(),1);
    Par2->setChanged();
  
    PndTransPar* Par3 = (PndTransPar*) rtdb->getContainer("PndTransPar");
    if (map ) {  Par3->SetParameters(map); }
    Par3->setInputVersion(fRun->GetRunId(),1);
    Par3->setChanged();
  */

    PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
    if (fField) {  Par->SetParameters(fField); }
    Par->setInputVersion(fRun->GetRunId(),1);
    Par->setChanged();

    
    rtdb->saveOutput();
    rtdb->print();
  // Transport nEvents
  // -----------------1
  
 Int_t nEvents = 2;
 fRun->Run(nEvents);
     
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
   
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0);

}  
  
