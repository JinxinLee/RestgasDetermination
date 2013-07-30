// Retrieve hits within a timewindow
/*
Int_t timestamp: Current time
Int_t timewindow: Timewindow how much younger hits than the current time will be included in the List
TObjArray* outputarray: Array of FairHit objects the new hits will be added to
Int_t& nexttime: Timestamp of the hit which will be processed the next time this function runs. This value MUST be initialized to 0 for the first run of this function and MUST be preserved in the calling function. Its value will be updated by this function and MUST NOT be changed elsewhere.
Int_t& entrynumber: Current entry in the source tree containing the hit data. This value MUST be initialized to 0 for the first run of this function and MUST be preserved in the calling function. Its value will be updated by this function and MUST NOT be changed elsewhere.
Int_t& clonesnumber: Current entry in the source TClonesArray containing the hit data. This value MUST be initialized to 0 for the first run of this function and MUST be preserved in the calling function. Its value will be updated by this function and MUST NOT be changed elsewhere.
TTree* sourcetree: Root tree which contains the hit data.
TClonesArray* sourceclones: Clones array within the sourcetree which contains the hit data stored in FairHit objects.
*/
void GetHitsInTime(Int_t timestamp, Int_t timewindow, TObjArray* outputarray, Int_t& nexttime, Int_t& entrynumber, Int_t& clonesnumber, TTree* sourcetree, TClonesArray* sourceclones) {
	FairHit* myhit = 0;
	while ((nexttime < timestamp + timewindow) && (entrynumber < sourcetree->GetEntriesFast())) {
		cout << "Looping at time  " << timestamp << endl;
		myhit = (FairHit*) sourceclones->At(clonesnumber);
		if (myhit) {
			nexttime = myhit->GetTimeStamp();
			cout << "Nexttime set to " << nexttime << endl;
		} else {
			cout << "FairHit Empty" << endl;
		}
		if (nexttime > timestamp + timewindow) {
			cout << "Nexttime " << nexttime << " > Windowend " << timestamp + timewindow << endl;
			continue;
		} else if (myhit) {
			cout << "Nexttime in window. Adding Hit to " << outputarray->GetName() << ", " << outputarray->GetEntriesFast() << endl;
			outputarray->AddLast(myhit->Clone());
		} else {
			cout << "Nexttime in window but no Hit" << endl;
		}
		cout << "Checking next clone." << endl;
		++clonesnumber;
		if (clonesnumber >= sourceclones->GetEntriesFast()) {
			cout << "End of TClonesArray. Checking next entry." << endl;
			clonesnumber = 0;
			++entrynumber;
			if (entrynumber < sourcetree->GetEntriesFast()) {
				sourcetree->GetEntry(entrynumber);
			} else {
				cout << "No more entries." << endl;
				continue;
			}
		}
	}
}

// Remove expired hits from list
/*
Int_t timestamp: Current time
Int_t timewindow: Timewindow how much older than the timestamp a hit may be until it expires
TObjArray* outputarray: Array of the FairHit objects to be processed
*/
void RemoveOldHits(Int_t timestamp, Int_t timewindow, TObjArray* outputarray) {
	cout << "Processing Draw Stack: " << outputarray->GetName() << ", " << outputarray->GetEntriesFast() << endl;
	if (outputarray->GetEntriesFast() > 0) {
		cout << "About to clean Draw Stack." << endl;
		outputarray->Compress();
		if (!(outputarray->First())) cout << "Whoops, invalid entry in outputarray." << endl;
		int i = 0;
		cout << ((FairHit*)(outputarray->First()))->GetTimeStamp() << endl;
		cout << timestamp - timewindow << endl;
		for (int i = 0; i < outputarray->GetEntriesFast(); i++) {
			if (((FairHit*)(outputarray->At(i)))->GetTimeStamp() < timestamp - timewindow) {
				outputarray->RemoveAt(i);
			} else {
				break;
			}
		}
		cout << "Draw Stack cleaned. New Entry Count: ";
		outputarray->Compress();
		cout << outputarray->GetEntries() << endl;
	} else {
		cout << "Draw Strack empty, not cleaning." << endl;
	}
}

