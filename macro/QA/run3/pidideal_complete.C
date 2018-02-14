// Macro for running Panda pid tasks
// to run the macro:
// root  pidideal_complete.C  or in root session root>.x  pidideal_complete.C
int pidideal_complete(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec"; 
  TString  output         = "pidideal";
  TString  friend1        = "digi";
  TString  friend2        = "recoideal";
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
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

  PndPidCorrelator* corr = new PndPidCorrelator();
  //corr->SetVerbose();
  corr->SetInputBranch("SttMvdGemGenTrack");
//  corr->SetInputIDBranch("SttMvdGemGenTrackID");
  corr->SetInputBranch2("FtsIdealGenTrack");
//  corr->SetInputIDBranch2("FtsIdealGenTrackID");
  corr->SetIdeal(kTRUE);
  corr->SetIdealHyp(kTRUE);
  //corr->SetDebugMode(kTRUE);
  //corr->SetFast(kTRUE);
  //corr->SetBackPropagate(kFALSE);
  fRun->AddTask(corr);

  PndMcCloner *clone = new PndMcCloner();
  fRun->AddTask(clone);
 
  PndPidIdealAssociatorTask *assMC= new PndPidIdealAssociatorTask();
  fRun->AddTask(assMC);

  PndPidMvdAssociatorTask *assMvd= new PndPidMvdAssociatorTask();
  fRun->AddTask(assMvd);

  PndPidMdtHCAssociatorTask *assMdt= new PndPidMdtHCAssociatorTask();
  fRun->AddTask(assMdt);

  PndPidDrcAssociatorTask *assDrc= new PndPidDrcAssociatorTask();
  fRun->AddTask(assDrc);

  PndPidDiscAssociatorTask *assDisc= new PndPidDiscAssociatorTask();
  fRun->AddTask(assDisc);

  PndPidSttAssociatorTask *assStt= new PndPidSttAssociatorTask();
  fRun->AddTask(assStt);

  PndPidEmcBayesAssociatorTask *assEMC= new PndPidEmcBayesAssociatorTask();
  fRun->AddTask(assEMC);
  
  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();

  return 0;
}
