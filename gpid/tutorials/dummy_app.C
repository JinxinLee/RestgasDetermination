{
  TStopwatch timer;
 
  CbmRunAna *fRun = new CbmRunAna();

  EvtGenTask *eGen = new EvtGenTask();
  fRun->AddTask(eGen);

  PndGpidTask *gpid = new PndGpidTask();
  gpid->SetAPPNAME("dummy");
  gpid->SetDIR("weights/");
  gpid->SetMVA(PndGpidTask::MLP);   // Other methods PndGpidTask::KNN , PndGpidTask::BDT, PndGpidTask::MLP
  fRun->AddTask(gpid);

  TString inFile = "sim_emc.root";
  fRun->SetInputFile(inFile);

  TString outFile = "outFile.root";
  fRun->SetOutputFile(outFile);

  fRun->Init();
  timer.Start();
  fRun->Run(0,100);
  timer.Stop();
   Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}
