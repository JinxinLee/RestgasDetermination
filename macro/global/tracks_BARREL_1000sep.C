void tracks_BARREL_1000sep(char* type="proof", Int_t nofFiles = 1, TString proofName="") {

  Int_t nparts = 22;
  Int_t pid = 13;
  Int_t nStations = 3;

  Int_t nEvents = 0;

  nEvents = nofFiles*1000;
  TString workDir = gSystem->WorkingDirectory();

  if ( nStations != 3 && nStations != 4 ) {
    cout << "WRONG number of stations, only 3 or 4 allowed." << endl;
    return;
  }

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Parameter file
  TString parFile  = Form("%s/params_22Part_n1000_f0_sep.root",workDir.Data());
  //"params_%dPart_n1000.root",workDir.Data(),nparts);

  // Output file
  TString outFile = Form("tracks_%dPart_n%d.root",nparts,nEvents);

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
  FairRunAna *fRun= new FairRunAna(type,proofName.Data());
  fRun->SetOutputFile(outFile.Data());
  //  fRun->SetProofParName("$VMCWORKDIR/gconfig/libFairRoot3.par");
  //  fRun->GetProof()->SetParameter("PROOF_PacketizerStrategy", (Int_t)0);
  
  fRun->SetInputFile(Form("file://%s/points_%dPart_n1000_f%d_sep.root",workDir.Data(),nparts,0));
  for ( Int_t ifile = 1 ; ifile < nofFiles ; ifile++ )
    fRun->AddFile   (Form("file://%s/points_%dPart_n1000_f%d_sep.root",workDir.Data(),nparts,ifile));
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/all.par";
  if ( nStations == 4 ) allDigiFile = sysFile+"/macro/params/all4.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();

  TList* fnamelist = new TList();

  for ( Int_t ifile = 0 ; ifile < nofFiles ; ifile++ )
    fnamelist->Add(new TObjString(Form("%s/params_%dPart_n1000_f%d_sep.root",workDir.Data(),nparts,ifile)));

  //  parInput1->open(parFile.Data());
  parInput1->open(fnamelist);       
  //parInput1->open(Form("%s/allParams_20120504_171441.root",workDir.Data()));

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
    
  // ##################################################################   STT
  PndSttHitProducerIdeal* sttHitProducer = new PndSttHitProducerIdeal();
  // PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
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
  barrelTF->UseMvdSttTpcGem(kTRUE,kTRUE,kFALSE,kTRUE);
  fRun->AddTask(barrelTF);
  
  /*
  PndBarrelTrackFinderQA* barrelQA = new PndBarrelTrackFinderQA();
  barrelQA->SetVerbose(0);
  barrelQA->UseMvdSttTpcGem(kTRUE,kTRUE,kFALSE,kTRUE);
  fRun->AddTask(barrelQA);
  */
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  //  fRun->SetProofServerName(proofName);
  //  TString anaType = type;
  //  if ( anaType.Contains("local") ) 
  fRun->Run(0,0);//nEvents);
  //  else
  //    fRun->Run(type,0,nEvents);

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


}



/*

Warning in <TClass::TClass>: no dictionary for class pair<double,FairTimeStamp*> is available
Error in <Pair Emulation Building>: const char* is not yet supported in pair emulation
Error in <Pair Emulation Building>: const char* is not yet supported in pair emulation
Error in <Pair Emulation Building>: const char* is not yet supported in pair emulation
Error in <Pair Emulation Building>: const char* is not yet supported in pair emulation
Error in <TClass::BuildRealData>: Cannot find any ShowMembers function for PndSdsCalcStrip!

*/

/*

Warning in <TClass::TClass>: no dictionary for class pair<double,FairTimeStamp*> is available
Error in <Pair Emulation Building>: const char* is not yet supported in pair emulation
Error in <Pair Emulation Building>: const char* is not yet supported in pair emulation
Error in <Pair Emulation Building>: const char* is not yet supported in pair emulation
Error in <Pair Emulation Building>: const char* is not yet supported in pair emulation
Error in <TClass::BuildRealData>: Cannot find any ShowMembers function for PndSdsCalcStrip!
Warning in <TClass::TClass>: no dictionary for class BinaryFunctor is available
Error in <TStreamerInfo::Build:>: PndSdsPixelClusterTask: BinaryFunctor* has no streamer or dictionary, data member fFunctor will not be saved
Error in <TStreamerInfo::Build:>: PndSdsPixelClusterTask: BinaryFunctor* has no streamer or dictionary, data member fStartFunctor will not be saved
Warning in <TClass::TClass>: no dictionary for class PndSdsPixelBackMapping is available
Error in <TStreamerInfo::Build:>: PndSdsPixelClusterTask: PndSdsPixelBackMapping* has no streamer or dictionary, data member fBackMapping will not be saved
Warning in <TClass::TClass>: no dictionary for class PndSdsFEAmpModelSimple is available
Error in <TStreamerInfo::Build:>: PndSdsFE: PndSdsFEAmpModelSimple* has no streamer or dictionary, data member fFrontEndModel will not be saved
Error in <Pair Emulation Building>: const char* is not yet supported in pair emulation
Error in <Pair Emulation Building>: const char* is not yet supported in pair emulation
Error in <TStreamerInfo::Build:>: PndSdsStripClusterTask: BinaryFunctor* has no streamer or dictionary, data member fFunctor will not be saved
Error in <TClass::New>: cannot create object of class PndSdsChargeWeightingAlgorithms

*/
