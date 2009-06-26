void run_sim_sttcombi_evtgen(double mom, Int_t nEvents=10)
{
  double mp=0.938272;
  double p=0,M=0;
  
  // determine the pbar mom and E_cms for DPM generator
  if (mom>0) 
  {
	p=mom;
  	double E=sqrt(mp*mp+mom*mom)+mp;
    M=sqrt(E*E-mom*mom);
  }
  else
  {
  	M=-mom;
  	double X = (M*M-2*mp*mp)/(2*mp);
 	p = sqrt(X*X-mp*mp);  	
  }
  


  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libGem");

  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");

  fRun->SetOutputFile("data/points_sttcombi.root");

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairModule *Magnet= new PndMagnet("MAGNET");
  //The newer Solenoid (v833) has badly processed volumes.
//  Magnet->SetGeometryFileName("PandaSolenoidV833.root");
  Magnet->SetGeometryFileName("FullSolenoid.root");
  fRun->AddModule(Magnet);

  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);

  //FairModule *Pipe= new PndPipe("PIPE");
  //fRun->AddModule(Pipe);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);

  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_pipe_120cm.geo");
  fRun->AddModule(Stt);

  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations.root");
  fRun->AddModule(Gem);
  
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root");
  fRun->AddModule(Emc);
  
  // This is breaking
//  FairDetector *Tof = new PndTof("TOF",kTRUE);
//  Tof->SetGeometryFileName("tofbarrel.geo");
//  fRun->AddModule(Tof);

  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetMdtVersion("torino");
  Muo->SetGeometryFileName("muopars.root");
  fRun->AddModule(Muo);

//  PndDrc *Drc = new PndDrc("DIRC", kFALSE);
//  Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
//  fRun->AddModule(Drc);

//  FairDetector *Dch = new PndDchDetector("DCH", kFALSE);
//  Dch->SetGeometryFileName("dch.root");
//  fRun->AddModule(Dch);

  // Create and Set Event Generator
  //-------------------------------


  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // EvtGen Generator
  FairEvtGenGenerator* evtGen = new FairEvtGenGenerator("output.evt");
  primGen->AddGenerator(evtGen);
  
	
	fRun->SetBeamMom( p );
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

	fRun->SetStoreTraj(kFALSE);
	
  fRun->Init();

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;

  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("data/params_sttcombi.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  fRun->Run(nEvents);

  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  //exit(0);

}

