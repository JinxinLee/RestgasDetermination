// root macro to analyze the clusterization output
{
  bool verbose = true;

  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  //gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  //  LoadPandaStyle();
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Helper.C");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  //TString HitFileName = "strahl_";
  TString HitFileName = "data100.root";
  TString CalibFileName = "ExampleBox.calib";
  TString MapFileName = "desyBox.mapping";

  TString directory = gSystem->Getenv("VMCWORKDIR");
  TString geomFile = directory + "/geometry/TrackingStationDesyAli.root";
  TString digiparFile = directory + "/macro/params/all.par";
  TString parFile = "par.root";
  
  TString outFile = HitFileName + ".digis.root";

  FairRunAna *fRun= new FairRunAna();
  //fRun->SetInputFile("mapout.root");
  fRun->SetOutputFile(outFile);
  
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParAsciiFileIo* parInput = new FairParAsciiFileIo();
  parInput->open(digiparFile.Data(),"in");
  rtdb->setFirstInput(parInput);
  
  FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
  output->open(parFile.Data());
  rtdb->setOutput(output);
  
  fRun->SetGeomFile(geomFile); // set filname
    
  // -----  Converter  -----------------------------------------------------
  PndMvdConvertApv* ApvConverter= new PndMvdConvertApv(CalibFileName, HitFileName);  
  PndMvdBoxMap* ApvMapper = new PndMvdBoxMap(MapFileName.Data());
  
  PndMvdConvertApvTask* convertTask = new PndMvdConvertApvTask(ApvConverter,ApvMapper);


    //convertTask->SetVerbose(3);
  fRun->AddTask(convertTask);
  
  fRun->Init();
  
  long int  nEvents = ApvConverter->GetNofEvents();
  if(nEvents>100) nEvents=100;
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

