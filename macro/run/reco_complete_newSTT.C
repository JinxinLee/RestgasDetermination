// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_complete.C  or in root session root>.x  digi_complete.C
int reco_complete_newSTT(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  prefix         = "evtcomplete";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output         = "reco";
  TString  friend1        = "sim";
  TString  friend2        = "digi";
  TString  friend3        = "";
  TString  friend4        = "";
  TString  fOptions       = "gf2"; // "gf2" for genfit 2

  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput(input);
  fRun->SetOutput(output);
  fRun->AddFriend(friend1);
  fRun->AddFriend(friend2);
  fRun->AddFriend(friend3);
  fRun->AddFriend(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);

  FairGeane *Geane = new FairGeane();
    fRun->AddTask(Geane);

    PndSttRootGeometryInitTask* task = new PndSttRootGeometryInitTask();
    fRun->AddTask(task);

    PndBarrelTrackFinder *tracking = new PndBarrelTrackFinder();
    tracking->UseMvdSttGem(kTRUE, kTRUE, kTRUE);
    tracking->SetPersistency(kTRUE);
   // tracking->DrawDetails();
   // tracking->DrawHistos();
    //tracking->SetVerbose(4);
    fRun->AddTask(tracking);


    if (!fOptions.Contains("gf2")){
		PndRecoKalmanTask* recoKalman = NULL;
		fRun->AddTask(recoKalman = new PndRecoKalmanTask());
		recoKalman->SetTrackInBranchName("BarrelTrack");
		recoKalman->SetTrackOutBranchName("BarrelGenTrack");

		recoKalman->SetBusyCut(50); // CHECK to be tuned
		//recoKalman->SetIdealHyp(kTRUE);
		//recoKalman->SetNumIterations(3);
		recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
		//recoKalman->SetPropagateToIP(kFALSE);
	} else {
		PndRecoKalmanTask2* recoKalman = NULL;
		fRun->AddTask(recoKalman = new PndRecoKalmanTask2());
		recoKalman->SetTrackInBranchName("BarrelTrack");
		recoKalman->SetTrackOutBranchName("BarrelGenTrack");

		recoKalman->SetBusyCut(50); // CHECK to be tuned
		//recoKalman->SetIdealHyp(kTRUE);
		//recoKalman->SetNumIterations(3);
		//recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
		//recoKalman->SetPropagateToIP(kFALSE);
	}

//	if (fOptions.Contains("filtered")){
//		  PndMissingPzCleanerTask* cleaner = NULL;
//		  fRun->AddTask(cleaner = new PndMissingPzCleanerTask()); //4
//		  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
//		  {
//			  cleaner->SetInputTrackBranch("SttMvdGemGenTrack");
//		  } else {
//			  cleaner->SetInputTrackBranch("SttMvdGenTrack");
//		  }
//		  cleaner->SetRemoveTrack(kTRUE);
//	}

	PndIdealTrackFinder* trackFts = NULL;
	fRun->AddTask(trackFts = new PndIdealTrackFinder());
	trackFts->SetTrackSelector("FtsTrackFunctor");
	trackFts->AddBranchName("FTSHit");
	trackFts->AddBranchName("MVDHitsPixel");
	trackFts->AddBranchName("MVDHitsStrip");
	trackFts->SetRelativeMomentumSmearing(0.05);
	trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
	trackFts->SetTrackingEfficiency(1.);
	trackFts->SetOutputBranchName("FtsIdealTrack");
	trackFts->SetPersistence(kFALSE);

	if (!fOptions.Contains("gf2")){
		PndRecoKalmanTask* recoKalmanFwd = NULL;
		fRun->AddTask(recoKalmanFwd = new PndRecoKalmanTask());
		recoKalmanFwd->SetTrackInBranchName("FtsIdealTrack");
		//recoKalmanFwd->SetTrackInIDBranchName("FtsIdealTrackID");
		recoKalmanFwd->SetTrackOutBranchName("FtsIdealGenTrack");
		recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
		//recoKalmanFwd->SetIdealHyp(kTRUE);
		//recoKalmanFwd->SetNumIterations(3);
		recoKalmanFwd->SetTrackRep(0); // 0 Geane (default), 1 RK
		//recoKalmanFwd->SetPropagateToIP(kFALSE);
	} else {
		PndRecoKalmanTask2* recoKalmanFwd = NULL;
		fRun->AddTask(recoKalmanFwd = new PndRecoKalmanTask2());
		recoKalmanFwd->SetTrackInBranchName("FtsIdealTrack");
		//recoKalmanFwd->SetTrackInIDBranchName("FtsIdealTrackID");
		recoKalmanFwd->SetTrackOutBranchName("FtsIdealGenTrack");
		recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
		//recoKalmanFwd->SetIdealHyp(kTRUE);
		//recoKalmanFwd->SetNumIterations(3);
		//recoKalmanFwd->SetTrackRep(0); // 0 Geane (default), 1 RK
		//recoKalmanFwd->SetPropagateToIP(kFALSE);
	}




  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
