void prod_rec(TString prefix="")
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  output         = "reco";
  TString  friend1        = "digi";
 
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput("dummy");
  fRun->SetOutput(output);
  fRun->SetFriend1(friend1);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);
  
  // -----   Add tasks   ----------------------------------------------------
  fRun->AddRecoTasks();
  
  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
}
