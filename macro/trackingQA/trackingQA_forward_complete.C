// Macro for running Panda reconstruction tasks
// to run the macro:
// root  reco_complete.C  or in root session root>.x  reco_complete.C
int trackingQA_forward_complete(TString prefix, TString branch, Int_t nEvents=0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "";
  TString  output         = "trackingQAfwd_";
  output.Append(branch);
  TString  friend1        = "recofwd";
  TString  friend2        = "digi";
  TString  friend3        = "";
  TString  friend4        = "";

  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput(input);
  fRun->SetOutput(output);
  fRun->SetFriend1(friend1);
  fRun->SetFriend2(friend2);
  fRun->SetFriend3(friend3);
  fRun->SetFriend4(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);
  
  // -----   Add tasks   ----------------------------------------------------
  // here comes your tracking code


    /// QA task
    PndTrackingQATask* trackingQA = new PndTrackingQATask(branch, "FwdIdealTrack");
    trackingQA->SetFunctorName("FtsTrackFunctor");
    trackingQA->SetVerbose(0);
    fRun->AddTask(trackingQA);

  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
