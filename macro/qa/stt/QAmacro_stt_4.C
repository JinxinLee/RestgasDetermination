// TEST 2: full reconstruction, i.e. digi fast + reco + kalman
// #include "/home/lavezzi/test_dev/original/trunk3/pnddata/PndDetectorList.h"
// void QAmacro_stt_2(){
void QAmacro_stt_4()
{
  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
 
  Bool_t fTest=kFALSE;

  // ------------------------------------------------------------------------
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "testrun.root";

  // Number of events to process
  Int_t nEvents = 0;

  // Parameter file
  TString parFile = "testparams.root";

  // Output file
  TString outFile = "testcomplete.root";

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------
 
  // ----- Prepare GEANE --------------------------------------------
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
  // ------------------------------------------------------------------------

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
    
  // -----   STT analysis tasks   --------------------------------------------
  // digitize ....
  PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
  // sttHitProducer->SetVerbose(3); // debug = print each evt num
  fRun->AddTask(sttHitProducer);

  // trackfinding ....
  PndSttTrackFinderReal* sttTrackFinder = new PndSttTrackFinderReal(iVerbose);
  PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
  sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttFindTracks);

/**
  // trackmatching ....
  PndSttMatchTracks* sttTrackMatcher = new PndSttMatchTracks("Match tracks", "STT", iVerbose);
  sttTrackMatcher->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttTrackMatcher);  

  // trackfitting ....
  PndSttTrackFitter* sttTrackFitter = new PndSttHelixTrackFitter(iVerbose);
  PndSttFitTracks* sttFitTracks = new PndSttFitTracks("STT Track Fitter", "FairTask", sttTrackFitter); 
  sttFitTracks->AddHitCollectionName("STTHit");
  fRun->AddTask(sttFitTracks);

  // helix hit production ....
  PndSttHelixHitProducer* sttHHProducer = new PndSttHelixHitProducer();
  fRun->AddTask(sttHHProducer);

  // kalman ...
  PndSttPatternRecoTask2* STTPR = new PndSttPatternRecoTask2();
  fRun->AddTask(STTPR);
  PndSttKalmanTask2 *Kalman = new PndSttKalmanTask2();
  fRun->AddTask(Kalman); 
**/
  // -----   Initialize and run   --------------------------------------------
  fRun->Init();
  fRun->Run();
  // ------------------------------------------------------------------------

  fTest = kTRUE;

  if (fTest){
    cout << " Test Passed" << endl;
    cout << " All Ok " << endl;  
  }else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;         
  }
 timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  exit(0); 
}  
  
