{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  CbmRunSim *fRun = new CbmRunSim();
  fRun->SetName("TGeant3");
  TString base="tstQA";
  TString parFile = base+".param.root";
  TString outFile = base+".root";
  fRun->SetOutputFile(outFile);
  fRun->SetMaterials("media_pnd.geo");
  
  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  CbmModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  fRun->AddModule(Pipe);
  
  CbmModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);


  CbmDetector *Dch = new PndDchDetector("DCH", kTRUE);
  Dch->SetGeometryFileName("dch.root");
  Dch->SetVerboseLevel(1);
  fRun->AddModule(Dch);

 // Create and Set Event Generator
 //-------------------------------

  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);

  gRandom->SetSeed(3523);
  CbmBoxGenerator* boxGen = new CbmBoxGenerator(2212, 3); // 13=muon; 2212=proton 1 = multipl.
  boxGen->SetPRange(1.,10.); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(-180,180); // Azimuth angle range [degree]
  boxGen->SetThetaRange(1, 5); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0.,0.,0.);
  primGen->AddGenerator(boxGen);

  fRun->SetStoreTraj(kFALSE);
  

  // Field Map Definition
  // --------------------
  PndMultiField *fField= 0;
  fField = new PndMultiField();
//   PndTransMap *map= new PndTransMap("TransMap", "R");
//   fField->AddField(map);
//   PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
//   fField->AddField(map2);
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  fField->AddField(map1);
  fRun->SetField(fField);

  fRun->Init(); 

  CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
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
  
  Int_t nEvents = 100;
  fRun->Run(nEvents);
   
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0); 
}  
  
