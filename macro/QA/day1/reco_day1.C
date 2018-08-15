// Macro for running Panda reconstruction tasks
// to run the macro:
// root  reco_day1.C  or in root session root>.x  reco_complete.C

int reco_day1(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile= "all.par";
  TString  input = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output= "reco";
  TString  friend1  = "digi";
  TString  friend2  = "sim"; //TODO remove MC from PndTracking2
  TString  friend3  = "";
  TString  friend4  = "";

  // -----Initial Settings--------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetOptions("day1+fakeonline"); // day1 phase1 gem3 nogem fakeonline
  fRun->SetInput(input);
  fRun->SetOutput(output);
  fRun->AddFriend(friend1);
  fRun->AddFriend(friend2);
  fRun->AddFriend(friend3);
  fRun->AddFriend(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup("fakeonline");

  // -----Add tasks----------------------------------------------------
  fRun->AddRecoTasks();

  // -----Intialise and run--------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  // --- now run!
  fRun->Run(0, nEvents);
  fRun->Finish();

  return 0;
}
