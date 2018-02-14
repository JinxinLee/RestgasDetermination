{
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  Int_t nStart = 400;
  Int_t nEvents  = 1000;
//   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  // Input file (MC events)
  //TString inFile = "Mvd_DPMfixed_4GeV_10000.root"; //"MvdG4_DPM405_Mag_5000.root";
  //TString inFile = "data/mvdparams.root";
  TString inFile = "MvdTpc_D+D-_10G_1000.root";
  // Parameter file
  TString parFile = "MvdParams.root";
  //TString parFile = "data/mvdparams.root";
  // Parameter output file
//   TString parOutFile = "Test/testParamsOutput.root";
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  TString MvdDigiparFile = sysFile;
  MvdDigiparFile += "/macro/params/all.par";



  // In general, the following parts need not be touched
  // ========================================================================
  // Output file
  PndFileNameCreator creator(inFile.Data());
  TString outFile = creator.GetDigiFileName().c_str(); //"MvdG4_DPM405_Mag_5000_digi.root";
  std::cout << "DigiFileName: " << outFile.Data() << std::endl;

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);
//   Bool_t kParameterMerged=kTRUE;
//   FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
//   output->open(parOutFile);
//   rtdb->setOutput(output);
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput2->open(MvdDigiparFile.Data(),"in");
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

  PndMvdNoiseProducer* mvdNoiseMaker = new PndMvdNoiseProducer();
  mvdNoiseMaker->SetVerbose(iVerbose);
  fRun->AddTask(mvdNoiseMaker);

  // -----    MVD hit producer   --------------------------------------------

 Double_t chargecut = 1.e5;
   PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask();
   mvdmccls->SetVerbose(iVerbose);
   fRun->AddTask(mvdmccls);

   PndMvdPixelClusterTask* mvdClusterizer = new PndMvdPixelClusterTask();
   mvdClusterizer->SetVerbose(iVerbose);
   fRun->AddTask(mvdClusterizer);

  // -----   STT analysis tasks   --------------------------------------------
    // digitize ....
//  PndSttHitProducerIdeal* sttHitProducer = new PndSttHitProducerIdeal();
//  PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
//  fRun->AddTask(sttHitProducer);

  // ------  TPC digi tasks ---------------------------
/*  PndTpcClusterizerTask* tpcClusterizer = new PndTpcClusterizerTask();
   //tpcClusterizer->SetPersistence();
   fRun->AddTask(tpcClusterizer);

   PndTpcDriftTask* tpcDrifter = new PndTpcDriftTask();
   // tpcDrifter->SetPersistence();
   tpcDrifter->SetDistort(false);
   fRun->AddTask(tpcDrifter);

   PndTpcGemTask* tpcGem = new PndTpcGemTask();
   //tpcGem->SetPersistence();
   fRun->AddTask(tpcGem);

   PndTpcPadResponseTask* tpcPadResponse = new PndTpcPadResponseTask();
   tpcPadResponse->SetPersistence();
   fRun->AddTask(tpcPadResponse);

   PndTpcElectronicsTask* tpcElec = new PndTpcElectronicsTask();
   tpcElec->SetPersistence();
   fRun->AddTask(tpcElec);
*/
    // -----   EMC hit producers   ---------------------------------
/*    PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
    fRun->AddTask(emcHitProd); // hit production

    //PndEmcMakeDigi* emcMakeDigi=new PndEmcMakeDigi();
    //fRun->AddTask(emcMakeDigi); // fast digitization

    PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(iVerbose);
    PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(iVerbose);
    fRun->AddTask(emcHitsToWaveform);  // full digitization
    fRun->AddTask(emcWaveformToDigi);  // full digitization

    PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
    fRun->AddTask(emcMakeCluster);

    PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
    fRun->AddTask(emcMakeBump);

    PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
    fRun->AddTask(emcHdrFiller); // ECM header
*/
//   FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
//   output->open(parOutFile.Data());
//   rtdb->setOutput(output);
  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
  //PndMvdGeoPar* geoPar  = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(nStart,nEvents);

  rtdb->saveOutput();
  rtdb->print();

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s\a\a" << endl;
  cout << endl;

}
