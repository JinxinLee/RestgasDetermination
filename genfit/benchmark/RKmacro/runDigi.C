void runDigi(TString base="mc", int nEvents = 0 )
{


  // ------------------------------------------------------------------------
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // ----------------------------------------------------------------------
  // Set of input file directory
  
  // Input file (MC events)
  TString inFile = base+".mc.root";
    //TString inFile1 = base+".mc_1.root";
    //TString inFile2 = base+".mc_2.root";

  //Parameter file
  TString parFile = base+".param.root";
  // Output file
  TString outFile = base+".digi.root";



  


  // ----  Load libraries   -------------------------------------------------
 
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libbenchmark");

  
  // -----   Reco run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  //fRun->AddFile(inFile1);
  //fRun->AddFile(inFile2);
  fRun->SetOutputFile(outFile);


  // Digitisation file (ascii) for EMC and all
  TString digiFile = "all.par";  
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;
  
  // ------------------------------------------------------------------------
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  fRun->LoadGeometry();
 
 
 
  // ------------------------------------------------------------------------
  // -----  Digitization part   --------------------------------------------

  // -----   GEM hit producers   ---------------------------------------
  RKtrackingTask* myTask = new RKtrackingTask("Runge-Kutta tracking", 0);
  fRun->AddTask(myTask);  

  // -----   Intialise and run   --------------------------------------------
  //fRun->SetStoreTraj(kFALSE);  
  fRun->Init();  

  //double x[3] = {10.,10.,10.};
  //double f[3];
  //fRun->GetField()->GetFieldValue(x,f);
  //std::cout << "######### " << f[2] << std::endl;
  myTask->setField(new PndFieldAdaptor(fRun->GetField()));

  myTask->setPDG(2212);

  fRun->Run(0,nEvents);

  gROOT->FindObject("FAIRGeom")->Write();

  myTask->writeFile();

  rtdb->saveOutput();
  rtdb->print();

  //DemoKalman->WriteHistograms();
  //DemoTools->WriteHistograms();
  

  // ------------------------------------------------------------------------
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  
  
  
}
