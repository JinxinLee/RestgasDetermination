// Macro for running sim  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
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
  cout<<"libPndData\t"<<gSystem->Load("libPndData")<<endl;
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
  Magnet->SetGeometryFileName("FullSolenoid.root");
  fRun->AddModule(Magnet);
  
  FairModule *dipole= new PndMagnet("MAGNET");
  dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(dipole);
 
  FairDetector *Dch = new PndDchDetector("DCH", kTRUE);
  Dch->SetGeometryFileName("dch.root");
  Dch->SetVerboseLevel(1);
  fRun->AddModule(Dch);

 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

 // Box Generator
  //  gRandom->SetSeed(3523);
  FairBoxGenerator* boxGen = new FairBoxGenerator(13, 1); // 13=muon; 2212=proton 1 = multipl.
  boxGen->SetPRange(1.,1.); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(0,360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(2, 8); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0.,0.,0.);
  primGen->AddGenerator(boxGen);

  fRun->SetStoreTraj(kFALSE);
  fRun->SetBeamMom(15.);

  // Field Map Definition
  // --------------------
  PndMultiField *fField= 0;
  fField = new PndMultiField();
  PndTransMap *map_t= new PndTransMap("TransMap", "R");
  PndSolenoidMap *map_s1= new PndSolenoidMap("SolenoidMap1", "R");
  PndSolenoidMap *map_s2= new PndSolenoidMap("SolenoidMap2", "R");
  PndSolenoidMap *map_s3= new PndSolenoidMap("SolenoidMap3", "R");
  PndSolenoidMap *map_s4= new PndSolenoidMap("SolenoidMap4", "R");
  PndDipoleMap *map_d1= new PndDipoleMap("DipoleMap1", "R");
  PndDipoleMap *map_d2= new PndDipoleMap("DipoleMap2", "R");
  fField->AddField(map_t);
  fField->AddField(map_s1);
  fField->AddField(map_s2);
  fField->AddField(map_s3);
  fField->AddField(map_s4);
  fField->AddField(map_d1);
  fField->AddField(map_d2);
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

  PndMultiFieldPar* fieldPar = 
    (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if(fField)  {  fieldPar->SetParameters(fField); }
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  fieldPar->setChanged(kTRUE);
  
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------
  Int_t nEvents = 500;
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
  
