#include "TStopwatch.h"
#include "TTree.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "basefunctions_ana.hxx"
//#include "FairRunAna.h"
//#include "FairParRootFileIo.h"
//#include "FairRuntimeDb.h"
//#include "PndSttTrackFinderReal.h"
//#include "PndMCTrack.h"
//#include "PndSttHit.h"
//#include "PndTrack.h"

// xy-projection of time-based MVD and STT Hits
// m.mertens@fz-juelich.de

void runTripletAna() {
	gROOT->Reset();
	// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
	Int_t iVerbose = 0;

	TStopwatch timer;
	timer.Start();
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();
	//gSystem->Load("libSttMvdTracking");

	TString simFileName = "Sim_Dpm_500.root";
	TString parFileName = "Sim_Dpm_500_params.root";
	TString digiFileName = "Sim_Dpm_500_digi.root";
	TString recoFileName = "Sim_Dpm_500_reco.root";
	TString outFileName = "Sim_Dpm_500_streamdisplay.root";

	TFile filedigi(digiFileName.Data());
	TFile filereco(recoFileName.Data());
	TFile filerecopixel(recoFileName.Data());

	TTree *treedigi = (TTree*) filedigi.Get("cbmsim");
	TClonesArray *sttsortedhits = new TClonesArray("PndSttHit");
	treedigi->SetBranchAddress("STTSortedHits",&sttsortedhits);

	TTree *mvdstriprecotree = (TTree*) filereco.Get("cbmsim");
	TClonesArray *mvdstripreco = new TClonesArray("PndSdsHit");
	mvdstriprecotree->SetBranchAddress("MVDHitsStrip",&mvdstripreco);

	TTree *mvdpixelrecotree = (TTree*) filerecopixel.Get("cbmsim");
	TClonesArray *mvdpixelreco = new TClonesArray("PndSdsHit");
	mvdpixelrecotree->SetBranchAddress("MVDHitsPixel",&mvdpixelreco);
  
	FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile(simFileName.Data());
	//fRun->AddFriend(recoFile.Data());
	fRun->AddFriend(digiFileName.Data());
	//fRun->AddFriend(trackF.Data());
	fRun->SetOutputFile(outFileName.Data());
	//fRun->RunWithTimeStamps();
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo();
	parInput1->open(parFileName.Data());
	rtdb->setFirstInput(parInput1);

	//works around the "geometry not supported by map" error
	PndSttTrackFinderReal* sttTrackFinder = new PndSttTrackFinderReal(0);
	PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
	sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
	//sttFindTracks->SetPersistence(kFALSE);
	fRun->AddTask(sttFindTracks);
	fRun->Init();

	PndGeoSttPar* fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
	PndSttMapCreator* mapper = new PndSttMapCreator(fSttParameters);
	TClonesArray* tubearray = mapper->FillTubeArray();

	c1 = new TCanvas("c1");
	c1->Range(-42,-42,42,42);
	c1->SetCanvasSize(1200, 1200);
	//TEllipse mylipse(0, 0, 0.5, 0.5);

	// single straw tube simulation -----------------------
	//PndSttSingleStraw stt;
	//setting the single straw tube simulation constants
	// 3 options currently available:
	// TConst(tube radius (cm), gas pressure (bar), Ar%, CO2%)
	// stt.TConst(0.4, 1, 0.9, 0.1); 
	// stt.TConst(0.5, 1, 0.9, 0.1);
	//stt.TConst(0.5, 2, 0.8, 0.2);
	// wire positioning   
	//stt.PutWireXYZ(0.,  0., -75., 0., 0., 75.);

	Int_t timelimit = 2200;
	//Int_t timelimit = 30;
	Int_t timewindow = 245;
	Int_t revtimewindow = 10;
	Int_t mvdtimewindow = 20;
	Int_t mvdrevtimewindow = 5;
	Int_t nexttime = 0;
	Int_t evpos = 0;
	Int_t clonespos = 0;
	Int_t stripnexttime = 0;
	Int_t stripevpos = 0;
	Int_t stripclonespos = 0;
	Int_t pixelnexttime = 0;
	Int_t pixelevpos = 0;
	Int_t pixelclonespos = 0;
	TObjArray *drawstackhits = new TObjArray();
	drawstackhits->SetName("drawstackhits");
	drawstackhits->SetOwner(kTRUE);
	TObjArray *drawstackstriphits = new TObjArray();
	drawstackstriphits->SetName("drawstackstriphits");
	drawstackstriphits->SetOwner(kTRUE);
	TObjArray *drawstackpixelhits = new TObjArray();
	drawstackpixelhits->SetName("drawstackpixelhits");
	drawstackpixelhits->SetOwner(kTRUE);
	//PndSttHit* stthit = 0;
	//PndSttTube* tube = 0;
	TText* mytext = new TText();

	treedigi->GetEntry(0);
	mvdstriprecotree->GetEntry(0);
	mvdpixelrecotree->GetEntry(0);

	//Overlapping background goes to loops when canvas is cleared
	//mylipse.DrawEllipse(-2, 0, 39, 40.5, 90, 270, 0);
	//mylipse.DrawEllipse(2, 0, 39, 40.5, 90, 270, 180);
	TEllipse* mybglipse = new TEllipse();
	mybglipse->SetFillColor(kWhite);
	Double_t xbg[8] = {2, 16, 16, 2, -2, -16, -16, -2};
	Double_t ybg[8] = {17.2, 9.2, -9.2, -17.2, 17.2, 9.2, -9.2, -17.2};
	TPolyLine* plinebg = new TPolyLine(8, xbg, ybg);
	plinebg->SetFillColor(kWhite);
	//plinebg->SetLineColor(kWhite);
	//plinebg->Draw("f");
	Double_t xr[4] = {2, 16, 16, 2};
	Double_t yr[4] = {17.2, 9.2, -9.2, -17.2};
	TPolyLine* pliner = new TPolyLine(4, xr, yr);
	//pliner->SetFillColor(kWhite);
	pliner->SetLineColor(kBlack);
	//pliner->Draw("f");
	//pliner->Draw();
	Double_t xl[4] = {-2, -16, -16, -2};
	Double_t yl[4] = {17.2, 9.2, -9.2, -17.2};
	TPolyLine* plinel = new TPolyLine(4, xl, yl);
	//plinel->SetFillColor(kWhite);
	plinel->SetLineColor(kBlack);
	//plinel->Draw("f");
	//plinel->Draw();
	Double_t xls[8] = {-2, -31.5, -31.5, -2, -2, -23.2, -23.2, -2};
	Double_t yls[8] = {35.2, 18.5, -18.5, -35.2, -25.7, -13.5, 13.5, 25.7};
	TPolyLine* plinels = new TPolyLine(8, xls, yls);
	//plinels->Draw();
	Double_t xrs[8] = {2, 31.5, 31.5, 2, 2, 23.2, 23.2, 2};
	Double_t yrs[8] = {35.2, 18.5, -18.5, -35.2, -25.7, -13.5, 13.5, 25.7};
	TPolyLine* pliners = new TPolyLine(8, xrs, yrs);
	//pliners->Draw();

	//Draw Background
	mybglipse->DrawEllipse(-2, 0, 39, 40.5, 90, 270, 0);
	mybglipse->DrawEllipse(2, 0, 39, 40.5, 90, 270, 180);
	plinebg->Draw("f");
	pliner->Draw();
	plinel->Draw();
	pliners->Draw();
	plinels->Draw();
	mybglipse->DrawEllipse(0, 0, 14, 14, 0, 360, 0);
	mybglipse->DrawEllipse(0, 0, 0.5, 0.5, 0, 360, 0);

	for (Int_t currenttime = 0; currenttime < timelimit; currenttime+=5) {
		//Get events up to mytime + timewindow
		GetHitsInTime(currenttime, timewindow, drawstackhits, nexttime, evpos, clonespos, treedigi, sttsortedhits);
		GetHitsInTime(currenttime, mvdtimewindow, drawstackstriphits, stripnexttime, stripevpos, stripclonespos, mvdstriprecotree, mvdstripreco);
		GetHitsInTime(currenttime, mvdtimewindow, drawstackpixelhits, pixelnexttime, pixelevpos, pixelclonespos, mvdpixelrecotree, mvdpixelreco);
		

		cout << drawstackhits->GetEntriesFast() << endl;
		if ((drawstackhits->IsEmpty()) && (drawstackstriphits->IsEmpty()) && (drawstackpixelhits->IsEmpty())) continue;

		//Discard events < mytime
		RemoveOldHits(currenttime, revtimewindow, drawstackhits);
		RemoveOldHits(currenttime, mvdrevtimewindow, drawstackstriphits);
		RemoveOldHits(currenttime, mvdrevtimewindow, drawstackpixelhits);

		//Calculate Isochrones and Draw
		c1->Clear();
		c1->cd();
		mybglipse->DrawEllipse(-2, 0, 39, 40.5, 90, 270, 0);
		mybglipse->DrawEllipse(2, 0, 39, 40.5, 90, 270, 180);
		plinebg->Draw("f");
		pliner->Draw();
		plinel->Draw();
		pliners->Draw();
		plinels->Draw();
		mybglipse->DrawEllipse(0, 0, 14, 14, 0, 360, 0);
		mybglipse->DrawEllipse(0, 0, 0.5, 0.5, 0, 360, 0);

		cout << " Drawing stuff " << endl;
		DrawIsochrones(currenttime, drawstackhits, tubearray);
		DrawFairHits(drawstackstriphits);
		DrawFairHits(drawstackpixelhits);

		mytext->DrawText(-42, -42, TString::Format("%d ns",currenttime));
		c1->Update();
		c1->Print("hitstream_tripletana.gif+15");
	}
	//c1->Print("hitstream.gif++");

/*
	for (int i = 0; i < tubearray->GetEntriesFast(); i++) {
		tube = (PndSttTube*)(tubearray->At(i));
		if (!tube) continue;
		mybglipse.SetX1(tube->GetPosition().X());
		mybglipse.SetY1(tube->GetPosition().Y());
		mybglipse.SetR1(0.5);
		mybglipse.SetR2(0.5);
		if ( ((i > 104) && (i < 215))
		|| ((i > 714) && (i < 855))
		|| ((i > 2956) && (i < 3175)) )
			mybglipse.DrawClone();
		std::cout << i << " " << tube->GetPosition().X() << " " << tube->GetPosition().Y() << " " << tube->GetPosition().Z() << " " << tube->GetWireDirection().X() << " " << tube->GetWireDirection().Y() << " " << tube->GetWireDirection().Z() << std::endl;
	}
*/
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
	cout << "Circle Parameters: " << px << " " << py << " " << x1 << " " << y1 << " " << x2 << " " << y2 << " " << TMath::RadToDeg()*zeroangle << " " << TMath::RadToDeg()*midangle << " " << TMath::RadToDeg()*endangle << " " << clockwise << endl;
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

void MakeTriplet(UInt_t leftID, TObjArray* stthits, TObjArray* stttubes) {
	UInt_t neighbors = 0;
	UInt_t rightID = 0;
	for (int i = 0; i < stthits->GetEntriesFast(); i++) {
		rightID = ((PndSttHit*)(stthits->At(i)))->GetTubeID();
		if (GetTubeDist(leftID, rightID, stttubes) < 1.5) {
			neighbors++;
		}
	}
}

Bool_t HasNeighbor(UInt_t leftID, TObjArray* stthits, TObjArray* stttubes) {
	UInt_t neighbors = 0;
	UInt_t rightID = 0;
	for (int i = 0; i < stthits->GetEntriesFast(); i++) {
		rightID = ((PndSttHit*)(stthits->At(i)))->GetTubeID();
		//cout << "Checking distance between tubes: " << leftID << ", " << rightID << endl;
		if (GetTubeDist(leftID, rightID, stttubes) < 1.5) {
			cout << "Neighbor found between tubes: " << leftID << ", " << rightID << ", " << GetTubeDist(leftID, rightID, stttubes) << endl;
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
		if (GetTubeDist(leftID, rightID, stttubes) < 1.5) {
			cout << "Neighbor found between tubes: " << leftID << ", " << rightID << ", " << GetTubeDist(leftID, rightID, stttubes) << endl;
			neighbors++;
			cmsx += ((PndSttTube*)(stttubes->At(rightID)))->GetPosition()->X();
			cmsy += ((PndSttTube*)(stttubes->At(rightID)))->GetPosition()->Y();
		}
	}
	cmspoint->Set(cmsx/neighbors, cmsy/neighbors);
	return (neighbors > 2);
}



Double_t ATan2Pi(Double_t y, Double_t x) {
	Double_t angle = TMath::ATan2(y, x);
	if (angle < 0) {
		angle = 2*TMath::Pi() + angle;
	}
	return angle;
}
