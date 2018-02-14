// root macro to analyze the clusterization output
{
  bool verbose = false;

  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("../Tools.C");
  LoadPandaStyle();
  gROOT->LoadMacro("../Helper.C");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  TString HitFileName="data/cosmic02.hit";
  TString CalibFileName="calib.par";

  PndMvdConvertApv* ApvConverter= new PndMvdConvertApv(CalibFileName, HitFileName);
  long int  nEvents = ApvConverter->GetNofEvents();
  for(long int i=0;i<nEvents;++i)
  {
    std::vector<PndSdsDigiStrip> = ApvConverter->ReadNext();
  }

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
