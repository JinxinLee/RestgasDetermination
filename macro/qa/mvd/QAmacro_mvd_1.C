void QAmacro_mvd_1()
{
  cout << "QA module for the MVD Simulation." << endl;
  TStopwatch timer;
  timer.Start();
  
  gDebug=0;
  int verboseLevel = 0;
  Int_t nEvents = 100;
  
  //FileNames
  TString simOutput="mvdqasim.root";
  TString parOutput="mvdqapar.root";
  TString digiFile = "all.par"; //The emc run the hit producer directly

  
  FairRunSim *fRun = new FairRunSim();
  
  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  
  fRun->SetOutputFile(simOutput);
  fRun->SetMaterials("media_pnd.geo");

  //-------Set the parameter output --------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);

  //---------------------Set Parameter output      ----------
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parOutput.Data());
  rtdb->setOutput(output);
  
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);
  
  FairModule *Pipe= new PndPipe("PIPE");
  fRun->AddModule(Pipe);
  
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  Mvd->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Mvd);
  
  //  FairModule *Magnet= new PndMagnet("MAGNET");
  //  Magnet->SetGeometryFileName("magnet.geo");
  //  fRun->AddModule(Magnet);
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  // Box Generator
  //Pions
  FairBoxGenerator *fBox = new FairBoxGenerator(211, 2);
  fBox->SetPRange(0.5,2.5); // GeV/c
  fBox->SetPhiRange(0.,360); // Azimuth angle range [degree]
  fBox->SetThetaRange(5,150); // Polar angle in lab system range [degree]
  fBox->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
  primGen->AddGenerator(fBox);
  
  FairBoxGenerator *fBox2 = new FairBoxGenerator(-211, 2);
  fBox2->SetPRange(0.5,2.5); // GeV/c
  fBox2->SetPhiRange(0.,360); // Azimuth angle range [degree]
  fBox2->SetThetaRange(5,150); // Polar angle in lab system range [degree]
  fBox2->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
  primGen->AddGenerator(fBox2);
  
  /** Set the field*/
  fRun->SetBeamMom(1.5);
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  
  
  fRun->Init();
  
  
//  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
//  Bool_t kParameterMerged=kTRUE;
//  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
//  output->open(parOutput.Data(),"RECREATE");
//  rtdb->setOutput(output);
  
  fRun->Run(nEvents);
  
  rtdb->saveOutput();
  rtdb->print();
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
	cout << " Sim passed" << endl;
  cout << " All ok " << endl;
  delete fRun;
  exit(0);
}
