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

// Macro for running the combination of HoughTrackFinder and ApolloniusTripletTrackFinder (Track finder for the Barrel part of PANDA.
// The z component is still missing)
// to run the macro:
// root  houghPlusApolloniusTripletTrackFinder.C  or in root session root>.x  houghPlusApolloniusTripletTrackFinder.C
int standardPlusApolloniusTripletTrackFinder(Int_t nEvents = 0, TString prefix = "data/dpm/DPM_barrel_test")
{
  //-----User Settings:------------------------------------------------------
  TString parAsciiFile = "all_hvmaps.par";
  //TString input = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString output = "standardPlusApolTripPz";
  TString friend1 = "sim";
  TString friend2 = "digi";
  TString friend3 = "";
  TString friend4 = "";
  TString fOptions = "";

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

  PndSttSkewedCombineTask *SkewedCombined = new PndSttSkewedCombineTask();
  fRun->AddTask(SkewedCombined);

  PndBarrelTrackFinder *tracking = new PndBarrelTrackFinder();
  tracking->UseMvdSttGem(kTRUE, kTRUE, kTRUE);
  tracking->SetPersistency(kTRUE);
  fRun->AddTask(tracking);

  PndUnassignedHitsTask *unassignedHits = new PndUnassignedHitsTask();
  unassignedHits->AddHitBranch("STTHit");
  unassignedHits->AddHitBranch("MVDHitsPixel");
  unassignedHits->AddHitBranch("MVDHitsStrip");
  unassignedHits->AddHitBranch("GEMHit");
  unassignedHits->AddTrackCands("BarrelTrackCand");
  unassignedHits->SetUnassignedBranchExtension("AfterStandardTrack");
  fRun->AddTask(unassignedHits);

  PndApolloniusTripletTrackFinderTask *ApolloniusTriplet = new PndApolloniusTripletTrackFinderTask();
  ApolloniusTriplet->SetSTTName("STTHitAfterStandardTrack");
  ApolloniusTriplet->AddBranchName("STTHitAfterStandardTrack");
  ApolloniusTriplet->AddBranchName("MVDHitsPixelAfterStandardTrack");
  ApolloniusTriplet->AddBranchName("MVDHitsStripAfterStandardTrack");
  ApolloniusTriplet->AddBranchName("GEMHitAfterStandardTrack");
  ApolloniusTriplet->AddBranchName("STTCombinedSkewedHits");
  ApolloniusTriplet->AddFoundTrackName("BarrelTrack");
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


  PndSttSkewStrawPzFinderAnalysisTask *pzana = new PndSttSkewStrawPzFinderAnalysisTask();
  pzana->setIdealTrackBranch("ApolloniusTripletTrack");
  pzana->setIdealTrackCandBranch("ApolloniusTripletTrackCand");
  pzana->setTrackBranch("FinalTrack");
  pzana->setTrackCandBranch("FinalTrackCand");
  pzana->setPzDataBranch("FinalPzData");
  pzana->setDraw(true);
  fRun->AddTask(pzana);


  PndTrackingQATask *trackingQA = new PndTrackingQATask("FinalTrack", "ApolloniusTripletTrack");
  fRun->AddTask(trackingQA);

  //fRun->AddRecoTasks(/*kFALSE*/);
  //fRun->AddPidTasks();
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