// Draws the detector outline
void DrawDetector() {
	//Overlapping background goes to loops when canvas is cleared
	//mylipse.DrawEllipse(-2, 0, 39, 40.5, 90, 270, 0);
	//mylipse.DrawEllipse(2, 0, 39, 40.5, 90, 270, 180);
	TEllipse mybglipse;
	mybglipse.SetFillColor(kWhite);
	Double_t xbg[8] = {2, 16, 16, 2, -2, -16, -16, -2};
	Double_t ybg[8] = {17.2, 9.2, -9.2, -17.2, 17.2, 9.2, -9.2, -17.2};
	TPolyLine plinebg(8, xbg, ybg);
	plinebg.SetFillColor(kWhite);
	//plinebg->SetLineColor(kWhite);
	//plinebg->Draw("f");
	Double_t xr[4] = {2, 16, 16, 2};
	Double_t yr[4] = {17.2, 9.2, -9.2, -17.2};
	TPolyLine pliner(4, xr, yr);
	//pliner->SetFillColor(kWhite);
	pliner.SetLineColor(kBlack);
	//pliner->Draw("f");
	//pliner->Draw();
	Double_t xl[4] = {-2, -16, -16, -2};
	Double_t yl[4] = {17.2, 9.2, -9.2, -17.2};
	TPolyLine plinel(4, xl, yl);
	//plinel->SetFillColor(kWhite);
	plinel.SetLineColor(kBlack);
	//plinel->Draw("f");
	//plinel->Draw();
	Double_t xls[8] = {-2, -31.5, -31.5, -2, -2, -23.2, -23.2, -2};
	Double_t yls[8] = {35.2, 18.5, -18.5, -35.2, -25.7, -13.5, 13.5, 25.7};
	TPolyLine plinels(8, xls, yls);
	//plinels->Draw();
	Double_t xrs[8] = {2, 31.5, 31.5, 2, 2, 23.2, 23.2, 2};
	Double_t yrs[8] = {35.2, 18.5, -18.5, -35.2, -25.7, -13.5, 13.5, 25.7};
	TPolyLine pliners(8, xrs, yrs);
	//pliners->Draw();
	//Draw Background
	mybglipse.DrawEllipse(-2, 0, 39, 40.5, 90, 270, 0);
	mybglipse.DrawEllipse(2, 0, 39, 40.5, 90, 270, 180);
	plinebg.DrawClone("f");
	pliner.DrawClone();
	plinel.DrawClone();
	pliners.DrawClone();
	plinels.DrawClone();
	mybglipse.DrawEllipse(0, 0, 14, 14, 0, 360, 0);
	mybglipse.DrawEllipse(0, 0, 0.5, 0.5, 0, 360, 0);
}

// Draws the detector outline in YZ projection
void DrawDetectorYZ() {
	TBox mybox;
	mybox.SetLineColor(kBlack);
	mybox.SetFillStyle(0);
	mybox.DrawBox(-40, -40.5, 110, 40.5);
	mybox.DrawBox(-17, -14, 23, 14);
	TEllipse myiplipse;
	myiplipse.SetLineColor(kBlack);
	myiplipse.SetFillStyle(0);
	myiplipse.DrawEllipse(0, 0, 1, 1, 0, 360, 0);
}

// Draws the detector outline in XZ projection
void DrawDetectorXZ() {
	TBox mybox;
	mybox.SetLineColor(kBlack);
	mybox.SetFillStyle(0);
	mybox.DrawBox(-40, -40.5, 110, 40.5);
	mybox.DrawBox(-17, -14, 23, 14);
	TEllipse myiplipse;
	myiplipse.SetLineColor(kBlack);
	myiplipse.SetFillStyle(0);
	myiplipse.DrawEllipse(0, 0, 1, 1, 0, 360, 0);
}

// Draws the detector outline in RZ projection
void DrawDetectorRZ() {
	TBox mybox;
	mybox.SetLineColor(kBlack);
	mybox.SetFillStyle(0);
	//stt
	mybox.DrawBox(-40, 16, 110, 40.5);
	mybox.DrawBox(-40, -16, 110, -40.5);
	//mvd
	mybox.DrawBox(-17, 1.0, 23, 15.5);
	mybox.DrawBox(-17, -1.0, 23, -15.5);
	TEllipse myiplipse;
	myiplipse.SetLineColor(kBlack);
	myiplipse.SetFillStyle(0);
	myiplipse.DrawEllipse(0, 0, 0.5, 0.5, 0, 360, 0);
}

// Draws list of FairHits in xy projection
/*
TObjArray* fairhits: Array of FairHit objects to be drawn in the xy projection
*/
void DrawFairHits(Double_t currenttime, TObjArray* fairhits, Double_t timeout) {
	TEllipse mylipse(0, 0, 0.2, 0.2);
	mylipse.SetFillColor(kBlack);
	FairHit* drawhit = 0;
	cout << "Drawing " << fairhits->GetName() << endl;
	for(int j = 0; j < fairhits->GetEntriesFast(); j++) {
		drawhit = (FairHit*) fairhits->At(j);
		if (!drawhit) {
			cout << "Error: No Hit Pointer in drawstack." << endl;
			continue;
		}
		Double_t timestamp = drawhit->GetTimeStamp();
		if ((timestamp>currenttime+timeout) || (timestamp < currenttime)) {
			cout << "Error: Hit time " << timestamp << " out of Window " << currenttime+200 << endl;
			continue;
		}
		//cout << "FairHit Timestamp: " << drawhit->GetTimeStamp() << ",  " << drawhit->GetX() << ",  " << drawhit->GetY() << endl;
		mylipse.SetX1(drawhit->GetX());
		mylipse.SetY1(drawhit->GetY());
		//mylipse.SetR1(0.1);
		//mylipse.SetR2(0.1);
		if (TMath::Sqrt(drawhit->GetX()*drawhit->GetX()+drawhit->GetY()*drawhit->GetY())/drawhit->GetZ() > 0.3) {
			mylipse.SetLineColor(kGreen);
			mylipse.SetFillColor(kGreen);
		} else {
			mylipse.SetFillColor(kBlack);
			mylipse.SetLineColor(kBlack);
		}
		mylipse.DrawClone();
	}
}

