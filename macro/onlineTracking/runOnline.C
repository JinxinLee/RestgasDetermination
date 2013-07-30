
void runOnline()  {
    gROOT->Reset();
    // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
    Int_t iVerbose = 0;

    enum DetectorId {
	kDCH,kDRC,kDSK,kEMC,kGEM,kLUMI,kMDT,kMVD,kRPC,kSTT,kTPC,kTOF,kFTS,kHYPG,kHYP};


    TStopwatch timer;
    timer.Start();
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
    rootlogon();


    TString simFileName = "Sim_Dpm_500.root";
    TString parFileName = "Sim_Dpm_500_params.root";
    TString digiFileName = "Sim_Dpm_500_digi.root";
    TString recoFileName = "Sim_Dpm_500_reco.root";
    TString outFileName = "Sim_Dpm_500_streamdisplay.root";

    TFile filedigi(digiFileName.Data());
    TFile filereco(recoFileName.Data());
    TFile filerecopixel(recoFileName.Data());

    TTree *treedigi = (TTree*) filedigi.Get("cbmsim");
    //TClonesArray *sttsortedhits = new TClonesArray("PndSttHit");
    //treedigi->SetBranchAddress("STTSortedHits",&sttsortedhits);
    
    TTree *recotree = (TTree*) filereco.Get("cbmsim");
    //TClonesArray *mvdstripreco = new TClonesArray("PndSdsHit");
    //recotree->SetBranchAddress("MVDHitsStrip",&mvdstripreco);
    //TClonesArray *mvdpixelreco = new TClonesArray("PndSdsHit");
    //recotree->SetBranchAddress("MVDHitsPixel",&mvdpixelreco);


    FairRunAna *fRun = new FairRunAna();
    fRun->SetInputFile(simFileName.Data());
    //fRun->AddFriend(recoFile.Data());
    fRun->AddFriend(digiFileName.Data());
    fRun->SetOutputFile(outFileName.Data());
    
    //FairRuntimeDb *rtdb = LoadRuntimeDB(parFileName, fRun);
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo* parInput1 = new FairParRootFileIo();
    parInput1->open(parFileName.Data());
    rtdb->setFirstInput(parInput1);
    

    // load geometry parameters
    //PndGeoSttPar* fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
    //PndSttMapCreator* mapper = new PndSttMapCreator(fSttParameters);
    //TClonesArray* tubearray = mapper->FillTubeArray();


    ///// works around the "geometry not supported by map" error
    ///// HACK
    PndSttTrackFinderReal* sttTrackFinder = new PndSttTrackFinderReal(0);
    PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
    sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
    //sttFindTracks->SetPersistence(kFALSE);
    fRun->AddTask(sttFindTracks);
    fRun->Init();
    ///////////////////////////////////////////////////////////


    PndOnlineHitProducer *online_hit_producer = new PndOnlineHitProducer();
    online_hit_producer->LoadStream(kSTT, "STTSortedHits");
    online_hit_producer->LoadStream(kMVD, "MVDHitsStrip");
    online_hit_producer->LoadStream(kMVD, "MVDHitsPixel");
    //online_hit_producer->LoadHits(kSTT);



    PndOnlineManager *online = new PndOnlineManager();
    online->AddHitProducer(online_hit_producer);
    online->SetRuntimeDB(rtdb);
    online->LoadSTTGeom();
    //online->SetActiveDetector(kMVD);    // WHY DOESNT THIS WORK??

    TClonesArray* tubearray = online->GetSTTGeom();

    // load task
    PndOnlineSttTripletFinder *triplet_finder = new PndOnlineSttTripletFinder(online, tubearray);
    //triplet_finder->Init();
    online->AddTask((FairTask*)triplet_finder);


    // do 1 go around of the event loop!
    //online->LoadHits( PndOnlineManager::kHESRRevolution );
    online->LoadHits( 200 );
    online->Process();
    //online->Clear();


    // now print out the triplets
    //online->PrintTracks();
    TObjArray *tracks = online->GetTrackObjectList();
    for(int i=0; i<tracks->GetEntriesFast(); ++i) {		       
	PndOnlineTrack *trk_ptr = (PndOnlineTrack *)(tracks->At(i));
	cout << " triplet cms;  ";  trk_ptr->Vertex().Print();
    }



  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
