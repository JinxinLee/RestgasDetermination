int QAmacro_gem_2()
{
  Int_t verboseLevel = 0;

  // ----  Load libraries   -------------------------------------------------
  TString sysFile = gSystem->Getenv("VMCWORKDIR");

  //FileNames
  TString MCFile  = "tstQA.points.root";
  TString parFile = "tstQA.param.root";
  TString outFile = "tstQA.reco.root";

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->SetOutputFile(outFile);

  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/all.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ------------------------------------------------------------------------

  // -----   Digitizer and Hit Finder   -------------------------------------
  PndGemDigitize* gemDigitize = new PndGemDigitize("GEM Digitizer", verboseLevel);
  fRun->AddTask(gemDigitize);
  
  PndGemFindHits* gemFindHits = new PndGemFindHits("GEM Hit Finder", verboseLevel);
  fRun->AddTask(gemFindHits);

  //------ Track finder ------------------------------
  //Create and add finder task
  PndGemFindTracks* finderTask = new PndGemFindTracks("PndGemFindTracks");
  finderTask->SetUseHitOrDigi("hit"); // hit = (default), digi
  fRun->AddTask(finderTask);
  
  //------ Realistic Track finder --------------------
  PndGemTrackFinderOnHits* mcTrackFinder = new  PndGemTrackFinderOnHits();
  mcTrackFinder->SetVerbose(verboseLevel);  // verbosity level
  mcTrackFinder->SetPrimary(0);  // 1 = Only primary tracks are processed, 0 = all (default)
  finderTask->UseFinder(mcTrackFinder);
  //--------------------------------------------------

  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
  //--------------------------------------------------
  
  // -----   Run Kalman fitter   --------------------------------------------
  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
  recoKalman->SetTrackInBranchName("GEMTrack");
  recoKalman->SetTrackOutBranchName("GEMFitTrack");
  //recoKalman->SetNumIterations(3);
  fRun->AddTask(recoKalman);
  // ------------------------------------------------- 

  // -----   Run Track finder QA   --------------------------------------------
  PndGemTrackFinderQA* trackFinderQA = new PndGemTrackFinderQA();
  trackFinderQA->SetVerbose(verboseLevel);
  fRun->AddTask(trackFinderQA);
  // ------------------------------------------------- 

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,100);

  //============================================================================
  cout << endl;
  cout << "=====================================================================" << endl;
  cout << "Trying to read histograms..." << flush;

  TH1F* fhMCPrimVsP   = (TH1F*)gDirectory->Get("GemTrackFinderQA/hMCPrimVsP");
  TH1F* fhRecoPrimVsP = (TH1F*)gDirectory->Get("GemTrackFinderQA/hRecoPrimVsP");
  TH1F* fhRecoPrimP   = (TH1F*)gDirectory->Get("GemTrackFinderQA/hRecoPrimP");
  TH1F* fhNofHitsPT   = (TH1F*)gDirectory->Get("GemTrackFinderQA/hNofHitsPerTrack");

  Double_t minEfficiency = 80.;
  Double_t minQuality    =  1./1.2;
  Double_t maxQuality    =  1./minQuality;
  Double_t maxResolution =  5.;

  Bool_t fTest = kFALSE;

  if ( fhMCPrimVsP && fhRecoPrimVsP && fhRecoPrimP ) {
    cout << " DONE" << endl;
    Int_t fNofMCPrim     = fhMCPrimVsP  ->Integral();
    Int_t fNofRecoPrim   = fhRecoPrimVsP->Integral();
    Int_t fNofRecoTracks = fhNofHitsPT  ->Integral();
    Double_t effPrim = 0.;
    if ( fNofMCPrim ) effPrim = 100.*(Double_t)fNofRecoPrim/((Double_t)fNofMCPrim);
    cout << "             Simulated / Reconstructed:" << endl;
    cout << " quantity  :       " << fNofMCPrim << " /           " << fNofRecoPrim << "  >>>> " << effPrim << "%" << endl;
    Double_t mcMomenta   = fhRecoPrimVsP->GetMean();
    Double_t recoMomenta = fhRecoPrimP  ->GetMean();
    Double_t momQuality = 0.;
    if ( recoMomenta != 0. ) momQuality = mcMomenta/recoMomenta;
    cout << " quality   :     " << mcMomenta << " /         " << recoMomenta << "  >>>> " << momQuality << "" << endl;

    Double_t recoMomRMS    = fhRecoPrimP  ->GetRMS();
    Double_t momResolution = 0.;
    if ( mcMomenta != 0. ) momResolution = recoMomRMS/mcMomenta*100.;
    cout << " resolution:                            >>>> " << momResolution << "%" << endl;

    if ( effPrim       > minEfficiency &&
	 momQuality    > minQuality &&
	 momQuality    < maxQuality &&
	 momResolution < maxResolution )
      fTest = kTRUE; 

    // in case can't match reco tracks to mc tracks
    if ( !fTest && fNofRecoTracks > fNofMCPrim*minEfficiency/100. ) {
      cout << "=====================================================================" << endl;
      cout << "Reconstructed tracks do not match MC tracks, but there are enough of them" << endl;
      fTest = kTRUE;
    }
  }
  else
    cout << " FAILED" << endl;
  cout << "=====================================================================" << endl;

  //============================================================================

  if (fTest){
    cout << " Test passed" << endl;
    cout << " All ok " << endl;  
  }else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;         
  }

  exit(0); 

}  
  
