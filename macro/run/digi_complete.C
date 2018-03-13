// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_complete.C  or in root session root>.x  digi_complete.C
int digiOnly_complete(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  prefix         = "evtcomplete";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
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
  fRun->Setup(prefix);

  // -----   Add tasks   ----------------------------------------------------
  fRun->Add(new PndSttHitProducerRealFast());

  fRun->Add(new PndMvdDigiTask());
  fRun->Add(new PndEmcHitsToWaveform());
  fRun->Add(new PndEmcWaveformToDigi());
  fRun->Add(new PndSciTDigiTask());
  fRun->Add(new PndMdtHitProducerIdeal());
  fRun->Add(new PndDrcHitProducerReal());
  fRun->Add(new PndGemDigitize("GEM Digitizer", 0));
  fRun->Add(new PndFtsHitProducerRealFast());
  fRun->Add(new PndFtofHitProducerIdeal());
  fRun->Add(new PndRichHitProducer());

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
