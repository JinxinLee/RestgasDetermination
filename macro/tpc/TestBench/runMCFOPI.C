void runMCFOPI(){
//MC for the FOPI setup
//Maxence Vandenbroucke 11/01/2010 from runMC.C

  TStopwatch timer;
  timer.Start();
  
  // Load basic libraries in rootlogon
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // --------------------------------------------------
  TString GEANT = "TGeant3";
  
  fRun->SetName(GEANT);
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");

  // SET NUMBER OF EVENTS
  // --------------------------------------------------
  Int_t nEvents = 10000;

  //Set JOBNAME + JOBDIR (will not be created!)
  // --------------------------------------------------
  TString jobname="1GeV_Protons";
  TString jobdir="TDR_Plots";
  

  TString basejobdir=gSystem->Getenv("VMCWORKDIR");
  jobdir=(basejobdir+"/")+jobdir+"/";

  std::cout<<jobdir<<std::endl;

  TString copy = jobdir;
  
  jobdir+=jobname;
  
  TString base=jobdir;
  TString outfile=base+".mc.root";
  TString dbfile=base+".mc.param.root";

  fRun->SetOutputFile(outfile);


  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");


 // Fill the Parameter containers for this run
  //-------------------------------------------
  TString digiFile = gSystem->Getenv("VMCWORKDIR");
  digiFile += "/tpc/TestBench/tpc.TBtestChamber.par";
  
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(digiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);
  Bool_t kParameterMerged=kTRUE;

  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  std::cout<<dbfile<<std::endl;
  output->open(dbfile.Data());
  rtdb->setOutput(output);

  rtdb->Print();

  
  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  PndTpcDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetGeometryFileName("tpc_prototype.root");
  PndTpc->SetMixture("TPCmixture"); // TPCmixture: Neon CO2 (90/10)
  if(GEANT=="TGeant3") PndTpc->SetAliMC();
  fRun->AddModule(PndTpc);
   
  
  // Create and Set Event Generator
  //-------------------------------
  std::cout<<"Setup EvtGens"<<std::endl;std::cout.flush();
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
 
  // Box Generator
  
  //pdgs 211=pion 13=muon 11=electron, 2212 proton ...
  //(PDG ID, MULTIPLICITY)
  FairBoxGenerator* boxGen = new FairBoxGenerator(2212, 10);
  
  boxGen->SetPRange(1.0,1.0); // GeV/c
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(5, 175); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // cm
  primGen->AddGenerator(boxGen);

  //FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  //fRun->SetGenerator(primGen);

  //DPM
  //TString dpmfile = basejobdir+"10k_2Gev_el_and_inel_DPMDATA.root";
  //PndDpmGenerator* dpmGen = new PndDpmGenerator(dpmfile);
  //primGen->AddGenerator(dpmGen);  
   
  
  //FairEvtGenGenerator* evtGen = new
  //FairEvtGenGenerator("../data/evtgen.y4260.jpsipipi.vvpipi.dat");
  //primGen->AddGenerator(evtGen);
  
  
  // Field Map Definition
  // --------------------
  // 1- Reading the new field map in the old format
  
  // FairFieldMap *fMagField= new FairFieldMap("FIELD.v04_pavel.map");
  // Constant Field
  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0., 0. , 6. ); // values are in kG
  fMagField->SetFieldRegion(-50, 50,-50, 50, -2000, 2000); // values are in cm
  fRun->SetField(fMagField);
   
//fRun->SetStoreTraj(kTRUE);
  //fRun->SetStoreTraj(kFALSE);
  
  std::cout<<"Starting INIT"<<std::endl;
  fRun->Init();
  std::cout<<"Ending INIT"<<std::endl;
  std::cout.flush();
  
  // -Trajectories Visualization (TGeoManager Only )
  // -----------------------------------------------
    
  // Set cuts for storing the trajectpries
  //   FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  //   trajFilter->SetStepSizeCut(0.01); // 1 cm
  //   trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
  //   trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
  //   trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
  //   trajFilter->SetStorePrimaries(kTRUE);
  //   trajFilter->SetStoreSecondaries(kTRUE);
  

  // Fill the Parameter containers for this run
  //-------------------------------------------

   
//PndConstPar* fieldPar = (PndConstPar*) rtdb->getContainer("PndConstPar");
// if ( fMagField ) {  fieldPar->SetParameters(fMagField); }
//  fieldPar->setInputVersion(fRun->GetRunId(),1);
//  fieldPar->setChanged(kTRUE);
  

  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------

  fRun->Run(nEvents);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
