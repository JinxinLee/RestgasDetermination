// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
  cout<<"libGeoBase\t"<<gSystem->Load("libGeoBase")<<endl;;
  cout<<"libParBase\t"<<gSystem->Load("libParBase")<<endl;
  cout<<"libBase\t\t"<<gSystem->Load("libBase")<<endl;
  cout<<"libMCStack\t"<<gSystem->Load("libMCStack")<<endl;
  cout<<"libField\t"<<gSystem->Load("libField")<<endl;
  cout<<"libPassive\t"<<gSystem->Load("libPassive")<<endl;
  cout<<"libGen\t\t"<<gSystem->Load("libGen")<<endl;
  cout<<"libgenfit\t"<<gSystem->Load("libgenfit")<<endl;
  cout<<"libDch\t\t"<<gSystem->Load("libDch")<<endl;

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  fRun->SetName("TGeant3");

  // Base File name
  TString base="tst";
  //Parameter file
  TString parFile = base+".param.root";
  // Output file
  TString outFile = base+".root";
  fRun->SetOutputFile(outFile);

  // Set Material file Name
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  fRun->AddModule(Pipe);
  
  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);


  FairDetector *Dch = new PndDchDetector("DCH", kTRUE);
  Dch->SetGeometryFileName("dch.root");
  Dch->SetVerboseLevel(1);
  fRun->AddModule(Dch);

 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

 // Particle Generator
  //FairParticleGenerator* partGen = new FairParticleGenerator(211, 10, 1, 0,3,kTRUE);
  //primGen->AddGenerator(partGen);
 
 // Box Generator
  //  gRandom->SetSeed(3523);
  FairBoxGenerator* boxGen = new FairBoxGenerator(2212, 3); // 13=muon; 2212=proton 1 = multipl.
  boxGen->SetPRange(1.,10.); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(-180,180); // Azimuth angle range [degree]
  boxGen->SetThetaRange(1, 5); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0.,0.,0.);
  primGen->AddGenerator(boxGen);

  fRun->SetStoreTraj(kTRUE);
  

  // Field Map Definition
  // --------------------
  //PndConstField *fField=new PndConstField();
  //fField->SetField(0, 10 ,0 ); // values are in kG    //dipole
  //fField->SetFieldRegion(-145, 145,-45, 45, 380, 560);  // dipole
  // fField->SetField(0, 0 ,20. ); // values are in kG    //solenoid
//   fField->SetFieldRegion(-500, 500,-500, 500, -200, 200);  // solenoid
  PndMultiField *fField= 0;
  fField = new PndMultiField();
//   PndTransMap *map= new PndTransMap("TransMap", "R");
//   fField->AddField(map);
//   PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
//   fField->AddField(map2);
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  fField->AddField(map1);
  fRun->SetField(fField);
  //-----------end of Bfield stuff

  fRun->Init(); 

  // Fill the Parameter containers for this run
  //-------------------------------------------

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parFile.Data());
  rtdb->setOutput(output);
  //PndConstPar* fieldPar = (PndConstPar*) rtdb->getContainer("PndConstPar");
  PndMultiFieldPar* fieldPar = 
    (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if(fField)  {  fieldPar->SetParameters(fField); }
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  fieldPar->setChanged(kTRUE);
  
  rtdb->saveOutput();
  rtdb->print();
  
  // Transport nEvents
  // -----------------
  Int_t nEvents = 100;
  fRun->Run(nEvents);
   
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  exit(0);
  // TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");
  //  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
  //  c1->SetFillColor(10);
  //  geoMan->SetVisLevel(3); 
  // geoMan->GetMasterVolume()->Draw("same");
 
}  
  
