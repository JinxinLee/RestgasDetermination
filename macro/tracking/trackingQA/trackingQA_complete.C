// Macro for running Panda reconstruction tasks
// to run the macro:
// root  reco_complete.C  or in root session root>.x  reco_complete.C
int trackingQA_complete(TString prefix, TString branch, Int_t nEvents=0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "";
  TString  output         = "trackingQA_";
  output.Append(branch);
  TString  friend1        = "standard";
  TString  friend2        = "digi";
  TString  friend3        = "sim";
  TString  friend4        = "idealbarreltracking";

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
  
  // -----   Add tasks   ----------------------------------------------------
  // here comes your tracking code


    /// QA task
    PndTrackingQATask* trackingQA = new PndTrackingQATask(branch, "BarrelIdealTrack");
    trackingQA->SetVerbose(0);
    fRun->AddTask(trackingQA);

  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
