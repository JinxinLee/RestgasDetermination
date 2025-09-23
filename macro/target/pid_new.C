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

// Macro for running Panda pid tasks
// to run the macro:
// root  pid_complete.C  or in root session root>.x  pid_complete.C
int pid_new(Int_t nEvents = 0, TString prefix = "../data/evtcomplete", TString options = "")
{ 
  //-----User Settings:------------------------------------------------------
  TString parAsciiFile = "all_hvmaps.par";
  //TString input = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString output = "pid_single";
  TString friend1 = "digi";
  TString friend2 = "reco";
  TString friend3 = "sim";
  TString friend4 = "";

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
  fRun->SetOptions(fRun->GetOptions() + options);
  fRun->Setup(prefix);

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddPidTasks();

  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
