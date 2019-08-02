// Macro for running Panda reconstruction tasks
// to run the macro:
// root  reco_complete.C  or in root session root>.x  reco_complete.C
int reco_complete(Int_t nEvents = 0, TString  prefix = "../data/evtcomplete", TString options="")
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
//  TString  options        = "multikalman"; //"genfit2"
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec"; 
  TString  output         = "reco_single";
  TString  friend1        = "sim";
  TString  friend2        = "digi";
  TString  friend3        = "";
  TString  friend4        = "";

  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput(input);
  fRun->SetOutput(output);
  fRun->AddFriend(friend1);
  fRun->AddFriend(friend2);
  fRun->AddFriend(friend3);
  fRun->AddFriend(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->SetOptions(options);
  fRun->Setup(prefix);

  // -----   Add tasks   ----------------------------------------------------

  fRun->AddRecoTasks();

  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
