void 
runEvtMixDigi(){
   // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libtpc");
  gSystem->Load("libgenfit");
  // ------------------------------------------------------------------------


  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (Signals events)
  TString inFile="/afs/e18/panda/DATA/fboehmer/dipl_data/SpaceCharge/07_01_2009/new_PndTpcDetector/GEANT3_ALICE_L5_1MeV_cuts_withPIPE_MVD/sigSlice1/2Gev_G3_ALICE_L5_1MeV_cuts_with_PIPE_MVD_10k_evts.sig.root";
  TString jobname="evtmix3";
  TString inDir=inFile(0,inFile.Last('/')+1);

  TString jobDir=inDir; jobDir+=jobname; jobDir+="/";
  TString cmd="mkdir ";
  cmd+=jobDir; 
  if(gSystem->Exec(cmd)){
    std::cout<<"Could not create Job-Directory "<<jobDir
	     <<". Aborting."<<std::endl;
    return;
  }
  
  TString outFile = inFile; 
  outFile.ReplaceAll(inDir,jobDir);
  outFile.ReplaceAll(".sig.root",".raw.root");

  TString paramIn = inFile;
  paramIn.ReplaceAll(".raw.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".raw.root",".param.root");

  TString bkgFile = inFile;

std::cout<<"Input: "<<inFile<<std::endl;
std::cout<<"Output: "<<outFile<<std::endl;
std::cout<<"Bkg: "<<bkgFile<<std::endl;
std::cout<<"ParamIn: "<<paramIn<<std::endl;
std::cout<<"ParamOut: "<<paramOut<<std::endl;



  // Number of events to process
  Int_t nEvents = 1;


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
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(paramIn.Data());
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString tpcDigiFile = gSystem->Getenv("VMCWORKDIR");
  tpcDigiFile += "/tpc/tpc.fullplane.par";
  parInput2->open(tpcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput2);
  rtdb->setSecondInput(parInput1);

  FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
  parOutput1->open(paramOut.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();
  
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  
//  GenfitTask *Genfit= new GenfitTask();
//  fRun->AddTask(Genfit);

// ------ QAplots
  QAPlotCollection* qa=new QAPlotCollection("TpcEvtMixQAPlots");


// ----- PndTpc Event Mixer: Mixes background tracks to events -------------

  PndTpcEvtMixTask* evtmixer = new PndTpcEvtMixTask();
  evtmixer->SetBkgFileName(bkgFile);
  evtmixer->SetNBkgEvts(2000);
  evtmixer->SetEvtRate(1E7);
  evtmixer->SetPersistence(); // write out time info
  fRun->AddTask(evtmixer);

  
  PndTpcElectronicsTask* tpcElec = new PndTpcElectronicsTask();
  tpcElec->SetPersistence();
  tpcElec->SetQAPlotCol(qa);
  //tpcElec->SetSamplePersistence();
  fRun->AddTask(tpcElec);

  //PndTpcDigiAnalysisTask* tpcDigiAna = new PndTpcDigiAnalysisTask();
  //fRun->AddTask(tpcDigiAna);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  rtdb->print();

  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
FairRootManager::Instance()->GetOutFile()->mkdir("QAPlots");
FairRootManager::Instance()->GetOutFile()->cd("QAPlots");
qa->Write();


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
