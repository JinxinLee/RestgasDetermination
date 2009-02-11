// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 0;
  Int_t nEvents = 20000;

  TString simOutput="../data/Lars/MvdDtsSim.root";
  TString parOutput="../data/Lars/MvdDtsParams.root";

  // Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  FairRunSim *fRun = new FairRunSim();

  fRun->SetName("TGeant4");
  // Choose the Geant Navigation System

  fRun->SetOutputFile(simOutput);

  fRun->SetMaterials("media_pnd.geo");

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 


  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
//  Mvd->SetGeometryFileName("OneSiliconSensor.geo");
  Mvd->SetGeometryFileName("../macro/mvd/Lars/dummysensor.root");
  Mvd->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Mvd);

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  FairBoxGenerator *fBox = new FairBoxGenerator(11, 1);//11 e-, 22 mu-
    fBox->SetPRange(1.001,0.999);//GeV/c
    fBox->SetThetaRange(0,0.0005);
    fBox->SetPhiRange(0.,360);
    fBox->SetCosTheta();
    primGen->AddGenerator(fBox);

  fRun->SetStoreTraj(kFALSE);

  fRun->Init();

  // Fill the Parameter containers for this run
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
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
}  
  
