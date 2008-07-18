//Macro designed to process the hits of the infile and calculate the Space-Charge-Map

{
    
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (MC events)
  TString inFile = "spacecharge_in1.5Gev.el_and_inel.GEANT3_STRA0.root";

  // Number of events to process
  Int_t nEvents = 5000;


  // Parameter file
  TString parFile = "eumel.param.root";

  // Output file
  TString outFile = "spacecharge_out.root";	//not needed

  
  
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
 
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");

  gSystem->Load("libMvd");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libGen");
  gSystem->Load("libgenfit");
  gSystem->Load("libtpc");
  gSystem->Load("librecotasks");

  gSystem->Load("libDpmEvtGen");
  gSystem->Load("libPGen");
  // -----------------------------------------------------------------------
  
  
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Digitization run   -------------------------------------------
  CbmRunAna* fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



 //-----  Parameter database   --------------------------------------------
// CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
// CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
// parInput1->open(parFile.Data());
// //CbmParAsciiFileIo* parInput2 = new CbmParAsciiFileIo();
// //TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
// //stsDigiFile += "/parameters/sts/sts_digi_new_standard.par";
// //parInput2->open(stsDigiFile.Data(),"in");
//  rtdb->setFirstInput(parInput1);
//  //rtdb->setSecondInput(parInput2);
//  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  // -----    Digi Sequence  --------------------------------------------

  PndTpcSpaceChargeTask* tpcSpaceCharge = new PndTpcSpaceChargeTask();
  tpcSpaceCharge->setGain(4000);
  fRun->AddTask(tpcSpaceCharge);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  tpcSpaceCharge->writeToFile("SCTemplate_1.5Gev.el_and_inel.gain4k.GEANT3_STRA0.dat");
  delete tpcSpaceCharge;
  

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  //  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
