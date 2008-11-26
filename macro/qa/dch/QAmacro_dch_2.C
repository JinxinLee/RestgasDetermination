{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  TString base="tstQA";
  TString inFile = base+".root";
  TString parFile = base+".param.root";
  TString outFile = base+".recoReal.root";
  
  Int_t nEvents = 0;

  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  rtdb->Print();

  // -----   Add tasks
  // ------------------------------------------------- 
  PndDchDigiProducer* digiProducer= new PndDchDigiProducer();
  digiProducer->SetVerbose(0);
  fRun->AddTask(digiProducer);
  // -------------------------------------------------   
  PndDchCylinderHitProducer* cylHitProducer= new PndDchCylinderHitProducer();
  cylHitProducer->SetVerbose(0);
  fRun->AddTask(cylHitProducer);
  // ------------------------------------------------- 

  //------ Ideal DCH track finder --------------------
  PndDchFindTracks* finderTask = new PndDchFindTracks("dchFindTracks");
  finderTask->SetUseHitOrDigi("chit");
  finderTask->SetVerbose(0);
  fRun->AddTask(finderTask);
  // ------------------------------------------------- 
  PndDchTrackFinderIdealCylHit* mcTrackFinder = new  PndDchTrackFinderIdealCylHit();
  mcTrackFinder->SetVerbose(0);  
  mcTrackFinder->SetPrimary(1);  // 1 = Only primary tracks are processed, 0 = all (default)
  finderTask->UseFinder(mcTrackFinder);
  //--------------------------------------------------
  PndDchMatchTracks *matchTask = new PndDchMatchTracks();//match PndDchTracks and MCTracks
  matchTask->SetUseHitOrDigi("chit");
  matchTask->SetVerbose(0);
  fRun->AddTask(matchTask);
  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  CbmGeane *Geane = new CbmGeane(inFile);
  // ------------------------------------------------- 
    PndDchPreFitterTR* dchPreFitter = new PndDchPreFitterTR();
  dchPreFitter->SetVerbose(0);
  fRun->AddTask(dchPreFitter);
  // ------------------------------------------------- 
  PndDchPreFitterTRQATask* dchPreFitterQA = new PndDchPreFitterTRQATask();
  dchPreFitterQA->SetVerbose(0);
  fRun->AddTask(dchPreFitterQA);
  // ------------------------------------------------- 
  PndDchPrepareKalmanTracks2 *prepareKalmanTracks = new PndDchPrepareKalmanTracks2();
  prepareKalmanTracks->SetVerbose(0);
  prepareKalmanTracks->UseGeane(kTRUE);
  prepareKalmanTracks->UseMC(kFALSE);
  prepareKalmanTracks->SetPDG(2212);
  prepareKalmanTracks->SetPersistence();
  fRun->AddTask(prepareKalmanTracks);
  // ------------------------------------------------- 
  PndDchKalmanTask2* dchKalman = new PndDchKalmanTask2();
  dchKalman->SetVerbose(0);
  dchKalman->SetNumIterations(1);
  dchKalman->SetSmooth(kFALSE);
  fRun->AddTask(dchKalman);
  // ------------------------------------------------- 
  PndDchKalmanQATask* dchKalmanQA = new PndDchKalmanQATask();
  dchKalmanQA->SetVerbose(0);
  fRun->AddTask(dchKalmanQA);
  // ------------------------------------------------- 

  fRun->Init();
  Geane->SetField(fRun->GetField());
  fRun->Run(0,nEvents);

  Double_t nofTracks = 3.*100.;

  DchPreFitterTRQA->cd();
  TH1F* pullpPrefitter = (TH1F*)gDirectory->Get("pullP");
  gDirectory->cd("../DchKalmanQA");
  TH1F* pullpKalman = (TH1F*)gDirectory->Get("pullP");

  Double_t pMeanPrefitter = 100.*pullpPrefitter->GetMean();
  Double_t pRmsPrefitter = 100.*pullpPrefitter->GetRMS();
  Double_t pMeanKalman = 100.*pullpKalman->GetMean();
  Double_t pRmsKalman = 100.*pullpKalman->GetRMS();

  cout<<"\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
  cout<<"~~~~~                                                                ~~~~"<<endl;
  cout<<"~~~~~                         Summary of QA:                         ~~~~"<<endl;
  cout<<"~~~~~                                                                ~~~~"<<endl;
  cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;

  Bool_t fTest=kTRUE;

  if(TMath::Abs(pMeanPrefitter)<5. && pRmsPrefitter<10.){
    cout<<"\n PreFitter: - ACCEPTABLE PERFORMANCE "<<endl;
          }
  else{
    cout<<" \n PreFitter: - SOMETHING WENT WRONG "<<endl;
    fTest=kFALSE;
  }
  cout<<"\n PreFitter: Mean of pull P = "<<pMeanPrefitter<<" % "<<endl;
  cout<<" PreFitter: RMS  of pull P = "<<pRmsPrefitter<<" % (momentum resolution)"<<endl;
  cout<<" PreFitter: Reconstruction efficiency so far = "<<pullpPrefitter->GetEntries()/nofTracks<<endl<<endl;
  cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
  if(TMath::Abs(pMeanKalman<1.) && pRmsKalman<2.){
    cout<<" \n Kalman: - ACCEPTABLE PERFORMANCE "<<endl;
  }
  else{
    cout<<" \nKalman: - SOMETHING WENT WRONG "<<endl;
    fTest=kFALSE;
  }
  cout<<" \n Kalman:    Mean of pull P = "<<pMeanKalman<<" %"<<endl;
  cout<<" Kalman:    RMS  of pull P = "<<pRmsKalman<<" % (momentum resolution)"<<endl;
  cout<<" Kalman:    Reconstruction efficiency so far = "<<pullpKalman->GetEntries()/nofTracks<<endl<<endl;

  cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n"<<endl;

  if (fTest){
    cout << " Test passed" << endl;
    cout << " All ok " << endl;  
  }else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;         
  }
  
  exit(0);
}
