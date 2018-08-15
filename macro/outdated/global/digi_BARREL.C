int digi_BARREL(Int_t nEvents, Int_t nparts = 33, Int_t pid = 13, Int_t nStations = 3) {
  
  if ( nStations != 3 && nStations != 4 ) {
    cout << "WRONG number of stations, only 3 or 4 allowed." << endl;
    return;
  }

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = Form("points_%dPart_n%d.root",nparts,nEvents);

  // Parameter file
  TString parFile  = Form("params_%dPart_n%d.root",nparts,nEvents);

  // Output file
  TString outFile = Form("digi_%dPart_n%d_BTF.root",nparts,nEvents);

  // Number of events to process
  Int_t nEvents = 0;
 
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  // ------------------------------------------------------------------------

  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------

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
  TString allDigiFile = sysFile+"/macro/params/all.par";
  if ( nStations == 4 ) allDigiFile = sysFile+"/macro/params/all4.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ------------------------------------------------------------------------

  // ##################################################################    MVD
  // -----   MDV digi producer   ----------------------------------------
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->SetVerbose(iVerbose);
  fRun->AddTask(mvddigi);
    // -----   MDV cluster producer   -----------------------------------
  Double_t chargecut = 5000., pixelrad=1.8; // one day this will move to the parameter db.
  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);
    // -----   MDV cluster producer   -----------------------------------
  //  Double_t chargecut = 5000., pixelrad=1.8; // one day this will move to the parameter db.
//   PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
//   mvdmccls->SetVerbose(iVerbose);
//   fRun->AddTask(mvdmccls);

  /*// -----   MDV strip digi producer   -------------------------------- 
  PndMvdStripHitProducer* mvdHitProd = new PndMvdStripHitProducer();
  mvdHitProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdHitProd);
  // -----   MDV hybrid digi producer   ------------------------------- 
  PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer();
  mvdPixProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdPixProd);
  // -----   MVD strip cluster task   ---------------------------------
  Double_t chargecut = 5000.
  PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask(chargecut, inFile);
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);
  // -----   MVD pixel cluster task   ---------------------------------
  PndMvdPixelClusterTask* mvdClusterizer = new PndMvdPixelClusterTask(1.8, inFile);
  mvdClusterizer->SetVerbose(iVerbose);
  fRun->AddTask(mvdClusterizer);*/

  // ##################################################################   STT
  PndSttHitProducerIdeal* sttHitProducer = new PndSttHitProducerIdeal();
    //PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
  fRun->AddTask(sttHitProducer);

  // ##################################################################   GEM
  // -----   GEM digi producer   --------------------------------------
  Int_t verboseLevel = 0;
  PndGemDigitize* gemDigitize = new PndGemDigitize("GEM Digitizer", verboseLevel);
  fRun->AddTask(gemDigitize);
  // -----   GEM hit producer   ---------------------------------------
  PndGemFindHits* gemFindHits = new PndGemFindHits("GEM Hit Finder", verboseLevel);
  fRun->AddTask(gemFindHits);

  // -----   GEM Ideal Hit Producer   ---------------------------------------
  //PndGemIdealHitProducer* gemHitProducer = new PndGemIdealHitProducer("GEM Ideal hit producer", verboseLevel);
  //fRun->AddTask(gemHitProducer);

  // ##################################################################

  PndBarrelTrackFinder* barrelTF = new PndBarrelTrackFinder();
  barrelTF->SetVerbose(0);
  barrelTF->UseMvdSttGem(kTRUE,kTRUE,kTRUE);
  fRun->AddTask(barrelTF);
  
  PndBarrelTrackFinderQA* barrelQA = new PndBarrelTrackFinderQA();
  barrelQA->SetVerbose(0);
  barrelQA->UseMvdSttGem(kTRUE,kTRUE,kTRUE);
  fRun->AddTask(barrelQA);

  // ##################################################################

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);

  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


  return 0;
}
