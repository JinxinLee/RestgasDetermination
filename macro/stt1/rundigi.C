{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "testrun.root";
  //  TString inFile = "auaumbias.root";

  // Number of events to process
  Int_t nEvents = 50;

  // Parameter file
  //  TString parFile = "parfiles/params.root";
  TString parFile = "parfiles/testparams.root";

  // Output file
  TString outFile = "testdigi.root";
  //  TString outFile = "auaudigi.root";


  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
   gSystem->Load("libGeom");
   gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libMCStack");
  gSystem->Load("libPassive");  //gSystem->Load("libKF");

  // ------------------------------------------------------------------------

  gSystem->Load("libGen");
  gSystem->Load("libStt1");
  // ------------------------------------------------------------------------


 
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
 //  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
//   CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
//   parInput1->open(parFile.Data());
//   rtdb->setFirstInput(parInput1);
  //fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  // -----   STT hit producers   --------------------------------------------
  CbmSttDigiProducer* digiProd = new CbmSttDigiProducer();
  fRun->AddTask(digiProd);
  // ------------------------------------------------------------------------


  // -----   Intialise and run   --------------------------------------------
  cout << "fRun->Init()" << endl;
  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
