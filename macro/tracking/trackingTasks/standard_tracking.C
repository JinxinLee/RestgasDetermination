// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_complete.C  or in root session root>.x  digi_complete.C
int standard_tracking(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  prefix         = "evtcomplete";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output         = "standard";
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


	PndTrkTracking2* tracking = NULL;
	fRun->AddTask(tracking = new PndTrkTracking2(0,false,false,true)); // 1
	tracking->SetInputBranchName("STTHit","MVDHitsPixel","MVDHitsStrip");
	tracking->NoMvdAloneTracking();
	tracking->SetPersistency(kFALSE);

    PndSttMvdGemTracking *SttMvdGemTracking = NULL;
    fRun->AddTask(SttMvdGemTracking = new PndSttMvdGemTracking(0));
    SttMvdGemTracking->SetPersistency(kTRUE);

    if (fOptions.Contains("gf1")){
		PndRecoKalmanTask* recoKalman = NULL;
		fRun->AddTask(recoKalman = new PndRecoKalmanTask());
		recoKalman->SetTrackInBranchName("SttMvdGemTrack");
		recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");

		recoKalman->SetBusyCut(50); // CHECK to be tuned
		//recoKalman->SetIdealHyp(kTRUE);
		//recoKalman->SetNumIterations(3);
		recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
		//recoKalman->SetPropagateToIP(kFALSE);
	} else if (fOptions.Contains("gf2")){
		PndRecoKalmanTask2* recoKalman = NULL;
		fRun->AddTask(recoKalman = new PndRecoKalmanTask2());
		recoKalman->SetTrackInBranchName("SttMvdGemTrack");
		recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");

		recoKalman->SetBusyCut(50); // CHECK to be tuned
		//recoKalman->SetIdealHyp(kTRUE);
		//recoKalman->SetNumIterations(3);
		//recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
		//recoKalman->SetPropagateToIP(kFALSE);
	}

	if (fOptions.Contains("filtered")){
		  PndMissingPzCleanerTask* cleaner = NULL;
		  fRun->AddTask(cleaner = new PndMissingPzCleanerTask()); //4
		  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
		  {
			  cleaner->SetInputTrackBranch("SttMvdGemGenTrack");
		  } else {
			  cleaner->SetInputTrackBranch("SttMvdGenTrack");
		  }
		  cleaner->SetRemoveTrack(kTRUE);
	}


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
