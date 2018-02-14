int runTimestampCheck()  {
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
    //online->SetActiveDetector(kMVD,stthitlifetime);   

    TClonesArray* tubearray = online_geometry->GetDetectorGeometry(kSTT);

    // load task
    PndOnlineSttTripletFinder *triplet_finder = new PndOnlineSttTripletFinder(online, tubearray);
    //triplet_finder->Init();
    online->AddTask((FairTask*)triplet_finder);

    // initialize display
    //TCanvas* c1 = new TCanvas("c1");
    //c1->Range(-42,-42,42,42);
    //c1->SetCanvasSize(1200, 1200);
    TText* mytext = new TText();
	

    // event loop!
    int current_time = 0;
    //int delta_t = 5;
    int delta_t = 20;
    int final_time = 300;
    online->Init();

	Int_t timeviolationcount = 0;
    
    while( current_time < final_time ) {
	//online->LoadHits( PndOnlineManager::kHESRRevolution );
	online->LoadHits( delta_t );
	//online->LoadHits( current_time );
	online->Process();
	//online->Clear();
	current_time += delta_t;
	
    
	//Draw the results
	//c1->Clear();
	//c1->cd();
	//DrawDetector();
	//cout << " Drawing stuff " << endl;
	DrawIsochrones(current_time-stthitlifetime, online->GetFairHitObjectList(kSTT), tubearray, &timeviolationcount);
	//DrawOnlineTrackVertices(current_time-stthitlifetime, online->GetTrackObjectList());
	//DrawOnlineTrackHack(current_time-stthitlifetime, online->GetTrackObjectList());
	//DrawFairHits(drawstackstriphits);
	//DrawFairHits(drawstackpixelhits);
	//DrawTripletTracks(innertriplets, midtriplets, 10);
	//DrawTripletTracks(innertriplets, outertriplets, 45);
	//DrawTripletTracks(midtriplets, outertriplets, 25);

	/**/
	// now print out the triplets
	//online->PrintTracks();
	TObjArray *tracks = online->GetTrackObjectList();
	for(int i=0; i<tracks->GetEntriesFast(); ++i) {		       
	    PndOnlineTrack *trk_ptr = (PndOnlineTrack *)(tracks->At(i));
	    //cout << " triplet cms;  ";  trk_ptr->Vertex().Print();
	}
	/**/

	//online->GetTrackList().clear();

	//mytext->DrawText(-42, -42, TString::Format("%d ns",current_time));
	//c1->Update();
	//c1->Print("hitdisplay.png");
	//c1->Print("hitdisplay.gif+15");
    }
    
	cout << endl << endl << endl << "-----" << endl << "Number of Time Window Violations: " << timeviolationcount << endl << "-----" << endl;

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


void DrawIsochrones(Int_t currenttime, TObjArray* stthits, TObjArray* stttubes, Int_t* timeviolationcount) {
	TEllipse mylipse;
	mylipse.SetFillStyle(0);
	PndSttTube* tube = 0;
	PndSttHit* drawhit = 0;
	Double_t isochrone = 0;
	Double_t timestamp = 0;
	Double_t drifttime = 0;
	Double_t recoisochrone = 0;

	PndSttSingleStraw stt;
	//setting the single straw tube simulation constants
	// 3 options currently available:
	// TConst(tube radius (cm), gas pressure (bar), Ar%, CO2%)
	// stt.TConst(0.4, 1, 0.9, 0.1); 
	// stt.TConst(0.5, 1, 0.9, 0.1);
	stt.TConst(0.5, 2, 0.8, 0.2);
	// wire positioning   
	stt.PutWireXYZ(0.,  0., -75., 0., 0., 75.);


	cout << "----- Checking Isochrones at " << currenttime << " with " << stthits->GetEntriesFast() << " active Hits." << endl;
	for(int j = 0; j < stthits->GetEntriesFast(); j++) {
		drawhit = (PndSttHit*) stthits->At(j);
		if (!drawhit) {
			cout << " Error: No Hit Pointer in drawstack." << endl;
			continue;
		}
		//cout << "Tube ID: " << drawhit->GetTubeID() << endl;
		tube = (PndSttTube*)(stttubes->At(drawhit->GetTubeID()));
		if (!tube) {
			cout << " Error: Hit pointed to invalid tube." << endl;
			continue;
		}
		//cout << "Calculating Isochrones." << endl;
		isochrone = drawhit->GetIsochrone();
		timestamp = drawhit->GetTimeStamp();
		if (timestamp>currenttime+200) {
			cout << " Error: Hit time " << timestamp << " larger than Window end time " << currenttime+200 << endl;
			(*timeviolationcount)++;
			continue;
		}
		//cout << "MC Isochrone: " << drawhit->GetIsochrone() << endl;
		//cout << "Hit Timestamp: " << drawhit->GetTimeStamp() << endl;
		drifttime = timestamp - currenttime;
		//cout << "Calculating Isochrones: " << isochrone << " " << timestamp << " " << drifttime << " " << recoisochrone << endl;
		if (drifttime < 0) drifttime = 0;
		if (drifttime > 245) drifttime = 245;
		recoisochrone = stt.TimnsToDiscm(drifttime);
		//cout << "Calculating Isochrones: " << isochrone << " " << timestamp << " " << drifttime << " " << recoisochrone << endl;
		if (recoisochrone < 0) recoisochrone = 0;
		if (recoisochrone > 0.5) recoisochrone = 0.5;
		//cout << "Calculating Isochrones: " << isochrone << " " << timestamp << " " << drifttime << " " << recoisochrone << endl;
		//std::cout << currenttime << " " << drawhit->GetTubeID() << " " << tube->GetPosition().X() << " " << tube->GetPosition().Y() << " " << tube->GetPosition().Z() << " " << tube->GetWireDirection().X() << " " << tube->GetWireDirection().Y() << " " << tube->GetWireDirection().Z() << " " << timestamp << " " << isochrone << " " << recoisochrone << std::endl;
		mylipse.SetX1(tube->GetPosition().X());
		mylipse.SetY1(tube->GetPosition().Y());
		
		//conformal transformation
		//Double_t r2 = tube->GetPosition().X() * tube->GetPosition().X() + tube->GetPosition().Y() * tube->GetPosition().Y();
		//Double_t u = tube->GetPosition().X() / r2;
		//Double_t v = tube->GetPosition().Y() / r2;
		//mylipse.SetX1(500*u);
		//mylipse.SetY1(500*v);

		mylipse.SetR1(recoisochrone);
		mylipse.SetR2(recoisochrone);

		if (TMath::Abs(recoisochrone-isochrone) < 0.04) {
			mylipse.SetLineColor(kGreen);
		} else if (recoisochrone+0.04 < isochrone) {
			mylipse.SetLineColor(kRed);
		} else {
			if ((tube->GetWireDirection().X() == 0) && (tube->GetWireDirection().Y() == 0)) {
				mylipse.SetLineColor(kBlack);
			} else {
				mylipse.SetLineColor(kBlue);
			}
		}
		//if (HasNeighbor(drawhit->GetTubeID(), stthits, stttubes)) {
		//	mylipse.SetFillColor(kGreen);
		//	mylipse.SetFillStyle(1001);
		//} else {
		//	mylipse.SetFillStyle(0);
		//}
		//mylipse.SetFillStyle(0);
		//mylipse.DrawClone();
	}
}
