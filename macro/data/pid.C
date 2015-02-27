void pid()
{
  // Macro created 02/10/2012 by S.Spataro
  // It loads a reconstruction file and compute PID informations

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
	// Number of events to process
  Int_t nEvents = 0;  // if 0 all the vents will be processed
  
  TString simFile = "sim.root";
  PndFileNameCreator creator(simFile.Data());
  TString digiFile = creator.GetDigiFileName();
  TString recoFile = creator.GetRecoFileName();
  TString pidFile = creator.GetPidFileName();
  TString parFile = creator.GetParFileName();
  
  // Digitisation file (ascii)
  TString digiParFile = "all.par";
  
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
    // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(simFile);
  fRun->AddFriend(digiFile);
  fRun->AddFriend(recoFile);
  fRun->SetOutputFile(pidFile);
  fRun->SetWriteRunInfoFile(kFALSE);
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

  // -----  Parameter database   --------------------------------------------
  TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
  emcDigiFile += "/macro/params/";
  emcDigiFile += digiParFile;
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(emcDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);

  // ------------------------------------------------------------------------

  PndPidCorrelator* corr = new PndPidCorrelator();
  //corr->SetVerbose();
  corr->SetInputBranch("SttMvdGemGenTrack");
  corr->SetInputIDBranch("SttMvdGemGenTrackID");
  corr->SetInputBranch2("FtsIdealGenTrack");
  corr->SetInputIDBranch2("FtsIdealGenTrackID");
  //corr->SetFast(kTRUE);
  //corr->SetBackPropagate(kFALSE);
  fRun->AddTask(corr);
  /*
    PndMcCloner *clone = new PndMcCloner();
    clone->SetCleanMc(kTRUE);
    fRun->AddTask(clone);
  */
  PndPidIdealAssociatorTask *assMC= new PndPidIdealAssociatorTask();
  fRun->AddTask(assMC);

  PndPidMvdAssociatorTask *assMvd= new PndPidMvdAssociatorTask();
  fRun->AddTask(assMvd);

  PndPidMdtHCAssociatorTask *assMdt= new PndPidMdtHCAssociatorTask();
  fRun->AddTask(assMdt);

  PndPidDrcAssociatorTask *assDrc= new PndPidDrcAssociatorTask();
  fRun->AddTask(assDrc);

  PndPidDiscAssociatorTask *assDisc= new PndPidDiscAssociatorTask();
  fRun->AddTask(assDisc);

  PndPidSttAssociatorTask *assStt= new PndPidSttAssociatorTask();
  fRun->AddTask(assStt);

  PndPidEmcBayesAssociatorTask *assEMC= new PndPidEmcBayesAssociatorTask();
  fRun->AddTask(assEMC);

  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  cout << "fRun->Init()" << endl;
  fRun->Init();

  timer.Start();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0);
}
