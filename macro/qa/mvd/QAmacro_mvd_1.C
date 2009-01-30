{
  cout << "QA module for the MVD Simulation." << endl;
  TStopwatch timer;
  timer.Start();

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  gDebug=0;
  int verboseLevel = 0;
  Int_t nEvents = 100;

  //FileNames
  TString simOutput="mvdqasim.root";
  TString parOutput="mvdqapar.root";

  CbmRunSim *fRun = new CbmRunSim();

  fRun->SetName("TGeant4");
  // Choose the Geant Navigation System

  fRun->SetOutputFile(simOutput);

  fRun->SetMaterials("media_pnd.geo");

  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  CbmModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);

  CbmDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  Mvd->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Mvd);

//  CbmModule *Magnet= new PndMagnet("MAGNET");
//  Magnet->SetGeometryFileName("magnet.geo");
//  fRun->AddModule(Magnet);

  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator
  //Pions
  CbmBoxGenerator *fBox = new CbmBoxGenerator(211, 5);
    fBox->SetPRange(0.1,2.5); // GeV/c
    fBox->SetPhiRange(0.,360); // Azimuth angle range [degree]
    fBox->SetThetaRange(5,150); // Polar angle in lab system range [degree]
    fBox->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
    primGen->AddGenerator(fBox);

   PndMultiField *fField= new PndMultiField();

   PndTransMap *map= new PndTransMap("TransMap", "R");
   PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
   PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
   fField->AddField(map);
   fField->AddField(map1);
   fField->AddField(map2);

   fRun->SetField(fField);

   fRun->SetStoreTraj(kTRUE); // toggle this for use with EVE
//   fRun->SetStoreTraj(kFALSE);

   fRun->Init();

  // Fill the Parameter containers for this run
  //-------------------------------------------
  CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();

  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

  Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
  output->open(parOutput.Data(),"RECREATE");
  rtdb->setOutput(output);

  fRun->Run(nEvents);

  rtdb->saveOutput();
  rtdb->print();

    timer.Stop();
    Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

	cout << " Test passed" << endl;
    cout << " All ok " << endl;

  exit(0);
}

