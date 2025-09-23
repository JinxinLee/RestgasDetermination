//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************
// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_complete.C  or in root session root>.x  digi_complete.C
int reco_complete(Int_t nEvents = 0, TString prefix = "../data/evtcomplete")
{ 
  //-----User Settings:------------------------------------------------------
  //TString parAsciiFile = "all_hvmaps.par";
  TString parAsciiFile   = "all.par";
  //TString input = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString output = "reco";
  TString friend1 = "sim";
  TString friend2 = "digi";
  TString friend3 = "";
  TString friend4 = "";
  TString fOptions = "gf2"; // "gf2" for genfit 2

  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun = new PndMasterRunAna();
  //fRun->SetInput(input);
  fRun->SetInput("dummy");
  fRun->SetOutput(output);
  fRun->AddFriend(friend1);
  fRun->AddFriend(friend2);
  fRun->AddFriend(friend3);
  fRun->AddFriend(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);

  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

  PndTrkTracking2 *tracking2 = nullptr;
  fRun->AddTask(tracking2 = new PndTrkTracking2(0, false, false, true)); // 1
  tracking2->SetInputBranchName("STTHit", "MVDHitsPixel", "MVDHitsStrip");
  tracking2->NoMvdAloneTracking();
  tracking2->SetPersistency(kTRUE);

//  PndSttMvdGemTracking *SttMvdGemTracking = nullptr;
//  fRun->AddTask(SttMvdGemTracking = new PndSttMvdGemTracking(0));
//  SttMvdGemTracking->SetPersistency(kTRUE);

  PndSttSkewedCombineTask *SkewedCombined = new PndSttSkewedCombineTask();
  fRun->AddTask(SkewedCombined);
//
//  PndBarrelTrackFinder *tracking = new PndBarrelTrackFinder();
//  tracking->UseMvdSttGem(kTRUE, kTRUE, kTRUE);
//  tracking->SetPersistency(kTRUE);
//  fRun->AddTask(tracking);

  PndUnassignedHitsTask *unassignedHits = new PndUnassignedHitsTask();
  unassignedHits->AddHitBranch("STTHit");
  unassignedHits->AddHitBranch("MVDHitsPixel");
  unassignedHits->AddHitBranch("MVDHitsStrip");
  unassignedHits->AddHitBranch("GEMHit");
  //unassignedHits->AddTrackCands("BarrelTrackCand");
  //unassignedHits->AddTrackCands("SttMvdTrackCand");
  unassignedHits->SetUnassignedBranchExtension("AfterStandardTrack");
  fRun->AddTask(unassignedHits);

  PndApolloniusTripletTrackFinderTask *ApolloniusTriplet = new PndApolloniusTripletTrackFinderTask();
  ApolloniusTriplet->SetSTTName("STTHit");
  ApolloniusTriplet->AddBranchName("STTHit");
  ApolloniusTriplet->AddBranchName("MVDHitsPixel");
  ApolloniusTriplet->AddBranchName("MVDHitsStrip");
  ApolloniusTriplet->AddBranchName("GEMHit");
  ApolloniusTriplet->AddBranchName("STTCombinedSkewedHits");
  //ApolloniusTriplet->AddFoundTrackName("BarrelTrack");
  //ApolloniusTriplet->AddFoundTrackName("SttMvdTrack");
  //ApolloniusTriplet->AddFoundTrackName("PidAlgoIdealCharged");
  ApolloniusTriplet->SetWithTubeReduction(true);
  ApolloniusTriplet->SetWithCombiReduction(true);
  fRun->AddTask(ApolloniusTriplet);

  PndSttSkewStrawPzFinderTask *pzfinder = new PndSttSkewStrawPzFinderTask();
  pzfinder->setTrackBranch("ApolloniusTripletTrack");
  pzfinder->setTrackCandBranch("ApolloniusTripletTrackCand");
  pzfinder->setOutputPrefix("Final");
  pzfinder->setMethod(1);
  pzfinder->StoreData(true);
  fRun->AddTask(pzfinder);

  if (!fOptions.Contains("gf2")) {
    PndRecoKalmanTask *recoKalman = nullptr;
    fRun->AddTask(recoKalman = new PndRecoKalmanTask());
    recoKalman->SetTrackInBranchName("FinalTrack");
    recoKalman->SetTrackOutBranchName("FinalGenTrack");

    recoKalman->SetBusyCut(50); // CHECK to be tuned
    // recoKalman->SetIdealHyp(kTRUE);
    // recoKalman->SetNumIterations(3);
    recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
    recoKalman->SetPropagateToIP(kFALSE);
  } else {
    PndRecoKalmanTask2 *recoKalman = nullptr;
    fRun->AddTask(recoKalman = new PndRecoKalmanTask2());
    recoKalman->SetTrackInBranchName("FinalTrack");
    //recoKalman->SetTrackOutBranchName("FinalGenTrack");
    recoKalman->SetTrackOutBranchName("FinalGenTrack");

    recoKalman->SetBusyCut(50); // CHECK to be tuned
    //recoKalman->SetIdealHyp(kTRUE);
    recoKalman->SetNumIterations(3);
    //recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
    recoKalman->SetPropagateToIP(kFALSE);
    recoKalman->SetVerbose(3);
  }

  if (fOptions.Contains("filtered")) {
    PndMissingPzCleanerTask *cleaner = nullptr;
    fRun->AddTask(cleaner = new PndMissingPzCleanerTask()); // 4
    if ((!fOptions.Contains("day1")) || (fOptions.Contains("gem"))) {
      cleaner->SetInputTrackBranch("SttMvdGemGenTrack");
    } else {
      cleaner->SetInputTrackBranch("SttMvdGenTrack");
    }
    cleaner->SetRemoveTrack(kTRUE);
  }

  PndIdealTrackFinder *trackFts = nullptr;
  fRun->AddTask(trackFts = new PndIdealTrackFinder());
  trackFts->SetTrackSelector("FtsTrackFunctor");
  trackFts->AddBranchName("FTSHit");
  trackFts->AddBranchName("MVDHitsPixel");
  trackFts->AddBranchName("MVDHitsStrip");
  trackFts->SetRelativeMomentumSmearing(0.05);
  trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
  trackFts->SetTrackingEfficiency(1.);
  trackFts->SetOutputBranchName("FtsIdealTrack");
  trackFts->SetPersistence(kTRUE);

  if (!fOptions.Contains("gf2")) {
    PndRecoKalmanTask *recoKalmanFwd = nullptr;
    fRun->AddTask(recoKalmanFwd = new PndRecoKalmanTask());
    recoKalmanFwd->SetTrackInBranchName("FtsIdealTrack");
    // recoKalmanFwd->SetTrackInIDBranchName("FtsIdealTrackID");
    recoKalmanFwd->SetTrackOutBranchName("FtsIdealGenTrack");
    recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
    // recoKalmanFwd->SetIdealHyp(kTRUE);
    // recoKalmanFwd->SetNumIterations(3);
    recoKalmanFwd->SetTrackRep(0); // 0 Geane (default), 1 RK
    //recoKalmanFwd->SetPropagateToIP(kFALSE);
  } else {
    PndRecoKalmanTask2 *recoKalmanFwd = nullptr;
    fRun->AddTask(recoKalmanFwd = new PndRecoKalmanTask2());
    recoKalmanFwd->SetTrackInBranchName("FtsIdealTrack");
    // recoKalmanFwd->SetTrackInIDBranchName("FtsIdealTrackID");
    recoKalmanFwd->SetTrackOutBranchName("FtsIdealGenTrack");
    recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
                                   // recoKalmanFwd->SetIdealHyp(kTRUE);
    // recoKalmanFwd->SetNumIterations(3);
    // recoKalmanFwd->SetTrackRep(0); // 0 Geane (default), 1 RK
    //recoKalmanFwd->SetPropagateToIP(kFALSE);
  }

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
