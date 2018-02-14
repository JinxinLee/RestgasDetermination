// Macro to generate 10 events with MVD and STT (120 cm long straws) geometry in pandaroot
// Updated 6.12.2010
// Gianluigi Boca
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 0;
  Int_t nEvents = 10;

  //FileNames
  TString simOutput="MvdStt_Test.root";
  TString parOutput="MvdStt_Params.root";

  // Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
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

  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);

  FairModule *Pipe= new PndPipe("PIPE");
  fRun->AddModule(Pipe);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root"); // only sensors, update follows
  Mvd->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Mvd);

  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_pipe_120cm.geo");
  fRun->AddModule(Stt);


  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);


 // Ion Generator
 //    FairIonGenerator *fIongen= new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
 //    primGen->AddGenerator(fIongen);

  // Box Generator
 FairBoxGenerator *fBox = new FairBoxGenerator(13, 10);  //10 muon events
  //Pions
// FairBoxGenerator *fBox = new FairBoxGenerator(211, 10);  //  positive pions.
    fBox->SetPRange(1.0,1.0);
    fBox->SetThetaRange(15,120);
    fBox->SetPhiRange(0.,360);
    fBox->SetCosTheta();
    fBox->SetXYZ(0.2,0.2,0.);
    primGen->AddGenerator(fBox);

  //EvtGen Generator
//    FairEvtGenGenerator* evtGen = new
//      FairEvtGenGenerator("/home/ralfk/Pandaroot/pandaroot/macro/mvd/output.evt");
//    primGen->AddGenerator(evtGen);

  // Urqmd  Generator
  //    FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator("../../input/00-03fm.100ev.f14");
  //    primGen->AddGenerator(urqmdGen);

  // DPM Generator
  //PndDpmGenerator* dpmGen = new PndDpmGenerator("/home/kph/stockman/fairroot/cbmroot/pandaroot/pgenerators/DpmEvtGen/Background-micro.root");
  //primGen->AddGenerator(dpmGen);

  // Field Map Definition
  // --------------------
  // 1- Reading the new field map in the old format

	fRun->SetBeamMom(15);
  //---------------------Create and Set the Field(s)---------- 
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  
  fRun->SetStoreTraj(kTRUE); // toggle this for use with EVE
  fRun->SetRadLenRegister(kFALSE); // toggle for material budget study

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
}

