/*
 * PndSttCellTrackingTestTask.cxx
 *
 *  Created on: Aug 01, 2015
 *      Author: Walter Ikegami Andersson
 */

#include <FairEventHeader.h>
#include <FairRootManager.h>
#include <PndRiemannTrack.h>
#include <PndSttCellTrackingTestTask.h>
#include <Rtypes.h>
#include <TArc.h>
#include <TAttFill.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TGenericClassInfo.h>
#include <TLine.h>
#include <TObjArray.h>
#include <TPad.h>
#include <TString.h>
#include <iostream>
#include <boost/tuple/tuple.hpp>

#include "TGraph.h"
#include "TAxis.h"
#include "TText.h"
#include "TColor.h"
#include "TPaveLabel.h"
#include "TArrow.h"
#include "PndSttMapCreator.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TMultiGraph.h"

#include "PndGeoSttPar.h"

#include "TVector2.h"
#include "TVector3.h"

#include "PndSttTube.h"

#include "FairMultiLinkedData.h"
ClassImp (PndSttCellTrackingTestTask);

using namespace std;

PndSttCellTrackingTestTask::PndSttCellTrackingTestTask() {
	FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
	fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

PndSttCellTrackingTestTask::~PndSttCellTrackingTestTask() {
}

//For sorting skew straws according to layer
struct sort_pred {
    bool operator()(const std::pair<PndSttHit*,int> &left, const std::pair<PndSttHit*,int> &right) {
        return left.second < right.second;
    }
};

InitStatus PndSttCellTrackingTestTask::Init() {
	FairRootManager *ioman = FairRootManager::Instance();

	if (!ioman) {
		cout << "-E- PndSttCellTrackingTestTask::Init: RootManager not instantiated!" << endl;
		return kFATAL;
	}

	fEventHeader = (TClonesArray*) ioman->GetObject("EventHeader.");
	if (!fEventHeader) {
		cout
			<< "-W-  PndSttCellTrackFinderAnalysisTask::Init: No EventHeader array! Needed for EventNumber"
			<< endl;
		return kERROR;
	}

	fFirstTrackCand = (TClonesArray*) ioman->GetObject("FirstTrackCand");
	fFirstRiemannTrack = (TClonesArray*) ioman->GetObject("FirstRiemannTrack");
	fCombiTrackCand = (TClonesArray*) ioman->GetObject("CombiTrackCand");
	fCombiTrack = (TClonesArray*) ioman->GetObject("CombiTrack");
	fCombiRiemannTrack = (TClonesArray*) ioman->GetObject("CombiRiemannTrack");


	fSTTHits = (TClonesArray*) ioman->GetObject("STTHit");
	if ( ! fSTTHits ) {
		cout << "-W- PndSttCellTrackingTestTask::Init: "
		<< "fSTTHits not instantiated, return!" << endl;
		return kERROR;
	}

	// open MVD pixel hit array
	fMvdPixelHitArray = (TClonesArray*) ioman->GetObject("MVDHitsPixel");
	if(!fMvdPixelHitArray) {
		Error("PndSttCellTrackingTestTask:Init","MVD pixel hit - array not found!");
		return kERROR;
	}

	// open MVD strip hit array
	fMvdStripHitArray = (TClonesArray*) ioman->GetObject("MVDHitsStrip");
	if(!fMvdStripHitArray) {
		Error("PndSttCellTrackingTestTask:Init","MVD strip hit - array not found!");
	    return kERROR;
	}

	fFinalTrackCand = ioman->Register("FinalTrackCand", "PndTrackCand", "STT", kTRUE);
	fFinalTrack = ioman->Register("FinalTrack", "PndTrack", "STT", kTRUE);

	// get tube-array
	PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
	fTubeArray = mapper->FillTubeArray();

	fSteps = 360.;
	fVerbose = 1;	//set to 1 to enable printout
	fDraw = true;
	return kSUCCESS;
}

void PndSttCellTrackingTestTask::Exec() {
	FairEventHeader* myEventHeader = (FairEventHeader*) fEventHeader;
	Int_t eventNumber;
	eventNumber = myEventHeader->GetMCEntryNumber();
	// init fMapTubeIDToHits
	if (fVerbose > 0){
		cout<<"|--- PndSttCellTrackingTestTask::Exec - event: "<<eventNumber<<" ---|"<<endl;
	}
	PndSttHit* hit;
	for (int j = 0; j < fSTTHits->GetEntriesFast(); ++j) {
		hit = (PndSttHit*) fSTTHits->At(j);
		fMapTubeIDToHits[hit->GetTubeID()].push_back(
		hit->GetLink(0).GetIndex());
		fMapHitIndexToTubeID[hit->GetLink(0).GetIndex()] = hit->GetTubeID();
	}

	//if (eventNumber == 9) {//==3 good in jpsi files,
		ExtractSkewedHits(eventNumber);
	//}//End single event if statement




}

void PndSttCellTrackingTestTask::FinishEvent() {

	fCombiTrackCand->Delete();
	fCombiTrack->Delete();
	fCombiRiemannTrack->Delete();

	fVectorPndTrackCand.clear();
	fVectorPndTrack.clear();

	fFinalTrackCand->Delete();
	fFinalTrack->Delete();

}

void PndSttCellTrackingTestTask::ExtractSkewedHits(Int_t eventNumber) {
	vector<vector<pair<PndSttHit*,int> > > Skewhitspair;
	vector<vector<PndSttHit*> > AllSkewedHits;
	vector<PndRiemannTrack*> AllRiemannTracks;
	vector<PndTrack*> AllTracks;
	vector<PndSttHit> firstlasthit;
//	int curHitIndex;
//	int hitCounter = 0;
	TVector3 wireDirection;

	//Draw stuff

	if (fDraw) {
		SttXYproj = new TCanvas();
		SttXYproj->SetCanvasSize(1000, 1000);//1000,1000
		SttXYproj->SetTitle("SttXYproj Event: "+TString::Itoa(eventNumber,10));
//		Double_t w = gPad->GetWw()*gPad->GetAbsWNDC();
//		Double_t h = gPad->GetWh()*gPad->GetAbsHNDC();
		Double_t xmin = -42;//-42 -25
		Double_t xmax = 42;//42 -5
		Double_t ymin = -42;//-42 10
		Double_t ymax = 42;//42 40
		SttXYproj->SetFixedAspectRatio();
		SttXYproj->Range(xmin,ymin,xmax,ymax);
		SttXYproj->cd();
		DrawStt(false);//DRAWING STT HERE!!!!
		//Mark origin
		TLine* xLine = new TLine(-50, 0, 50, 0);
		TLine* yLine = new TLine(0, 50, 0, -50);
		xLine->Draw("SAME");
		yLine->Draw("SAME");
	}

	//Loop over all tracks
	for (int i = 0; i < fCombiTrackCand->GetEntriesFast(); ++i) {
		PndTrackCand* myTrackCand = (PndTrackCand*) fCombiTrackCand->At(i);
		AllRiemannTracks.push_back( (PndRiemannTrack*) (fCombiRiemannTrack->At(i)) );
		AllTracks.push_back((PndTrack*) (fCombiTrack->At(i)));
		vector< PndTrackCandHit > tracklist = myTrackCand->GetSortedHits();
		vector<PndSttHit*> skewHits;
		vector<pair<PndSttHit*,int> > hitPairs;
		for (unsigned int j = 0; j < tracklist.size(); ++j)
		{
			PndSttHit* hit = (PndSttHit*) fSTTHits->At(tracklist.at(j).GetHitId());
			PndSttTube* myTube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
			if (myTube->IsSkew()) {
				wireDirection = myTube->GetWireDirection();
				//skewHits.push_back(hit);
				hitPairs.push_back(pair<PndSttHit*,int> (hit,hit->GetTubeID()));
				if (fDraw) DrawSkewTubeLines(hit);	//Draw skewed hits
			}
			else {
				if (fDraw) DrawHitIsochrone(hit, kMagenta+2);	//Draw parallel hit isochrone
			}
			std::sort(hitPairs.begin(), hitPairs.end(), sort_pred());
		}

		vector<PndSttHit*> skewHits2;
		for (unsigned int j = 0; j < hitPairs.size();j++){
			skewHits2.push_back(hitPairs.at(j).first);
		}

		AllSkewedHits.push_back(skewHits2);

		//TODO: Order skewed hits in increasing order of Skew layer
	}

	//Loop over all tracks
	for (int i = 0; i < fCombiTrackCand->GetEntriesFast(); ++i) {
		//if (i != 1) continue;//Check only one track
		if (fCombiTrackCand->GetEntriesFast()>10) {
			cout<<"Too many tracks, unrealistic ..."<<endl;
			break;
		}
		if (fDraw) DrawTrackParams(AllTracks.at(i));

		vector< vector<TVector2> > ZPhiPairVector;
		Double_t xnew = AllRiemannTracks.at(i)->orig()[0];
		Double_t ynew = AllRiemannTracks.at(i)->orig()[1];
		Double_t rnew = AllRiemannTracks.at(i)->r();

		if (fDraw) {
			TArc *riecirc = new TArc(xnew, ynew, rnew);
			riecirc->SetFillStyle(0);
			riecirc->Draw("SAME");
		}

		TVector2 riemanncirclenew(xnew,ynew);
		if (fVerbose >0) cout<<"ExtractSkewedHits - Correcting hits for track: "<<i<<endl;
		vector< vector<PndSttHit> > output = MoveSkewedHitsToCircle2(riemanncirclenew,rnew,AllSkewedHits.at(i));//CHANGE FUNCTION HERE
		if (output.empty()) {
			cout<<"ExtractSkewedHits - No hits in track: "<<i<<" were adjusted!"<<endl;
			continue;
		}

		for (unsigned int j = 0; j < output.size(); ++j)//Loop over all stt hits
		{
			vector<TVector2> ZPhiPair;
			if (output.at(j).empty()) {
				cout<<"ExtractSkewedHits - Hit: "<<j<<" in track: "<<i<<  "was not adjusted adjusted!"<<endl;
				continue;
			}
			cout<<"New Track: "<<j<<endl;
			//cout<<"ExtractSkewedHits - New list of adjusted hits"<<endl;
			for (unsigned int k = 0; k < output.at(j).size(); ++k)//Loop over all possible adjusted hits for one hit
			{
				Double_t x0 = output.at(j).at(k).GetX() - xnew;
				Double_t y0 = output.at(j).at(k).GetY() - ynew;
				TVector2 pos(x0,y0);

				//Double_t phi = pos.Phi()*TMath::RadToDeg();//CHange back to radian
				//TVector2 ZPhi(output.at(j).at(k).GetZ(),phi);	//Add back *rnew Should be changed to circular travel distance starting from s=0 at first hit

				Double_t phi = pos.Phi();
				TVector2 ZPhi(output.at(j).at(k).GetZ(),phi*rnew);
				if (fVerbose > -1 ) cout<<"ExtractSkewedHits - SkewHit Z: "<<output.at(j).at(k).GetZ()<<" Phi: "<<phi<<endl;


				ZPhiPair.push_back(ZPhi);
			}
			ZPhiPairVector.push_back(ZPhiPair);
		}



		if (ZPhiPairVector.size() < 3) {
			cout<<"<=2 skewed straw hits, Cannot continue!"<<endl;
			continue;
		}

		vector<TVector2> TrueZPhi = LineCombiIsoFinder2(ZPhiPairVector);


		//TH2D *HoughSpace = new TH2D("HoughSpace","HoughSpace", 360, 0, 360, 200, -200, 200);//For HoughIsoFinder
		//TH2D *HoughSpace = new TH2D("HoughSpace","HoughSpace", 180, 0, 180, 200, -100, 200);//ForHoughIsoFinder2
		//TVector2 resultparams;

		//vector<TVector2> TrueZPhi = HoughTrueIsoFinder(ZPhiPairVector, HoughSpace, resultparams);

/*
		//Start drawing HoughSpace
		HoughSpace->SetStats(kFALSE);
		HoughSpace->SetTitle("");
		HoughSpace->GetXaxis()->SetTitle("#theta /Deg");
		HoughSpace->GetXaxis()->SetTitleSize(0.06);
		HoughSpace->GetXaxis()->SetTitleOffset(0.5);
		HoughSpace->GetXaxis()->SetLabelOffset(-0.01);
		HoughSpace->GetXaxis()->SetLabelSize(0.055);
		HoughSpace->GetYaxis()->SetTitle("r /cm  ");
		HoughSpace->GetYaxis()->SetTitleSize(0.06);
		HoughSpace->GetYaxis()->SetTitleOffset(0.5);
		HoughSpace->GetYaxis()->SetLabelOffset(0);
		HoughSpace->GetYaxis()->SetLabelSize(0.055);
*/

		if (fDraw) {
			TCanvas *HoughCanvas = new TCanvas();
			HoughCanvas->SetTitle("z - S projection Track: "+TString::Itoa(i,10));
			HoughCanvas->SetCanvasSize(350, 500);

			//HoughCanvas->Divide(2,1);
			//HoughCanvas->cd(2);
			//HoughSpace->Draw("colz");
			//HoughCanvas->cd(1);
			//HoughCanvas->SetGrid();


			//Print z-phi hits

			//Used for plotting in TGraph
			double x[ZPhiPairVector.size()*2], y[ZPhiPairVector.size()*2];
			int l=0;
			for (unsigned int j=0;j<ZPhiPairVector.size();j++) {
				for (int k=0;k<2;k++) {
					x[l] = ZPhiPairVector.at(j).at(k).X();
					y[l] = ZPhiPairVector.at(j).at(k).Y();
					l++;
				}
			}

			TGraph *ZPhiGraph = new TGraph(ZPhiPairVector.size()*2,x,y);
			ZPhiGraph->SetMarkerStyle(3);
			ZPhiGraph->SetMarkerColor(kRed);
			ZPhiGraph->SetMarkerSize(2);
			double xtrue[TrueZPhi.size()], ytrue[TrueZPhi.size()];
			for (Int_t j=0;j<(int)TrueZPhi.size();j++) {
				xtrue[j] = TrueZPhi.at(j).X();
				ytrue[j] = TrueZPhi.at(j).Y();
			}

			TGraph *TrueZPhiGraph = new TGraph(TrueZPhi.size(),xtrue,ytrue);
			TrueZPhiGraph->SetMarkerStyle(4);
			TrueZPhiGraph->SetMarkerSize(2);
			TrueZPhiGraph->SetMarkerColor(kBlue);


			//double truelinex[2], trueliney[2];
			//truelinex[0]=TrueZPhi.at(0).X();
			//trueliney[0]=TrueZPhi.at(0).X()*lineparams.X()+lineparams.Y();
			//truelinex[1]=TrueZPhi.at(TrueZPhi.size()-1).X();
			//trueliney[0]=TrueZPhi.at(TrueZPhi.size()-1).X()*lineparams.X()+lineparams.Y();
			//TGraph *trueline = new TGraph(2,truelinex,trueliney);
			//trueline->SetMarkerColor(kGreen);
			//trueline->SetDrawOption("L");
			//trueline->Fit("pol1","FQ");


			TMultiGraph *mg = new TMultiGraph();
			mg->SetTitle("");
			mg->Add(ZPhiGraph);
			//mg->Add(TrueZPhiGraph);
			//mg->Add(trueline);
			mg->Draw("ap");
			//mg->SetTitle("Skewed hits position in z-#phi");
			mg->GetXaxis()->SetTitle("z /cm");
			mg->GetXaxis()->SetTitleSize(0.06);
			mg->GetXaxis()->SetTitleOffset(0.5);
			mg->GetXaxis()->SetLabelOffset(-0.01);
			mg->GetXaxis()->SetLabelSize(0.055);
			//mg->GetYaxis()->SetTitle("#phi /Deg    ");//Change back to ("R*#phi /cm")
			mg->GetYaxis()->SetTitle("Arch length /cm");
			mg->GetYaxis()->SetTitleSize(0.06);
			mg->GetYaxis()->SetTitleOffset(0.5);
			mg->GetYaxis()->SetLabelOffset(0);
			mg->GetYaxis()->SetLabelSize(0.055);

/*
			TLine* ALine = new TLine();
			double deg = 45.*TMath::DegToRad();
			double length =6.;
			ALine->DrawLine(ZPhiPairVector.at(4).at(0).X()-length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(4).at(0).X()+length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()+length*TMath::Sin(deg));
			//ALine->DrawLine(ZPhiPairVector.at(5).at(1).X()-length*TMath::Cos(deg), ZPhiPairVector.at(5).at(1).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(5).at(1).X()+length*TMath::Cos(deg), ZPhiPairVector.at(5).at(1).Y()+length*TMath::Sin(deg));
			//ALine->DrawLine(ZPhiPairVector.at(6).at(1).X()-length*TMath::Cos(deg), ZPhiPairVector.at(6).at(1).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(6).at(1).X()+length*TMath::Cos(deg), ZPhiPairVector.at(6).at(1).Y()+length*TMath::Sin(deg));
			deg=0;
			length=8;
			ALine->DrawLine(ZPhiPairVector.at(4).at(0).X()-length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(4).at(0).X()+length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()+length*TMath::Sin(deg));
			//ALine->DrawLine(ZPhiPairVector.at(5).at(1).X()-length*TMath::Cos(deg), ZPhiPairVector.at(5).at(1).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(5).at(1).X()+length*TMath::Cos(deg), ZPhiPairVector.at(5).at(1).Y()+length*TMath::Sin(deg));
			//ALine->DrawLine(ZPhiPairVector.at(6).at(1).X()-length*TMath::Cos(deg), ZPhiPairVector.at(6).at(1).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(6).at(1).X()+length*TMath::Cos(deg), ZPhiPairVector.at(6).at(1).Y()+length*TMath::Sin(deg));
			deg=90.*TMath::DegToRad();
			length=5;
			ALine->DrawLine(ZPhiPairVector.at(4).at(0).X()-length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(4).at(0).X()+length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()+length*TMath::Sin(deg));
			//ALine->DrawLine(ZPhiPairVector.at(5).at(1).X()-length*TMath::Cos(deg), ZPhiPairVector.at(5).at(1).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(5).at(1).X()+length*TMath::Cos(deg), ZPhiPairVector.at(5).at(1).Y()+length*TMath::Sin(deg));
			//ALine->DrawLine(ZPhiPairVector.at(6).at(1).X()-length*TMath::Cos(deg), ZPhiPairVector.at(6).at(1).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(6).at(1).X()+length*TMath::Cos(deg), ZPhiPairVector.at(6).at(1).Y()+length*TMath::Sin(deg));
			deg=135.*TMath::DegToRad();
			length=6;
			ALine->DrawLine(ZPhiPairVector.at(4).at(0).X()-length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(4).at(0).X()+length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()+length*TMath::Sin(deg));
			//ALine->DrawLine(ZPhiPairVector.at(5).at(1).X()-length*TMath::Cos(deg), ZPhiPairVector.at(5).at(1).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(5).at(1).X()+length*TMath::Cos(deg), ZPhiPairVector.at(5).at(1).Y()+length*TMath::Sin(deg));
			//ALine->DrawLine(ZPhiPairVector.at(6).at(1).X()-length*TMath::Cos(deg), ZPhiPairVector.at(6).at(1).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(6).at(1).X()+length*TMath::Cos(deg), ZPhiPairVector.at(6).at(1).Y()+length*TMath::Sin(deg));

			deg=22.5*TMath::DegToRad();
			length=7;
			ALine->DrawLine(ZPhiPairVector.at(4).at(0).X()-length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(4).at(0).X()+length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()+length*TMath::Sin(deg));
			deg=-22.5*TMath::DegToRad();
			length=7;
			ALine->DrawLine(ZPhiPairVector.at(4).at(0).X()-length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(4).at(0).X()+length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()+length*TMath::Sin(deg));

			deg=11.25*TMath::DegToRad();
			length=7.5;
			ALine->DrawLine(ZPhiPairVector.at(4).at(0).X()-length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(4).at(0).X()+length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()+length*TMath::Sin(deg));
			deg=-11.25*TMath::DegToRad();
			length=7.5;
			ALine->DrawLine(ZPhiPairVector.at(4).at(0).X()-length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()-length*TMath::Sin(deg), ZPhiPairVector.at(4).at(0).X()+length*TMath::Cos(deg), ZPhiPairVector.at(4).at(0).Y()+length*TMath::Sin(deg));
*/

			//DrawCombiLines(ZPhiPairVector);	//DRAW COMBI LINES HERE!!
			//DrawCombiLinesResult(TrueZPhi);
/*
			TLine* BLine = new TLine(0,0,0,0);
			BLine->SetLineWidth(3);
			double CosAngle, Angle;
			TVector2 v1, v2;

			int asdf = 4;
			int j = 0;
			int k = 1;
			int k2=1;
			BLine->SetLineColor(kBlue);

			//BLine->DrawLine(ZPhiPairVector.at(asdf+1).at(k).X(), ZPhiPairVector.at(asdf+1).at(k).Y(), ZPhiPairVector.at(asdf).at(j).X(), ZPhiPairVector.at(asdf).at(j).Y());
			//BLine->DrawLine(ZPhiPairVector.at(asdf+2).at(k2).X(), ZPhiPairVector.at(asdf+2).at(k2).Y(), ZPhiPairVector.at(asdf+1).at(k).X(), ZPhiPairVector.at(asdf+1).at(k).Y());
			v1 = ZPhiPairVector.at(asdf+1).at(k) - ZPhiPairVector.at(asdf).at(j);
			v2 = ZPhiPairVector.at(asdf+1).at(k) - ZPhiPairVector.at(asdf+2).at(k2);
			CosAngle = v1*v2/(TMath::Sqrt(v1*v1)*TMath::Sqrt(v2*v2));
			Angle = TMath::ACos(CosAngle) * 180./TMath::Pi();
			if (fVerbose >0) cout<<"Angle line 1: "<<Angle<<endl;

			asdf = 2;
			j = 1;
			k = 1;
			k2=1;
			//BLine->SetLineColor(kGreen+1);

			//BLine->DrawLine(ZPhiPairVector.at(asdf+1).at(k).X(), ZPhiPairVector.at(asdf+1).at(k).Y(), ZPhiPairVector.at(asdf).at(j).X(), ZPhiPairVector.at(asdf).at(j).Y());
			//BLine->DrawLine(ZPhiPairVector.at(asdf+2).at(k2).X(), ZPhiPairVector.at(asdf+2).at(k2).Y(), ZPhiPairVector.at(asdf+1).at(k).X(), ZPhiPairVector.at(asdf+1).at(k).Y());
			v1 = ZPhiPairVector.at(asdf+1).at(k) - ZPhiPairVector.at(asdf).at(j);
			v2 = ZPhiPairVector.at(asdf+1).at(k) - ZPhiPairVector.at(asdf+2).at(k2);
			CosAngle = v1*v2/(TMath::Sqrt(v1*v1)*TMath::Sqrt(v2*v2));
			Angle = TMath::ACos(CosAngle) * 180./TMath::Pi();
			if (fVerbose >0) cout<<"Angle line 2: "<<Angle<<endl;

			asdf = 0;
			j = 0;
			k = 0;
			k2=0;
			//BLine->SetLineColor(kRed);

			//BLine->DrawLine(ZPhiPairVector.at(asdf+1).at(k).X(), ZPhiPairVector.at(asdf+1).at(k).Y(), ZPhiPairVector.at(asdf).at(j).X(), ZPhiPairVector.at(asdf).at(j).Y());
			//BLine->DrawLine(ZPhiPairVector.at(asdf+2).at(k2).X(), ZPhiPairVector.at(asdf+2).at(k2).Y(), ZPhiPairVector.at(asdf+1).at(k).X(), ZPhiPairVector.at(asdf+1).at(k).Y());
			v1 = ZPhiPairVector.at(asdf+1).at(k) - ZPhiPairVector.at(asdf).at(j);
			v2 = ZPhiPairVector.at(asdf+1).at(k) - ZPhiPairVector.at(asdf+2).at(k2);
			CosAngle = v1*v2/(TMath::Sqrt(v1*v1)*TMath::Sqrt(v2*v2));
			Angle = TMath::ACos(CosAngle) * 180./TMath::Pi();
			if (fVerbose >0) cout<<"Angle line 3: "<<Angle<<endl;
*/

			gPad->Modified();

			SttXYproj->cd();
		}


		TVector2 lineparams = PzLineFitExtract(TrueZPhi);
		//HoughSpace->Clear();

		//CreatePndTrack(tana);

		FairTrackParP trackParamFirst = AllTracks.at(i)->GetParamFirst();
		FairTrackParP trackParamLast = AllTracks.at(i)->GetParamLast();

		Double_t pz = TMath::Sqrt(trackParamFirst.GetPx()*trackParamFirst.GetPx()+trackParamFirst.GetPy()*trackParamFirst.GetPy());
		pz = pz/(lineparams.Y());
		if (fVerbose > 0) cout<<"circle radius: "<<rnew<<endl;



		//Make new trackParamFirst
		TVector2 firstPos(trackParamFirst.GetX() - xnew, trackParamFirst.GetY() - ynew);
		Double_t firstPhi = firstPos.Phi()*rnew;//*TMath::RadToDeg();
		TVector3 firstHit(trackParamFirst.GetX(),trackParamFirst.GetY(),(firstPhi - lineparams.X())/lineparams.Y());
		TVector3 firstHitError(trackParamFirst.GetDX(),trackParamFirst.GetDY(),2);
		if (fVerbose > 0) cout<<"First hit Phi*R: "<<firstPhi<<endl;
		if (fVerbose > 0) cout<<"New First position: "<<firstHit.X()<<" "<<firstHit.Y()<<" "<<firstHit.Z()<<endl;
        TVector3 originFirst(firstHit);

		TVector3 firstMom(trackParamFirst.GetPx(),trackParamFirst.GetPy(),pz);
		if (fVerbose > 0) cout<<"New First momentum vector: "<<firstMom.X()<<" "<<firstMom.Y()<<" "<<firstMom.Z()<<endl;

		//Make new trackParamLast
		TVector2 lastPos(trackParamLast.GetX() - xnew, trackParamLast.GetY() - ynew);
		Double_t lastPhi = lastPos.Phi()*rnew;//*TMath::RadToDeg();
		TVector3 lastHit(trackParamLast.GetX(),trackParamLast.GetY(),(lastPhi - lineparams.X())/lineparams.Y());
		TVector3 lastHitError(trackParamLast.GetDX(),trackParamLast.GetDY(),2);
		if (fVerbose > 0) cout<<"Last hit Phi*R: "<<lastPhi<<endl;
		if (fVerbose > 0) cout<<"New Last position: "<<lastHit.X()<<" "<<lastHit.Y()<<" "<<lastHit.Z()<<endl;
        TVector3 originLast(lastHit);

		TVector3 lastMom(trackParamLast.GetPx(),trackParamLast.GetPy(),pz);
		if (fVerbose > 0) cout<<"New Last momentum vector: "<<lastMom.X()<<" "<<lastMom.Y()<<" "<<lastMom.Z()<<endl;

		//Make PndTrack
        TVector3 momError(0.02, 0.02, 0.02);
        TVector3 dj(1,0,0);
        TVector3 dk(0,1,0);

		FairTrackParP trackParamFirstNew(firstHit,firstMom,firstHitError,momError,trackParamFirst.GetQ(),firstHit,dj,dk);
		FairTrackParP trackParamLastNew(lastHit,lastMom,lastHitError,momError,trackParamLast.GetQ(),lastHit,dj,dk);

		PndTrackCand myCand = *(PndTrackCand*) fCombiTrackCand->At(i);
		PndTrack myTrack(trackParamFirstNew,trackParamLastNew,myCand);

		fVectorPndTrackCand.push_back(myCand);
		fVectorPndTrack.push_back(myTrack);


	}

	for (unsigned int i = 0; i < fVectorPndTrack.size(); ++i) {
		PndTrackCand* Cand = new ((*fFinalTrackCand)[i]) PndTrackCand(fVectorPndTrackCand.at(i));
		PndTrack* Track = new ((*fFinalTrack)[i]) PndTrack(fVectorPndTrack.at(i));
		Track->SetLinks(*Cand->GetPointerToLinks());
		Track->SetTrackCandRef(Cand);
		Track->SetTrackCand(*Cand);
	}


}

void PndSttCellTrackingTestTask::DrawStt(bool isskew) {
	TArc *riecirc = new TArc(0., 0., 41.);
	riecirc->SetFillStyle(0);
	riecirc->Draw("SAME");
	PndSttTube* tube;
	TVector3 pos;
	double r,x,y;
	//double z;
	for (int j = 0; j < fTubeArray->GetEntriesFast(); ++j) {
		if (!fTubeArray->At(j)) continue;
		tube = (PndSttTube*) fTubeArray->At(j);
		pos = tube->GetPosition();
		x = pos.X();
		y = pos.Y();
		z = pos.Z();
		if(tube->IsParallel()){
			r = tube->GetRadIn();
			TArc *circ = new TArc(x, y, r);
			circ->SetFillStyle(0);
			circ->SetLineColor(kGreen-8);
			circ->Draw("SAME");
		}
		else if (isskew && tube->IsSkew()) {
			if (tube->GetLayerID() == 10 || tube->GetLayerID() == 11 || tube->GetLayerID() == 14 ||tube->GetLayerID() == 15) {
				r = tube->GetRadIn();
				TArc *circ = new TArc(x, y, r);
				circ->SetFillStyle(0);
				circ->SetLineColor(kRed);
				circ->Draw("SAME");
			}
			else{
				r = tube->GetRadIn();
				TArc *circ = new TArc(x, y, r);
				circ->SetFillStyle(0);
				circ->SetLineColor(kBlue);
				circ->Draw("SAME");
			}
		}
	}

	TText *xa = new TText(30,0.1,"X-axis");
	xa->SetTextAlign(11);
	xa->SetTextSize(0.05);
	xa->Draw("SAME");
	TText *ya = new TText(-0.1,30,"Y-axis");
	ya->SetTextAlign(11);
	ya->SetTextAngle(90);
	ya->SetTextSize(0.05);
	ya->Draw("SAME");
	TText *le = new TText(20,-12,"41 cm");
	le->SetTextAlign(11);
	le->SetTextAngle(-31);
	le->SetTextSize(0.05);
	le->Draw("SAME");
	TArrow *firstLine = new TArrow(0., 0., 35., -TMath::Sqrt(41.*41. - 35.*35.), 0.9,"|>");
	firstLine->SetLineColor(kBlack);
	firstLine->SetLineWidth(3);
	firstLine->Draw("SAME");
}
void PndSttCellTrackingTestTask::DrawSkewTubeLines(PndSttHit* hit) {
	PndSttTube* myTube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
	Double_t tubeLengthHalf = myTube->GetHalfLength();
	TVector3 wireDirection = myTube->GetWireDirection();
	TVector3 tubePosition = myTube->GetPosition();
	TVector3 startPoint = tubePosition - (tubeLengthHalf * wireDirection);
	TVector3 endPoint = tubePosition + (tubeLengthHalf * wireDirection);

	TLine* tubeLine = new TLine(startPoint.X(), startPoint.Y(), endPoint.X(), endPoint.Y());
	tubeLine->Draw("SAME");
}
void PndSttCellTrackingTestTask::DrawHitIsochrone(PndSttHit* hit, Color_t color) {
	Double_t isor = hit->GetIsochrone();
	Double_t isox = hit->GetX();
	Double_t isoy = hit->GetY();
	TArc *circ = new TArc(isox, isoy, isor);
	circ->SetFillStyle(0);
	circ->SetLineColor(color);
	circ->Draw("SAME");
}
void PndSttCellTrackingTestTask::DrawHitSkewedIsochrone(PndSttHit* hit, Color_t color, Double_t beta, Double_t a, Double_t b) {
//	Double_t isor = hit->GetIsochrone();
	Double_t isox = hit->GetX();
	Double_t isoy = hit->GetY();
	TEllipse *ell = new TEllipse(isox, isoy, a, b, 0, 360, -beta);
	ell->SetFillStyle(0);
	ell->SetLineColor(color);
	ell->Draw("SAME");
}
void PndSttCellTrackingTestTask::DrawTrackParams(PndTrack* track) {
	FairTrackParP trackParamFirst = track->GetParamFirst();
	FairTrackParP trackParamLast = track->GetParamLast();
	Double_t xfirst = trackParamFirst.GetX();
	Double_t yfirst = trackParamFirst.GetY();
	TVector2 Ptfirst(trackParamFirst.GetPx(),trackParamFirst.GetPy());
	Ptfirst = Ptfirst.Unit();
	Double_t xlast = trackParamLast.GetX();
	Double_t ylast = trackParamLast.GetY();
	TVector2 Ptlast(trackParamLast.GetPx(),trackParamLast.GetPy());
	Ptlast = Ptlast.Unit();
	TArrow *firstLine = new TArrow(xfirst, yfirst, xfirst+Ptfirst.X(), yfirst+Ptfirst.Y(), 1,"|>");
	firstLine->SetLineColor(kRed);
	firstLine->SetLineWidth(2);
	TArrow *lastLine = new TArrow(xlast, ylast, xlast+Ptlast.X(), ylast+Ptlast.Y(), 1,"|>");
	lastLine->SetLineColor(kBlue);
	lastLine->SetLineWidth(2);
	firstLine->Draw("SAME");
	lastLine->Draw("SAME");
}
/*
void PndSttCellTrackingTestTask::DrawCombiLines(vector<vector< TVector2> > ZPhiPairVector){
	for (int i = 0; i < ZPhiPairVector.size()-1; i++){//Loop over straws
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){//Loop over all Z-phi in straw i-1
			for (int k = 0; k < ZPhiPairVector.at(i+1).size(); k++){//Loop over all Z-phi in straw i
				TLine* ALine = new TLine(ZPhiPairVector.at(i+1).at(k).X(), ZPhiPairVector.at(i+1).at(k).Y(), ZPhiPairVector.at(i).at(j).X(), ZPhiPairVector.at(i).at(j).Y());
				ALine->Draw("SAME");
			}
		}
	}
}
*/
void PndSttCellTrackingTestTask::DrawCombiLines(vector<vector< TVector2> > ZPhiPairVector){
	TVector2 v1, v2;
//	double CosAngle;
	double Angle;
	double x0, y0;
	TLine* ALine = new TLine();
	for (int i = 1; i < (int)ZPhiPairVector.size()-1; i++){//Loop over straws
		for (int j = 0; j < (int)ZPhiPairVector.at(i-1).size(); j++){//Loop over all Z-phi in straw i-1
			for (int k = 0; k < (int)ZPhiPairVector.at(i).size(); k++){//Loop over all Z-phi in straw i
				for (int l = 0; l < ZPhiPairVector.at(i+1).size(); l++) {//Loop over all Z-phi in straw i+1
					v1 = ZPhiPairVector.at(i+1).at(l) - ZPhiPairVector.at(i).at(k);
					v2 = ZPhiPairVector.at(i-1).at(j) - ZPhiPairVector.at(i).at(k);
					CosAngle = v1*v2/(TMath::Sqrt(v1*v1)*TMath::Sqrt(v2*v2));
					Angle = TMath::ACos(CosAngle) * 180./TMath::Pi();
					//if (Angle > 160) {
						x0 = (ZPhiPairVector.at(i).at(k).X() + ZPhiPairVector.at(i-1).at(j).X())/2;
						y0 = (ZPhiPairVector.at(i).at(k).Y() + ZPhiPairVector.at(i-1).at(j).Y())/2;
						ALine->DrawLine(ZPhiPairVector.at(i).at(k).X(), ZPhiPairVector.at(i).at(k).Y(), x0, y0);

						x0 = (ZPhiPairVector.at(i).at(k).X() + ZPhiPairVector.at(i+1).at(l).X())/2;
						y0 = (ZPhiPairVector.at(i).at(k).Y() + ZPhiPairVector.at(i+1).at(l).Y())/2;
						ALine->DrawLine(ZPhiPairVector.at(i).at(k).X(), ZPhiPairVector.at(i).at(k).Y(), x0, y0);
					//}
				}
			}
		}
	}
	/*
	 * Plot start- and end lines
	 */
	for (int j = 0; j < 2; j++){
		for (int k = 0; k < 2; k++){
			x0 = (ZPhiPairVector.at(0).at(j).X() + ZPhiPairVector.at(1).at(k).X())/2;
			y0 = (ZPhiPairVector.at(0).at(j).Y() + ZPhiPairVector.at(1).at(k).Y())/2;
			ALine->DrawLine(ZPhiPairVector.at(0).at(j).X(), ZPhiPairVector.at(0).at(j).Y(), x0, y0);
			ALine->DrawLine(ZPhiPairVector.at(0).at(j).X(), ZPhiPairVector.at(0).at(j).Y(), x0, y0);

			x0 = (ZPhiPairVector.at(ZPhiPairVector.size()-1).at(j).X() + ZPhiPairVector.at(ZPhiPairVector.size()-2).at(k).X())/2;
			y0 = (ZPhiPairVector.at(ZPhiPairVector.size()-1).at(j).Y() + ZPhiPairVector.at(ZPhiPairVector.size()-2).at(k).Y())/2;
			ALine->DrawLine(ZPhiPairVector.at(ZPhiPairVector.size()-1).at(j).X(), ZPhiPairVector.at(ZPhiPairVector.size()-1).at(j).Y(), x0, y0);
			ALine->DrawLine(ZPhiPairVector.at(ZPhiPairVector.size()-1).at(j).X(), ZPhiPairVector.at(ZPhiPairVector.size()-1).at(j).Y(), x0, y0);
		}
	}

}
void PndSttCellTrackingTestTask::DrawCombiLinesResult(vector< TVector2> ZPhiPairVector){
	for (int i = 0; i < (int)ZPhiPairVector.size()-1; i++){//Loop over straws
		TLine* ALine = new TLine(ZPhiPairVector.at(i+1).X(), ZPhiPairVector.at(i+1).Y(), ZPhiPairVector.at(i).X(), ZPhiPairVector.at(i).Y());
		ALine->Draw("SAME");
	}
}

vector<vector<PndSttHit> > PndSttCellTrackingTestTask::MoveSkewedHitsToCircle(TVector2 circle, Double_t circlerad, vector<PndSttHit*> skewhits){
	vector<vector <PndSttHit> > result;
	for (Int_t i = 0; i < (int)skewhits.size(); i++){
		if (fVerbose > 0 ) cout<<"MoveSkewedHitsToCircle - Hit: "<<i<<" is being adjusted"<<endl;
		vector<PndSttHit> tempResult;
		PndSttHit* myHit = skewhits.at(i);//Should translate
		PndSttTube* myTube = (PndSttTube*) fTubeArray->At(myHit->GetTubeID());
		TVector2 normal = circle.Unit();
		TVector2 translation(circle.X() - circlerad*normal.X(),circle.Y() - circlerad*normal.Y());
		Double_t tubeLengthHalf = myTube->GetHalfLength();
		TVector3 wireDirection = myTube->GetWireDirection();
		TVector3 tubePosition = myTube->GetPosition();//Should translate
		//Test translation
		/*
		tubePosition.SetX(tubePosition.X() - translation.X());
		tubePosition.SetX(tubePosition.Y() - translation.Y());
		myHit->SetX(tubePosition.X() - translation.X());
		myHit->SetY(tubePosition.Y() - translation.Y());
		circle.Set(circle.X() - translation.X(),circle.Y() - translation.Y());
		*/
		TVector3 startPoint = tubePosition - (tubeLengthHalf * wireDirection);
		TVector3 endPoint = tubePosition + (tubeLengthHalf * wireDirection);

		Double_t length1 = circle.Mod() + myHit->GetIsochrone();
		Double_t length2 = circle.Mod() - myHit->GetIsochrone();

		TVector2 sP(startPoint.X(), startPoint.Y());
		TVector2 wDir(wireDirection.X(), wireDirection.Y());

		Double_t p_half = (wDir.X()*(sP.X() - circle.X())) + (wDir.Y()*(sP.Y() - circle.Y()));
		p_half /= wDir.Mod2();
		Double_t q1 = (((sP.X() - circle.X()) * (sP.X() - circle.X()) + ((sP.Y() - circle.Y()) * (sP.Y() - circle.Y())) - length1 * length1))/wDir.Mod2();
		Double_t q2 = (((sP.X() - circle.X()) * (sP.X() - circle.X()) + ((sP.Y() - circle.Y()) * (sP.Y() - circle.Y())) - length2 * length2))/wDir.Mod2();

		vector<Double_t> t;

		if (p_half * p_half > q1){
			t.push_back((-1) * p_half + TMath::Sqrt(p_half * p_half - q1));
			t.push_back((-1) * p_half - TMath::Sqrt(p_half * p_half - q1));
		}
		if (p_half * p_half > q2){
			t.push_back((-1) * p_half + TMath::Sqrt(p_half * p_half - q2));
			t.push_back((-1) * p_half - TMath::Sqrt(p_half * p_half - q2));
		}

		for (int j = 0; j < t.size(); j++){
			if (TMath::Abs(t[j]) < 2 * tubeLengthHalf){
				TVector3 posError(0.2,0.2,2);
				TVector3 pos(startPoint + t[j]*wireDirection);
				TVector2 pos2D(pos.X(), pos.Y());
				TVector2 corrPos = (pos2D - circle).Unit();
				PndSttHit fixhit = *myHit;
				fixhit.SetPosition(pos);
				if (fDraw) DrawHitIsochrone(&fixhit, 4);
				corrPos *= circle.Mod();
				corrPos += circle;
				pos.SetX(corrPos.X());
				pos.SetY(corrPos.Y());
				fixhit.SetPosition(pos);
				if (fVerbose > 0 ) cout << "MoveSkewedHitsToCircle - FindSkewedHits: Pos after fix: " << pos.X() << "/" << pos.Y() << "/" << pos.Z() << endl;
				//fixhit.SetX(fixhit.GetX() + translation.X());//translate back
				//fixhit.SetY(fixhit.GetY() + translation.Y());//translate back
				tempResult.push_back(fixhit);
				//std::cout << std::endl;
			}
		}
		result.push_back(tempResult);
	}

	return result;
}

vector<vector<PndSttHit> > PndSttCellTrackingTestTask::MoveSkewedHitsToCircle2(TVector2 circle, Double_t circlerad, vector<PndSttHit*> skewhits){
	vector<vector <PndSttHit> > result;
	for (Int_t i = 0; i < skewhits.size(); i++){
		if (fVerbose > 0 ) cout<<"MoveSkewedHitsToCircle2 - Hit: "<<i<<" is being adjusted"<<endl;
		vector<PndSttHit> tempResult;
		PndSttHit* myHit = skewhits.at(i);
		PndSttTube* myTube = (PndSttTube*) fTubeArray->At(myHit->GetTubeID());
		Double_t tubeLengthHalf = myTube->GetHalfLength();
		TVector3 wireDirection = myTube->GetWireDirection();
		TVector3 tubePosition = myTube->GetPosition();

		TVector3 startPoint = tubePosition - (tubeLengthHalf * wireDirection);
		TVector3 endPoint = tubePosition + (tubeLengthHalf * wireDirection);

		TVector2 intersection1, intersection2;
		Int_t nofint = ComputeSegmentCircleIntersection(TVector2(endPoint.X(), endPoint.Y()), TVector2(startPoint.X(), startPoint.Y()), circle.X(), circle.Y(), circlerad, intersection1, intersection2);
		if (nofint == 0) continue;
		PndSttHit fixhit = *myHit;

		if (fVerbose > 0 ) cout<<"MoveSkewedHitsToCircle2 - Intersection1 X: "<<intersection1.X()<<" Y: "<<intersection1.Y()<<endl;
		if (nofint == 2) {
			if (fVerbose > 0 ) cout<<"MoveSkewedHitsToCircle2 - Intersection2 X: "<<intersection1.X()<<" Y: "<<intersection1.Y()<<endl;
		}

		TVector2 tangent = ComputeTangentInPoint(circle.X(), circle.Y(), intersection1);

		// 3. rotate clockwise the tangent/point/(wire, not explicitely)
		// in order to have the wire parallel to the x axis;
		// then translate everything to have the wire ON the x axis
		double beta = wireDirection.Phi();
		if(beta < 0) beta += TMath::Pi();
		// ... rotate the tangent
		double rtx = TMath::Cos(beta) * tangent.X() + TMath::Sin(beta) * tangent.Y();
		double rty = TMath::Cos(beta) * tangent.Y() - TMath::Sin(beta) * tangent.X();
		TVector2 rottangent(rtx, rty);
		rottangent = rottangent.Unit();
		// ... rotate the point
		double rx = TMath::Cos(beta) * intersection1.X() + TMath::Sin(beta) * intersection1.Y();
		double ry = TMath::Cos(beta) * intersection1.Y() - TMath::Sin(beta) * intersection1.X();

		// translation
		Double_t deltay = ry;
		rty -= deltay;
		ry -= deltay;

		// rotm, rotp
		Double_t rotm = rottangent.Y()/rottangent.X();
		Double_t rotp = ry - rotm * rx;

		// ellipsis
		double a = myHit->GetIsochrone() * TMath::Cos(3*TMath::DegToRad()); // CHECK skew angle hard coded
		double b = myHit->GetIsochrone();

		// center of ellipsis
		Double_t x0a, x0b, y0;
		y0 = 0.;
		x0a = (-rotp + TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;
		x0b = (-rotp - TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;

		// intersection point
		double intxa = (x0a * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
		double intya = rotm * intxa + rotp;
		double intxb = (x0b * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
		double intyb = rotm * intxb + rotp;

		// 4. retraslate/rerotate all back to the original plane
		// retranslate
		y0 += deltay;
		intya  += deltay;
		intyb  += deltay;

		// rerotate
		double x0anew = TMath::Cos(beta) * x0a - TMath::Sin(beta) * y0;
		double y0anew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0a;
		double x0bnew = TMath::Cos(beta) * x0b - TMath::Sin(beta) * y0;
		double y0bnew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0b;

		double intxanew = TMath::Cos(beta) * intxa - TMath::Sin(beta) * intya;
		double intyanew = TMath::Cos(beta) * intya + TMath::Sin(beta) * intxa;
		double intxbnew = TMath::Cos(beta) * intxb - TMath::Sin(beta) * intyb;
		double intybnew = TMath::Cos(beta) * intyb + TMath::Sin(beta) * intxb;

		intxa = intxanew;
		intya = intyanew;
		intxb = intxbnew;
		intyb = intybnew;

		// x0a, y0a - center of the first ellipse
		// x0b, y0b - center of the second ellipse
		x0a = x0anew;
		double y0a = y0anew;
		x0b = x0bnew;
		double y0b = y0bnew;

	    double dx = intxa - x0a;
	    double dy = intya - y0a;
	    TVector3 dxdy(dx, dy, 0.0);

	    TVector3 tfirst = endPoint + dxdy;
	    TVector3 tsecond = startPoint + dxdy;

	    double t = ((intxa + intya) - (tfirst.X() + tfirst.Y())) /  ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
	    double intza = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;

	    tfirst = endPoint - dxdy;
	    tsecond = startPoint - dxdy;

	    t = ((intxb + intyb) - (tfirst.X() + tfirst.Y())) /  ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
	    double intzb = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;

	    //Set new coordinates for hits and plot isochrones
		fixhit.SetX(x0a);
		fixhit.SetY(y0a);
		if (fDraw) DrawHitSkewedIsochrone(&fixhit, 4, beta, a, b);
		fixhit.SetX(intxa);
		fixhit.SetY(intya);
		fixhit.SetZ(intza);
		tempResult.push_back(fixhit);

		fixhit.SetX(x0b);
		fixhit.SetY(y0b);
		if (fDraw) DrawHitSkewedIsochrone(&fixhit, 4, beta, a, b);
		fixhit.SetX(intxb);
		fixhit.SetY(intyb);
		fixhit.SetZ(intzb);
		tempResult.push_back(fixhit);

		result.push_back(tempResult);
	}
	return result;
}

vector<TVector2> PndSttCellTrackingTestTask::LineCombiIsoFinder(vector<vector< TVector2> > ZPhiPairVector){
	//vector<vector<pair<double,pair<int, int> > > > matrix;
	vector<map<pair<int,int>,double> > matrix;
	map<pair<int,int>,double> tmp;
	pair<pair<int,int>,double> tmppair;
	TVector2 first, second;
	int FirstLineindex;
	int SecondLineindex;
	TVector2 v1, v2;
	double Angle;

	for (int i = 1; i < ZPhiPairVector.size()-1; i++){//Loop over straws
		FirstLineindex = 0;
		SecondLineindex = 0;
		if (fVerbose > 0 ) cout<<"LineCombiIsoFinder - New segment centered around straw: "<<i<<endl;
		for (int j = 0; j < ZPhiPairVector.at(i-1).size(); j++){//Loop over all Z-phi in straw i-1
			for (int k = 0; k < ZPhiPairVector.at(i).size(); k++){//Loop over all Z-phi in straw i
				for (int l = 0; l < ZPhiPairVector.at(i+1).size(); l++){//Loop over all Z-phi in straw i+1
					v1 = ZPhiPairVector.at(i+1).at(l) - ZPhiPairVector.at(i).at(k);
					v2 = ZPhiPairVector.at(i).at(k) - ZPhiPairVector.at(i-1).at(j);
					Angle = TMath::ACos(v1*v2/(TMath::Sqrt(v1*v1)*TMath::Sqrt(v2*v2))) * 180./TMath::Pi();
					pair<int,int>ab(FirstLineindex,SecondLineindex);
					tmp[ab]=Angle;
					if (fVerbose > 0 ) cout<<"FirstLine: "<<ab.first<<" SecondLine: "<<ab.second<<" Angle: "<<Angle<<endl;
					SecondLineindex++;
				}
				FirstLineindex++;
			}
			SecondLineindex=0;
		}
		matrix.push_back(tmp);
		tmp.clear();
	}

	vector<TVector2> result;
	return result;
}

vector<TVector2> PndSttCellTrackingTestTask::LineCombiIsoFinder2(vector<vector< TVector2> > ZPhiPairVector){
	vector<vector<pair<vector<int>,double> > > matrix2;
	vector<pair<vector<int>,double> > tmp2;
	vector<int> tmpIndices;
	TVector2 first, second;
	TVector2 v1, v2;
	double CosAngle, Angle;
	vector<int> nPairArray;

	for (int i = 1; i < ZPhiPairVector.size()-1; i++){//Loop over straws
		if (fVerbose > 0 ) cout<<"LineCombiIsoFinder - New segment centered around straw: "<<i<<endl;
		for (int j = 0; j < ZPhiPairVector.at(i-1).size(); j++){//Loop over all Z-phi in straw i-1
			for (int k = 0; k < ZPhiPairVector.at(i).size(); k++){//Loop over all Z-phi in straw i
				for (int l = 0; l < ZPhiPairVector.at(i+1).size(); l++){//Loop over all Z-phi in straw i+1
					v1 = ZPhiPairVector.at(i+1).at(l) - ZPhiPairVector.at(i).at(k);
					v2 = ZPhiPairVector.at(i-1).at(j) - ZPhiPairVector.at(i).at(k);
					CosAngle = v1*v2/(TMath::Sqrt(v1*v1)*TMath::Sqrt(v2*v2));
					Angle = TMath::ACos(CosAngle) * 180./TMath::Pi();
					//if(Angle > 140){//exclude paths
						tmpIndices.push_back(j);
						tmpIndices.push_back(k);
						tmpIndices.push_back(l);
						tmp2.push_back(pair<vector<int>,double> (tmpIndices,Angle));
						tmpIndices.clear();
						if (fVerbose > 0 ) cout<<"LineCombiIsoFinder - Indices j: "<<j<<" k: "<<k<<" l: "<<l<<" Angle: "<<Angle<<endl;
					//}
				}
			}
		}
		nPairArray.push_back(tmp2.size());
		matrix2.push_back(tmp2);
		tmp2.clear();
	}

	vector<int> current;
	current.assign(matrix2.size(),0);
	pair<vector<int>,double> bestIndices;
	bestIndices.first.assign(matrix2.size(),0);
	bestIndices.second=0.;
	NestedFor(nPairArray,current,bestIndices,0.,matrix2,0);
	if (fVerbose > 0 ) cout<<"Final answer: "<<endl;
	if (fVerbose > 0 ) {
		for (int i = 0; i < bestIndices.first.size();i++){
			cout<<bestIndices.first.at(i)<<" ";
		}
		cout<<endl;
	}


	if (fVerbose > 0 ) cout<<"sum: "<<bestIndices.second<<endl;
	int a, b, c;
	vector<TVector2> result;
	for (int i = 0; i<matrix2.size();i++) {
		a = matrix2.at(i).at(bestIndices.first.at(i)).first.at(0);
		result.push_back(ZPhiPairVector.at(i).at(a));
		if (i == matrix2.size()-1){
			b = matrix2.at(i).at(bestIndices.first.at(i)).first.at(1);
			c = matrix2.at(i).at(bestIndices.first.at(i)).first.at(2);
			result.push_back(ZPhiPairVector.at(i+1).at(b));
			result.push_back(ZPhiPairVector.at(i+2).at(c));
		}
	}
/*
	//Transform Phi*r to arclength starting at s=0
	Double_t startPhi = result.at(0).Y();
	Double_t oldval;
	for (int i = 0; i<result.size();i++){
		oldval = result.at(i).Y();
		result.at(i).Set(result.at(i).X(),TMath::Abs(result.at(i).Y()-startPhi));
		if (fVerbose > 0 ) cout<<"LineCombiIsoFinder - R*phi: "<<oldval<<" Arc length: "<<result.at(i).Y()<<endl;;
	}
*/
	return result;
}

void PndSttCellTrackingTestTask::NestedFor(vector<int> &times, vector<int> &current, pair<vector<int>,double> &best, double sum, vector<vector<pair<vector<int>,double> > > matrix, int depth) {
    double tmp;
    bool possible = true;
	if (depth == times.size()) {

    	if (sum > best.second) {
    		for (int i = 0; i<matrix.size();i++) {
    			int a, b, c;
    			a = matrix.at(i).at(current.at(i)).first.at(0);
    			b = matrix.at(i).at(current.at(i)).first.at(1);
    			c = matrix.at(i).at(current.at(i)).first.at(2);
    		}
        	best.first = current;
        	best.second = sum;
    	}
    	return;
    }

    for (int i = 0; i < times[depth]; ++i) {
    	if (depth>0) {
    		if (matrix.at(depth).at(i).first.at(1) != matrix.at(depth-1).at(current.at(depth-1)).first.at(2)) continue;
    		if (matrix.at(depth).at(i).first.at(0) != matrix.at(depth-1).at(current.at(depth-1)).first.at(1)) continue;
    	}
    	tmp = sum;
    	current[depth]=i;
    	sum+=matrix.at(depth).at(i).second;
    	NestedFor(times, current, best, sum, matrix, depth + 1);
        sum = tmp;
    }
}

vector<TVector2> PndSttCellTrackingTestTask::HoughTrueIsoFinder(vector<vector< TVector2> > ZPhiPairVector, TH2D *HoughSpace, TVector2 &lineparams){
	//Generate lines for each Z-phi point, fill Houghspace with generated line parameters
	for (int i = 0; i < ZPhiPairVector.size(); i++){
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){
			pair<Double_t, Double_t> AngleRange(0.,360.);
			Double_t theta = AngleRange.first;
			Double_t StepSize = ( AngleRange.second-AngleRange.first)/fSteps;
			Double_t Z = ZPhiPairVector.at(i).at(j).X();
			Double_t Phi = ZPhiPairVector.at(i).at(j).Y();
			for (int k = 0; k < fSteps; k++) {
				Double_t dist = Z*TMath::Cos(theta*TMath::DegToRad()) + Phi*TMath::Sin(theta*TMath::DegToRad());
				HoughSpace->Fill(theta,dist);
				theta += StepSize;
			}
		}
	}
	//Find maximum in Hough space
	Int_t MaxDist, MaxTheta, MaxBin;
	HoughSpace->GetMaximumBin(MaxDist,MaxTheta,MaxBin);
	Double_t MaxdDist = HoughSpace->GetXaxis()->GetBinCenter(MaxDist);
	Double_t MaxdTheta = HoughSpace->GetYaxis()->GetBinCenter(MaxTheta);
	Double_t MaxdBin = HoughSpace->GetBinContent(MaxDist,MaxTheta);
	if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder - HoughSpaceMaxima x: "<<MaxdDist<<" y: "<<MaxdTheta<<endl;
	Double_t slope = 1/TMath::Tan(MaxdTheta*TMath::DegToRad());
	//Calculate true line parameters
	Double_t m = MaxdDist/TMath::Sin(MaxdTheta*TMath::DegToRad());
	lineparams.Set(slope,m);
	if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder - Z-Phi line Slope: "<<slope<<" Intercept: "<<m<<endl;
	//Loop over Z-phi pairs and reject the one with larges POCA for each skewed straw
	vector<TVector2> result;
	for (int i = 0; i < ZPhiPairVector.size(); i++){
		Double_t poca = 999999.;
		Int_t index = 0;
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){
			Double_t temppoca = TMath::Abs(slope*ZPhiPairVector.at(i).at(j).X() - 1*ZPhiPairVector.at(i).at(j).Y() + m)/TMath::Sqrt(pow(slope,2) + pow(-1.,2));

			if (temppoca < poca) {
				poca = temppoca;
				index = j;
			}
		}
		result.push_back(ZPhiPairVector.at(i).at(index));
	}
	return result;
}

vector<TVector2> PndSttCellTrackingTestTask::HoughTrueIsoFinder2(vector<vector< TVector2> > ZPhiPairVector, TH2D *HoughSpace, TVector2 &lineparams){
	//Generate only lines between all z-phi candidates, fill Hough space
	for (int i = 0; i < ZPhiPairVector.size(); i++){
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){
			double z1 = ZPhiPairVector.at(i).at(j).X();
			double phi1 = ZPhiPairVector.at(i).at(j).Y();
			for (int k = i+1; k < ZPhiPairVector.size(); k++){
				for (int l = 0; l < ZPhiPairVector.at(k).size(); l++){
					double z2 = ZPhiPairVector.at(k).at(l).X();
					double phi2 = ZPhiPairVector.at(k).at(l).Y();
					double theta = TMath::ATan(-(phi2-phi1)/(z2-z1));
					double dist = z1*TMath::Cos(theta) + phi1*TMath::Sin(theta);
					if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder2 - theta: "<<theta*TMath::RadToDeg()<<" dist: "<<dist<<endl;
					HoughSpace->Fill(theta*TMath::RadToDeg(),dist);
				}
			}
		}
	}
	//Find maximum in Hough space
	Int_t MaxDist, MaxTheta, MaxBin;
	HoughSpace->GetMaximumBin(MaxTheta,MaxDist,MaxBin);
	Double_t MaxdTheta = HoughSpace->GetXaxis()->GetBinCenter(MaxTheta);
	Double_t MaxdDist = HoughSpace->GetYaxis()->GetBinCenter(MaxDist);
	Double_t MaxdBin = HoughSpace->GetBinContent(MaxTheta,MaxDist);
	if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder2 - HoughSpaceMaxima x: "<<MaxdTheta<<" y: "<<MaxdDist<<endl;
	//Calculate true line parameters
	Double_t slope = -1/TMath::Tan(MaxdTheta*TMath::DegToRad());
	Double_t m = MaxdDist/TMath::Sin(MaxdTheta*TMath::DegToRad());
	lineparams.Set(slope,m);
	if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder2 - Z-Phi line Slope: "<<slope<<" Intercept: "<<m<<endl;
	//Loop over Z-phi pairs and reject the one with larges POCA for each skewed straw
	vector<TVector2> result;
	for (int i = 0; i < ZPhiPairVector.size(); i++){
		Double_t poca = 999999.;
		Int_t index = 0;
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){
			Double_t temppoca = TMath::Abs(slope*ZPhiPairVector.at(i).at(j).X() - 1*ZPhiPairVector.at(i).at(j).Y() + m)/TMath::Sqrt(pow(slope,2) + pow(-1.,2));

			if (temppoca < poca) {
				poca = temppoca;
				index = j;
			}
		}
		result.push_back(ZPhiPairVector.at(i).at(index));
	}

	return result;
}

TVector2 PndSttCellTrackingTestTask::PzLineFitExtract(vector< TVector2> ZPhi){
	//Simple linear regression, Z on X-axis Phi on Y-axis
	Double_t meanZ = 0.;
	Double_t meanPhi = 0.;
	for (int i = 0; i < ZPhi.size(); i++){
		if (fVerbose > 0 ) cout<<"PzLineFitExtract - True hit Z: "<<ZPhi.at(i).X()<<" Phi*R: "<<ZPhi.at(i).Y()<<endl;
		meanZ += ZPhi.at(i).X();
		meanPhi += ZPhi.at(i).Y();
	}
	meanZ /=(ZPhi.size());
	meanPhi /=(ZPhi.size());
	Double_t betanume = 0.;
	Double_t betadeno = 0.;
	for (int i = 0; i < ZPhi.size(); i++){
		betanume += (ZPhi.at(i).X() - meanZ)*(ZPhi.at(i).Y() - meanPhi);
		betadeno += pow(ZPhi.at(i).X() - meanZ,2);
	}
	Double_t beta = betanume/betadeno;
	Double_t alpha = meanPhi - beta*meanZ;
	if (fVerbose > 0 ) cout<<"PzLineFitExtract - tan(Helix Angle): "<<beta<<" Intercept: "<<alpha<<endl;
	TVector2 result(alpha,beta);
	return result;
}

void PndSttCellTrackingTestTask::CreatePndTrack(Double_t tanalpha){

}

//Function from PndTrkTools.h for finding intersection between a circle (riemann circle of track)
//and a segment of a line (xy projection of skewed straw)
//Author: Lia Lavezzi
Int_t PndSttCellTrackingTestTask::ComputeSegmentCircleIntersection(TVector2 ex1, TVector2 ex2, double xc, double yc, double R, TVector2 &intersection1, TVector2 &intersection2) {
	double xa, ya, xb, yb;
	double delta;

	if((ex2.X() - ex1.X()) != 0) {
		double m = (ex2.Y() - ex1.Y())/(ex2.X() - ex1.X());
		double p = ex2.Y() - m * ex2.X();;

		delta = - (m * xc + p - yc) * (m * xc + p - yc) +  R * R * (m * m + 1);

		if(delta < 0 || TMath::IsNaN(delta))  return 0;
		xa = (-(m * (p - yc) - xc) + sqrt(delta))/(m * m + 1);
		ya = m * xa + p;
		xb = (-(m * (p - yc) - xc) - sqrt(delta))/(m * m + 1);
		yb = m * xb + p;
	}
	else {

		delta = R * R - (ex1.X() - xc) * (ex1.X() - xc);

		if(delta < 0 || TMath::IsNaN(delta)) return 0;

		xa = ex1.X();
		ya = yc + sqrt(delta);
		xb = ex1.X();
		yb = yc - sqrt(delta);
	}

	TVector2 int1(xa, ya);
	TVector2 int2(xb, yb);

	TVector2 distance11 = int1 - ex1;
	TVector2 distance12 = int1 - ex2;

	TVector2 distance21 = int2 - ex1;
	TVector2 distance22 = int2 - ex2;

	TVector2 length = ex1 - ex2;
	Int_t result = 0;

	if(distance11.Mod() < length.Mod() && distance12.Mod() < length.Mod()) {
		// cout << "intersection 1" << endl;
		result++;
		intersection1 = int1;
	}

	if(distance22.Mod() < length.Mod() && distance21.Mod() < length.Mod()) {
		// cout << "intersection 2" << endl;
		if(result == 1) intersection2 = int2;
		else intersection1 = int2;
		result++;
	}
	// if delta = 0 we have two identical intersections
	if(delta == 0) result--; // CHECK
	return result;
}

//Function from PndTrkTools.h
// I have a circle (xc, yc, R) and a
// point on the circle.
// I want the tangent to the circle in that point
//Author: Lia Lavezzi
TVector2 PndSttCellTrackingTestTask::ComputeTangentInPoint(double xc, double yc, TVector2 point) {
	TVector2 center(xc, yc);
	TVector2 fromcentertoint = point - center;
	TVector2 tangent(fromcentertoint.Y(), -fromcentertoint.X()); // CHECK
	return tangent;
}

//From line 2269 in PndTrkTrackFinder.cxx
/*
for(int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++)
{
	hit = skewhitlist.GetHit(ihit);
	if(!hit) continue;

	TVector3 fin_intersection11(-999, -999, -999),  fin_intersection12(-999, -999, -999);
	double phi11 = -999, phi12 = -999;

	if(hit->IsStt() == kTRUE) {
		PndTrkSkewHit *skewhit1 = (PndTrkSkewHit*) hit;

		fin_intersection11 = skewhit1->GetIntersection1();
		fin_intersection12 = skewhit1->GetIntersection2();
		phi11 = skewhit1->GetPhi1();
		phi12 = skewhit1->GetPhi2();
	}
	else {
		fin_intersection11 = hit->GetPosition();
		phi11 = hit->GetPhi();
	}


	for(int jhit = ihit + 1; jhit < skewhitlist.GetNofHits(); jhit++)
	{
		PndTrkHit *hitj = skewhitlist.GetHit(jhit);
		if(!hitj) continue;

		TVector3 fin_intersection21(-999, -999, -999),  fin_intersection22(-999, -999, -999);
		double phi21 = -999, phi22 = -999;

		if(hitj->IsStt() == kTRUE) {
			PndTrkSkewHit *skewhit2 = (PndTrkSkewHit*) hitj;

			fin_intersection21 = skewhit2->GetIntersection1();
			fin_intersection22 = skewhit2->GetIntersection2();
			phi21 = skewhit2->GetPhi1();
			phi22 = skewhit2->GetPhi2();
		}
		else {
			fin_intersection21 = hitj->GetPosition();
			phi21 = hitj->GetPhi();
		}


		// 1 1
		double cost = (fin_intersection21.Z() - fin_intersection11.Z()) / TMath::Sqrt((phi11 - phi21) * (phi11 - phi21) + (fin_intersection21.Z() - fin_intersection11.Z()) * (fin_intersection21.Z() - fin_intersection11.Z()));
		double theta = TMath::ACos(cost);
		double r1 = phi11 * cost + fin_intersection11.Z() * TMath::Sin(theta);
		double r2 = phi21 * cost + fin_intersection21.Z() * TMath::Sin(theta);
		if(fabs(r1 - r2) > 1.e-9) {
			theta = -TMath::ACos(cost);
			r1 = phi11 * cost + fin_intersection11.Z() * TMath::Sin(theta);
		}
		fLineHisto->Fill(theta * TMath::RadToDeg(), r1);
		double fitm11 =  -TMath::Cos(theta)/TMath::Sin(theta);
		double fitq11 = r1/TMath::Sin(theta);
		TLine *line11 = new TLine(0, fitq11, 360, 360 * fitm11 + fitq11);
		//    line11->Draw("SAME");
		// 	    cout << phi11 << " " << phi21 << endl;

		if(phi12 != -999)  {
			// 2 1
			cost = (fin_intersection21.Z() - fin_intersection12.Z()) / TMath::Sqrt((phi12 - phi21) * (phi12 - phi21) + (fin_intersection21.Z() - fin_intersection12.Z()) * (fin_intersection21.Z() - fin_intersection12.Z()));
			theta = TMath::ACos(cost);
			r1 = phi12 * cost + fin_intersection12.Z() * TMath::Sin(theta);
			r2 = phi21 * cost + fin_intersection21.Z() * TMath::Sin(theta);
			if(fabs(r1 - r2) > 1.e-9) {
				theta = -TMath::ACos(cost);
				r1 = phi12 * cost + fin_intersection12.Z() * TMath::Sin(theta);
			}
			fLineHisto->Fill(theta * TMath::RadToDeg(), r1);
			double fitm21 =  -TMath::Cos(theta)/TMath::Sin(theta);
			double fitq21 = r1/TMath::Sin(theta);
			TLine *line21 = new TLine(0, fitq21, 360, 360 * fitm21 + fitq21);
			// 	      line21->Draw("SAME");
			// 	      cout << phi12 << " " << phi21 << endl;
		}

		if(phi22 != -999)  {
			// 1 2
			cost = (fin_intersection11.Z() - fin_intersection22.Z()) / TMath::Sqrt((phi22 - phi11) * (phi22 - phi11) + (fin_intersection11.Z() - fin_intersection22.Z()) * (fin_intersection11.Z() - fin_intersection22.Z()));
			theta = TMath::ACos(cost);
			r1 = phi22 * cost + fin_intersection22.Z() * TMath::Sin(theta);
			r2 = phi11 * cost + fin_intersection11.Z() * TMath::Sin(theta);
			if(fabs(r1 - r2) > 1.e-9) {
				theta = -TMath::ACos(cost);
				r1 = phi22 * cost + fin_intersection22.Z() * TMath::Sin(theta);
			}
			fLineHisto->Fill(theta * TMath::RadToDeg(), r1);
			double fitm12 =  -TMath::Cos(theta)/TMath::Sin(theta);
			double fitq12 = r1/TMath::Sin(theta);
			TLine *line12 = new TLine(0, fitq12, 360, 360 * fitm12 + fitq12);
			// 	      line12->Draw("SAME");
			// 	      cout << phi11<< " " << phi22<< endl;
		}

		if(phi12 != -999 && phi22 != -999)  {
			// 2 2
			cost = (fin_intersection12.Z() - fin_intersection22.Z()) / TMath::Sqrt((phi22 - phi12) * (phi22 - phi12) + (fin_intersection12.Z() - fin_intersection22.Z()) * (fin_intersection12.Z() - fin_intersection22.Z()));
			theta = TMath::ACos(cost);
			r1 = phi22 * cost + fin_intersection22.Z() * TMath::Sin(theta);
			r2 = phi12 * cost + fin_intersection12.Z() * TMath::Sin(theta);
			if(fabs(r1 - r2) > 1.e-9) {
				theta = -TMath::ACos(cost);
				r1 = phi22 * cost + fin_intersection22.Z() * TMath::Sin(theta);
			}
			fLineHisto->Fill(theta * TMath::RadToDeg(), r1);
			double fitm22 =  -TMath::Cos(theta)/TMath::Sin(theta);
			double fitq22 = r1/TMath::Sin(theta);
			TLine *line22 = new TLine(0, fitq22, 360, 360 * fitm22 + fitq22);
			// 	       line22->Draw("SAME");
			// 	       cout << phi12 << " " << phi22<< endl;
		}
	}
}

int bin = fLineHisto->GetMaximumBin();
int binx, biny, binz;
fLineHisto->GetBinXYZ(bin, binx, biny, binz);
double tpeak = fLineHisto->GetXaxis()->GetBinCenter(binx);
double rpeak = fLineHisto->GetYaxis()->GetBinCenter(biny);
//  cout << "tpeak " << tpeak << " rpeak " << rpeak << endl;
fitm3 = -TMath::Cos(tpeak * TMath::DegToRad())/TMath::Sin(tpeak * TMath::DegToRad());
fitq3 = rpeak/TMath::Sin(tpeak * TMath::DegToRad());
*/

/*
//From T.Stockmann's Circle Hough track finder, finds skewed hits and adjusts the isochrones
std::vector<std::vector<FairHit> > PndIsochroneTrackFinder::FindSkewedHits(TVector2 circle){

	std::vector<std::vector <FairHit> > result;
	for (int i = 0; i < fSkewedHits.size(); i++){
		std::vector<FairHit> tempResult;
		PndSttHit* myHit = fSkewedHits.at(i);
		if (myHit->GetTubeID() < fSttTubeArray->GetEntriesFast()) {
			PndSttTube* myTube = (PndSttTube*) fSttTubeArray->At(myHit->GetTubeID());

			if (0 == myTube) {
				std::cout << "-E- PndSttIsochroneDraw Tube does not exist! "
						<< myHit->GetTubeID() << std::endl;
				continue;
			}
			Double_t tubeLengthHalf = myTube->GetHalfLength();
			TVector3 wireDirection = myTube->GetWireDirection();
			TVector3 tubePosition = myTube->GetPosition();

			TVector3 startPoint = tubePosition - (tubeLengthHalf * wireDirection);
			TVector3 endPoint = tubePosition + (tubeLengthHalf * wireDirection);

			Double_t length1 = circle.Mod() + myHit->GetIsochrone();
			Double_t length2 = circle.Mod() - myHit->GetIsochrone();

			TVector2 sP(startPoint.X(), startPoint.Y());
			TVector2 wDir(wireDirection.X(), wireDirection.Y());
			//TVector2 wD = wDir.Unit();
			Double_t p_half = (wDir.X()*(sP.X() - circle.X())) + (wDir.Y()*(sP.Y() - circle.Y()));
			p_half /= wDir.Mod2();
			Double_t q1 = (((sP.X() - circle.X()) * (sP.X() - circle.X()) + ((sP.Y() - circle.Y()) * (sP.Y() - circle.Y())) - length1 * length1))/wDir.Mod2();
			Double_t q2 = (((sP.X() - circle.X()) * (sP.X() - circle.X()) + ((sP.Y() - circle.Y()) * (sP.Y() - circle.Y())) - length2 * length2))/wDir.Mod2();

			std::vector<Double_t> t;
			//Double_t t1 = -1;
			//Double_t t2 = -1;
			//Double_t t3 = -1;
			//Double_t t4 = -1;

			if (p_half * p_half > q1){
				t.push_back((-1) * p_half + TMath::Sqrt(p_half * p_half - q1));
				t.push_back((-1) * p_half - TMath::Sqrt(p_half * p_half - q1));
			}
			if (p_half * p_half > q2){
				t.push_back((-1) * p_half + TMath::Sqrt(p_half * p_half - q2));
				t.push_back((-1) * p_half - TMath::Sqrt(p_half * p_half - q2));
			}
			std::cout << "FindSkewedHits: TubePos: " << tubePosition.X() << "/" << tubePosition.Y() << "/" << tubePosition.Z() <<  " halflength: " << tubeLengthHalf << std::endl;
			std::cout << "FindSkewedHits: startPos: " << startPoint.X() << "/" << startPoint.Y() << "/" << startPoint.Z() << std::endl;
			std::cout << "FindSkewedHits: endPos: " << endPoint.X() << "/" << endPoint.Y() << "/" << endPoint.Z() << std::endl;
			std::cout << "FindSkewedHits: dirVec: " << wireDirection.X() << "/" << wireDirection.Y() << "/" << wireDirection.Z() << std::endl;
	//		std::cout << "AssignSkewedHits: dirVec2D: " << wD.X() << "/" << wD.Y() << std::endl;

			std::cout << "Circle: " << circle.X() << "/" << circle.Y() << std::endl;
			std::cout << "P_half " << p_half << " q1 " << q1 << " q2 " << q2 << std::endl;
			//std::cout << "t1: " << t1 << " t2 " << t2 << " t3 " << t3 << " t4 " << t4 << std::endl;
			for (int j = 0; j < t.size(); j++){
				if (TMath::Abs(t[j]) < 2 * tubeLengthHalf){
					TVector3 posError(0.2,0.2,2);
					TVector3 pos(startPoint + t[j]*wireDirection);
					TVector2 pos2D(pos.X(), pos.Y());
					TVector2 corrPos = (pos2D - circle).Unit();
					std::cout << "FindSkewedHits: Pos before fix: " << pos.X() << "/" << pos.Y() << "/" << pos.Z() << std::endl;
					corrPos *= circle.Mod();
					corrPos += circle;
					pos.SetX(corrPos.X());
					pos.SetY(corrPos.Y());


					std::cout << "FindSkewedHits: Pos after fix: " << pos.X() << "/" << pos.Y() << "/" << pos.Z() << std::endl;
					FairHit hit(-1, pos, posError, -1);
					hit.AddLink(myHit->GetEntryNr());
					tempResult.push_back(hit);
					hit.SetEntryNr(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(fSkewedBranchName), -1));
	//				FairHit* newSkewedHit = new ((*fSttSkewedArray)[fSttSkewedArray->GetEntriesFast()]) FairHit(hit);
	//				newSkewedHit->SetEntryNr(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(fSkewedBranchName), fSttSkewedArray->GetEntriesFast() - 1));

					std::cout << "FindSkewedHits: Hit " << j << " : " << t[j] << " " << hit.GetX() << "/" << hit.GetY() << "/" << hit.GetZ() << std::endl;
				}
			}
			std::cout << std::endl;
			result.push_back(tempResult);
		}
	}
	return result;
}
*/

/*
//Alternative way of getting STTHits with FairLinks, not working atm
// get all FairLinks of STTHits
FairMultiLinkedData hitLinks = myTrackCand->GetLinksWithType(
FairRootManager::Instance()->GetBranchId("STTHit"));
// for each STTHit
cout<<hitLinks.GetNLinks()<<endl;
for (int j = 0; j < hitLinks.GetNLinks(); ++j) {

	// get HitIndex -> get Tube -> get Position
	curHitIndex = hitLinks.GetLink(j).GetIndex();
	PndSttTube* tube = (PndSttTube*) fTubeArray->At(
	fMapHitIndexToTubeID[curHitIndex]);
	TVector3 pos = tube->GetPosition();

	//ignore skewed straw tubes
	if (!tube->IsSkew()) {
		++hitCounter;
	}
}
*/
