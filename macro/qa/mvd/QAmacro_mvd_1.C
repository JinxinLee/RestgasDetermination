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

  FairRunSim *fRun = new FairRunSim();

  fRun->SetName("TGeant4");
  // Choose the Geant Navigation System

  fRun->SetOutputFile(simOutput);

  fRun->SetMaterials("media_pnd.geo");

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  Mvd->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Mvd);

//  FairModule *Magnet= new PndMagnet("MAGNET");
//  Magnet->SetGeometryFileName("magnet.geo");
//  fRun->AddModule(Magnet);

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator
  //Pions
  FairBoxGenerator *fBox = new FairBoxGenerator(211, 5);
    fBox->SetPRange(0.1,2.5); // GeV/c
    fBox->SetPhiRange(0.,360); // Azimuth angle range [degree]
    fBox->SetThetaRange(5,150); // Polar angle in lab system range [degree]
    fBox->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
    primGen->AddGenerator(fBox);

   /** Set the field*/
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
 

  // Fill the Parameter containers for this run
  //-------------------------------------------
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();

  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
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

