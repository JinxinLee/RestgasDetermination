// root macro to analyze the clusterization output
{
  bool verbose = true;

  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  LoadPandaStyle();
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Helper.C");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  TString HitFileName = "strahl_gneu7.hit.5";
  TString CalibFileName = HitFileName + ".calib";
  TString MapFileName = HitFileName + ".mapping";

  TString outFile = "test.root";
  FairRunAna *fRun= new FairRunAna();
  //fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  PndMvdConvertApv* ApvConverter= new PndMvdConvertApv(CalibFileName, HitFileName);
  long int  nEvents = ApvConverter->GetNofEvents();
  
  PndMvdMapApv* ApvMapper = new PndMvdMapApv(MapFileName);
  
  PndMvdConvertApvTask* convertTask = new PndMvdConvertApvTask(ApvConverter,ApvMapper);
  convertTask->SetVerbose(3);
  
  fRun->AddTask(convertTask);
  
  fRun->Init();
  fRun->Run(0,nEvents);

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

