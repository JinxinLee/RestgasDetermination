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
  // TString prodjob="PION40deg";
//   TString jobname="laserdigi6";

//   jobdb* db=new jobdb("$PANDAMC/FAIRRoot/JOBDB/pandaJobDB.root");
//   int prodid=db->findProducerJobByName(prodjob);
//   jobob injob=db->getJob(prodid);

//   TString inFile = injob.output();
//   TString inDir=inFile(0,inFile.Last('/')+1);
//   // make new subdir
//   TString jobDir=inDir; jobDir+=jobname; jobDir+="/";
//   TString cmd="mkdir ";
//   cmd+=jobDir; 
//   if(gSystem->Exec(cmd)){
//     std::cout<<"Could not create Job-Directory "<<jobDir
// 	     <<". Aborting."<<std::endl;
//     return;
//   }
  
  TString inFile = "eumel.mc.root";
  TString outFile = inFile; 
//  outFile.ReplaceAll(inDir,jobDir);
  outFile.ReplaceAll(".mc.root",".laser.raw.root");

  TString paramIn = "eumel.param.root";
  TString paramOut = outFile;
  paramOut.ReplaceAll(".raw.root",".param.root");

std::cout<<"Input: "<<inFile<<std::endl;
std::cout<<"Output: "<<outFile<<std::endl;
std::cout<<"ParamIn: "<<paramIn<<std::endl;
std::cout<<"ParamOut: "<<paramOut<<std::endl;
 

  // ---  Register Job in database   -------------
  // ------------------------------------------------------------------------

// write jobdb
//   jobob* job=new jobob(jobname,inFile,outFile,paramOut,jobob::kDigi);
//   int id=db->addJob(*job);
//   std::cout<<"Added job with ID="<<id<<std::endl;
//   delete db;
//   delete job;


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
  tpcClusterizer->SetPersistence();
  fRun->AddTask(tpcClusterizer);
 
  PndTpcLaserTask* tpcLaser = new  PndTpcLaserTask();
  tpcLaser->setLaserFile("LaserGridTest.dat");
  fRun->AddTask(tpcLaser);

  PndTpcDriftTask* tpcDrifter = new PndTpcDriftTask();
  tpcDrifter->SetDeviationFile("DevMap_corrected_E_and_B_30-05-08.dat");
  tpcDrifter->SetPersistence();
  tpcDrifter->SetDistort(true);
  tpcDrifter->SetDiffusion(true);
//double deg=TMath::Pi()/180;
//tpcDrifter->SetPhiCut(-5*deg,5*deg);
  fRun->AddTask(tpcDrifter);

  PndTpcGemTask* tpcGem = new PndTpcGemTask();
  tpcGem->SetPersistence();
  fRun->AddTask(tpcGem);

  PndTpcPadResponseTask* tpcPadResponse = new PndTpcPadResponseTask();
  tpcPadResponse->SetPersistence();
  fRun->AddTask(tpcPadResponse);


//PndTpcEvtMixTask* evtmixer = new PndTpcEvtMixTask();
//  evtmixer->SetBkgFileName("bkg2.raw.root");
//  evtmixer->SetNBkgEvts(500);
//  evtmixer->SetEvtRate(1E7);
//fRun->AddTask(evtmixer);

  PndTpcElectronicsTask* tpcElec = new PndTpcElectronicsTask();
  tpcElec->SetPersistence();
  fRun->AddTask(tpcElec);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  rtdb->print();

  fRun->Run(0,1); // process all events from input file
  // ------------------------------------------------------------------------


//tpcDrifter->WriteHistograms();
//tpcGem->WriteHistograms();
//tpcPadResponse->WriteHistograms();
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
