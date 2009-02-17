pid_reco1(TString inFile = "inFile.root", TString outFile = "outFile.root")
{
// ========================================================================
// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
Int_t iVerbose = 0;

// Input file (MC events)
//TString inFile = "points_tpcmvdemc.root";

// Parameter file
TString parFile = "testparams.root";

// Output file
//TString outFile = "tracks_tpcmvdemc.root";

// Number of events to process
Int_t nEvents = 0;
 
// ----  Load libraries   -------------------------------------------------
gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
basiclibs();
gSystem->Load("libGeoBase");
gSystem->Load("libParBase");
gSystem->Load("libBase");
gSystem->Load("libMCStack");
gSystem->Load("libField");
gSystem->Load("libPassive");
gSystem->Load("libGen");  
gSystem->Load("libEmc"); 
gSystem->Load("libgenfit");
gSystem->Load("libtpc"); 
gSystem->Load("libTof"); 
gSystem->Load("libtpcreco");
gSystem->Load("libtrackrep");
gSystem->Load("librecotasks");
gSystem->Load("libMvd");
gSystem->Load("libMvdReco");
gSystem->Load("libLHETrack");
//gSystem->Load("libMyObj");
gSystem->Load("libGpid");
 gSystem->Load("libTrkBase");  
 gSystem->Load("libGeane");  

//   gSystem->Load("libGeaneExEmc");




TStopwatch timer;
timer.Start();
// ------------------------------------------------------------------------

// -----   Digitization run   -------------------------------------------
CbmRunAna *fRun= new CbmRunAna();
fRun->SetInputFile(inFile);
fRun->SetOutputFile(outFile);
// ------------------------------------------------------------------------

// -----  Parameter database   --------------------------------------------
/*
TString allDigiFile = "./all.par";

CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
parInput1->open(parFile.Data());
	
CbmParAsciiFileIo* parIo1 = new CbmParAsciiFileIo();
parIo1->open(allDigiFile.Data(),"in");
        
rtdb->setFirstInput(parInput1);
rtdb->setSecondInput(parIo1);
fRun->LoadGeometry();
*/
//Geane starting
  CbmGeane *Geane = new CbmGeane(inFile);
/*
  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0, 0 ,20. ); // values are in kG
  fMagField->SetFieldRegion(-500, 500,-500, 500, -200, 200);
  Geane->SetField(fMagField);
*/


/*
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
*/


//PndTpcMCDEdxTask* dedxreco = new PndTpcMCDEdxTask();
//fRun->AddTask(dedxreco);

PndTofHitProducerIdeal* tofHitPro = new PndTofHitProducerIdeal();
fRun->AddTask(tofHitPro);

//PndDrcHitProducer* drcHitPro  = new PndDrcHitProducer();
//fRun->AddTask(drcHitPro);

PndTpcLheHitsMaker* trackMS = new PndTpcLheHitsMaker("Tracking routine");
//trackMS->SetSimulation(kFALSE); 
//trackMS->SetVerbose(kTRUE);
trackMS->SetTpcMode(1, -1);  // 0 OFF, 1 TpcPoint, 2 TpcCluster // TpcPoint smearing [cm], if negative no smearing
trackMS->SetMvdMode(1, -1);  // 0 OFF, 1 MVDPoint, 2 MVDHit     // MVDPoint smearing [cm], if negative no smearing
trackMS->SetEmcMode(0);      // 0 OFF,             2 EmcCluster, 3 EmcBump
fRun->AddTask(trackMS);

PndTpcLheTrackFinder* trackFinder    = new PndTpcLheTrackFinder();
fRun->AddTask(trackFinder);


PndTpcLheTrackFitter* trackFitter    = new PndTpcLheTrackFitter("fitting");
fRun->AddTask(trackFitter);



  PndGeaneTrTpcTof *tr= new PndGeaneTrTpcTof();
  fRun->AddTask(tr);

PndGpidTask *pid = new PndGpidTask()
fRun->AddTask(pid);
 
//  PndLheEmcCatcher *ca = new PndLheEmcCatcher();
//  fRun->AddTask(ca);

// -----   Intialise and run   --------------------------------------------
fRun->Init();
fRun->Run(0, nEvents);
//fRun->Run(0, 10);
// ----------deleting the objs--------------------------------------------------------------
/*
delete PndTpcLheTrackFitter1;
delete PndTpcLheTrackFinder;
delete PndTpcLheHitsMaker;
delete PndEmcMakeCluster;
delete PndEmcWaveformToDigi;
delete  PndEmcHitsToWaveform;
*/
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
}
