{
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libtpc");
  gSystem->Load("libgenfit");
  gSystem->Load("libjobdb");
  // ------------------------------------------------------------------------

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (MC events)
  TString prodjob="DPM5";
  TString jobname="sigSlice1";
  TString comment="geant4";

  jobdb* db=new jobdb("$PANDAMC/FAIRRoot/JOBDB/pandaJobDB.root");
  int prodid=db->findProducerJobByName(prodjob);
  jobob injob=db->getJob(prodid);

  TString inFile = injob.output();
  TString inDir=inFile(0,inFile.Last('/')+1);
  // make new subdir
  TString jobDir=inDir; jobDir+=jobname; jobDir+="/";
  TString cmd="mkdir ";
  cmd+=jobDir; 
  if(gSystem->Exec(cmd)){
    std::cout<<"Could not create Job-Directory "<<jobDir
	     <<". Aborting."<<std::endl;
    return;
  }
  
  TString outFile = inFile; 
  outFile.ReplaceAll(inDir,jobDir);
  outFile.ReplaceAll(".mc.root",".sig.root");

  TString paramIn = injob.param();
  TString paramOut = outFile;
  paramOut.ReplaceAll(".sig.root",".param.root");

std::cout<<"Input: "<<inFile<<std::endl;
std::cout<<"Output: "<<outFile<<std::endl;
std::cout<<"ParamIn: "<<paramIn<<std::endl;
std::cout<<"ParamOut: "<<paramOut<<std::endl;
 

  // ---  Register Job in database   -------------
  // ------------------------------------------------------------------------

// write jobdb
  jobob* job=new jobob(jobname,inFile,outFile,paramOut,jobob::kDigi);
  job->setComment(comment);
  int id=db->addJob(*job);
  std::cout<<"Added job with ID="<<id<<std::endl;
  delete db;
  delete job;


  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(paramIn.Data());
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString tpcDigiFile = gSystem->Getenv("VMCWORKDIR");
  tpcDigiFile += "/tpc/tpc.par";
  parInput2->open(tpcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput2);
  rtdb->setSecondInput(parInput1);

  PndTpcDigiPar* par = (PndTpcDigiPar*) rtdb->getContainer("PndTpcDigiPar");
  par->setInputVersion(fRun->GetRunId(),1);
  par->setChanged(kTRUE);

  FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
  parOutput1->open(paramOut.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();

  fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  
//  GenfitTask *Genfit= new GenfitTask();
//  fRun->AddTask(Genfit);

// ----- PndTpc Event Mixer: Mixes background tracks to events -------------

    // -----    Digi Sequence  --------------------------------------------
  PndTpcClusterizerTask* tpcClusterizer = new PndTpcClusterizerTask();
//tpcClusterizer->SetPersistence();
  fRun->AddTask(tpcClusterizer);
 
  PndTpcDriftTask* tpcDrifter = new PndTpcDriftTask();
  tpcDrifter->SetPersistence();
  tpcDrifter->SetDistort(false);
  double deg=TMath::Pi()/180;
  tpcDrifter->SetPhiCut(-12*deg,12*deg);
  fRun->AddTask(tpcDrifter);

  PndTpcGemTask* tpcGem = new PndTpcGemTask();
//tpcGem->SetPersistence();
//tpcGem->SetGainFluctuations();
  fRun->AddTask(tpcGem);

  PndTpcPadResponseTask* tpcPadResponse = new PndTpcPadResponseTask();
  tpcPadResponse->SetPersistence();
  unsigned int ids[14]={0,1,2,3,4,5,6,217,218,219,220,221,222,223};
  for(int i=0;i<14;++i)tpcPadResponse->SelectSector(ids[i]);

  fRun->AddTask(tpcPadResponse);



  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  rtdb->print();

  fRun->Run(0,0); // process all events from input file
  // ------------------------------------------------------------------------


tpcDrifter->WriteHistograms();
//tpcGem->WriteHistograms();
tpcPadResponse->WriteHistograms();
  // -----   Finish   -------------------------------------------------------


//delete tpcSplitter;

  rtdb->saveOutput();
  rtdb->print();


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << paramOut << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
