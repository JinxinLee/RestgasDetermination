{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Number of events to process
  Int_t nEvents = 500; // process all events in input file
  // Base File name
  TString base="../data/pions/mcevtmix2/test1";

  TString mcFile="../data/pions/test1.mc.root";

  // Input file (MC events)
  TString inFile = base+".mix.root";
  //Parameter file
  TString parFile = base+".param.root";
  // Output file
  TString outFile = base+".deconv2.root";

  

// ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libgeant321");
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
  gSystem->Load("libPGen");

  gSystem->Load("libTrkBase");
  gSystem->Load("libGeane");
  gSystem->Load("libgenfit");
  gSystem->Load("libtrackrep");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");
gSystem->Load("libMvd");
gSystem->Load("libMvdReco");



gSystem->ListLibraries();



// ------------------------------------------------------------------------

// -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reco run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  CbmGeane *Geane = new CbmGeane(mcFile);

  // ------------------------------------------------------------------------
  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  rtdb->Print();
  
  // Set the field(if any) to Geane
  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0, 0 ,20. ); // values are in kG
  fMagField->SetFieldRegion(-150, 150,-150, 150, -2000, 2000);
  fRun->SetField(fMagField);

  CbmField* field=fRun->GetField();
  
    Geane->SetField(fMagField);
       
  // ------------------------------------------------------------------------
  // -----  Reco Sequence   --------------------------------------------

  TpcMCEvtDeconvTask* deconv=new TpcMCEvtDeconvTask();
  deconv->SetTWindow(500);
  deconv->SetPersistence();
  fRun->AddTask(deconv);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  Geane->SetField(fMagField);

//rtdb->print();
//Geane->SetField(fRun->GetField());

  fRun->Run(0,nEvents);

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
