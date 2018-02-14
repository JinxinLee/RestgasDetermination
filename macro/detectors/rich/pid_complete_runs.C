void pid_complete_runs(Int_t nRun = 0)
{
   time_t rawtime;
   struct tm * timeinfo;
   time (&rawtime);
   timeinfo = localtime (&rawtime);
   std::cout << "Start = " << asctime(timeinfo) << std::endl;
   
  //-----User Settings:-----------------------------------------------
   TString  RunsListFile  = "runs.txt";
   std::ifstream runsFileInput(RunsListFile.Data());
   char str[1000];
   bool runexists = false;
   int nrun, nrunmax = -1;
   int pid, mult;
   Float_t pmin, pmax;
   Float_t tmin, tmax;
   Float_t fmin, fmax;
   Int_t nEvents;
   std::string strg;
   runsFileInput >> strg;
   while(!runsFileInput.eof()) {
      if (strg[0]!='#') {
         sscanf(strg.c_str(),"%d",&nrun);
         if (nrun==nRun) {
            runexists = true;
            runsFileInput >> strg; sscanf(strg.c_str(),"%d",&pid);
            runsFileInput >> strg; sscanf(strg.c_str(),"%d",&nEvents);
            runsFileInput >> strg; sscanf(strg.c_str(),"%d",&mult);
            runsFileInput >> strg; sscanf(strg.c_str(),"%f",&pmin);
            runsFileInput >> strg; sscanf(strg.c_str(),"%f",&pmax);
            runsFileInput >> strg; sscanf(strg.c_str(),"%f",&tmin);
            runsFileInput >> strg; sscanf(strg.c_str(),"%f",&tmax);
            runsFileInput >> strg; sscanf(strg.c_str(),"%f",&fmin);
            runsFileInput >> strg; sscanf(strg.c_str(),"%f",&fmax);
         }
         else {
            for(size_t iw=0;iw<9;iw++) runsFileInput >> strg;
         }
         nrunmax = nrun>nrunmax ? nrun : nrunmax;
      }
      runsFileInput >> strg;
   }
   runsFileInput.close();

   if (!runexists) {
      std::cout << "Run number " << nRun << " is not defined!" << std::endl;
      exit(0);
   }

   std::cout << "run = " << nRun << " " << pid << " " << nEvents << " " << mult << " " << pmin << " "
      << pmax << " " << tmin << " " << tmax << " " << fmin << " " << fmax << std::endl;

   Double_t mom = pmax;

  // Macro created 02/10/2012 by S.Spataro
  // It loads a reconstruction file and compute PID informations

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
	// Number of events to process
  Int_t nEvents = 0;  // if 0 all the vents will be processed
  
  // Parameter file
  //TString parFile = "simparams.root"; // at the moment you do not need it
  TString  parFile  = Form("simparams_v%d_r%d.root",0,nRun);
  
  // Digitisation file (ascii)
  TString digiFile = "all.par";
  
  // Output file
  //TString outFile = "pid_complete.root";
  TString  outFile     =  Form("pid_complete_v%d_r%d.root",0,nRun);
  TString  sim_File     =  Form("sim_complete_v%d_r%d.root",0,nRun);
  TString  digi_File     =  Form("digi_complete_v%d_r%d.root",0,nRun);
  TString  reco_File     =  Form("reco_complete_v%d_r%d.root",0,nRun);
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
    // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  //fRun->SetInputFile("sim_complete.root");
  //fRun->AddFriend("digi_complete.root");
  //fRun->AddFriend("reco_complete.root");
  fRun->SetInputFile(sim_File);
  fRun->AddFriend(digi_File);
  fRun->AddFriend(reco_File);
  fRun->SetOutputFile(outFile);
  fRun->SetGenerateRunInfo(kFALSE);
  fRun->SetUseFairLinks(kTRUE);
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

  // -----  Parameter database   --------------------------------------------
  TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
  emcDigiFile += "/macro/params/";
  emcDigiFile += digiFile;
  
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
  //corr->SetDebugMode(kTRUE);
  //corr->SetFast(kTRUE);
  //corr->SetBackPropagate(kFALSE);
  //corr->SetIdeal(kTRUE);
  fRun->AddTask(corr);

  PndPidBremCorrector *bremCorr = new PndPidBremCorrector();
  fRun->AddTask(bremCorr);

  PndMcCloner *clone = new PndMcCloner();
  fRun->AddTask(clone);
 
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

  PndPidRichAssociatorTask *assRich= new PndPidRichAssociatorTask();
  fRun->AddTask(assRich);

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
