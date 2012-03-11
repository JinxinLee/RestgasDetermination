// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector with the STT option
// to run the macro:
// root  sim_complete_stt.C  or in root session root>.x  sim_complete_stt.C
// to run with different options:(e.g more events, different momentum, Geant4)
// root  sim_complete_stt.C"(100, "TGeant4",2)"

rad_complete_stt(Int_t nEvents = 1, TString  SimEngine ="TGeant3", Float_t mom = 7.24)
{
  //-----User Settings:-----------------------------------------------
  TString  OutputFile     ="rad_complete.root";
  TString  ParOutputfile  ="radparams.root";
  Double_t BeamMomentum   =15.0;
  TString  MediaFile      ="media_pnd.geo";
  gDebug                  = 0;
  TString digiFile        = "emc.par"; //The emc run the hit producer directly 
  // choose your event generator 
  Bool_t UseEvtGen	      =kFALSE;     
  Bool_t UseDpm 	      =kFALSE;
  Bool_t UseBoxGenerator  =kTRUE;
  
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
 
 //-------Set the parameter output --------------------
 // FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
 //  parIo1->open(emcDigiFile.Data(),"in");
 //  rtdb->setFirstInput(parIo1);        

 //---------------------Set Parameter output      ---------- 
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(ParOutputfile.Data());
  rtdb->setOutput(output);

  fRun->SetRadLenRegister(kTRUE);

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
  FairDetector *Stt= new PndStt("STT", kFALSE);
  Stt->SetGeometryFileName("straws_skewed_blocks_pipe_120cm.geo");
  fRun->AddModule(Stt);
 //-------------------------  MVD       -----------------
  FairDetector *Mvd = new PndMvdDetector("MVD", kFALSE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);
 //-------------------------  EMC       -----------------
  PndEmc *Emc = new PndEmc("EMC",kFALSE);
  Emc->SetGeometryVersion(15);
  // See PndEmc::SetGeometryVersion() for available geometries and add there new one if necessary
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);
 //-------------------------  TOF       -----------------  
 /* FairDetector *Tof = new PndTof("TOF",kFALSE);
  Tof->SetGeometryFileName("tofbarrel.geo");
  fRun->AddModule(Tof);
 *///-------------------------  DRC       -----------------
  PndDrc *Drc = new PndDrc("DIRC", kFALSE);
  Drc->SetGeometryFileName("dirc.geo"); 
  Drc->SetRunCherenkov(kFALSE);
  fRun->AddModule(Drc); 
  //-------------------------  MDT       -----------------
  PndMdt *Muo = new PndMdt("MDT",kFALSE);
  Muo->SetBarrel("fast");
  Muo->SetEndcap("fast");
  Muo->SetMuonFilter("fast");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);
   //-------------------------  DCH       -----------------
  /*FairDetector *Dch = new PndDchDetector("DCH", kFALSE);
  Dch->SetGeometryFileName("dch.root"); 
  fRun->AddModule(Dch);
 */
   //-------------------------  GEM      -----------------
  FairDetector *Gem = new PndGemDetector("GEM", kFALSE);
  Gem->SetGeometryFileName("gem_3Stations.root");
  fRun->AddModule(Gem);
  
  //-------------------------  DSK      -----------------
  PndDsk* Dsk = new PndDsk("DSK", kFALSE);
  Dsk->SetGeometryFileName("dsk.geo");
  fRun->AddModule(Dsk);



  // Create and Set Event Generator
  //-------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
	 
  if(UseBoxGenerator){	// Box Generator
     FairBoxGenerator* boxGen = new FairBoxGenerator(0, 10); // 13 = muon; 1 = multipl.
     boxGen->SetPtRange(mom,mom); // GeV/c
     boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
     boxGen->SetThetaRange(0., 90.); // Polar angle in lab system range [degree]
     boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
     primGen->AddGenerator(boxGen);
  }
  if(UseDpm){
  	  PndDpmDirect *Dpm= new PndDpmDirect(mom,1);
	  primGen->AddGenerator(Dpm);
  }
  if(UseEvtGen){	
	  TString  EvtInput =gSystem->Getenv("VMCWORKDIR");
	  EvtInput+="/input/psi2s_jpsi2pi_1k.evt";	
	  FairEvtGenGenerator* evtGen = new FairEvtGenGenerator(EvtInput.Data());
	  primGen->AddGenerator(evtGen);
  }	
	


 //-------------------------  Initialize the RUN  -----------------  
  fRun->Init();
 //-------------------------  Run the Simulation  -----------------   
  fRun->Run(nEvents);
 //-------------------------  Save the parameters ----------------- 
  rtdb->saveOutput();
 //------------------------Print some info and exit----------------     
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  
//  delete fRun;
//  exit(0);

}  
  
