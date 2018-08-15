// Macro for running Panda simulation  with Geant3  or Geant4 (M. Al-Turany)
// This macro is supposed to run the full simulation of the panda detector
// to run the macro:
// root  sim_complete.C  or in root session root>.x  sim_complete_stt.C
// to run with different options:(e.g more events, different momentum, Geant4)
// root  sim_complete.C"(100, "TGeant4",2)"

sim_complete(Int_t nEvents = 1000, TString  SimEngine ="TGeant3", Float_t mom = 6.231552)
{
  //-----User Settings:-----------------------------------------------
  TString  OutputFile     ="sim_complete.root";
  TString  ParOutputfile  ="simparams.root";
  TString  MediaFile      ="media_pnd.geo";
  gDebug                  = 0;
  TString digiFile        = "all.par"; //The emc run the hit producer directly
  // choose your event generator
  Bool_t UseEvtGen	      =kFALSE;
  Bool_t UseEvtGenDirect      =kFALSE;
  Bool_t UseDpm 	      =kFALSE;
  Bool_t UseBoxGenerator      =kTRUE;

  Double_t BeamMomentum = 0.; // beam momentum ONLY for the scaling of the dipole field.
  if (UseBoxGenerator)
    {
      BeamMomentum   =15.0; // ** change HERE if you run Box generator
    }
  else
    {
      BeamMomentum = mom;  // for DPM/EvtGen BeamMomentum is always = mom
    }	
  //------------------------------------------------------------------
  TLorentzVector fIni(0, 0, mom, sqrt(mom*mom+9.3827203e-01*9.3827203e-01)+9.3827203e-01);  
  TDatabasePDG::Instance()->AddParticle("pbarpSystem","pbarpSystem",fIni.M(),kFALSE,0.1,0, "",88888); 
  //------------------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  gRandom->SetSeed(); 

  // Create the Simulation run manager--------------------------------
  FairRunSim *fRun = new FairRunSim();
  fRun->SetName(SimEngine.Data() );
  fRun->SetOutputFile(OutputFile.Data());
  fRun->SetGenerateRunInfo(kFALSE);
  fRun->SetBeamMom(BeamMomentum);
  fRun->SetMaterials(MediaFile.Data());
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  
  // Set the parameters 
  //-------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;
 
 
  //-------Set the parameter output --------------------
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);        

 //---------------------Set Parameter output      ---------- 
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(ParOutputfile.Data());
  rtdb->setOutput(output);

   // Create and add detectors

 //-------------------------  CAVE      -----------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  //-------------------------  Magnet   ----------------- 
  //FairModule *Magnet= new PndMagnet("MAGNET");
  //Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  //Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  //fRun->AddModule(Magnet);
  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);
  //-------------------------  Pipe     -----------------
  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201309.root");
  fRun->AddModule(Pipe);
  //-------------------------  STT       -----------------
  //FairDetector *Stt= new PndStt("STT", kTRUE);
  //Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  //fRun->AddModule(Stt);
   //-------------------------  MVD       -----------------
  //FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  //Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  //fRun->AddModule(Mvd);
  //-------------------------  GEM       -----------------
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations_Tube.root");
  fRun->AddModule(Gem);
  //-------------------------  EMC       -----------------
  //PndEmc *Emc = new PndEmc("EMC",kTRUE);
  //Emc->SetGeometryVersion(1);
  //Emc->SetStorageOfData(kFALSE);
  //fRun->AddModule(Emc);
  //-------------------------  SCITIL    -----------------
  //FairDetector *SciT = new PndSciT("SCIT",kTRUE);
  //SciT->SetGeometryFileName("barrel-SciTil_07022013.root");
  //fRun->AddModule(SciT);
  //-------------------------  DRC       -----------------
  //PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  //Drc->SetGeometryFileName("dirc_l0_p0_updated.root"); 
  //Drc->SetRunCherenkov(kFALSE);
  //fRun->AddModule(Drc); 
  //-------------------------  DISC      -----------------
  //PndDsk* Dsk = new PndDsk("DSK", kTRUE);
  //Dsk->SetStoreCerenkovs(kFALSE);
  //Dsk->SetStoreTrackPoints(kFALSE);
  //fRun->AddModule(Dsk);
  //-------------------------  MDT       -----------------
  //PndMdt *Muo = new PndMdt("MDT",kTRUE);
  //Muo->SetBarrel("fast");
  //Muo->SetEndcap("fast");
  //Muo->SetMuonFilter("fast");
  //Muo->SetForward("fast");
  //Muo->SetMdtMagnet(kTRUE);
  //Muo->SetMdtCoil(kTRUE);
  //Muo->SetMdtMFIron(kTRUE);
  //fRun->AddModule(Muo);
  //-------------------------  FTS       -----------------
  FairDetector *Fts= new PndFts("FTS", kTRUE);
  Fts->SetGeometryFileName("fts_reduced.geo");
  fRun->AddModule(Fts); 
  //-------------------------  FTOF      -----------------
  FairDetector *FTof = new PndFtof("FTOF",kTRUE);
  FTof->SetGeometryFileName("ftofwall.root");
  fRun->AddModule(FTof);
  //-------------------------  RICH       ----------------
  FairDetector *Rich= new PndRich("RICH",kFALSE);
  Rich->SetGeometryFileName("rich_v2_shift.geo");
  fRun->AddModule(Rich);

  // Create and Set Event Generator
  //-------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
	 
