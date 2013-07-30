#include "drawhelpers.hxx"

void SaveAndUpdateHisto(TH1* currenthisto, TFile& storagefile)
{
	if (storagefile.Get(currenthisto->GetName()) != 0) {
		currenthisto->Add((TH1*)storagefile.Get(currenthisto->GetName()));
	}
	cout << currenthisto->GetName() << ": " << currenthisto->GetEntries() << endl;
	currenthisto->Write(currenthisto->GetName(), TObject::kWriteDelete);
}

void runOnlineDisplayMCCheckFaster(Int_t maximumTime = 2050)  {
	gROOT->Reset();
	// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
	Int_t iVerbose = 0;

	enum DetectorId {
		kDCH,kDRC,kDSK,kEMC,kGEM,kLUMI,kMDT,kMVD,kRPC,kSTT,kTPC,kTOF,kFTS,kHYPG,kHYP};

	TStopwatch timer;
	timer.Start();
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	TDatabasePDG *pdg = new TDatabasePDG();

	// Reco Parameters
	Double_t stthitlifetime = 245;

	TString simFileName = "Sim_Dpm_500.root";
	TString parFileName = "Sim_Dpm_500_params.root";
	TString digiFileName = "Sim_Dpm_500_digi.root";
	TString recoFileName = "Sim_Dpm_500_reco.root";
	TString outFileName = "Sim_Dpm_500_streamdisplay.root";
	TString histofilename = "onlinetrkmccheck.root";

	TFile filedigi(digiFileName.Data());
	TFile filereco(recoFileName.Data());
	TFile filerecopixel(recoFileName.Data());

	TTree *treedigi = (TTree*) filedigi.Get("cbmsim");
	TTree *recotree = (TTree*) filereco.Get("cbmsim");

	TFile simfile(simFileName.Data());
	TTree* mctree = (TTree*) simfile.Get("cbmsim");
	TClonesArray* mcarray = new TClonesArray("PndMCTrack");
	mctree->SetBranchAddress("MCTrack", &mcarray);

	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(simFileName.Data());
	fRun->AddFriend(recoFileName.Data());
	fRun->AddFriend(digiFileName.Data());
	fRun->SetOutputFile(outFileName.Data());
	fRun->Init();
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

	PndOnlineGeometryManager *online_geometry = new PndOnlineGeometryManager(rtdb, parFileName.Data());

	PndOnlineManager *online = new PndOnlineManager();
	online->AddGeometryManager(online_geometry);

	online->LoadStream(PndOnline::kSTT,"STTSortedHits",stthitlifetime);
	online->LoadStream(PndOnline::kMVDPixel,"MVDHitsPixel",stthitlifetime);
	online->LoadStream(PndOnline::kMVDStrip,"MVDHitsStrip",stthitlifetime);
	//online->SetHESRRevolutionDuration(3000);

	TClonesArray* tubearray = online_geometry->GetDetectorGeometry(PndOnline::kSTT);

	// load task
	PndOnlineSttTripletFinder *triplet_finder = new PndOnlineSttTripletFinder(online, tubearray);
	online->AddTask((FairTask*)triplet_finder);

	// initialize display
	TCanvas* c1 = new TCanvas("c1");
	c1->Range(-42,-42,42,42);
	c1->SetCanvasSize(1200, 1200);
	TText* mytext = new TText();

	// event loop!
	int current_time = 0;
	int drawtime = 0;
	int delta_t = 2050;
	int drawdeltat = 5;
	int final_time = maximumTime;

	online->SetHESRRevolutionDuration(delta_t);

	online->Init();

	TObjArray* online_fairhits_stt = 0;
	TObjArray* online_tracks = 0;
	TObjArray* online_tracks2 = 0;
	TObjArray* online_fairhits_mvd = 0;

	std::vector<double> mctracktime;
	std::vector<double> mctrackpt;
	std::vector<double> mctrackphi;
	std::vector<int> mctrackstatus;
	std::vector<double> recotracktime;
	std::vector<double> recotrackpt;
	std::vector<double> recotrackphi;
	std::vector<int> recotrackstatus;
	std::vector<int> recotrackmcid;
	int totalmctracks = 0;
	int goodmctracks = 0;
	int totalrecotracks = 0;
	int goodrecotracks = 0;
	int norecotracks = 0;
	int deltaphifailed = 0;
	int deltaptfailed = 0;
	int deltabothfailed = 0;
	int secondaries = 0;
	int globalmaxmcid = 0;

	//std::map<int, std::map<int, int> > foundmap90;
	//std::map<std::string, int> foundmap90;
	//std::map<int, std::map<int, int> > foundmap80;
	//std::map<int, std::map<int, int> > foundmapbad;

	int mctracknullpointers = 0;
	int recotracknullpointers = 0;
	int mcidgood90 = 0;
	int mcidgood80 = 0;
	int mcidbad = 0;

	//output histos
	TH2D* hmcall = new TH2D("hmcall", "hmcall", 1500, -14, 16, 75, 0, 1.5);
	TH2D* hmcprime = new TH2D("hmcprime", "hmcprime", 1500, -14, 16, 75, 0, 1.5);
	TH2D* hmcrecoable = new TH2D("hmcrecoable", "hmcrecoable", 1500, -14, 16, 75, 0, 1.5);
	TH2D* hdafuq = new TH2D("hdafuq", "hdafuq", 1500, -14, 16, 75, 0, 1.5);
	TH2D* hmcrecoed = new TH2D("hmcrecoed", "hmcrecoed", 1500, -14, 16, 75, 0, 1.5);
	TH2D* hmcnotrecoed = new TH2D("hmcnotrecoed", "hmcnotrecoed", 1500, -14, 16, 75, 0, 1.5);

	TH1D* hphires = new TH1D("hphires", "hphires", 701, -3.505, 3.505);
	TH2D* hptres = new TH2D("hptres", "hptres", 300, -1400, 1600, 200, -1, 1);
	TH1D* hphiresmc = new TH1D("hphiresmc", "hphiresmc", 701, -3.505, 3.505);
	TH2D* hptresmc = new TH2D("hptresmc", "hptresmc", 300, -1400, 1600, 200, -1, 1);
	TH1D* hphiresmc90 = new TH1D("hphiresmc90", "hphiresmc90", 701, -3.505, 3.505);
	TH2D* hptresmc90 = new TH2D("hptresmc90", "hptresmc90", 300, -1400, 1600, 200, -1, 1);
	TH1D* hphiresmc80 = new TH1D("hphiresmc80", "hphiresmc80", 701, -3.505, 3.505);
	TH2D* hptresmc80 = new TH2D("hptresmc80", "hptresmc80", 300, -1400, 1600, 200, -1, 1);
	TH1D* hphiresmcbad = new TH1D("hphiresmcbad", "hphiresmcbad", 701, -3.505, 3.505);
	TH2D* hptresmcbad = new TH2D("hptresmcbad", "hptresmcbad", 300, -1400, 1600, 200, -1, 1);

    
	for (current_time = 0; current_time < final_time; current_time += delta_t) {
		online->Clear();
		//online->LoadHits( PndOnlineManager::kHESRRevolution );
		online->ClearTracks();
		online->LoadHits( delta_t );
		//online->LoadHits( current_time );
		online->Process();
		//online->Clear();
	
		online_tracks = online->GetTrackObjectList();
		int maxmcid = 0;
		int minmcid = 0;
		int firsti = 0;
		if (online_tracks->GetEntriesFast() > 0) {
			PndOnlineTrack* firstTrack = (PndOnlineTrack*) online_tracks->At(firsti);
			while (firstTrack->DrawMode() != PndOnlineTrack::kVertexMomentumAsCircleHack) {
				firsti++;
				if (firsti > online_tracks->GetEntriesFast()) {
					cout << "No drawable online Track in Array. We have a severe problem." << endl;
					break;
				}
				firstTrack = (PndOnlineTrack*) online_tracks->At(firsti);
			}
			maxmcid = firstTrack->MCEntryNumber();
			minmcid = firstTrack->MCEntryNumber();
		}
		for (int i = 0; i < online_tracks->GetEntriesFast(); i++) {
			PndOnlineTrack* myTrack = (PndOnlineTrack*) online_tracks->At(i);
			if (myTrack->DrawMode() != PndOnlineTrack::kVertexMomentumAsCircleHack) {
				continue;
			}
			totalrecotracks++;

			mctree->GetEntry(myTrack->MCEntryNumber());
			//cout << "MC Entry count: " << mcarray->GetEntriesFast() << endl;
			PndMCTrack* mctrack = (PndMCTrack*)mcarray->At(myTrack->MCTrackNumber());
			if (mctrack == 0) {
				cout << "MC Track NULL pointer in event " << myTrack->MCEntryNumber() << ", track " << myTrack->MCTrackNumber() << endl;
				cout << "Event Majority: " << myTrack->MCEntryMajority() << ", Track Majority: " << myTrack->MCTrackMajority() << endl;
				recotracknullpointers++;
				continue;
			}
			if (mctrack->GetMotherID() != -1) {
				//continue;
			}

			TVector2 innerhit = TVector2(myTrack->Vertex().x(), myTrack->Vertex().y());
			TVector2 outerhit = TVector2(myTrack->Momentum().x(), myTrack->Momentum().y());

// 			TVector2 circleorigin;
// 			Double_t circleradius;
// 			Int_t clockwise;
// 			CalculateCircle(&innerhit, &outerhit, &circleorigin, &circleradius, &clockwise);
// 			Double_t orthophi = circleorigin.Phi();
// 			Double_t trackphi = orthophi + 0.5*TMath::Pi()*clockwise;
// 			if (trackphi < 0) {
// 				trackphi += 2*TMath::Pi();
// 			}
// 			if (trackphi > 2*TMath::Pi()) {
// 				trackphi -= 2*TMath::Pi();
// 			}

			TVector2 circleorigin = myTrack->CircleOrigin();
			Double_t orthophi = circleorigin.Phi();
			Double_t circleradius = myTrack->CircleRadius();
			Int_t clockwise = myTrack->CircleClockwise();
			Double_t trackphi = myTrack->CirclePhi();
			Double_t trackpt = circleradius * 2.0 * 2.99792458;
/*
			cout << "Origin: " << endl;
			circleorigin.Print();
			myTrack->CircleOrigin().Print();
			cout << "Radius1: " << circleradius << endl;
			cout << "Radius2: " << myTrack->CircleRadius() << endl;
			cout << "Phi1: " << trackphi << endl;
			cout << "Phi2: " << myTrack->CirclePhi() << endl;
			cout << "Clockwise1: " << clockwise << endl;
			cout << "Clockwise2: " << myTrack->CircleClockwise() << endl;
*/

// 			cout << "Reco Track MCEntryNumber: " << myTrack->MCEntryNumber() << endl;
// 			cout << "Reco Track MCEntryMajority: " << myTrack->MCEntryMajority() << endl;
// 			cout << "Reco Track MCTrackNumber: " << myTrack->MCTrackNumber() << endl;
// 			cout << "Reco Track MCTrackMajority: " << myTrack->MCTrackMajority() << endl;
// 			cout << "Reco Track HitCount: " << myTrack->HitCount() << endl;

			//compare reconstructed tracks with the MC input
			//full check of all MC input data is done afterwards in a separate step

			Double_t mcphi = mctrack->GetMomentum().Phi();
			Double_t mcpt = mctrack->GetMomentum().Pt()*1000;
			if (mcphi < 0) {
				mcphi += 2*TMath::Pi();
			}
			Double_t calcphimc = mcphi;
			if (calcphimc < 0) calcphimc += 2*TMath::Pi();
			Double_t calcphireco = trackphi;
			if (calcphireco < 0) calcphireco += 2*TMath::Pi();
			Double_t deltaphi = calcphireco - calcphimc;
			if (deltaphi < 0) {
				deltaphi *= -1;
			}
			if (deltaphi > TMath::Pi()) {
				deltaphi = 2*TMath::Pi() - deltaphi;
			}
			Double_t deltapt = trackpt - mcpt;
			Int_t chargesign = 1;
			if ( (mctrack->GetMotherID() == -1) && (pdg->GetParticle(mctrack->GetPdgCode())->Charge() < 0) ) {
				chargesign = -1;
			}
			/*if ( (deltaphi > 2.9) && (deltaphi < 3.2) ) {
				cout << "Reco Track MCEntryNumber: " << myTrack->MCEntryNumber() << endl;
				cout << "Reco Track MCEntryMajority: " << myTrack->MCEntryMajority() << endl;
				cout << "Reco Track MCTrackNumber: " << myTrack->MCTrackNumber() << endl;
				cout << "Reco Track MCTrackMajority: " << myTrack->MCTrackMajority() << endl;
				cout << "Reco Track HitCount: " << myTrack->HitCount() << endl;
				cout << "Reco Raw Phi: " << trackphi << endl;
				cout << "Reco Phi: " << calcphireco << endl;
				cout << "MC Comp Phi: " << calcphimc << endl;
				cout << "MC Raw Phi: " << mcphi << endl;
				cout << "Orthophi: " << orthophi << endl;
				mctrack->GetMomentum().Print();
				cout << "Inner: " << endl;
				innerhit.Print();
				cout << "Outer: " << endl;
				outerhit.Print();
			} else {
				//cout << "Delta Phi: " << deltaphi << endl;
			}*/
			hphiresmc->Fill(deltaphi*chargesign);
			hptresmc->Fill(mcpt, deltapt/mcpt);
			int mceventid = myTrack->MCEntryNumber();
			int mctrackid = myTrack->MCTrackNumber();
			if ( (myTrack->MCEntryMajority() > 0.9) && (myTrack->MCTrackMajority() > 0.9) ) {
				mcidgood90++;
				hphiresmc90->Fill(deltaphi*chargesign);
				hptresmc90->Fill(mcpt, deltapt/mcpt);
			}
			if ( (myTrack->MCEntryMajority() > 0.8) && (myTrack->MCTrackMajority() > 0.8) ) {
				mcidgood80++;
				hphiresmc80->Fill(deltaphi*chargesign);
				hptresmc80->Fill(mcpt, deltapt/mcpt);
			} else {
				mcidbad++;
				hphiresmcbad->Fill(deltaphi*chargesign);
				hptresmcbad->Fill(mcpt, deltapt/mcpt);
			}

			//bool isrecoable = true;
			//if (mctrack->GetMomentum().Pt() < 0.052) isrecoable = false;
			if ( (mctrack->GetMomentum().Pt() < 0.052) && (mctrack->GetNPoints(kSTT) > 0) ) {
				cout << "Low pt hit" << endl;
			}
			
			int mcentrynumber = myTrack->MCEntryNumber();
			if (mcentrynumber > maxmcid) {
				maxmcid = mcentrynumber;
			}
			if (mcentrynumber < minmcid) {
				minmcid = mcentrynumber;
			}

			//std::cout << "RecoTrack: " << totalrecotracks << ", MCEntryNumber: " << mcentrynumber << " DigiTime: " << myTrack->T0Min() << ", T0time: " << myTrack->T0() << ", Pt " << trackpt << ", Phi: " << trackphi << " TriPhi: " << innerhit.Phi() << ", Radius: " << circleradius << ", DeltaT0: " << myTrack->T0Max()-myTrack->T0Min() << endl;

			//std::cout << "RecoTrack: " << totalrecotracks << ", MCEntryNumber: " << mcentrynumber << ", Pt " << trackpt << ", Phi: " << trackphi << " TriPhi: " << innerhit.Phi() << ", Radius: " << circleradius << endl;

			//recotracktime.push_back(myTrack->T0Min());
			recotrackpt.push_back(trackpt);
			recotrackphi.push_back(trackphi);
			recotrackstatus.push_back(0);
			recotrackmcid.push_back(mcentrynumber);
			
			
		}
		if (maxmcid > globalmaxmcid) {
			globalmaxmcid = maxmcid;
		}
		cout << "MC ID Range: " << minmcid << ", " << maxmcid << endl;
		for (int j = minmcid; j <= maxmcid; j++) {
			cout << "MC outer loop: " << j << endl;
			if (j > globalmaxmcid) {
				break;
			}
			mctree->GetEntry(j);
			cout << "MC Entry count: " << mcarray->GetEntriesFast() << endl;
			for (int k = 0; k < mcarray->GetEntriesFast(); k++) {
				PndMCTrack* mctrack = (PndMCTrack*)mcarray->At(k);
				if (mctrack == 0) {
					cout << "MC Track has NULL pointer. Oooops." << endl;
					mctracknullpointers++;
					continue;
				}
				hmcall->Fill(mctrack->GetMomentum().Pz(), mctrack->GetMomentum().Pt());
				bool isrecoable = false;
				if (mctrack->GetMotherID() == -1) {
					hmcprime->Fill(mctrack->GetMomentum().Pz(), mctrack->GetMomentum().Pt());
					isrecoable = true;
					if (mctrack->GetMomentum().Pz() < -0.4) {
						cout << "Critical Track at Event: " << j << " Track " << k << " Particle: " << mctrack->GetPdgCode() << endl;
						//cout << "Particle Mass: " << pdg->GetParticle(mctrack->GetPdgCode())->Mass() << " Charge: " << pdg->GetParticle(mctrack->GetPdgCode())->Charge() << endl;
						//cout << "MCTrack STT Hits: " << mctrack->GetNPoints(kSTT) << ", Pz: " << mctrack->GetMomentum().Pz() << ", Pt: " << mctrack->GetMomentum().Pt() << ", Phi: " << mctrack->GetMomentum().Phi() << ", Radius: " << mctrack->GetMomentum().Pt() / (3.0*2.0) << endl;
						cout << "MCVertex: " << mctrack->GetStartVertex().Mag() << " XYZ: " << mctrack->GetStartVertex().x() << ", " << mctrack->GetStartVertex().y() << ", " << mctrack->GetStartVertex().z() << endl;
					}
				}
				//if (mctrack->GetMotherID() == -1 && mctrack->GetNPoints(kSTT) > 0) {
					//cout << TRho::Instance()->GetPDG()->GetParticle(mctrack->GetPdgCode())->Mass() << endl;
				//}

				//bool isrecoable = true;
				if (mctrack->GetMomentum().Pt() < 0.052) isrecoable = false;
				if ( (mctrack->GetMomentum().Pt() < 0.052) && (mctrack->GetNPoints(kSTT) > 0) ) {
					cout << "Low pt hit" << endl;
				}
				if ( (mctrack->GetMomentum().Pt()/mctrack->GetMomentum().Pz() < 28.0/110.0) && (mctrack->GetMomentum().Pz() > 0) ) isrecoable = false;
				if ( (-(mctrack->GetMomentum().Pt())/mctrack->GetMomentum().Pz() < 28.0/55.0) && (mctrack->GetMomentum().Pz() < 0) ) isrecoable = false;
				if ( (mctrack->GetMotherID() == -1) && isrecoable && (mctrack->GetNPoints(kSTT) < 3) ) {
					if (TMath::Abs(pdg->GetParticle(mctrack->GetPdgCode())->Charge()) < 1) isrecoable = false;
					if (pdg->GetParticle(mctrack->GetPdgCode())->Mass() > 1) isrecoable = false;
				}
				if ( (mctrack->GetMotherID() == -1) && isrecoable && (mctrack->GetMomentum().Mag()/pdg->GetParticle(mctrack->GetPdgCode())->Mass() < 0.4) ) {
					cout << "Low betagamma particle" << endl;
					isrecoable = false;
				}
				if ( (mctrack->GetMotherID() == -1) && isrecoable && (mctrack->GetNPoints(kSTT) < 3) ) {
					cout << "Meh, reco possibility mismatch." << endl;
					hdafuq->Fill(mctrack->GetMomentum().Pz(), mctrack->GetMomentum().Pt());
					cout << "Dafuq at Event: " << j << " Track " << k << " Particle: " << mctrack->GetPdgCode() << " Charge: " << pdg->GetParticle(mctrack->GetPdgCode())->Charge() << endl;
					mctrack->GetMomentum().Print();
					isrecoable = false;
				}

				//if (mctrack->GetMotherID() == -1 && mctrack->GetNPoints(kSTT) > 0) {
				if (mctrack->GetMotherID() == -1 && isrecoable) {
					hmcrecoable->Fill(mctrack->GetMomentum().Pz(), mctrack->GetMomentum().Pt());
					Double_t mcphi = mctrack->GetMomentum().Phi();
					Double_t mcpt = mctrack->GetMomentum().Pt()*1000;
					if (mcphi < 0) {
						mcphi += 2*TMath::Pi();
					}
					//cout << "MCTrack: " << j << " " << k << ", Pt: " << mcpt << ", Phi: " << mcphi << ", Radius: " << mcpt / (3.0*2.0) << endl;
					//cout << "MCVertex: " << mctrack->GetStartVertex().Mag() << " XYZ: " << mctrack->GetStartVertex().x() << ", " << mctrack->GetStartVertex().y() << ", " << mctrack->GetStartVertex().z() << endl;
					int trackstatus = 0;
					if (mctrack->GetStartVertex().Mag() < 2) {
						for (int r = 0; r < recotrackmcid.size(); r++) {
							if (recotrackmcid.at(r) == j) {
								//Double_t deltaphi = recotrackphi.at(r) - mcphi;
								Double_t calcphimc = mcphi;
								if (calcphimc < 0) calcphimc += 2*TMath::Pi();
								Double_t calcphireco = recotrackphi.at(r);
								if (calcphireco < 0) calcphireco += 2*TMath::Pi();
								Double_t deltaphi = calcphireco - calcphimc;
								if (deltaphi < 0) {
									deltaphi *= -1;
								}
								if (deltaphi > TMath::Pi()) {
									deltaphi = 2*TMath::Pi() - deltaphi;
								}
								Double_t deltapt = recotrackpt.at(r) - mcpt;

								Int_t chargesign = 1;
								if (pdg->GetParticle(mctrack->GetPdgCode())->Charge() < 0) {
									chargesign = -1;
								}
									
								hphires->Fill(deltaphi*chargesign);
								hptres->Fill(mcpt, deltapt/mcpt);

								if ( (deltapt/mcpt < -0.75) && (deltapt/mcpt > -0.8) && (mcpt > 300) && (mcpt < 650) ) {
									cout << "Funny resolution at Event: " << j << " Track " << k << " Particle: " << mctrack->GetPdgCode() << " Charge: " << pdg->GetParticle(mctrack->GetPdgCode())->Charge() << endl;
									mctrack->GetMomentum().Print();
									cout << "MC pt: " << mcpt << endl;
									cout << "Re pt: " << recotrackpt.at(r) << endl;
								}

								bool phiok = (deltaphi < 0.1);

								if (deltapt < 0) {
									deltapt *= -1;
								}
								bool ptok = ( (deltapt < mcpt * 0.1) || ((mcpt > 450)&&(recotrackpt.at(r) > 450)) );
								if (phiok && ptok) {
									trackstatus++;
									recotrackstatus.at(r) = 1;
									//hmcrecoed->Fill(mctrack->GetMomentum().Pz(), mctrack->GetMomentum().Pt());
								} else {
									//hmcnotrecoed->Fill(mctrack->GetMomentum().Pz(), mctrack->GetMomentum().Pt());
								}
								if ((!phiok) && ptok) {
									deltaphifailed++;
								}
								if (phiok && (!ptok)) {
									deltaptfailed++;
								}
								if ((!phiok) && (!ptok)) {
									deltabothfailed++;
								}
							}
						}
						//cout << "Found " << trackstatus << " matching reco tracks." << endl;
						if (trackstatus == 0) {
							hmcnotrecoed->Fill(mctrack->GetMomentum().Pz(), mctrack->GetMomentum().Pt());
							norecotracks++;
						} else {
							hmcrecoed->Fill(mctrack->GetMomentum().Pz(), mctrack->GetMomentum().Pt());
						}
						mctrackstatus.push_back(trackstatus);
						goodmctracks++;
					} else {
						secondaries++;
					}
				}
			}
		}
	}

	for (int s = 0; s < mctrackstatus.size(); s++) {
		if (mctrackstatus.at(s) > 0) {
			goodrecotracks++;
		}
	}

	int recotrackstatusgoodcount = 0;
	int recotrackstatusbadcount = 0;

	for (int s = 0; s < recotrackstatus.size(); s++) {
		if (recotrackmcid.at(s) <= globalmaxmcid) {
			if (recotrackstatus.at(s) > 0) {
				recotrackstatusgoodcount++;
			} else {
				recotrackstatusbadcount++;
			}
		}
	}

	//for (std::map<int, std::map<int, int> >::iterator iterevent = foundmap90.begin(); iterevent != foundmap90.end(); iterevent++) {
		//for (itertrack = *iterevent.begin(); itertrack != *iterevent.end(); itertrack++
	//}

	cout << "Total reconstructed track count: " << totalrecotracks << endl;
	cout << "Reached MC ID: " << globalmaxmcid << endl;
	cout << "Reconstructable MC track count: " << goodmctracks << endl;
	cout << "MC Tracks reconstructed: " << goodrecotracks << endl;
	cout << "MC Tracks NOT reconstructed: " << norecotracks << endl;
	cout << "Reco Status Good: " << recotrackstatusgoodcount << endl;
	cout << "Reco Status Bad: " << recotrackstatusbadcount << endl;
	cout << "Delta phi failed: " << deltaphifailed << endl;
	cout << "Delta pt failed: " << deltaptfailed << endl;
	cout << "Delta both failed: " << deltabothfailed << endl;
	cout << "Secondary Tracks: " << secondaries << endl;

	cout << "MC Match Good 90: " << mcidgood90 << endl;
	cout << "MC Match Good 80: " << mcidgood80 << endl;
	cout << "MC Match Bad: " << mcidbad << endl;
	cout << "MC Tracks with NULL pointers in Reco: " << recotracknullpointers << endl;
	cout << "MC Tracks with NULL pointers: " << mctracknullpointers << endl;

	//c1->Print("hitdisplay.gif++");

	TFile outputstorage(histofilename, "UPDATE");
	outputstorage.cd();
	SaveAndUpdateHisto(hmcall, outputstorage);
	SaveAndUpdateHisto(hmcprime, outputstorage);
	SaveAndUpdateHisto(hmcrecoable, outputstorage);
	SaveAndUpdateHisto(hdafuq, outputstorage);
	SaveAndUpdateHisto(hmcrecoed, outputstorage);
	SaveAndUpdateHisto(hmcnotrecoed, outputstorage);
	SaveAndUpdateHisto(hphires, outputstorage);
	SaveAndUpdateHisto(hptres, outputstorage);
	SaveAndUpdateHisto(hphiresmc, outputstorage);
	SaveAndUpdateHisto(hptresmc, outputstorage);
	SaveAndUpdateHisto(hphiresmc90, outputstorage);
	SaveAndUpdateHisto(hptresmc90, outputstorage);
	SaveAndUpdateHisto(hphiresmc80, outputstorage);
	SaveAndUpdateHisto(hptresmc80, outputstorage);
	SaveAndUpdateHisto(hphiresmcbad, outputstorage);
	SaveAndUpdateHisto(hptresmcbad, outputstorage);
   
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
