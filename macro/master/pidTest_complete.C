// Macro for running Panda pid tasks
// to run the macro:
// root  pid_complete.C  or in root session root>.x  pid_complete.C
int pidTest_complete(Int_t nEvents = 0, TString  prefix = "evtcomplete")
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec"; 
  TString  output         = "pidTest";
  TString  friend1        = "digi";
  TString  friend2        = "reco";
  TString  friend3        = "";
  TString  friend4        = "pid";
  TString  options        = "";
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput(input);
  fRun->SetOutput(output);
  fRun->AddFriend(friend1);
  fRun->AddFriend(friend2);
  fRun->AddFriend(friend3);
  fRun->AddFriend(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->SetOptions(fRun->GetOptions()+options);
  fRun->Setup(prefix);

  // -----   Add tasks   ----------------------------------------------------
//  fRun->SetOptions("multikalman");
  PndPidTestTask *anaTask = new PndPidTestTask();
  anaTask->SetVerbose(3);
  anaTask->SetClassifier("PidAlgoMvd;PidAlgoStt;PidAlgoDrc;PidAlgoMdtHardCuts;PidAlgoEmcBayes;PidAlgoDisc;PidAlgoSciT"); //("PidAlgoIdealCharged");
  anaTask->SetSelector("Tight");
  fRun->AddTask(anaTask);

  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
