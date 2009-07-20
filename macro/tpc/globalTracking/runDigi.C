void runDigi(TString base="mc", int nEvents = 0 )
{


  // ------------------------------------------------------------------------
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // ----------------------------------------------------------------------
  // Set of input file directory
  
  // Input file (MC events)
  TString inFile = base+".mc.root";
    //TString inFile1 = base+".mc_1.root";
    //TString inFile2 = base+".mc_2.root";

  //Parameter file
  TString parFile = base+".param.root";
  // Output file
  TString outFile = base+".digi.root";



  


  // ----  Load libraries   -------------------------------------------------
 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon(); 


  
  // -----   Reco run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  //fRun->AddFile(inFile1);
  //fRun->AddFile(inFile2);
  fRun->SetOutputFile(outFile);


  // Digitisation file (ascii) for EMC and all
  TString digiFile = "all.par";  
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;
  
  // ------------------------------------------------------------------------
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  fRun->LoadGeometry();
 
 
 
  // ------------------------------------------------------------------------
  // -----  Digitization part   --------------------------------------------

/*  // -----   STT analysis tasks   --------------------------------------------
  //PndSttHitProducerIdeal* sttHitProducer = new PndSttHitProducerIdeal();
  PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
  fRun->AddTask(sttHitProducer);


  // trackfinding ....
  PndSttTrackFinderIdeal* sttTrackFinder = new PndSttTrackFinderIdeal(iVerbose);
  PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
  sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttFindTracks);

  // trackmatching ....
  PndSttMatchTracks* sttTrackMatcher = new PndSttMatchTracks("Match tracks", "STT", iVerbose);
  sttTrackMatcher->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttTrackMatcher);

  // trackfitting ....
  PndSttTrackFitter* sttTrackFitter = new PndSttHelixTrackFitter(0);
  PndSttFitTracks* sttFitTracks = new PndSttFitTracks("STT Track Fitter", "FairTask", sttTrackFitter);
  sttFitTracks->AddHitCollectionName("STTHit");
  fRun->AddTask(sttFitTracks);

  // helix hit production ....
  PndSttHelixHitProducer* sttHHProducer = new PndSttHelixHitProducer();
  fRun->AddTask(sttHHProducer);
*/

  
  // -----   TPC digi producers   ---------------------------------
  PndTpcClusterizerTask* tpcClusterizer = new PndTpcClusterizerTask();
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

  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  tpcCF->SetPersistence();
  tpcCF->timeslice(20); // = 4 sample times = 100ns @ 40MHz
  fRun->AddTask(tpcCF);







  // -----   MDV digi producers   --------------------------------- 
  PndMvdStripHitProducer* mvdHitProd = new PndMvdStripHitProducer();
  mvdHitProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdHitProd);

  PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer();
  mvdPixProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdPixProd);
 
  // CLUST
  // Cluster finding for strip detectors 
  //This is obtained from macro/run
  //Double_t chargecut = 5000., pixelrad=1.8;
  //PndMvdClusterTask* mvdmccls = new PndMvdClusterTask(pixelrad,chargecut,"sim_complete.root");
  //fRun->AddTask(mvdmccls); 
  
  Double_t chargecut = 5000.;
  PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask(chargecut, inFile);
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);
  
  // Cluster finder for pixel detectors
  PndMvdPixelClusterTask* mvdClusterizer = new PndMvdPixelClusterTask(1.8, inFile);
  mvdClusterizer->SetVerbose(iVerbose);
  fRun->AddTask(mvdClusterizer);

  PndMvdIdealRecoTask* mvdirec = new PndMvdIdealRecoTask(0.01,0.01,0.005);
  mvdirec->SetVerbose(iVerbose);
  fRun->AddTask(mvdirec);

  // ------------------------------------------------- 
  PndDchDigiProducer* digiProducer= new PndDchDigiProducer();
  digiProducer->SetVerbose(0);
  fRun->AddTask(digiProducer);
  // -------------------------------------------------   
  PndDchCylinderHitProducer* cylHitProducer= new PndDchCylinderHitProducer();
  cylHitProducer->SetVerbose(0);
  fRun->AddTask(cylHitProducer);
  



 
 
  // -----   GEM hit producers   ---------------------------------------
  PndGemIdealHitProducer* gemHitProducer = new PndGemIdealHitProducer("GEM Ideal hit producer", 0);
  fRun->AddTask(gemHitProducer);  
  //PndGemDigitize* gemDigitize = new PndGemDigitize("GEM Digitizer", 0);
  //fRun->AddTask(gemDigitize);
  //PndGemFindHits* gemFindHits = new PndGemFindHits("GEM Hit Finder", 0);
  //fRun->AddTask(gemFindHits);

  // ------------------------------------------------- 
  // -----   Intialise and run   --------------------------------------------
  //fRun->SetStoreTraj(kFALSE);  
  fRun->Init();  
  //Geane->SetField(fRun->GetField());
  fRun->Run(0,nEvents);

  gROOT->FindObject("FAIRGeom")->Write();

  rtdb->saveOutput();
  rtdb->print();

  //DemoKalman->WriteHistograms();
  //DemoTools->WriteHistograms();


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
