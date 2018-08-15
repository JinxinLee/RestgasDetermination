int run_sim_complete(TString FileName="test",Int_t lambdadiskposition=0, Double_t Momenta = 1.642, Int_t nEvents=100000,Int_t DecayModel=1,Int_t Seed=6){
  
  // This script should simulate the decay p pbar -> lambda lambdabar -> p pbar pi+ pi- with the hole panda detector.
  // The default beam momenta is 1.642, since the dataset for the parametrisation of the decay was recorded with this momenta.
    
    
  //---- User Settings ------------------------------------------------------------
  
  // choose your simulation seed
  gRandom->SetSeed(Seed);
  
  gDebug=0;
  
  TString MediaFile             = "media_pnd.geo";

  Bool_t VisTrack               = kTRUE;

  Bool_t SetStoreTree			= kTRUE;

  Double_t BeamMomentum         = 15.0; // beam momentum ONLY for the scaling of the dipole field. For the generator use "mom"

  // choose your simulation engine
  TString SimEngine             = "TGeant3";

  // choose your event generator
  Bool_t UseEvtGen              = kFALSE; 	  
  Bool_t UseEvtGenDirect        = kTRUE;
  Bool_t UseDpm                 = kFALSE;
  Bool_t UseBoxGenerator        = kFALSE;

  //options for Dpm
  Int_t Mode                    = 1; // 0 = No elastic scattering, only inelastic
                                     // 1 = Elastic and inelastic interactions
                                     // 2 = Ony elatstic scattering, no inelastic one

  // options for EvtGen + EvtGenDirect 
  //  TString EvtDecayFile          = gSystem->Getenv("VMCWORKDIR");
  //  EvtDecayFile                  +="/macro/run/2pipi.dec";  
  if(DecayModel==0)
  {
	  TString EvtDecayFile          = "llbar_EvtModel_PHSP.DEC";
  }
  else if(DecayModel==1)
  {
	  TString EvtDecayFile          = "llbar_EvtModel_LambdaLambdaBarPol_1-642.DEC";
  }
  else if(DecayModel==2)
  {
	  TString EvtDecayFile			= "llbar_EvtModel_LambdaLambdaBarPol_1-918.DEC";
  }
  else if(DecayModel==3)
  {
	  TString EvtDecayFile 			= "llbar_EvtModel_LambdaLambdaBarPol_6.DEC";
  }
  else if(DecayModel==4)
  {
 	  TString EvtDecayFile          = "llbar_EvtModel_LambdaLambdaBar_1-642.DEC";
  } 
  else if(DecayModel==5)
  {
  	  TString EvtDecayFile			= "llbar_EvtModel_LambdaLambdaBar_1-918.DEC";
  }
  else if(DecayModel==6)
  {
	  TString EvtDecayFile  		= "llbar_EvtModel_LambdaLambdaBar_6.DEC";
  }  
  else if(DecayModel==7)
  {
	  TString EvtDecayFile 			= "llbar_EvtModel_LambdaLambdaBarHE.DEC";

  }

  else
  {
	  TString EvtDecayFile          = "llbar_EvtModel_PHSP.DEC";
  }

  // options for BoxGenerator
  Int_t Particle                = 22;  // DPG Code http://pdg.lbl.gov/2012/mcdata/mc_particle_id_contents.html
  Int_t Multiplicity            = 1;
  Double_t PRangeMin            = Momenta; // GeV/c
  Double_t PRangeMax            = Momenta; // GeV/c
  Bool_t CosTheta               = kTRUE;
  Double_t PhiRangeMin          = 0.;  // Azimuth angle range [degree]
  Double_t PhiRangeMax          = 360.;// Azimuth angle range [degree]
  Double_t ThetaRangeMin        = 0.;  // Polar angle in lab system range [degree]
  Double_t ThetaRangeMax        = 90.; // Polar angle in lab system range [degree] 
  Double_t VertexX              = 0.;  // cm
  Double_t VertexY              = 0.;  // cm
  Double_t VertexZ              = 0.;  // cm 

  //================================================================================
  //================================================================================
  
  TStopwatch timer;
  timer.Start();
  
  // Load basic libraries 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  // File Names
  PndFileNameCreator creator(FileName.Data());
  TString outputFile 	        = creator.GetSimFileName();
  TString parFile 				= creator.GetParFileName();
  TString digiFile 				= "all.par"; //The emc run the hit producer directly
  
  // Create the simulation run manager
  FairRunSim *fRun = new FairRunSim();
  fRun->SetStoreTraj(VisTrack);
  fRun->SetName(SimEngine.Data());
  fRun->SetOutputFile(outputFile);
  fRun->SetBeamMom(BeamMomentum);
  fRun->SetMaterials(MediaFile.Data());
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  // Set the parameters
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;
 
  // Set the parameter output
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);        
  Bool_t kParameterMerged=kTRUE;	
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parFile);
  rtdb->setOutput(output);
  

  //---- Create and add detectors ---------------------------------------------------

  // Cave
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  // Magnet
  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  fRun->AddModule(Magnet);

  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);

  // Pipe 
  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201112.root");
  fRun->AddModule(Pipe);

  // STT
  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  fRun->AddModule(Stt);

  // MVD (without Lambda Disks)
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);         
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  fRun->AddModule(Mvd);

  // MVD (Just the Lambda Disks)
  FairDetector *LambdaDisks = new PndMvdDetector("MVD", kTRUE);   // Just Lambda Disks

  if(lambdadiskposition==0)
  {
	  LambdaDisks->SetGeometryFileName("LambdaDisksSeparatedSupport.root");		  // Lambda Disks at 40cm & 60 cm  - Position 0
	  LambdaDisks->SetVerboseLevel(0);
	  fRun->AddModule(LambdaDisks);
  }
  else if(lambdadiskposition==1)
  {
	  LambdaDisks->SetGeometryFileName("Mvd_AddDisks_smalldiskdistance_position1_40cm.root");   // Lambda Disks at 37cm & 43 cm - Position 1
	  LambdaDisks->SetVerboseLevel(0);
	  fRun->AddModule(LambdaDisks);
  }
  else if(lambdadiskposition==2)
  {
	  LambdaDisks->SetGeometryFileName("Mvd_AddDisks_smalldiskdistance_position2_60cm.root");   // Lambda Disks at 57cm & 63 cm - Position 2
	  LambdaDisks->SetVerboseLevel(0);
	  fRun->AddModule(LambdaDisks);
  }
  else if(lambdadiskposition==3)
  {
	  LambdaDisks->SetGeometryFileName("Mvd_AddDisks_smalldiskdistance_position3_80cm.root");   // Lambda Disks at 77cm & 83 cm - Position 2
	  LambdaDisks->SetVerboseLevel(0);
	  fRun->AddModule(LambdaDisks);
  }
  else if(lambdadiskposition==-1)
  {
	  std::cout <<"No LambdaDisks Setting - No LambdaDisc geometry is loaded"<< std::endl;
  }
  else
  {
	  std::cout << "No LambdaDisk position given - No LambdaDisks will be added" << std::endl;
	  //exit(0);
  }


  // GEM
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations.root");
  fRun->AddModule(Gem);

  // EMC
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryVersion(1);
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);

  // SCITIL
  FairDetector *SciT = new PndSciT("SCIT",kTRUE);
  SciT->SetGeometryFileName("barrel-SciTil_07022013.root");
  fRun->AddModule(SciT);

  // DRC 
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetGeometryFileName("dirc_l0_p0_updated.root");
  Drc->SetRunCherenkov(kFALSE);
  fRun->AddModule(Drc);

  // DISC  
  PndDsk* Dsk = new PndDsk("DSK", kTRUE);
  Dsk->SetStoreCerenkovs(kFALSE);
  Dsk->SetStoreTrackPoints(kFALSE);
  fRun->AddModule(Dsk);

  // MDT
  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetBarrel("fast");
  Muo->SetEndcap("fast");
  Muo->SetMuonFilter("fast");
  Muo->SetForward("fast");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);

  // FTS 
  FairDetector *Fts= new PndFts("FTS", kTRUE);
  Fts->SetGeometryFileName("fts.geo");
  fRun->AddModule(Fts);

  // FTOF 
  FairDetector *FTof = new PndFtof("FTOF",kTRUE);
  FTof->SetGeometryFileName("ftofwall.root");
  fRun->AddModule(FTof);

  // RICH     
  FairDetector *Rich= new PndRich("RICH",kFALSE);
  Rich->SetGeometryFileName("rich_v2.geo");
  fRun->AddModule(Rich);
 
  //---- Event Generator ------------------------------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  if(UseBoxGenerator){ 
    FairBoxGenerator* boxGen = new FairBoxGenerator(Particle, Multiplicity);
    boxGen->SetPRange(PRangeMin,PRangeMax); 
    boxGen->SetPhiRange(PhiRangeMin, PhiRangeMax); 
    boxGen->SetThetaRange(ThetaRangeMin, ThetaRangeMax);
    boxGen->SetXYZ(VertexX, VertexY, VertexZ);
    if(CosTheta==kTRUE)
      {
	boxGen->SetCosTheta();
      }
    primGen->AddGenerator(boxGen);
  }
  if(UseDpm){
    PndDpmDirect *Dpm= new PndDpmDirect(Momenta,Mode);
    primGen->AddGenerator(Dpm);
  }
  if(UseEvtGen){           
    FairEvtGenGenerator* evtGen = new FairEvtGenGenerator(EvtDecayFile.Data());
    primGen->AddGenerator(evtGen);
  }     
  if(UseEvtGenDirect){   
    PndEvtGenDirect *EvtGen = new PndEvtGenDirect("pbarpSystem", EvtDecayFile.Data(), Momenta);
    EvtGen->SetStoreTree(SetStoreTree);
    primGen->AddGenerator(EvtGen);
  } 
  
  // Create and Set Magnetic Field
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);

  // EMC Hit producer
  PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  fRun->AddTask(emcHitProd);
	  
  //---- Initialize the RUN ------------------------------------------------------------
  fRun->Init();

  //---- Run the Simulation  -----------------------------------------------------------   
  fRun->Run(nEvents);

  //---- Save the parameters -----------------------------------------------------------
  rtdb->saveOutput();

  //---- Print some info and exit ------------------------------------------------------     
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
 
  cout << endl << endl;
  cout << "Sim macro finished successfully." << endl;
  cout << "Output file is "    << outputFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  
  return 0;
}
