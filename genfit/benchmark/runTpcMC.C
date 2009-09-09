// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");


  Int_t nEvents = 100;

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  fRun->SetOutputFile("tpcMC.root");

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  

  std::cout<< "Materials set" << std::endl;
  

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetGeometryFileName("tpc.geo");
  //  PndTpc->SetGeometryFileName("benchmark.geo");
  fRun->AddModule(PndTpc);


   
 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  FairBoxGenerator* boxGen = new FairBoxGenerator(13, 1); // 13 = muon; 1 = multipl.
  boxGen->SetPRange(1.,1.); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(40.,50.); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
  primGen->AddGenerator(boxGen);

  //FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  /*
  PndConstField *fField=new PndConstField();
  fField->SetField(0, 20. ,13. ); // values are in kG
  fField->SetFieldRegion(-500, 500,-500, 500, -50., 100);
  fRun->SetField(fField);
     // Field Map Definition
  // --------------------
  */

  PndMultiField *fField= new PndMultiField();

  PndTransMap *map= new PndTransMap("TransMap", "R");
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
  fField->AddField(map);
  fField->AddField(map1);
  fField->AddField(map2);
  
/**
 // New field oct.2008
  PndTransMap  *tmap= new PndTransMap("Trans1", "R");
  PndDipoleMap *dmap1= new PndDipoleMap("Dipole1", "R");
  PndDipoleMap *dmap2= new PndDipoleMap("Dipole2", "R");
    
  PndSolenoidMap *smap1= new PndSolenoidMap("Solenoid1", "R");
  PndSolenoidMap *smap2= new PndSolenoidMap("Solenoid2", "R");
  PndSolenoidMap *smap3= new PndSolenoidMap("Solenoid3", "R");
  PndSolenoidMap *smap4= new PndSolenoidMap("Solenoid4", "R");
        
  fField->AddField(tmap);
  fField->AddField(dmap1);
  fField->AddField(dmap2);

  fField->AddField(smap1);
  fField->AddField(smap2);
  fField->AddField(smap3);
  fField->AddField(smap4);
*/






  //magnetic field
  /*  PndConstField *fMagField=new PndConstField();
      fMagField->SetField(0.,0.,20.); // values are in kG
      fMagField->SetFieldRegion(-50, 50,-50, 50, -100, 100);// values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
      fField->AddField(fMagField);
  */
  fRun->SetField(fField);

  
  fRun->Init();


  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("tpcMC.param.root");
  rtdb->setOutput(output);

  double f[3];
  double x[3] = {10.,10.,10.};
  fRun->GetField()->GetFieldValue(x,f);
  std::cout <<"#################### "<< fRun->GetField() << " " << f[2] << std::endl;

  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

  
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------

  
  fRun->Run(nEvents);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
 printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
