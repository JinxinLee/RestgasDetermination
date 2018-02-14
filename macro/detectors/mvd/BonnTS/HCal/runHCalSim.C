// Macro to simulate the HCal of the Tracking Station
// Updated 21.03.2010
// Simone Bianco

int runHCalSim(Double_t beam_mom = 0.7, Int_t nEvents = 100)

{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 0;
 
  //FileNames
  TString simOutput="HCalMC.root";
  TString parOutput="HCalMC_Params.root";

  // Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
 
  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant4");
  // Choose the Geant Navigation System

  fRun->SetOutputFile(simOutput);

  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairDetector *HCal = new PndMvdDetector("HCal", kTRUE);
  HCal->SetGeometryFileName("HCal.root");
  HCal->SetVerboseLevel(verboseLevel);
  fRun->AddModule(HCal);

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator

  FairBoxGenerator *fBox = new FairBoxGenerator(2212, 1);
    fBox->SetPRange(beam_mom,beam_mom);
    fBox->SetThetaRange(0.,0.);
    fBox->SetPhiRange(0.,360);
    fBox->SetXYZ(0.,0.,-5.);
    primGen->AddGenerator(fBox);

    //    fRun->SetStoreTraj(kFALSE); // toggle this for use with EVE
    fRun->SetStoreTraj(kTRUE); // toggle this for use with EVE
    //fRun->SetRadLenRegister(kTRUE); 

    fRun->Init();

    // Fill the Parameter containers for this run
    //-------------------------------------------
    FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
    Bool_t kParameterMerged=kTRUE;
    FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
    output->open(parOutput.Data(),"RECREATE");
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
  return 0;

}

