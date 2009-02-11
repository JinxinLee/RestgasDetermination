// root macro to analyze the clusterization output
{
  bool verbose = false;

  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  LoadPandaStyle();

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  std::string HitFileName="testhits.hit";
  std::string CalibFileName="calib.cal";

  PndMvdConvertApv* ApvConverter= new PndMvdConvertApv(CalibFileName, HitFileName);

  int  nEvents = ApvConverter->GetNofEvents();
  if (nEvents > 1000) nEvents = 1000;
  //nEvents = 3;

  FairRunAna *fRun= new FairRunAna();

  fRun->SetOutputFile("output.root");

  PndMvdConvertApvTask* ApvTask = new PndMvdConvertApvTask(ApvConverter);
  fRun->AddTask(ApvTask);
  fRun->Init();
  fRun->DummyRun(0, nEvents);

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

}
