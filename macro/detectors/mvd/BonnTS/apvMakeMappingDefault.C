// root macro to analyze the clusterization output
{
  bool verbose = true;
  
  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
//  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
//  LoadPandaStyle();
//  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Helper.C");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  TString directory = gSystem->Getenv("VMCWORKDIR");
  TString outFile = "daq.mapping";
  TString geomFile = directory + "/geometry/TrackingStation.root";
  TString digiparFile = directory + "/macro/params/all.par";
  TString parFile = "par.root";
  FairRunAna *fRun= new FairRunAna();
  //fRun->SetInputFile(inFile);
  fRun->SetOutputFile("mapout.root");

    
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParAsciiFileIo* parInput = new FairParAsciiFileIo();
  parInput->open(digiparFile.Data(),"in");
  rtdb->setFirstInput(parInput);
  
  FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
  output->open(parFile.Data());
  rtdb->setOutput(output);
    
  fRun->SetGeomFile(geomFile); // set filname
  fRun->LoadGeometry(); // set the flag

  PndMvdCreateDefaultApvMap* creator = new PndMvdCreateDefaultApvMap();
  fRun->AddTask(creator);

  fRun->Init();


  // now we have a complete run object
  if(! creator->CreateFile(outFile) ) return;

  //fRun->Run(0,nEvents);
  
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
  
