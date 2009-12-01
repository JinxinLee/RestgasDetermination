// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// This macro is supposed to run the full simulation of the panda detector. and to save some visualization 
// points to display track in the eventdisplay

run_sim_vis(Int_t nEvents = 10)
{
  //-----User Settings:-----------------------------------------------
  TString  SimEngine      ="TGeant3"; 
  TString  OutputFile     ="sim_complete.root";
  TString  ParOutputfile  ="simparams.root";
  Double_t BeamMomentum   =15.0;
  TString  MediaFile      ="media_pnd.geo";
  gDebug                  = 0;
  
  
  //------------------------------------------------------------------


  TStopwatch timer;
  timer.Start();
 
  // Load basic libraries---------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
 
  
  // Create the Simulation run manager--------------------------------
  FairRunSim *fRun = new FairRunSim();
  fRun->SetName(SimEngine.Data() );
  fRun->SetOutputFile(OutputFile.Data());
  fRun->SetBeamMom(BeamMomentum);
  fRun->SetMaterials(MediaFile.Data());
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();

   // Create and add detectors

 //-------------------------  CAVE      -----------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  //-------------------------  Magnet   ----------------- 
/* FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid.root");
  fRun->AddModule(Magnet);
*/
  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);
 //-------------------------  STT       -----------------
  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks.geo");
  fRun->AddModule(Stt);
 //-------------------------  MVD       -----------------
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);
 //-------------------------  EMC       -----------------
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root"); // if you want to use new geometry for FwEndCap
  fRun->AddModule(Emc);
 //-------------------------  TOF       -----------------  
  FairDetector *Tof = new PndTof("TOF",kTRUE);
  Tof->SetGeometryFileName("tofbarrel.geo");
  fRun->AddModule(Tof);
 //-------------------------  DRC       -----------------
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetGeometryFileName("dirc.geo"); 
  Drc->SetRunCherenkov(kFALSE);
  fRun->AddModule(Drc); 
  //-------------------------  MDT       -----------------
  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetBarrel("torino");
  Muo->SetEndcap("torino");
  Muo->SetMuonFilter("torino");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);
   //-------------------------  DCH       -----------------
  FairDetector *Dch = new PndDchDetector("DCH", kTRUE);
  Dch->SetGeometryFileName("dch.root"); 
  fRun->AddModule(Dch);



  // Create and Set Event Generator
  //-------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator
  FairBoxGenerator* boxGen = new FairBoxGenerator(13, 10); // 13 = muon; 1 = multipl.
  //  boxGen->SetPRange(1.,1.1); // GeV/c
  boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0., 90.); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
  primGen->AddGenerator(boxGen);


 //---------------------Create and Set the Field(s)---------- 
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);


 //---------------------Set Parameter output      ---------- 
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(ParOutputfile.Data());
  rtdb->setOutput(output);
  
 //-------------------------- switch on the vis manager-----------//

 fRun->SetStoreTraj(kTRUE);

 //-------------------------  Initialize the RUN  -----------------  
  fRun->Init();
   
  
 //----------------- Set some cuts for the visualization-----------
 
     FairTrajFilter* trajFilter = FairTrajFilter::Instance();
 // Set cuts for storing the trajectpries
     trajFilter->SetStepSizeCut(0.04); // 1 cm
//     trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
//     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
     trajFilter->SetStorePrimaries(kTRUE);
     trajFilter->SetStoreSecondaries(kTRUE);

 //-------------------------  Run the Simulation  -----------------   
  fRun->Run(nEvents);
  
//-------------------------  Save the parameters ----------------- 
  rtdb->saveOutput();
  
      
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
   
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  delete fRun;
  exit(0);

}  
  

