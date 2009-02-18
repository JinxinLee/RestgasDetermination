{
// ========================================================================
// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
Int_t iVerbose = 0;

// Input file (MC events)
TString inFile = "points_tpcmvdemc.root";

// Parameter file
TString parFile = "testparams.root";

// Output file
TString outFile = "tracks_tpcmvdemc.root";

// Number of events to process
Int_t nEvents = 0;
 
// ----  Load libraries   -------------------------------------------------
gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
basiclibs();
gSystem->Load("libGeoBase");
gSystem->Load("libParBase");
gSystem->Load("libBase");
gSystem->Load("libPndData");
gSystem->Load("libField");
gSystem->Load("libPassive");
gSystem->Load("libGen");  
gSystem->Load("libEmc"); 
gSystem->Load("libgenfit");
gSystem->Load("libtpc"); 
gSystem->Load("libtpcreco");
gSystem->Load("libtrackrep");
gSystem->Load("librecotasks");
gSystem->Load("libMvd");
gSystem->Load("libMvdReco");
gSystem->Load("libLHETrack");
// ------------------------------------------------------------------------

// ---  Now choose concrete engines for the different tasks   -------------
// ------------------------------------------------------------------------

// In general, the following parts need not be touched
// ========================================================================

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
TString allDigiFile = "./all.par";

FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
FairParRootFileIo* parInput1 = new FairParRootFileIo();
parInput1->open(parFile.Data());
	
FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
parIo1->open(allDigiFile.Data(),"in");
        
rtdb->setFirstInput(parInput1);
rtdb->setSecondInput(parIo1);
fRun->LoadGeometry();
// ------------------------------------------------------------------------

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

//PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
//tpcSPR->SetTrkFinderParameters(2.,// proxcut
//	 		       0.02, // proxcut on rieman sphere
//		 	       2.E-3, // planecut
//			       4.0, // szcut
//			       4); // minnumhits for fit
//tpcSPR->SetPersistence();
//  fRun->AddTask(tpcSPR);

// -----   MDV digi producers   --------------------------------- 
// DIGI
double   topPitch=0.015,//cm
  botPitch=0.015,//cm
  orient=TMath::Pi()*(0.5),
  skew=TMath::Pi()*(0.5);
TVector2 topAnchor(0.,0.);
TVector2 botAnchor(0.,0.);
int      topFE = 10,
  botFE = 4,
  nrFEChannels = 128;
double   threshold=3000., noise=1000.;

PndMvdStripHitProducer* mvdHitProd
= new PndMvdStripHitProducer(topPitch, botPitch,
			     orient, skew,
			     topAnchor,botAnchor,
			     topFE, botFE, nrFEChannels,
			     threshold, noise);
mvdHitProd->SetVerbose(iVerbose);
fRun->AddTask(mvdHitProd);

Double_t  lx=0.01, ly=0.01, threshold=600, noise=200;
PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer(lx,ly,threshold,noise);
mvdPixProd->SetVerbose(iVerbose);
fRun->AddTask(mvdPixProd);
 
// CLUST
// Cluster finding for strip detectors
Double_t noise = 1000.; /// put such stuff inside the task
Double_t chargecut = 3. * noise;
PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask(chargecut,inFile);
mvdmccls->SetVerbose(iVerbose);
fRun->AddTask(mvdmccls);
  
// Cluster finder for pixel detectors
PndMvdPixelClusterTask* mvdClusterizer = new PndMvdPixelClusterTask(1.8,76,84, inFile);
mvdClusterizer->SetVerbose(iVerbose);
fRun->AddTask(mvdClusterizer);
 
// TRACKFINDER
//PndMvdIdealTrackingTask* mvdmctrk = new PndMvdIdealTrackingTask();
//mvdmctrk->SetVerbose(iVerbose);
//fRun->AddTask(mvdmctrk);

// -----   EMC hit producers   ---------------------------------
PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
fRun->AddTask(emcHitProd); // hit production 

PndEmcMakeDigi* emcMakeDigi=new PndEmcMakeDigi();
//fRun->AddTask(emcMakeDigi); // fast digitization

PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(iVerbose);
PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(iVerbose);
fRun->AddTask(emcHitsToWaveform);  // full digitization
fRun->AddTask(emcWaveformToDigi);  // full digitization

PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
fRun->AddTask(emcMakeCluster);

PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
fRun->AddTask(emcHdrFiller); // ECM header

PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
fRun->AddTask(emcMakeBump);

PndEmcMakeRecoHit* emcMakeRecoHit= new PndEmcMakeRecoHit();
fRun->AddTask(emcMakeRecoHit);

// -----   LHETRACK  ---------------------------------

PndTpcLheHitsMaker* trackMS = new PndTpcLheHitsMaker("Tracking routine");
//trackMS->SetSimulation(kFALSE); 
//trackMS->SetVerbose(kTRUE);
trackMS->SetTpcMode(2, -1);  // 0 OFF, 1 TpcPoint, 2 TpcCluster // TpcPoint smearing [cm], if negative no smearing
trackMS->SetMvdMode(2, -1);  // 0 OFF, 1 MVDPoint, 2 MVDHit     // MVDPoint smearing [cm], if negative no smearing
trackMS->SetEmcMode(0);      // 0 OFF,             2 EmcCluster, 3 EmcBump
fRun->AddTask(trackMS);

PndTpcLheTrackFinder* trackFinder    = new PndTpcLheTrackFinder();
fRun->AddTask(trackFinder);

PndTpcLheTrackFitter* trackFitter    = new PndTpcLheTrackFitter("fitting");
fRun->AddTask(trackFitter);

// -----   Intialise and run   --------------------------------------------
fRun->Init();
fRun->Run(0, nEvents);
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