//  if(UseBoxGenerator){	// Box Generator
//     FairBoxGenerator* muMinusBoxGen = new FairBoxGenerator(13, 6); // 13 = muon-; 1 = multipl.
//     muMinusBoxGen->SetPRange(0.3,3.0); // GeV/c
//     muMinusBoxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
//     muMinusBoxGen->SetThetaRange(0., 20.); // Polar angle in lab system range [degree]
//     muMinusBoxGen->SetXYZ(0., 0., 0.); // cm
//     primGen->AddGenerator(muMinusBoxGen);
//
//     FairBoxGenerator* muPlusBoxGen = new FairBoxGenerator(-13, 6); // 13 = muon-; 1 = multipl.
//     muPlusBoxGen->SetPRange(0.3,3.0); // GeV/c
//     muPlusBoxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
//     muPlusBoxGen->SetThetaRange(0., 20.); // Polar angle in lab system range [degree]
//     muPlusBoxGen->SetXYZ(0., 0., 0.); // cm
//     primGen->AddGenerator(muPlusBoxGen);
//  }
  if(UseBoxGenerator){	// Box Generator
      FairBoxGenerator* muMinusBoxGen = new FairBoxGenerator(13, 1); // 13 = muon-; 1 = multipl.
      muMinusBoxGen->SetPRange(1.0,3.0); // GeV/c
      muMinusBoxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
      muMinusBoxGen->SetThetaRange(3., 7.); // Polar angle in lab system range [degree]
      muMinusBoxGen->SetXYZ(0., 0., 0.); // cm
      primGen->AddGenerator(muMinusBoxGen);
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
  if(UseEvtGenDirect){
          TString  EvtInput =gSystem->Getenv("VMCWORKDIR");
          EvtInput+="/macro/run/psi2s_Jpsi2pi_Jpsi_mumu.dec";	
          PndEvtGenDirect *EvtGen = new PndEvtGenDirect("pbarpSystem", EvtInput.Data(), mom);
	  EvtGen->SetStoreTree(kTRUE);
	  primGen->AddGenerator(EvtGen);
  }	

 //---------------------Create and Set the Field(s)---------- 
  PndMultiField *fField= new PndMultiField("AUTO");
  fRun->SetField(fField);

 // EMC Hit producer
  //-------------------------------
  //PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  //fRun->AddTask(emcHitProd);
 
  //-------------------------- switch on the vis manager-----------
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
 //------------------------Print some info and exit----------------     
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  
  //exit(0);

}  
  
