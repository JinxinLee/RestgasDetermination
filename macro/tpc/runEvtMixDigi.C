void 
runEvtMixDigi( TString inFile, 
	       TString jobDir,
	       TString jobname,
	       Int_t startEvent, // event number where we start processing
	       Int_t nEvents,
	       TString bkgFile, Int_t nBkgEvts=1000){
   // ----  Load libraries   -------------------------------------------------
   // ------------------------------------------------------------------------


  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (Signals events)
  //TString inFile="/afs/e18/panda/DATA/fboehmer/dipl_data/SpaceCharge/07_01_2009/new_PndTpcDetector/GEANT3_ALICE_L5_1MeV_cuts_withPIPE_MVD/sigSlice3/2Gev_G3_ALICE_L5_1MeV_cuts_with_PIPE_MVD_10k_evts.sig.root";

  //  TString inFile="/afs/e18/panda/DATA/fboehmer/dipl_data/SpaceCharge/07_01_2009/new_PndTpcDetector/GEANT3_ALICE_L5_1MeV_cuts_withPIPE_MVD/sigSlice1/2Gev_G3_ALICE_L5_1MeV_cuts_with_PIPE_MVD_10k_evts.sig.root";

  //TString inFile = "TEST/physics.16s.raw.root";
  //TString inFile = "TEST/physics.skim.DD.raw.root";
  //TString jobname="SkimEvtmix1000DD";
  //TString bkgFile = "TEST/DPM5k.DD.16s.raw.root";
  //TString bkgFile = "TEST/DPM.DD.raw.root";
  //unsigned int nBkgEvts=1000;
  TString parfile="/nfs/nas/data/panda/tpc/SIM/evtmix/all.par";
  double EvtRate=1.E4*nBkgEvts; // simulate 100mus = 1.E4s^-1

  TString inDir=inFile(0,inFile.Last('/')+1);

  jobDir+=jobname; jobDir+="/";
  TString cmd="mkdir ";
  cmd+=jobDir; 
  gSystem->Exec(cmd);
  // if(gSystem->Exec(cmd)){
  //   std::cout<<"Could not create Job-Directory "<<jobDir
  // 	     <<". Aborting."<<std::endl;
  //   return;
  // }
  
  TString outFile = inFile; 
  outFile.ReplaceAll(inDir,jobDir);
  outFile.ReplaceAll(".raw.root",".mixed.root");
  TString search=outFile;
  if(gSystem->FindFile(jobDir.Data(), search)!=NULL){
     std::cout<<"OutFile "<<outFile
 	     <<" already existing. Aborting."<<std::endl;
     return;
  } 

  TString paramIn = inFile;
  paramIn.ReplaceAll(".raw.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".mixed.root",".param.root");



std::cout<<"Input: "<<inFile<<std::endl;
std::cout<<"Output: "<<outFile<<std::endl;
std::cout<<"Bkg: "<<bkgFile<<std::endl;
std::cout<<"ParamIn: "<<paramIn<<std::endl;
std::cout<<"ParamOut: "<<paramOut<<std::endl;



  


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
  //FairParRootFileIo* parInput1 = new FairParRootFileIo();
  //parInput1->open(paramIn.Data());
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString tpcDigiFile = parfile;
  parInput2->open(tpcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput2);
  //rtdb->setSecondInput(parInput1);

  FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
  parOutput1->open(paramOut.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();
  
  //fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  
//  GenfitTask *Genfit= new GenfitTask();
//  fRun->AddTask(Genfit);

// ------ QAplots
  //QAPlotCollection* qa=new QAPlotCollection("TpcEvtMixQAPlots");


// ----- PndTpc Event Mixer: Mixes background tracks to events -------------

  PndTpcEvtMixTask* evtmixer = new PndTpcEvtMixTask();
  evtmixer->SetBkgFileName(bkgFile);
  evtmixer->SetNBkgEvts(nBkgEvts);
  evtmixer->SetEvtRate(EvtRate);
  evtmixer->SetT0(-evtmixer->MeanEvtSpacing()*0.5*nBkgEvts);
  evtmixer->SetInBranchName("PndTpcDigi");
  evtmixer->SetBkgBranchName("PndTpcDigi");
  evtmixer->SetPersistence(); // write out time info

  // unsigned int sectors[40]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79};

  //unsigned int offset=0;
  //for(unsigned int is=0;is<5;++is){
  //  evtmixer->AddSector(sectors[offset+is]);
  // }

  fRun->AddTask(evtmixer);

  
  //PndTpcElectronicsTask* tpcElec = new PndTpcElectronicsTask();
  //tpcElec->SetPersistence();
  //tpcElec->SetQAPlotCol(qa);
  //tpcElec->SetSamplePersistence();
  // fRun->AddTask(tpcElec);

  //PndTpcDigiAnalysisTask* tpcDigiAna = new PndTpcDigiAnalysisTask();
  //fRun->AddTask(tpcDigiAna);

  
PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
 tpcCF->SetDigiBranchName("PndTpcDigiMixed");
 //tpcCF->SetMode(2); // individual timeslice
  tpcCF->SetPersistence();
  //tpcCF->SetDigiPersistence();
  tpcCF->SetClusterAmpCut(0);
  tpcCF->timeslice(9); //  sample times 
  tpcCF->SetSimpleClustering();
  //tpcCF->SetClusterTimeCut(15);
  tpcCF->SetErrorPars(600,400);

  //tpcCF->SetSingleDigiClusterAmpCut(10);
  fRun->AddTask(tpcCF);



  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  rtdb->print();

  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  //FairRootManager::Instance()->GetOutFile()->mkdir("QAPlots");
  //FairRootManager::Instance()->GetOutFile()->cd("QAPlots");
  //qa->Write();


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
