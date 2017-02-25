// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
int runMC(TString fname="dsdsj20k_4291.evt", int nEvents = 10 )
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  //Int_t nEvents = 2000;

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  TString outfilename=fname+".mc.root";
  TString paramfilename=fname+".param.root";
  
  fRun->SetOutputFile(outfilename.Data());

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  

  std::cout<< "Materials set" << std::endl;
  

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  //fRun->AddModule(Pipe);
  
  
  //FairModule *Magnet= new PndMagnet("MAGNET");
  //Magnet->SetGeometryFileName("magnet.geo");
  //fRun->AddModule(Magnet);
  
  FairDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetGeometryFileName("tpc.geo");
  fRun->AddModule(PndTpc);
  
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  //Emc->SetGeometryFileName("emc_module12345.dat"); // if you want to use old geometry for FwEndCap
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root"); // if you want to use new geometry for FwEndCap
  fRun->AddModule(Emc);
   
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD14.root");
  fRun->AddModule(Mvd);

   
 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

 // Urqmd  Generator
//  FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator("../../input/00-03fm.100ev.f14");
//  primGen->AddGenerator(urqmdGen);

 // Particle Generator
  //FairParticleGenerator* partGen = new FairParticleGenerator(211, 10, 1, 0,3,kTRUE);
  //primGen->AddGenerator(partGen);
 
 // Ion Generator
 //    FairIonGenerator *fIongen= new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
 //    primGen->AddGenerator(fIongen);
 // Box Generator
 
 
  FairEvtGenGenerator* evtGen = new FairEvtGenGenerator(fname.Data());//infile.Data());
  primGen->AddGenerator(evtGen);
  
  
  // Box Generator
  /*
  FairBoxGenerator* boxGen = new FairBoxGenerator(3122, 1); // 13 = muon; 1 = multipl.
  boxGen->SetPRange(0.5,0.5); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(35, 35); // Azimuth angle range [degree]
  boxGen->SetThetaRange(60,60); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
  primGen->AddGenerator(boxGen);
 */
  //FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  //fRun->SetGenerator(primGen);

  // PndDpmGenerator* dpmGen = new PndDpmGenerator("pgenerators/DpmEvtGen/Background-micro.root");
  //primGen->AddGenerator(dpmGen);  

 // Field Map Definition
 // --------------------
  // 1- Reading the new field map in the old format

     // FairFieldMap *fMagField= new FairFieldMap("FIELD.v04_pavel.map");
   // Constant Field
      PndConstField *fMagField=new PndConstField();
      fMagField->SetField(0, 0 ,20. ); // values are in kG
     // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
  fMagField->SetFieldRegion(-500, 500,-500, 500, -200, 200);

  // 2- Reading the new field map in the new format

//  FairField *fMagField= new FairFieldMapSym3("FieldActive");
  // Active Shielding

  fRun->SetField(fMagField);
   
   //fRun->SetStoreTraj(kTRUE);
    

   fRun->Init();


 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------

;
 // Set cuts for storing the trajectpries
//   FairTrajFilter* trajFilter = FairTrajFilter::Instance();
//   trajFilter->SetStepSizeCut(0.01); // 1 cm
//   trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
//   trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//   trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
//   trajFilter->SetStorePrimaries(kTRUE);
//   trajFilter->SetStoreSecondaries(kTRUE);
   

  // Fill the Parameter containers for this run
  //-------------------------------------------

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(paramfilename.Data());
  rtdb->setOutput(output);

  PndConstPar* fieldPar = (PndConstPar*) rtdb->getContainer("PndConstPar");
  if ( fMagField ) {  fieldPar->SetParameters(fMagField); }
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  fieldPar->setChanged(kTRUE);
  
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------

  
  fRun->Run(nEvents);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
 printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}  
  
