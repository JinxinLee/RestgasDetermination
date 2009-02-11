// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 0;
  Int_t nEvents = 1000;
  
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  TString parFile = "../data/MvdTrackingParams.root";
  PndMvdFileNameCreator namecreator("../data/MvdTracking.root");
  namecreator.SetVerbose(1);
  TString MCFile = namecreator.GetSimFileName();

  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System

  fRun->SetOutputFile(MCFile);

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
 
  FairModule *Pipe= new PndPipe("PIPE");
  //Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);

  FairDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
  Tpc->SetGeometryFileName("tpc.geo");
  fRun->AddModule(Tpc);




  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  //EvtGen Generator
  FairEvtGenGenerator* evtGen = new
    FairEvtGenGenerator("../data/Dmesons/Dmesons.evt");
  primGen->AddGenerator(evtGen);

  PndMultiField *fField= new PndMultiField();

  PndTransMap *map= new PndTransMap("TransMap", "R");
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
  fField->AddField(map);
  fField->AddField(map1);
  fField->AddField(map2);
  
   fRun->SetField(fField);

   fRun->SetStoreTraj(kTRUE);

   fRun->Init();


  // Fill the Parameter containers for this run
  //-------------------------------------------
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;

  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parFile.Data());
  rtdb->setOutput(output);
  
  // Transport nEvents
  // -----------------

  fRun->Run(nEvents);

  rtdb->saveOutput();
  rtdb->print();


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
