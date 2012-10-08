#include "TStopwatch.h"
#include "TTree.h"
#include "TFile.h"
#include "TClonesArray.h"
//#include "FairRunAna.h"
//#include "FairParRootFileIo.h"
//#include "FairRuntimeDb.h"
//#include "PndSttTrackFinderReal.h"
//#include "PndMCTrack.h"
//#include "PndSttHit.h"
//#include "PndTrack.h"

// xy-projection of time-based MVD and STT Hits
// m.mertens@fz-juelich.de

void runStreamDisplay() {
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

	TTree *mvdpixelrecotree = (TTree*) filereco.Get("cbmsim");
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
		c1->Print("hitstream.gif+15");
	}
	//c1->Print("hitstream.gif++");
/*
	for (int i = 0; i < tubearray->GetEntriesFast(); i++) {
		tube = (PndSttTube*)(tubearray->At(i));
		if (!tube) continue;
		std::cout << i << " " << tube->GetPosition().X() << " " << tube->GetPosition().Y() << " " << tube->GetPosition().Z() << " " << tube->GetWireDirection().X() << " " << tube->GetWireDirection().Y() << " " << tube->GetWireDirection().Z() << std::endl;
	}
*/
}

void GetHitsInTime(Int_t starttime, Int_t timewindow, TObjArray* outputarray, Int_t& nexttime, Int_t& entrynumber, Int_t& clonesnumber, TTree* sourcetree, TClonesArray* sourceclones) {
	FairHit* myhit = 0;
	while ((nexttime < starttime + timewindow) && (entrynumber < sourcetree->GetEntriesFast())) {
		cout << "Looping at time  " << starttime << endl;
		myhit = (FairHit*) sourceclones->At(clonesnumber);
		if (myhit) {
			nexttime = myhit->GetTimeStamp();
			cout << "Nexttime set to " << nexttime << endl;
		} else {
			cout << "FairHit Empty" << endl;
		}
		if (nexttime > starttime + timewindow) {
			cout << "Nexttime " << nexttime << " > Windowend " << starttime + timewindow << endl;
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

void RemoveOldHits(Int_t starttime, Int_t timewindow, TObjArray* outputarray) {
	cout << "Processing Draw Stack: " << outputarray->GetName() << ", " << outputarray->GetEntriesFast() << endl;
	if (outputarray->GetEntriesFast() > 0) {
		cout << "About to clean Draw Stack." << endl;
		outputarray->Compress();
		if (!(outputarray->First())) cout << "Whoops, invalid entry in outputarray." << endl;
		int i = 0;
		cout << ((FairHit*)(outputarray->First()))->GetTimeStamp() << endl;
		cout << starttime - timewindow << endl;
		for (int i = 0; i < outputarray->GetEntriesFast(); i++) {
			if (((FairHit*)(outputarray->At(i)))->GetTimeStamp() < starttime - timewindow) {
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

void DrawIsochrones(Int_t starttime, TObjArray* stthits, TObjArray* stttubes) {
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

	for(int j = 0; j < stthits->GetEntriesFast(); j++) {
		drawhit = (PndSttHit*) stthits->At(j);
		if (!drawhit) {
			cout << "Error: No Hit Pointer in drawstack." << endl;
			continue;
		}
		cout << "Tube ID: " << drawhit->GetTubeID() << endl;
		tube = (PndSttTube*)(stttubes->At(drawhit->GetTubeID()));
		if (!tube) {
			cout << "Error: Hit pointed to invalid tube." << endl;
			continue;
		}
		//cout << "Calculating Isochrones." << endl;
		isochrone = drawhit->GetIsochrone();
		timestamp = drawhit->GetTimeStamp();
		drifttime = timestamp - starttime;
		if (drifttime < 0) drifttime = 0;
		if (drifttime > 245) drifttime = 245;
		recoisochrone = stt.TimnsToDiscm(drifttime);
		if (recoisochrone < 0) recoisochrone = 0;
		if (recoisochrone > 0.5) recoisochrone = 0.5;
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

void DrawFairHits(TObjArray* fairhits) {
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
		cout << "FairHit Timestamp: " << drawhit->GetTimeStamp() << ",  " << drawhit->GetX() << ",  " << drawhit->GetY() << endl;
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

Bool_t HasNeighbor(UInt_t leftID, TObjArray* stthits, TObjArray* stttubes) {
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

Double_t GetTubeDist(Int_t leftID, Int_t rightID, TObjArray* stttubes) {
	PndSttTube* lefttube = (PndSttTube*)(stttubes->At(leftID));
	PndSttTube* righttube = (PndSttTube*)(stttubes->At(rightID));
	Double_t dx = righttube->GetPosition().X()-lefttube->GetPosition().X();
	Double_t dy = righttube->GetPosition().Y()-lefttube->GetPosition().Y();
	return (TMath::Sqrt(dx*dx+dy*dy));
}

