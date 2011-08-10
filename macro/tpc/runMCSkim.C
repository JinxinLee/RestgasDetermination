{
  // ----  Load libraries   -------------------------------------------------
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  // ------------------------------------------------------------------------
  //SET NUMBER OF EVENTS
  // ------------------------------------------------------------------------

  Int_t nEvents=30;
  TString basedir = gSystem->Getenv("VMCWORKDIR");
  
  // Set INPUT DIRECTORY (MC files) and JOBNAME
  // ------------------------------------------------------------------------
  TString inDir="/nfs/nas/data/panda/tpc/SIM/evtmix/";
  TString jobname="phys_JPsiPiPi";

  //inDir=(basedir+"/")+inDir;
  TString inFile=(inDir+"/")+jobname;
  inFile+=".mc.root";
 
  TString outFile = inFile;
  outFile.ReplaceAll(".mc.root", ".skimtest.mc.root");
  TString paramIn = inFile;
  paramIn.ReplaceAll(".mc.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".mc.root",".param.root");
  

  std::cout<<"Input: "<<inFile<<std::endl;
  std::cout<<"Output: "<<outFile<<std::endl;
  std::cout<<"ParamIn: "<<paramIn<<std::endl;
  std::cout<<"ParamOut: "<<paramOut<<std::endl;
 


  // In general, the following parts need not be touched
  // ========================================================================


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(paramIn.Data());
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString tpcDigiFile = gSystem->Getenv("VMCWORKDIR");
  tpcDigiFile += "/macro/params/all.par";
  parInput2->open(tpcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput2);
  rtdb->setSecondInput(parInput1);

  PndTpcDigiPar* par = (PndTpcDigiPar*) rtdb->getContainer("PndTpcDigiPar");
  par->setInputVersion(fRun->GetRunId(),1);
  par->setChanged(kTRUE);

  FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
  parOutput1->open(paramOut.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();

  //fRun->LoadGeometry();
  // ------------------------------------------------------------------------
     
  PndTpcMCSkimmingTask* skimmer=new PndTpcMCSkimmingTask();
  skimmer->SetMinTpcPoints(200);
  fRun->AddTask(skimmer);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  rtdb->print();

  fRun->Run(0,nEvents); // process all events from input file
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------

  
  rtdb->saveOutput();
  rtdb->print();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << paramOut << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

}