void DrawFairHitsYZ(Double_t currenttime, TObjArray* fairhits, Double_t timeout) {
	TEllipse mylipse(0, 0, 0.2, 0.2);
	mylipse.SetFillColor(kBlack);
	FairHit* drawhit = 0;
	cout << "YZ Drawing " << fairhits->GetName() << " with " << fairhits->GetEntriesFast() << endl;
	for(int j = 0; j < fairhits->GetEntriesFast(); j++) {
		drawhit = (FairHit*) fairhits->At(j);
		if (!drawhit) {
			cout << "Error: No Hit Pointer in drawstack." << endl;
			continue;
		}
		Double_t timestamp = drawhit->GetTimeStamp();
		if ((timestamp>currenttime+timeout) || (timestamp < currenttime)) {
			cout << "Error: Hit time " << timestamp << " out of Window " << currenttime+200 << endl;
			continue;
		}
		cout << "FairHit Timestamp: " << drawhit->GetTimeStamp() << ",  " << drawhit->GetX() << ",  " << drawhit->GetY() << ",  " << drawhit->GetZ() << endl;
		mylipse.SetX1(drawhit->GetZ());
		mylipse.SetY1(drawhit->GetY());
		//mylipse.SetR1(0.1);
		//mylipse.SetR2(0.1);
		if (TMath::Sqrt(drawhit->GetX()*drawhit->GetX()+drawhit->GetY()*drawhit->GetY())/drawhit->GetZ() > 0.3) {
			mylipse.SetLineColor(kGreen);
			mylipse.SetFillColor(kGreen);
		} else {
			mylipse.SetFillColor(kBlack);
			mylipse.SetLineColor(kBlack);
		}
		//mylipse.DrawClone();
		mylipse.DrawEllipse(drawhit->GetZ(), drawhit->GetY(), 0.3, 0.3, 0, 360, 0);
	}
}

void DrawFairHitsRZ(Double_t currenttime, TObjArray* fairhits, Double_t timeout) {
	TEllipse mylipse(0, 0, 0.2, 0.2);
	mylipse.SetFillColor(kBlack);
	FairHit* drawhit = 0;
	cout << "YZ Drawing " << fairhits->GetName() << " with " << fairhits->GetEntriesFast() << endl;
	for(int j = 0; j < fairhits->GetEntriesFast(); j++) {
		drawhit = (FairHit*) fairhits->At(j);
		if (!drawhit) {
			cout << "Error: No Hit Pointer in drawstack." << endl;
			continue;
		}
		Double_t timestamp = drawhit->GetTimeStamp();
		if ((timestamp>currenttime+timeout) || (timestamp < currenttime)) {
			cout << "Error: Hit time " << timestamp << " out of Window " << currenttime+200 << endl;
			continue;
		}
		cout << "FairHit Timestamp: " << drawhit->GetTimeStamp() << ",  " << drawhit->GetX() << ",  " << drawhit->GetY() << ",  " << drawhit->GetZ() << endl;
		Double_t r = TMath::Sqrt(drawhit->GetX()*drawhit->GetX()+drawhit->GetY()*drawhit->GetY());
		Double_t rsign = drawhit->GetY() / TMath::Abs(drawhit->GetY());
		r *= rsign;
		mylipse.SetX1(drawhit->GetZ());
		mylipse.SetY1(r);
		//mylipse.SetR1(0.1);
		//mylipse.SetR2(0.1);
		if (TMath::Sqrt(drawhit->GetX()*drawhit->GetX()+drawhit->GetY()*drawhit->GetY())/drawhit->GetZ() > 0.3) {
			mylipse.SetLineColor(kGreen);
			mylipse.SetFillColor(kGreen);
		} else {
			mylipse.SetFillColor(kBlack);
			mylipse.SetLineColor(kBlack);
		}
		//mylipse.DrawClone();
		mylipse.DrawEllipse(drawhit->GetZ(), r, 0.3, 0.3, 0, 360, 0);
	}
}

