{
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  Int_t nEvents  = 100;

//   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  // Input file (MC events)
  //TString inFile = "Mvd_DPMfixed_4GeV_10000.root"; //"MvdG4_DPM405_Mag_5000.root";
  //TString inFile = "data/mvdparams.root";
//  TString inFile = "Mvd_D+D-withTPC.root";
  // Parameter file
//  TString parFile = "MvdParamsNewVersion.root";
  //TString parFile = "data/mvdparams.root";
  // Parameter output file

//   TString parOutFile = "Test/testParamsOutput.root";

  // Input file (MC events)
  TString inFile="/home/stockman/fairroot/cbmsoft/pandaroot/macro/data/MvdTpc_D+D-_2Disks/Combined.mc.root";
  TString jobname="digiMVD";

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
  outFile.ReplaceAll(".mc.root",".raw.root");

  TString paramIn = inFile;
  paramIn.ReplaceAll(".mc.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".raw.root",".param.root");
  
  TString digiparFile = gSystem->Getenv("VMCWORKDIR");
  digiparFile += "/mvd/MvdTools/mvd.digi.par";

  
  // In general, the following parts need not be touched
  // ========================================================================
  // Output file
  

//     
//     PndFileNameCreator creator(inFile.Data());
//       TString outFile = creator.GetDigiFileName().c_str(); //"MvdG4_DPM405_Mag_5000_digi.root";
//       std::cout << "DigiFileName: " << outFile.Data() << std::endl;
     
     // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
 // FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
 // parInput1->open(parFile.Data(),"UPDATE");
 // rtdb->setFirstInput(parInput1);
//  Bool_t kParameterMerged=kTRUE;

  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput2->open(digiparFile.Data(),"in");
  rtdb->setSecondInput(parInput2);

  fRun->LoadGeometry();


  // -----   Digitization   ---------------------------------------
//  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
//   mvddigi->SetVerbose(iVerbose);
//  fRun->AddTask(mvddigi);
  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  // -----    MVD Strip hit producer   ---------------------------------------

//   double   topPitch=0.015,
//            botPitch=0.015,
//            orient=TMath::Pi()*(0.5),
//            skew=TMath::Pi()*(0.5);
//   TVector2 topAnchor(0.,0.);
//   TVector2 botAnchor(0.,0.);
//   int      topFE = 10,
//            botFE = 4,
//            nrFEChannels = 128;
//   double   threshold=3000., noise=1000.;
//   PndMvdStripHitProducer* mvdStripProd
//     = new PndMvdStripHitProducer(topPitch, botPitch,
//                               orient, skew,
//                               topAnchor,botAnchor,
//                               topFE, botFE, nrFEChannels,
//                               threshold, noise);

  PndMvdStripHitProducer* mvdStripProd = new PndMvdStripHitProducer();
  mvdStripProd->SetVerbose(iVerbose);



  fRun->AddTask(mvdStripProd);
  // -----    MVD Pixel hit producer   ---------------------------------------
//   Double_t  lx=0.01, ly=0.01, threshold=600, noise=200;
//   PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer(lx,ly,threshold,noise);
  PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer();
  mvdPixProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdPixProd);



   FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
   output->open(paramOut.Data());
   rtdb->setOutput(output);
//  rtdb->setOutput(parInput1);

//  PndMvdNoiseProducer* mvdNoiseMaker = new PndMvdNoiseProducer();
//  mvdNoiseMaker->SetVerbose(iVerbose);
//  fRun->AddTask(mvdNoiseMaker);
   
 //  PndTpcClusterizerTask* tpcClusterizer = new PndTpcClusterizerTask();
    //tpcClusterizer->SetPersistence();
 //   fRun->AddTask(tpcClusterizer);
   
 /*   PndTpcDriftTask* tpcDrifter = new PndTpcDriftTask();
    tpcDrifter->SetPersistence();
    tpcDrifter->SetDistort(false);
    double deg=TMath::Pi()/180;
    //tpcDrifter->SetPhiCut(-15*deg,15*deg);
    //tpcDrifter->SetQAPlotCol(qa);
    fRun->AddTask(tpcDrifter);

    PndTpcGemTask* tpcGem = new PndTpcGemTask();
  //tpcGem->SetPersistence();
    fRun->AddTask(tpcGem);

    PndTpcPadResponseTask* tpcPadResponse = new PndTpcPadResponseTask();
    tpcPadResponse->SetPersistence();
    //tpcPadResponse->SetQAPlotCol(qa);
    fRun->AddTask(tpcPadResponse);


  //PndTpcEvtMixTask* evtmixer = new PndTpcEvtMixTask();
  //  evtmixer->SetBkgFileName("bkg2.raw.root");
  //  evtmixer->SetNBkgEvts(500);
  //  evtmixer->SetEvtRate(1E7);
  //fRun->AddTask(evtmixer);

    PndTpcElectronicsTask* tpcElec = new PndTpcElectronicsTask();
    tpcElec->SetPersistence();
    //tpcElec->SetQAPlotCol(qa);
    fRun->AddTask(tpcElec);
*/
//   FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
//   output->open(parOutFile.Data());
//   rtdb->setOutput(output);

  rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
  PndMvdGeoPar* geoPar  = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar")); 
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);

  rtdb->saveOutput();
  rtdb->print();

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << paramOut << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s\a\a" << endl;
  cout << endl;

}
