//PhiSplitter cannot handle clusters with digis having identical energy depositions inside
//As a workaround, we remove it from the bumpSplitting routines
//Call this function after Init...
void deactivatePhiSplitter(FairTask* bumpSplitter) {
	FairTask* phi_splitter= dynamic_cast<FairTask*>(bumpSplitter->GetListOfTasks()->FindObject("PndEmcPhiBumpSplitter"));
	if(phi_splitter) {
		cout << "Deactivating PhiBumpSplitter!" << endl;
		phi_splitter->SetActive(kFALSE);
	} else {
		cout << "No PndEmcPhiBumpSplitter available" << endl;
	}
}

int run_reco_timebased()
{
  // Macro created 20/09/2006 by S.Spataro
  // It loads a simulation file and digitize hits 

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // Input file (MC events)
  TString inFile = "dpm_simulation.root";
  Int_t nEvents = 0;
  
  PndFileNameCreator creator(inFile.Data());
  TString parFile = creator.GetParFileName().c_str();
  TString digiFile = creator.GetDigiFileName("timebased").c_str();
  TString outFile = creator.GetRecoFileName("timebased").c_str();
  std::cout << "DigiFileName: " << outFile.Data() << std::endl;
  // Digitisation file (ascii)
  TString paramFile = "all.par";
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(digiFile);
  fRun->AddFriend(inFile);
  fRun->SetOutputFile(outFile);
  fRun->SetGenerateRunInfo(kFALSE);
  fRun->RunWithTimeStamps();
  fRun->SetUseFairLinks(kTRUE); 

  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += paramFile;
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  
   // -----   MVD reco   ---------------------------------
  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);

  // -----   EMC reco   ---------------------------------
  Int_t verbose = 0;
   Bool_t storeclusters = kTRUE;
   Bool_t storebumps = kTRUE;

   PndEmcMakeCluster* clusterTask = new PndEmcMakeCluster(verbose, storeclusters);
   clusterTask->StoreClusterBaseDigis(kTRUE);
   fRun->AddTask(clusterTask);

   //TODO: tasks needed due to limitiations in EmcCluster data class
   //Removes multiple entries of same crystal in one cluster --> PndEmcCluster cannot handle this
   PndEmcClusterRemoveDuplCrys* corrTask = new PndEmcClusterRemoveDuplCrys(0);
   clusterTask->Add(corrTask);
   corrTask->SetActive(kTRUE);

   PndEmcMakeBump* bumperTask = new PndEmcMakeBump(verbose, storebumps);
   fRun->AddTask(bumperTask);

  //PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  //fRun->AddTask(emcHdrFiller); // ECM header

  // -----   SciT hit producers   ---------------------------
  Double_t SciTDeadtime = 1000;
  Double_t SciTdt = 0.1;

  PndSciTDigiTask* SciTDigi = new PndSciTDigiTask();
  SciTDigi->SetVerbose(iVerbose);
  SciTDigi->SetDeadTime(SciTDeadtime);
  SciTDigi->SetTimeResolution(SciTdt);
  SciTDigi->RunTimeBased();
  fRun->AddTask(SciTDigi);

  // -----   MDT hit producers   ---------------------------------
  PndMdtTrkProducer* mdtTrkProd = new PndMdtTrkProducer();
  fRun->AddTask(mdtTrkProd);

  // -----   DRC hit producers   ---------------------------------
  PndDrcHitProducerReal* drchit = new PndDrcHitProducerReal();
  drchit->SetVerbose(iVerbose);
  fRun->AddTask(drchit);

  // -----   GEM hit producers   ---------------------------------
  PndGemFindHits* gemFindHits = new PndGemFindHits("GEM Hit Finder", iVerbose);
  fRun->AddTask(gemFindHits);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  deactivatePhiSplitter(bumperTask);

  timer.Start();
  fRun->Run(0, nEvents);

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

  return 0;
}
