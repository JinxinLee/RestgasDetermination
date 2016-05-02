// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_box.C  or in root session root>.x  digi_box.C
void digi_box(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "box_1pi_1gev_theta10-120"; 
  TString  output         = "digi";
  TString  friend1        = "";
  TString  friend2        = "";
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
  fRun->Setup();

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddDigiTasks();

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
 
  exit(0);
}
