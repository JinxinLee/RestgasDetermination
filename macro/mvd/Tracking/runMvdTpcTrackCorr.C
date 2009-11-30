{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (MC events)
//  TString MCFile = "Mvd_TestNewVersion.root";
  // Parameter file
//  TString parFile = "MvdParamsNewVersion.root";
  // Parameter output file
//  TString parOutFile = "MvdParamsNewVersion.root";
  // Number of events to process
  Int_t nEvents = 100;
  // ----  Load libraries   -------------------------------------------------
 //  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");  
  gSystem->Load("libriemann");

  // ------------------------------------------------------------------------

  TString inFile="$SIMPATH/pandaroot/macro/data/MvdTpc_D+D-_2Disks/digiMVD/recoMVD/riemannMVD/Combined.riemann.root";
  TString jobname="combinedMVDTPC";

  TString mcFile="$SIMPATH/pandaroot/macro/data/MvdTpc_D+D-_2Disks/Combined.mc.root";
  TString recoMVDFile = "$SIMPATH/pandaroot/macro/data/MvdTpc_D+D-_2Disks/digiMVD/recoMVD/Combined.reco.root";
  TString recoTPCFile = "$SIMPATH/pandaroot/macro/data/MvdTpc_D+D-_2Disks/digiTPC/recoTPC/Combined.reco.root";
    
  
  TString SIMPATH = gSystem->Getenv("SIMPATH");
  
  inFile.ReplaceAll("$SIMPATH",SIMPATH);
  recoMVDFile.ReplaceAll("$SIMPATH",SIMPATH);
  recoTPCFile.ReplaceAll("$SIMPATH",SIMPATH);

  TString inDir=inFile(0,inFile.Last('/')+1);
  // make new subdir
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
  outFile.ReplaceAll(".riemann.root",".combined.root");

  TString paramIn = inFile;
  paramIn.ReplaceAll(".riemann.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".combined.root",".param.root");

/*
  TString mcDir = inDir;
  mcDir=mcDir(0,mcDir.Last('/')); // remove last /
  mcDir=mcDir(0,mcDir.Last('/')+1); // one directory up
  TString mcFile= inFile;
  mcFile.ReplaceAll(inDir,mcDir);
  mcFile.ReplaceAll(".raw.root",".mc.root");
  */
  
  std::cout<<"Input: "<<inFile<<std::endl;
  std::cout<<"Output: "<<outFile<<std::endl;
  std::cout<<"RecoMVDFile: "<<recoMVDFile<<std::endl;
  std::cout<<"RecoTPCFile: "<<recoTPCFile<<std::endl;
  std::cout<<"ParamIn: "<<paramIn<<std::endl;
  std::cout<<"ParamOut: "<<paramOut<<std::endl;

  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

  fRun->SetInputFile(inFile);
  fRun->AddFriend(recoMVDFile);
  fRun->AddFriend(recoTPCFile);
  
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(paramIn.Data(),"UPDATE");
//   FairParAsciiFileIo* parInput1 = new FairParAsciiFileIo();
//   parInput1->open(parFile.Data(),"in");

  rtdb->setFirstInput(parInput1);


//  fRun->LoadGeometry();

  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  // -----    MVD hit producer   --------------------------------------------

 
  double MaxDist = 1;
  double MaxSZ = 1;
  double MaxSZChi2 = 1;
  
  PndMvdTpcRiemannCorrelatorTask* mvdtpccorr = new PndMvdTpcRiemannCorrelatorTask();
  mvdtpccorr->SetMaxDist(MaxDist);
  mvdtpccorr->SetMaxSZ(MaxSZ);
  mvdtpccorr->SetMaxSZChi2(MaxSZChi2);
  mvdtpccorr->SetVerbose(iVerbose);
  fRun->AddTask(mvdtpccorr);
  
   FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
   output->open(paramOut.Data());
   rtdb->setOutput(output);
//  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
//   PndMvdGeoPar* geoPar  = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar")); 
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------
  //TCanvas* myCan1 = new TCanvas();
  //mvdtpccorr->DrawDistHistos(myCan1, 0);
  
  //TCanvas* myCan2 = new TCanvas();
  //mvdtpccorr->DrawSZHistos(myCan2, 0);
  
 // TCanvas* myCan3 = new TCanvas();
 // mvdtpccorr->DrawSZChi2Histos(myCan3, 0);
// rtdb->saveOutput();
  // -----   Finish   -------------------------------------------------------
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
