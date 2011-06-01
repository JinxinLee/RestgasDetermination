void runMomresMC_batch(float mom, int angle, int PDG, unsigned int MULT, 
		  TString outdir, int nEvents=5000, unsigned int seed=0) {
  
  // ------------------------------------------------------------------------
  gRandom->SetSeed(seed);
  
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;
  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // --------------------------------------------------
  TString mcMode = "TGeant3";
  fRun->SetName(mcMode);



  TString jobdir=outdir;
  int length = jobdir.Length();
  if(jobdir[length-1] != "/") 
    jobdir.Append("/");
  
  TString jobname="Momres_PDG";
  char bufferPDG[5];
  sprintf(bufferPDG, "%i", PDG);
  jobname.Append(bufferPDG);
  char buffermom[5];
  jobname.Append("_mom");
  sprintf(buffermom, "%.1f", mom);
  jobname.Append(buffermom);
  jobname.Append("_deg");
  char bufferang[5];
  sprintf(bufferang, "%i", angle);
  jobname.Append(bufferang);
  jobname.Append("_mult");
  char buffemult[5];
  sprintf(buffermult, "%i", MULT);
  jobname.Append(buffermult);
  jobname.Append(".mc.root");

  TString outfile = jobdir+jobname;
  std::cout<<outfile<<std::endl;
  fRun->SetOutputFile(outfile);
  std::cout<<"Set output file to "<<outfile<<std::endl;
  
  TString copy = outfile;
  TString parfile = copy.ReplaceAll("mc.root","param.root");
  TString digifile = gSystem->Getenv("VMCWORKDIR");
  digifile+="/macro/params/all.par";
  
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(digifile.Data(),"in");
  rtdb->setFirstInput(parIo1);       
  Bool_t kParameterMerged=kTRUE;
  
  FairParRootFileIo* parOut=new FairParRootFileIo(kParameterMerged);
  parOut->open(parfile.Data());
  rtdb->setOutput(parOut);
  
  
   
  // In general, the following parts need not be touched
  // ========================================================================


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  

  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  fRun->AddModule(Magnet);
    
  FairModule *Pipe= new PndPipe("PIPE");
  fRun->AddModule(Pipe);

  PndTpcDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
  Tpc->SetGeometryFileName("TPC_V1.1.root");    //new ROOT geometry
  if(mcMode=="TGeant3")  Tpc->SetAliMC();
  fRun->AddModule(Tpc);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  fRun->AddModule(Mvd);

  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  FairBoxGenerator* boxGen = new FairBoxGenerator(PDG, MULT); 
  boxGen->SetPRange(mom,mom); // GeV/c 
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(angle, angle); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); 
  primGen->AddGenerator(boxGen);


  // Create and Set Magnetic Field
  //-------------------------------
  fRun->SetBeamMom(15);
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  
  /**Initialize the session*/
  fRun->Init();
  
  //rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------
  fRun->Run(nEvents);


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outfile << endl;
  cout << "Parameter file is " << parfile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
