// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
   gSystem->Load("libEnDrc");          // our library

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
  
//   FairModule *Magnet= new PndMagnet("MAGNET");
//   Magnet->SetGeometryFileName("magnet.geo");
//   fRun->AddModule(Magnet);


//   FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
//   Mvd->SetGeometryFileName("MVD14.root");
//   fRun->AddModule(Mvd);


//   FairDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
//   PndTpc->SetGeometryFileName("tpc.geo");
//   fRun->AddModule(PndTpc);
  
  
//   FairDetector *Emc = new PndEmc("EMC",kTRUE);
//   Emc->SetGeometryFileName("emc_module12345.dat");
//   //Emc->SetGeometryFileName("emc_module3.root"); 
//    fRun->AddModule(Emc);  
// //    FairDetector *Muo = new PndMuo("MUO",kTRUE);
// //    Muo->SetGeometryFileName("muon_super_light2.geo"); 
// //    fRun->AddModule(Muo);
    

   FairDetector *Drc = new PndDrc("DIRC", kTRUE);
   Drc->SetGeometryFileName("dirc.geo"); 
   fRun->AddModule(Drc);


   //And add the enddrc
  FairDetector* Dsk = new PndEnDrc("EDRC", kTRUE);
  Dsk->SetGeometryFileName("endrc.root");
  fRun->AddModule(Dsk);

    // Create and Set Event Generator
    //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Urqmd  Generator
  // FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator("../../input/00-03fm.100ev.f14");
  // primGen->AddGenerator(urqmdGen);

  // Particle Generator
  FairEvtGenGenerator* partGen = new FairEvtGenGenerator("/home/dglazier/panda/pandaroot/pgenerators/EvtGen/output.evt");
 primGen->AddGenerator(partGen);

 // Box Generator

  // 
  fRun->SetStoreTraj(kTRUE);
  
   PndMultiField *fField= new PndMultiField();

   PndTransMap *map= new PndTransMap("TransMap", "R");
   PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
   PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
   fField->AddField(map);
   fField->AddField(map1);
   fField->AddField(map2);
  
   //magnetic field
  /*  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0.,0.,20.); // values are in kG
  fMagField->SetFieldRegion(-50, 50,-50, 50, -100, 100);// values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
  fField->AddField(fMagField);
  */
  fRun->SetField(fField);
  


  fRun->Init();

 // -Trajectories Visualization
 // ----------------------------
     FairTrajFilter* trajFilter = FairTrajFilter::Instance();
 // Set cuts for storing the trajectpries
     trajFilter->SetStepSizeCut(0.01); // 1 cm
//     trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
//     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
     trajFilter->SetStorePrimaries(kTRUE);
     trajFilter->SetStoreSecondaries(kTRUE);

// 
//   // Fill the Parameter containers for this run
//   //-------------------------------------------
//      
   FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
   Bool_t kParameterMerged=kTRUE;
  
   
   
     
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

    FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
    output->open("testparams.root");
    rtdb->setOutput(output);
    rtdb->saveOutput();
    rtdb->print();
  // Transport nEvents
  // -----------------1
  
 Int_t nEvents = 10;
 fRun->Run(nEvents);
     
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
   
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0);

}  
  
