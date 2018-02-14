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

// Draws list of FairHits in xy projection
/*
TObjArray* fairhits: Array of FairHit objects to be drawn in the xy projection
*/
void DrawFairHits(Double_t currenttime, TObjArray* fairhits, Double_t timeout) {
	TEllipse mylipse(0, 0, 0.2, 0.2);
	mylipse.SetFillColor(kBlack);
	FairHit* drawhit = 0;
	cout << "----- Drawing FairHits at " << currenttime << " with " << fairhits->GetEntriesFast() << " active Hits." << endl;
	for(int j = 0; j < fairhits->GetEntriesFast(); j++) {
		drawhit = (FairHit*) fairhits->At(j);
		if (!drawhit) {
			cout << "Error: No Hit Pointer in drawstack." << endl;
			continue;
		}
		Double_t timestamp = drawhit->GetTimeStamp();
		if ((timestamp>currenttime+timeout) || (timestamp < currenttime - 20)) {
			cout << "Error: Hit time " << timestamp << " out of Window " << currenttime+timeout << endl;
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
			cout << "Error: Hit time " << timestamp << " out of Window " << currenttime+245 << endl;
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
			cout << "Goodness abuse is deprecated. Use DrawMode instead!" << endl;
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

Double_t ATan2Pi(Double_t y, Double_t x) {
	Double_t angle = TMath::ATan2(y, x);
	if (angle < 0) {
		angle = 2*TMath::Pi() + angle;
	}
	return angle;
}
