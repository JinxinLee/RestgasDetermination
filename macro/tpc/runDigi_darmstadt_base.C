{
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libtpc");
  gSystem->Load("libgenfit");
  // ------------------------------------------------------------------------

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  Int_t nEvents=10000;
  
  // Input file (MC events)
  //TString inDir="/afs/e18/panda/DATA/MC_data_snapshot_darmstadt/150cm/";
  TString inDir="/lustre/e18/user/fboehmer/DATA/darmstadt_overhaul/150cm/";
  TString jobname="JOBNAME";

  TString inFile=inDir+jobname;
  inFile+=".mc.root";
  // make new subdir
  // TString jobDir=inDir; jobDir+=jobname; jobDir+="/";
//   TString cmd="mkdir ";
//   cmd+=jobDir; 
//   if(gSystem->Exec(cmd)){
//     std::cout<<"Could not create Job-Directory "<<jobDir
// 	     <<". Aborting."<<std::endl;
//     return;
//   }


  TString digiDir="digi/"; 
  TString outDir=inDir+digiDir;
  TString outFile = outDir+jobname; 
  outFile+=".raw.root";

  TString paramIn = inFile;
  paramIn.ReplaceAll(".mc.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".raw.root",".param.root");

// // copy this macro to the data directory
//   cmd="cp macro/tpc/runDigi.C ";
//   cmd+=jobDir; 
//  if(gSystem->Exec(cmd)){
//     std::cout<<"Could not copy runDigi.C to Job-Directory "<<jobDir
// 	     <<". Aborting."<<std::endl;
//     return;
//   }

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
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // ------ QAplots
  QAPlotCollection* qa=new QAPlotCollection("TpcDigiQAPlots");

  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo(kTRUE);
  parInput1->open(paramIn.Data());
  CbmParAsciiFileIo* parInput2 = new CbmParAsciiFileIo();
  TString tpcDigiFile = gSystem->Getenv("VMCWORKDIR");
  tpcDigiFile += "/tpc/tpc.par";
  parInput2->open(tpcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput2);
  rtdb->setSecondInput(parInput1);

  PndTpcDigiPar* par = (PndTpcDigiPar*) rtdb->getContainer("PndTpcDigiPar");
  par->setInputVersion(fRun->GetRunId(),1);
  par->setChanged(kTRUE);

  CbmParRootFileIo* parOutput1 = new CbmParRootFileIo(kTRUE);
  parOutput1->open(paramOut.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();

  fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  
//  GenfitTask *Genfit= new GenfitTask();
//  fRun->AddTask(Genfit);

// ----- PndTpc Event Mixer: Mixes background tracks to events -------------

    // -----    Digi Sequence  --------------------------------------------
  PndTpcClusterizerTask* tpcClusterizer = new PndTpcClusterizerTask();
  tpcClusterizer->SetPersistence();
  tpcClusterizer->SetMereChargeConversion();
  fRun->AddTask(tpcClusterizer);
  

  /**   use Alice Style MC    
   				make one hit per collision with atom
				use other straggling
		WARNING:	
   	    1. geant3 has to be used!
   	    2. LOSS = 5 has to be set!
	    3. DCUTE und DCUTM should be set to 10 keV.	
	    4. For Digitaization: PndTpcClusterizerTask
	    		tpcClusterizer->SetMereChargeConversion() has to be set!
	    5. if you do not use this option make sure 2., 4. are not set!
		:-(	
	    6. SetMaxNStep should be set to a high value
   */ 
 
  PndTpcDriftTask* tpcDrifter = new PndTpcDriftTask();
  tpcDrifter->SetPersistence();
  tpcDrifter->SetDistort(false);
  double deg=TMath::Pi()/180;
  //tpcDrifter->SetPhiCut(-15*deg,15*deg);
  tpcDrifter->SetQAPlotCol(qa);
  fRun->AddTask(tpcDrifter);

  PndTpcGemTask* tpcGem = new PndTpcGemTask();
//tpcGem->SetPersistence();
  fRun->AddTask(tpcGem);

  PndTpcPadResponseTask* tpcPadResponse = new PndTpcPadResponseTask();
  tpcPadResponse->SetPersistence();
  tpcPadResponse->SetQAPlotCol(qa);
  fRun->AddTask(tpcPadResponse);


//PndTpcEvtMixTask* evtmixer = new PndTpcEvtMixTask();
//  evtmixer->SetBkgFileName("bkg2.raw.root");
//  evtmixer->SetNBkgEvts(500);
//  evtmixer->SetEvtRate(1E7);
//fRun->AddTask(evtmixer);

  PndTpcElectronicsTask* tpcElec = new PndTpcElectronicsTask();
  tpcElec->SetPersistence();
  tpcElec->SetQAPlotCol(qa);
  fRun->AddTask(tpcElec);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  rtdb->print();

  fRun->Run(0,nEvents); // process all events from input file
  // ------------------------------------------------------------------------


//tpcDrifter->WriteHistograms();
//tpcPadResponse->WriteHistograms();
//tpcElec->WriteHistograms();

CbmRootManager::Instance()->GetOutFile()->mkdir("QAPlots");
CbmRootManager::Instance()->GetOutFile()->cd("QAPlots");
qa->Write();

  // -----   Finish   -------------------------------------------------------


//delete tpcSplitter;

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
