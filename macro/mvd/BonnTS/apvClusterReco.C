// root macro to analyze the clusterization output
{
  int iVerbose = 3;
  long int  nEvents = 10000;

  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  LoadPandaStyle();
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Helper.C");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  TString HitFileName = "data27.hit";
  TString inFile = HitFileName + ".digis.root";
  TString outFile = HitFileName + ".hits.root";

  TString directory = gSystem->Getenv("VMCWORKDIR");
  TString geomFile = directory + "/geometry/TrackingStation.root";
  TString digiparFile = directory + "/macro/params/all.par";
  TString parFile = "par.root";

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
  output->open(parFile.Data());
  rtdb->setFirstInput(output);
  rtdb->setOutput(output);

  FairParAsciiFileIo* parInput = new FairParAsciiFileIo();
  parInput->open(digiparFile.Data(),"in");
  rtdb->setSecondInput(parInput);
  
  
  fRun->SetGeomFile(geomFile); // set filname
  fRun->LoadGeometry(); // set the flag
  
  // -----  Converter  -----------------------------------------------------
  
  PndMvdStripClusterTask* recotask = new PndMvdStripClusterTask();
  recotask->SetVerbose(iVerbose);
  fRun->AddTask(recotask);
  
  fRun->Init();
  
  cout<<" ---- Start RUN ----"<<endl;
  fRun->Run(0,nEvents);
  
  rtdb->print();
  rtdb->saveOutput();

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