// Draws the STT isochrones
/*
Int_t currenttime: T0 to calculate the isochrone radii for
TObjArray* stthits: Array holding the PndSttHit objects
TObjArray* stttubes: Array storing the setup of the PndSttTube objects comprising the STT
*/
void DrawIsochrones(Int_t currenttime, TObjArray* stthits, TObjArray* stttubes) {
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


	cout << "----- Drawing Isochrones at " << currenttime << " with " << stthits->GetEntriesFast() << " active Hits." << endl;
	for(int j = 0; j < stthits->GetEntriesFast(); j++) {
		drawhit = (PndSttHit*) stthits->At(j);
		if (!drawhit) {
			cout << "Error: No Hit Pointer in drawstack." << endl;
			continue;
		}
		//cout << "Tube ID: " << drawhit->GetTubeID() << endl;
		tube = (PndSttTube*)(stttubes->At(drawhit->GetTubeID()));
		if (!tube) {
			cout << "Error: Hit pointed to invalid tube." << endl;
			continue;
		}
		//cout << "Calculating Isochrones." << endl;
		isochrone = drawhit->GetIsochrone();
		timestamp = drawhit->GetTimeStamp();
		if ((timestamp>currenttime+245) || (timestamp < currenttime-10)) {
			cout << "Error: Hit time " << timestamp << " out of Window " << currenttime+200 << endl;
			//continue;
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
		mylipse.SetFillStyle(0);
		mylipse.DrawClone();
	}
}

void DrawOnlineTrackVertices(Double_t currenttime, TObjArray* onlinetracks) {
	TEllipse mylipse;
	mylipse.SetFillColor(kBlack);
	mylipse.SetFillStyle(1001);
	mylipse.SetLineColor(kRed);
	mylipse.SetR1(0.3);
	mylipse.SetR2(0.3);
	PndOnlineTrack* drawtrack = 0;
	Double_t timestamp = 0;
	cout << "----- Drawing OnlineTrack Vertices at " << currenttime << " with " << onlinetracks->GetEntriesFast() << " active Hits." << endl;
	for(int j = 0; j < onlinetracks->GetEntriesFast(); j++) {
		drawtrack = (PndOnlineTrack*) onlinetracks->At(j);
		if (!drawtrack) {
			cout << "Error: No Hit Pointer in drawstack." << endl;
			continue;
		}
		// goodness for triplet hack points is 0
		if (drawtrack->DrawMode() != PndOnlineTrack::kVertexAsPoint) {
			continue;
		}
		if (drawtrack->Goodness() > 5) {
			continue;
		}
		timestamp = drawtrack->T0();
		if ((timestamp>currenttime+10) || (timestamp < currenttime-245)) {
			cout << "Error: Hit time " << timestamp << " out of Window: " << currenttime -245 << " - " << currenttime+10 << endl;
			//continue;
		}
		std::cout << "Vertex data: " << currenttime << " " << timestamp << " " << drawtrack->Vertex().X() << " " << drawtrack->Vertex().Y() << endl;
		mylipse.SetX1(drawtrack->Vertex().X());
		mylipse.SetY1(drawtrack->Vertex().Y());
		//mylipse.DrawEllipse(drawtrack->Vertex().X(),drawtrack->Vertex().Y(), 0.3, 0.3, 0, 360, 0);
		mylipse.DrawClone();
	}
}

void DrawOnlineTrackVerticesRZ(Double_t currenttime, TObjArray* onlinetracks) {
	TEllipse mylipse;
	mylipse.SetFillColor(kBlack);
	mylipse.SetFillStyle(1001);
	mylipse.SetLineColor(kRed);
	mylipse.SetR1(0.3);
	mylipse.SetR2(0.3);
	PndOnlineTrack* drawtrack = 0;
	Double_t timestamp = 0;
	cout << "----- Drawing OnlineTrack Vertices at " << currenttime << " with " << onlinetracks->GetEntriesFast() << " active Hits." << endl;
	for(int j = 0; j < onlinetracks->GetEntriesFast(); j++) {
		drawtrack = (PndOnlineTrack*) onlinetracks->At(j);
		if (!drawtrack) {
			cout << "Error: No Hit Pointer in drawstack." << endl;
			continue;
		}
		// goodness for triplet hack points is 0
		if (drawtrack->Goodness() > 5) {
			continue;
		}
		timestamp = drawtrack->T0();
		if ((timestamp>currenttime+245) || (timestamp < currenttime)) {
			cout << "Error: Hit time " << timestamp << " out of Window " << currenttime+200 << endl;
			continue;
		}
		//std::cout << "RZ Vertex data: " << currenttime << " " << timestamp << " " << drawtrack->Vertex().X() << " " << drawtrack->Vertex().Y() << " " << drawtrack->Vertex().Z() << endl;
		Double_t r = TMath::Sqrt(drawtrack->Vertex().X()*drawtrack->Vertex().X()+drawtrack->Vertex().Y()*drawtrack->Vertex().Y());
		Double_t rsign = drawtrack->Vertex().Y() / TMath::Abs(drawtrack->Vertex().Y());
		r *= rsign;
		//mylipse.SetX1(drawtrack->Vertex().Z());
		//mylipse.SetY1(r);
		mylipse.DrawEllipse(drawtrack->Vertex().Z(),r, 0.3, 0.3, 0, 360, 0);
	}
}

void DrawOnlineTrackHack(Double_t currenttime, TObjArray* onlinetracks) {
	Double_t timestamp = 0;
	cout << "----- Drawing OnlineTrack Hacks at " << currenttime << " with " << onlinetracks->GetEntriesFast() << " active Tracks." << endl;
	for(int j = 0; j < onlinetracks->GetEntriesFast(); j++) {
		drawtrack = (PndOnlineTrack*) onlinetracks->At(j);
		if (!drawtrack) {
			cout << "Error: No Hit Pointer in drawstack." << endl;
			continue;
		}
		// goodness for track hack circles is 10
		if (drawtrack->DrawMode() != PndOnlineTrack::kVertexMomentumAsCircleHack) {
			continue;
		}
		if ((drawtrack->Goodness() < 5) || (drawtrack->Goodness() > 15)) {
			cout << "Goodness abuse is deprecated. Use DrawMode instead!" << endl;
			continue;
		}
		timestamp = drawtrack->T0();
		if ((timestamp>currenttime+10) || (timestamp < currenttime-245)) {
			cout << "Error: Hit time " << timestamp << " out of Window: " << currenttime -245 << " - " << currenttime+10 << endl;
			continue;
		}
		TVector2 innerhit(drawtrack->Vertex().X(), drawtrack->Vertex().Y());
		TVector2 outerhit(drawtrack->Momentum().X(), drawtrack->Momentum().Y());
		DrawOriginTrackLipse(innerhit.X(), innerhit.Y(), outerhit.X(), outerhit.Y(), kBlue).DrawClone("only")->SetBit(kCanDelete);
	}
}

void DrawOnlineTrackHackHitCheck(Double_t currenttime, TObjArray* onlinetracks, TObjArray* stthits, TObjArray* stttubes) {
	Double_t timestamp = 0;
	cout << "----- Drawing OnlineTrack Hacks with Hit Check at " << currenttime << " with " << onlinetracks->GetEntriesFast() << " active Tracks." << endl;

	for(int j = 0; j < onlinetracks->GetEntriesFast(); j++) {
		drawtrack = (PndOnlineTrack*) onlinetracks->At(j);
		if (!drawtrack) {
			cout << "Error: No Hit Pointer in drawstack." << endl;
			continue;
		}
		// goodness for track hack circles is 10
		if (drawtrack->DrawMode() != PndOnlineTrack::kVertexMomentumAsCircleHack) {
			continue;
		}
		if ((drawtrack->Goodness() < 5) || (drawtrack->Goodness() > 15)) {
			cout << "Goodness abuse is deprecated. Use DrawMode instead!" << endl;
			continue;
		}
		timestamp = drawtrack->T0();
		if ((timestamp>currenttime+10) || (timestamp < currenttime-245)) {
			cout << "Error: Hit time " << timestamp << " out of Window: " << currenttime -245 << " - " << currenttime+10 << endl;
			continue;
		}
		TVector2 innerhit(drawtrack->Vertex().X(), drawtrack->Vertex().Y());
		TVector2 outerhit(drawtrack->Momentum().X(), drawtrack->Momentum().Y());
		TVector2 circleorigin;
		Double_t circleradius = 0;
		Int_t circleclockwise = 0;
		CalculateCircle(&innerhit, &outerhit, &circleorigin, &circleradius, &circleclockwise);
		Int_t hitcount = 0;
		for (int k = 0; k < stthits->GetEntriesFast(); k++) {
			PndSttHit* drawhit = (PndSttHit*)stthits->At(k);
			if ((drawhit->GetTubeID() < 1001) || (drawhit->GetTubeID() > 2744)) {
				PndSttTube* tube = (PndSttTube*)(stttubes->At(drawhit->GetTubeID()));
				TVector2 hitpos(tube->GetPosition().X(), tube->GetPosition().Y());
				TVector2 distvec = hitpos - circleorigin;
				TVector2 originvec = (-1)* circleorigin;
				if (TMath::Abs(distvec.Mod() - circleradius) < 0.6) {
					//cout << "Clockwise: " << circleclockwise << endl;
					if (originvec.DeltaPhi(distvec)*circleclockwise > 0) {
						hitcount++;
					}
				}
			}
		}

		if ( (hitcount > 15) || ((circleradius > 25) && (hitcount > 9)) || ((circleradius > 50) && (hitcount > 6)) ) {
			DrawOriginTrackLipse(innerhit.X(), innerhit.Y(), outerhit.X(), outerhit.Y(), kBlue).DrawClone("only")->SetBit(kCanDelete);
		} else {
			DrawOriginTrackLipse(innerhit.X(), innerhit.Y(), outerhit.X(), outerhit.Y(), kYellow).DrawClone("only")->SetBit(kCanDelete);
		}
	}
}

void DrawOnlineTrackMomentum(Double_t currenttime, TObjArray* onlinetracks) {
	Double_t timestamp = 0;
	cout << "----- Drawing OnlineTrack Momenta at " << currenttime << " with " << onlinetracks->GetEntriesFast() << " active Tracks." << endl;
	for(int j = 0; j < onlinetracks->GetEntriesFast(); j++) {
		drawtrack = (PndOnlineTrack*) onlinetracks->At(j);
		if (!drawtrack) {
			cout << "Error: No Hit Pointer in drawstack." << endl;
			continue;
		}
		// goodness for track hack circles is 20
		if (drawtrack->DrawMode() != PndOnlineTrack::kVertexMomentumAsArrow) {
			continue;
		}
		if ((drawtrack->Goodness() < 15) || (drawtrack->Goodness() > 25)) {
			cout << "Goodness abuse is deprecated. Use DrawMode instead!" << endl;
			continue;
		}
		timestamp = drawtrack->T0();
		if ((timestamp>currenttime+200) || (timestamp < currenttime)) {
			cout << "Error: Hit time " << timestamp << " out of Window " << currenttime+200 << endl;
			continue;
		}
		TVector2 vertex(drawtrack->Vertex().X(), drawtrack->Vertex().Y());
		TVector2 momentum(drawtrack->Momentum().X(), drawtrack->Momentum().Y());
		TArrow myarrow;
		myarrow.SetLineWidth(3);
		myarrow.DrawArrow(vertex.X(), vertex.Y(), vertex.X()+momentum.X(), vertex.Y()+momentum.Y(), 0.02, ">");
	}
}

void FindTriplets(TObjArray* stthits, TObjArray* stttubes, TObjArray* tripletlist, Int_t starttube, Int_t stoptube) {
	PndSttTube* tube = 0;
	PndSttHit* drawhit = 0;
	for(int j = 0; j < stthits->GetEntriesFast(); j++) {
		drawhit = (PndSttHit*) stthits->At(j);
		if (!drawhit) {
			cout << "Error: No Hit Pointer in Stack." << endl;
			continue;
		}
		//cout << "Tube ID: " << drawhit->GetTubeID() << endl;
		tube = (PndSttTube*)(stttubes->At(drawhit->GetTubeID()));
		if (!tube) {
			cout << "Error: Hit pointed to invalid tube." << endl;
			continue;
		}
		if ((drawhit->GetTubeID()>starttube) && (drawhit->GetTubeID()<stoptube)) {
			TVector2 cmspoint;
			if (HasNeighborCMS(drawhit->GetTubeID(), stthits, stttubes, &cmspoint)) {
				cout << "Triplet found. Adding to list." << endl;
				//innerlist.AddLast(tube->Clone());
				tripletlist->AddLast(cmspoint.Clone());
			}
			cout << "Drawhit finished" << endl;
		}
		cout << "End of loop " << j << endl;
	}
}

DrawTripletTracks(TObjArray* innerpoints, TObjArray* outerpoints, Int_t maxdeflection) {
	TEllipse mylipse;
	for (int inneri = 0; inneri < innerpoints->GetEntriesFast(); inneri++) {
		for (int outeri = 0; outeri < outerpoints->GetEntriesFast(); outeri++) {
			cout << "CMS Vector " << inneri << ", " << outeri << endl;
			TVector2* innerhit = (TVector2*)innerpoints->At(inneri);
			TVector2* outerhit = (TVector2*)outerpoints->At(outeri);
			cout << " Phis: " << innerhit->Phi()*TMath::RadToDeg() << ", " << outerhit->Phi()*TMath::RadToDeg() << endl;
			if (TMath::Abs(innerhit->Phi() - outerhit->Phi() ) > maxdeflection*TMath::DegToRad()) continue;
			DrawOriginTrackLipse(innerhit->X(), innerhit->Y(), outerhit->X(), outerhit->Y(), kBlue).DrawClone("only")->SetBit(kCanDelete);
			mylipse.SetFillColor(kBlack);
			mylipse.SetFillStyle(1001);
			mylipse.SetLineColor(kRed);
			mylipse.SetX1(innerhit->X());
			mylipse.SetY1(innerhit->Y());
			mylipse.SetR1(0.3);
			mylipse.SetR2(0.3);
			mylipse.DrawClone();
			mylipse.SetX1(outerhit->X());
			mylipse.SetY1(outerhit->Y());
			mylipse.SetR1(0.3);
			mylipse.SetR2(0.3);
			mylipse.DrawClone();
		}
	}
}

DrawVector2(TObjArray* points) {
	TEllipse mylipse;
	for (int i = 0; i < points->GetEntriesFast(); i++) {
		TVector2* myhit = (TVector2*)points->At(i);
		mylipse.SetFillColor(kBlack);
		mylipse.SetFillStyle(1001);
		mylipse.SetLineColor(kRed);
		mylipse.SetX1(myhit->X());
		mylipse.SetY1(myhit->Y());
		mylipse.SetR1(0.3);
		mylipse.SetR2(0.3);
		mylipse.DrawClone();
	}
}

DrawVector3(TObjArray* points) {
	TEllipse mylipse;
	for (int i = 0; i < points->GetEntriesFast(); i++) {
		TVector3* myhit = (TVector3*)points->At(i);
		mylipse.SetFillColor(kBlack);
		mylipse.SetFillStyle(1001);
		mylipse.SetLineColor(kRed);
		mylipse.SetX1(myhit->X());
		mylipse.SetY1(myhit->Y());
		mylipse.SetR1(0.3);
		mylipse.SetR2(0.3);
		mylipse.DrawClone();
	}
}

DrawVector3YZ(TObjArray* points) {
	TEllipse mylipse;
	for (int i = 0; i < points->GetEntriesFast(); i++) {
		TVector3* myhit = (TVector3*)points->At(i);
		mylipse.SetFillColor(kBlack);
		mylipse.SetFillStyle(1001);
		mylipse.SetLineColor(kRed);
		mylipse.SetX1(myhit->Z());
		mylipse.SetY1(myhit->Y());
		mylipse.SetR1(0.3);
		mylipse.SetR2(0.3);
		mylipse.DrawEllipse(myhit->Z(), myhit->Y(), 0.3, 0.3, 0, 360, 0);
	}
}

DrawVector3XZ(TObjArray* points) {
	TEllipse mylipse;
	for (int i = 0; i < points->GetEntriesFast(); i++) {
		TVector3* myhit = (TVector3*)points->At(i);
		mylipse.SetFillColor(kBlack);
		mylipse.SetFillStyle(1001);
		mylipse.SetLineColor(kRed);
		mylipse.SetX1(myhit->Z());
		mylipse.SetY1(myhit->X());
		mylipse.SetR1(0.3);
		mylipse.SetR2(0.3);
		mylipse.DrawEllipse(myhit->Z(), myhit->X(), 0.3, 0.3, 0, 360, 0);
	}
}

DrawVector3RZ(TObjArray* points) {
	TEllipse mylipse;
	for (int i = 0; i < points->GetEntriesFast(); i++) {
		TVector3* myhit = (TVector3*)points->At(i);
		mylipse.SetFillColor(kBlack);
		mylipse.SetFillStyle(1001);
		mylipse.SetLineColor(kRed);
		Double_t r = TMath::Sqrt(myhit->X()*myhit->X()+myhit->Y()*myhit->Y());
		Double_t rsign = myhit->Y() / TMath::Abs(myhit->Y());
		r *= rsign;
		mylipse.SetX1(myhit->Z());
		mylipse.SetY1(r);
		mylipse.SetR1(0.3);
		mylipse.SetR2(0.3);
		mylipse.DrawEllipse(myhit->Z(), r, 0.3, 0.3, 0, 360, 0);
	}
}

void CalculateCircle(TVector2* innerhit, TVector2* outerhit, TVector2* circleorigin, Double_t* circleradius, Int_t* clockwise) {

	Double_t x1= innerhit->X();
	Double_t y1= innerhit->Y();
	Double_t x2= outerhit->X();
	Double_t y2= outerhit->Y();
	Double_t a= x1;
	Double_t b= y1;
	Double_t c= x2;
	Double_t d= y2;
	Double_t px = 0.5*(b*c*c-a*a*d-b*b*d+b*d*d)/(b*c-a*d);
	Double_t py = 0.5*(a*a*c+b*b*c-a*c*c-a*d*d)/(b*c-a*d);
	Double_t r = TMath::Sqrt(px*px+py*py);
	circleorigin->Set(px, py);
	*circleradius = r;
	Bool_t isclockwise = (y1 > x1*y2/x2);
	if (x2 < 0) {
		isclockwise = !(isclockwise);
	}
	if (isclockwise) {
		*clockwise = 1;
	} else {
		*clockwise = -1;
	}
}

TEllipse DrawOriginTrackLipse(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Color_t linecolor = kBlack) {
	Double_t a= x1;
	Double_t b= y1;
	Double_t c= x2;
	Double_t d= y2;
	Double_t px = 0.5*(b*c*c-a*a*d-b*b*d+b*d*d)/(b*c-a*d);
	Double_t py = 0.5*(a*a*c+b*b*c-a*c*c-a*d*d)/(b*c-a*d);
	Double_t r = TMath::Sqrt(px*px+py*py);
	TEllipse mylipse(px, py, r, r);
	mylipse.SetFillStyle(0);
	mylipse.SetLineColor(linecolor);
	Double_t zeroangle = ATan2Pi(-py, -px);
	Double_t midangle = ATan2Pi(y1-py, x1-px);
	Double_t endangle = ATan2Pi(y2-py, x2-px);
	Double_t mirrorangle = zeroangle - TMath::Pi();
	Bool_t clockwise = (y1 > x1*y2/x2);
	if (x2 < 0) {
		clockwise = !(clockwise);
	}
	//cout << "Circle Parameters: " << px << " " << py << " " << x1 << " " << y1 << " " << x2 << " " << y2 << " " << TMath::RadToDeg()*zeroangle << " " << TMath::RadToDeg()*midangle << " " << TMath::RadToDeg()*endangle << " " << clockwise << endl;
	if (clockwise) {
		/*
		Double_t diffangle = zeroangle - endangle;
		if (diffangle < 0) {
			diffangle = zeroangle + 2*TMath::Pi() - endangle;
		}
		if (diffangle < TMath::Pi()) {
			endangle = zeroangle - TMath::Pi();
			if (endangle < 0) {
				endangle += 2*TMath::Pi();
			}
		}
		*/
		mylipse.SetPhimin(TMath::RadToDeg()*endangle);
		mylipse.SetPhimax(TMath::RadToDeg()*zeroangle);
		mylipse.SetPhimin(180);
		mylipse.SetPhimax(360);
	} else {
		/*
		Double_t diffangle = endangle - zeroangle;
		if (diffangle < 0) {
			diffangle = 2*TMath::Pi() - zeroangle + endangle;
		}
		if (diffangle < TMath::Pi()) {
			endangle = zeroangle + TMath::Pi();
			if (endangle > 2*TMath::Pi()) {
				endangle -= 2*TMath::Pi();
			}
		}
		*/
		mylipse.SetPhimin(TMath::RadToDeg()*zeroangle);
		mylipse.SetPhimax(TMath::RadToDeg()*endangle);
		mylipse.SetPhimin(0);
		mylipse.SetPhimax(180);
	}
	
	mylipse.SetTheta(TMath::RadToDeg()*zeroangle);
	return mylipse;
}

// Calculates distance in cm between two Straw Tubes
/*
Int_t leftID: Index of the first Tube
Int_t rightID: Index of the second Tube
TObjArray* stttubes: Array storing the setup of the PndSttTube objects comprising the STT
returns: Distance in cm between the two given tubes in xy Projection
*/
Double_t GetTubeDist(Int_t leftID, Int_t rightID, TObjArray* stttubes) {
	PndSttTube* lefttube = (PndSttTube*)(stttubes->At(leftID));
	PndSttTube* righttube = (PndSttTube*)(stttubes->At(rightID));
	if ((lefttube == 0) || (righttube == 0)) {
		cout << "Distance for invalid tubes!" << endl;
		return -1;
	}
	Double_t dx = righttube->GetPosition().X()-lefttube->GetPosition().X();
	Double_t dy = righttube->GetPosition().Y()-lefttube->GetPosition().Y();
	return (TMath::Sqrt(dx*dx+dy*dy));
}

// Determines if the selected straw has at least minneighbors which also have a hit
/*
UInt_t leftID: Index of the Tube to be tested for neighboring hits
TObjArray* stthits: Array holding the PndSttHit objects which represent active hits
TObjArray* stttubes: Array storing the setup of the PndSttTube objects comprising the STT
UInt_t minneighbors: Number of neighboring straws which must have an active hit.
*/
Bool_t HasNeighbor(UInt_t leftID, TObjArray* stthits, TObjArray* stttubes, UInt_t minneighbors=2) {
	UInt_t neighbors = 0;
	UInt_t rightID = 0;
	for (int i = 0; i < stthits->GetEntriesFast(); i++) {
		rightID = ((PndSttHit*)(stthits->At(i)))->GetTubeID();
		if (GetTubeDist(leftID, rightID, stttubes) < 1.5) {
			neighbors++;
		}
	}
	return (neighbors > 2);
}

Bool_t HasNeighborCMS(UInt_t leftID, TObjArray* stthits, TObjArray* stttubes, TVector2* cmspoint) {
	UInt_t neighbors = 0;
	UInt_t rightID = 0;
	Double_t cmsx = 0;
	Double_t cmsy = 0;
	cmspoint->Set(cmsx, cmsy);
	for (int i = 0; i < stthits->GetEntriesFast(); i++) {
		rightID = ((PndSttHit*)(stthits->At(i)))->GetTubeID();
		//cout << "Checking distance between tubes: " << leftID << ", " << rightID << endl;
		if (GetTubeDist(leftID, rightID, stttubes) < 1.1) {
			cout << "Neighbor found between tubes: " << leftID << ", " << rightID << ", " << GetTubeDist(leftID, rightID, stttubes) << endl;
			neighbors++;
			cmsx += ((PndSttTube*)(stttubes->At(rightID)))->GetPosition().X();
			cmsy += ((PndSttTube*)(stttubes->At(rightID)))->GetPosition().Y();
		}
	}
	cmspoint->Set(cmsx/neighbors, cmsy/neighbors);
	cout << "Returning CMS Point" << endl;
	if (neighbors > 2) {
		cout << "TRUE" << endl;
		return kTRUE;
	} else {
		cout << "FALSE" << endl;
		return kFALSE;
	}
}

// Prints details of the STT assembly
/*
TObjArray* stttubes: Array storing the setup of the PndSttTube objects comprising the STT
*/
void PrintSttStats(TObjArray* stttubes)
{
	TEllipse mylipse(0, 0, 1, 1);
	mylipse.SetFillStyle(0);
	TText mytext;
	mytext.SetTextSize(0.003);
	for (int i = 0; i < stttubes->GetEntriesFast(); i++) {
		tube = (PndSttTube*)(stttubes->At(i));
		if (!tube) {
			cout << "Error: No Tube at position " << i << endl;
			continue;
		}
		std::cout << i << " " << tube->GetPosition().Perp() << " " << tube->GetPosition().Phi() << " " << tube->GetPosition().X() << " " << tube->GetPosition().Y() << " " << tube->GetPosition().Z() << " " << tube->GetWireDirection().X() << " " << tube->GetWireDirection().Y() << " " << tube->GetWireDirection().Z() << std::endl;
		mylipse.DrawEllipse(tube->GetPosition().X(), tube->GetPosition().Y(), 0.5, 0.5, 0, 360, 0);
		mytext->DrawText(tube->GetPosition().X()-0.25, tube->GetPosition().Y()-0.13, TString::Format("%d",i));
	}
}

Double_t ATan2Pi(Double_t y, Double_t x) {
	Double_t angle = TMath::ATan2(y, x);
	if (angle < 0) {
		angle = 2*TMath::Pi() + angle;
	}
	return angle;
}
