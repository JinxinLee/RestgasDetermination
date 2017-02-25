#include "basefunctions.hxx"

int runEventInfo()  {
    gROOT->Reset();
    // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
    Int_t iVerbose = 0;

    enum DetectorId {
	kDCH,kDRC,kDSK,kEMC,kGEM,kLUMI,kMDT,kMVD,kRPC,kSTT,kTPC,kTOF,kFTS,kHYPG,kHYP};


    TStopwatch timer;
    timer.Start();
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
    rootlogon();

	// Reco Parameters
	Double_t stthitlifetime = 200;

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
    fRun->AddFriend(recoFileName.Data());
    fRun->AddFriend(digiFileName.Data());
    fRun->SetOutputFile(outFileName.Data());
    fRun->Init();
    //FairRuntimeDb *rtdb = LoadRuntimeDB(parFileName, fRun);
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

    // load geometry parameters
    ///PndGeoSttPar* fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
    //PndSttMapCreator* mapper = new PndSttMapCreator(fSttParameters);
    //TClonesArray* tubearray = mapper->FillTubeArray();

    PndOnlineGeometryManager *online_geometry = new PndOnlineGeometryManager(rtdb, parFileName.Data());


    /**
    ///// works around the "geometry not supported by map" error
    ///// HACK
   
    PndSttTrackFinderReal* sttTrackFinder = new PndSttTrackFinderReal(0);
    PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
    sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
    sttFindTracks->SetPersistence(kFALSE);
    fRun->AddTask(sttFindTracks);
    
    fRun->Init();
    ///////////////////////////////////////////////////////////
    **/

    PndOnlineHitProducer *online_hit_producer = new PndOnlineHitProducer();
    online_hit_producer->LoadStream(kSTT, "STTSortedHits");
    online_hit_producer->LoadStream(kMVD, "MVDHitsStrip");
    online_hit_producer->LoadStream(kMVD, "MVDHitsPixel");


    PndOnlineManager *online = new PndOnlineManager();
    online->AddHitProducer(online_hit_producer);
    online->AddGeometryManager(online_geometry);
    online->SetActiveDetector(kSTT,stthitlifetime);   
    online->SetActiveDetector(kMVD,stthitlifetime);   

    TClonesArray* tubearray = online_geometry->GetDetectorGeometry(kSTT);

    // load task
    PndOnlineSttTripletFinder *triplet_finder = new PndOnlineSttTripletFinder(online, tubearray);
    //triplet_finder->Init();
    online->AddTask((FairTask*)triplet_finder);

    // initialize display
	TCanvas* rzcanvas = new TCanvas("rzcanvas");
	rzcanvas->Range(-54,-42,114,42);
	rzcanvas->SetCanvasSize(1200, 600);
	rzcanvas->cd();
    TCanvas* c1 = new TCanvas("c1");
    c1->Range(-42,-42,42,42);
    c1->SetCanvasSize(1200, 1200);
    TText* mytext = new TText();
	

    // event loop!
    int current_time = 0;
    //int delta_t = 5;
    int delta_t = 5;
    int final_time = 2205;
    online->Init();

	TObjArray* online_fairhits_stt = 0;
	TObjArray* online_tracks = 0;
	TObjArray* online_fairhits_mvd = 0;
    
    while( current_time < final_time ) {
	//online->LoadHits( PndOnlineManager::kHESRRevolution );
	online->LoadHits( delta_t );
	//online->LoadHits( current_time );
	online->Process();
	//online->Clear();
	current_time += delta_t;
	
    
	//Draw the results
	c1->Clear();
	c1->cd();
	DrawDetector();
	cout << " Drawing stuff " << endl;

	online_fairhits_stt = online->GetFairHitObjectList(kSTT);
	online_tracks = online->GetTrackObjectList();
	online_fairhits_mvd = online->GetFairHitObjectList(kMVD);

	DrawIsochrones(current_time-stthitlifetime, online_fairhits_stt, tubearray);
	// triplet location
	DrawOnlineTrackVertices(current_time-stthitlifetime, online_tracks);
	// tracks through triplets
	//DrawOnlineTrackHack(current_time-stthitlifetime, online_tracks);
	DrawOnlineTrackHackHitCheck(current_time-stthitlifetime, online_tracks, online_fairhits_stt, tubearray);
	//DrawOnlineTrackMomentum(current_time-stthitlifetime, online_tracks);
	DrawFairHits(current_time-stthitlifetime, online_fairhits_mvd, 20);
	//DrawFairHits(drawstackstriphits);
	//DrawFairHits(drawstackpixelhits);
	//DrawTripletTracks(innertriplets, midtriplets, 10);
	//DrawTripletTracks(innertriplets, outertriplets, 45);
	//DrawTripletTracks(midtriplets, outertriplets, 25);

	//draw in rz projection
	rzcanvas->Clear();
	rzcanvas->cd();
	DrawDetectorRZ();
	DrawFairHitsRZ(current_time-stthitlifetime, online_fairhits_mvd, 20);
	DrawOnlineTrackVerticesRZ(current_time-stthitlifetime, online_tracks);
	mytext->DrawText(-54, -42, TString::Format("%d ns",current_time));
	rzcanvas->Update();
	rzcanvas->Print("hitdisplayrz.gif+20");

	c1->cd();
	mytext->DrawText(-42, -42, TString::Format("%d ns",current_time));
	c1->Update();
	//c1->Print("hitdisplay.png");
	c1->Print("hitdisplay.gif+20");

	delete online_fairhits_stt;
	delete online_tracks;
	delete online_fairhits_mvd;

	/**/
	// now print out the triplets
	//online->PrintTracks();
	//TObjArray *tracks = online->GetTrackObjectList();
	//for(int i=0; i<tracks->GetEntriesFast(); ++i) {		       
	    //PndOnlineTrack *trk_ptr = (PndOnlineTrack *)(tracks->At(i));
	    //cout << " triplet cms;  ";  trk_ptr->Vertex().Print();
	//}
	/**/

	//online->GetTrackList().clear();

    }

c1->Print("hitdisplay.gif++");
rzcanvas->Print("hitdisplayrz.gif++");
    
    // -----   Finish   -------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------


  return 0;
}
