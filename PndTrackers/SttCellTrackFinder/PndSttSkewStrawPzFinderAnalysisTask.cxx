/*
 * PndSttSkewStrawPzFinderAnalysisTask.cxx
 *
 *  Created on: Apr 6, 2016
 *      Author: walan603
 */

#include "PndSttSkewStrawPzFinderAnalysisTask.h"
#include <PndSttSkewStrawPzFinderData.h>

#include <FairEventHeader.h>
#include <FairRootManager.h>
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

#include "TGraph.h"
#include "TAxis.h"
#include "TText.h"
#include "TColor.h"
#include "TPaveLabel.h"
#include "TArrow.h"
#include "PndSttMapCreator.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"
#include "TMultiGraph.h"

#include "PndGeoSttPar.h"
//#include "PndMCMatch.h"
//#include "PndMCEntry.h"
//#include "PndMCResult.h"
#include "PndMCTrack.h"
#include "PndTrackingQualityMCInfo.h"
#include "PndTrackingQualityRecoInfo.h"
#include "PndHelixPropagator.h"

#include "TVector2.h"
#include "TVector3.h"

#include "PndSttTube.h"

#include "FairMultiLinkedData.h"
ClassImp (PndSttSkewStrawPzFinderAnalysisTask);

using namespace std;

PndSttSkewStrawPzFinderAnalysisTask::PndSttSkewStrawPzFinderAnalysisTask() : fVerbose(0) {
	FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
	fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

PndSttSkewStrawPzFinderAnalysisTask::~PndSttSkewStrawPzFinderAnalysisTask() {
}

InitStatus PndSttSkewStrawPzFinderAnalysisTask::Init() {
	FairRootManager *ioman = FairRootManager::Instance();

	if (!ioman) {
		cout << "-E- PndSttSkewStrawPzFinderAnalysisTask::Init: RootManager not instantiated!"
				<< endl;
		return kFATAL;
	}

	/*
	fMCMatch = (PndMCMatch*) ioman->GetObject("MCMatch");
	if (!fMCMatch) {
		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No fMCMatch array!"
				<< endl;
		return kERROR;
	}
*/

	fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");

	fEventHeader = (TClonesArray*) ioman->GetObject("EventHeader.");
	if (!fEventHeader) {
		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No EventHeader array! Needed for EventNumber"
				<< endl;
		return kERROR;
	}

	fFirstTrackCand = (TClonesArray*) ioman->GetObject("FirstTrackCand");
	if (!fFirstTrackCand) {
		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No FirstTrackCand array!"
				<< endl;
		return kERROR;
	}

	fFirstTrack = (TClonesArray*) ioman->GetObject("FirstTrack");
	if (!fFirstTrack) {
		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No FirstTrack array!"
				<< endl;
		return kERROR;
	}

	fFirstRiemannTrack = (TClonesArray*) ioman->GetObject("FirstRiemannTrack");
	if (!fFirstRiemannTrack) {
		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No FirstRiemannTrack array!"
				<< endl;
		return kERROR;
	}

	fCombiTrackCand = (TClonesArray*) ioman->GetObject("CombiTrackCand");
	if (!fCombiTrackCand) {
		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No CombiTrackCand array!"
				<< endl;
		return kERROR;
	}

	fCombiTrack = (TClonesArray*) ioman->GetObject("CombiTrack");
	if (!fCombiTrack) {
		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No CombiTrack array!"
				<< endl;
		return kERROR;
	}

	fCombiRiemannTrack = (TClonesArray*) ioman->GetObject("CombiRiemannTrack");
	if (!fCombiRiemannTrack) {
		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No CombiRiemannTrack array!"
				<< endl;
		return kERROR;
	}

	fFinalTrackCand = (TClonesArray*) ioman->GetObject("FinalTrackCand");
	if (!fFinalTrackCand) {
		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No FinalTrackCand array!"
				<< endl;
		return kERROR;
	}

	fFinalTrack = (TClonesArray*) ioman->GetObject("FinalTrack");
	if (!fFinalTrack) {
		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No FinalTrack array!"
				<< endl;
		return kERROR;
	}

	fFinalRiemannTrack = (TClonesArray*) ioman->GetObject("FinalRiemannTrack");
	if (!fFinalRiemannTrack) {
		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No FinalRiemannTrack array!"
				<< endl;
		return kERROR;
	}

	fPzData = (TClonesArray*) ioman->GetObject("FinalPzData");
	if (!fPzData) {
		cout << "-W-  PndSttSkewStrawPzFinderAnalysisTask::Init: No PzData array!"
				<< endl;
		return kERROR;
	}

	fSTTHits = (TClonesArray*) ioman->GetObject("STTHit");
	if ( ! fSTTHits ) {
		cout << "-W- PndSttSkewStrawPzFinderAnalysisTask::Init: No fSTTHits array!"
				<< endl;
		return kERROR;
	}

	/*
	 * TrackingQA output branches
	 */
/*
	fMCTrackInfo = (TClonesArray*) ioman->GetObject("MCTrackInfo");

	if ( ! fMCTrackInfo ) {
		cout << "-W- PndSttSkewStrawPzFinderAnalysisTask::Init: No fMCTrackInfo array!"
				<< endl;
		return kERROR;
	}

	fRecoTrackInfo = (TClonesArray*) ioman->GetObject("RecoTrackInfo");

	if ( ! fRecoTrackInfo ) {
		cout << "-W- PndSttSkewStrawPzFinderAnalysisTask::Init: No fRecoTrackInfo array!"
				<< endl;
		return kERROR;
	}
*/
	//Get tube array
	PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
	fTubeArray = mapper->FillTubeArray();

	return kSUCCESS;
}

void PndSttSkewStrawPzFinderAnalysisTask::Exec(Option_t* opt) {
	FairRootManager* ioman = FairRootManager::Instance();

	FairEventHeader* myEventHeader = (FairEventHeader*) fEventHeader;
	Int_t eventNumber = myEventHeader->GetMCEntryNumber();

	FairField* Field = FairRunAna::Instance()->GetField();
	Double_t po[3], BB[3];
	po[0]=0.;
	po[1]=0.;
	po[2]=0.;
	Field->GetFieldValue(po,BB);
	cout<<"Field Strength: "<<BB[2]/10.<<endl;

	/*
	fMCMatch->CreateArtificialStage("MCTrack");

	PndMCResult trackToMCTrack = fMCMatch->GetMCInfo("CombiTrackCand",
			"MCTrack");
	*/

	Bool_t DrawEvent = kTRUE;

	//if(eventNumber == 1) {
	if (DrawEvent) {
		SttXYproj = new TCanvas();
		SttXYproj->SetCanvasSize(1000, 1000);//1000,1000
		SttXYproj->SetTitle("SttXYproj Event: "+TString::Itoa(eventNumber,10));
		Double_t w = gPad->GetWw()*gPad->GetAbsWNDC();
		Double_t h = gPad->GetWh()*gPad->GetAbsHNDC();
		Double_t xmin = -42;//-42 -25
		Double_t xmax = 42;//42 -5
		Double_t ymin = -42;//-42 10
		Double_t ymax = 42;//42 40
		SttXYproj->SetFixedAspectRatio();
		SttXYproj->Range(xmin,ymin,xmax,ymax);
		SttXYproj->cd();

		//Drawing Stt here
		DrawStt(false);
	}

	for (int i = 0; i < fPzData->GetEntriesFast(); ++i) {
		PndSttSkewStrawPzFinderData* data = (PndSttSkewStrawPzFinderData*) fPzData->At(i);
		PndRiemannTrack rieman = *(PndRiemannTrack*) (fFinalRiemannTrack->At(i));

		vector< vector<TVector2> > ZPhiPairVector = data->getPhiPairVector();
		vector<TVector2> TrueZPhi = data->getTrueZPhi();
		Double_t slope = data->getLineSlope();
		Double_t intercept = data->getLineIntercept();

		/*
		PndTrackingQualityRecoInfo* TrackingQARecoInfo = (PndTrackingQualityRecoInfo*) fRecoTrackInfo->At(i);

		PndMCTrack* MCTrack = (PndMCTrack*) fMCTrack->At(TrackingQARecoInfo->GetMCTrackID());

		TLorentzVector MC4 = MCTrack->Get4Momentum();
		TVector3 MCvertex = MCTrack->GetStartVertex();
		Double_t MCpz = MC4.Pz();
		Double_t MCpt = MC4.Pt();
		Double_t MCslope = MCpt/MCpz;
		Double_t MCcharge = TrackingQARecoInfo->GetMCTrackInfo().GetCharge();

		if (MCcharge > 0) {
			MCslope = MCslope*(-1);
		}
		//Double_t MCintercept = MC4.Phi()*rieman.r()+3*TMath::Pi()*rieman.r();
		Double_t MCintercept = TrueZPhi.at(0).Y() - MCslope*TrueZPhi.at(0).X();

		PndHelixPropagator MCHelix(BB[2]/10.,MCvertex,MCTrack->GetMomentum(),MCcharge);

		cout<<"Helix propagator S: "<<MCHelix.PropagateToZ(TrueZPhi.front().X()).GetMomentum().Phi()*rieman.r()<<endl;

		cout<<"Reco track: "<<i<<" Most probable MC track ID: "<<TrackingQARecoInfo->GetMCTrackID()<<
				" MC pt: "<<MCpt<<
				" MC pz: "<<MCpz<<
				"\nReco slope: "<<slope<<
				"Mc slope: "<<MCslope<<
				"\nReco charge: "<<TrackingQARecoInfo->GetCharge()<<
				" MC charge: "<<MCcharge<<
				endl;
*/
		Double_t MCslope = 0;//dummyslope
		Double_t MCintercept = 0;//dummyintercept


		if (DrawEvent) {
			DrawZPhiBase(ZPhiPairVector,TrueZPhi,slope,intercept,MCslope,MCintercept);
			DrawCombiLines(ZPhiPairVector);

			gPad->Modified();

			SttXYproj->cd();
		}

	}

}

void PndSttSkewStrawPzFinderAnalysisTask::Finish() {
	fFirstTrackCand->Delete();
	fFirstTrack->Delete();
	fFirstRiemannTrack->Delete();

	fCombiTrackCand->Delete();
	fCombiTrack->Delete();
	fCombiRiemannTrack->Delete();

	fFinalTrackCand->Delete();
	fFinalTrack->Delete();
	fFinalRiemannTrack->Delete();

	fPzData->Delete();
}

/*
 * Private functions
 */

void PndSttSkewStrawPzFinderAnalysisTask::DrawStt(bool isskew) {
	TArc *riecirc = new TArc(0., 0., 41.);
	riecirc->SetFillStyle(0);
	riecirc->Draw("SAME");
	PndSttTube* tube;
	TVector3 pos;
	double r,x,y,z;
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

	//Mark origin
	TLine* xLine = new TLine(-50, 0, 50, 0);
	TLine* yLine = new TLine(0, 50, 0, -50);
	xLine->Draw("SAME");
	yLine->Draw("SAME");
}

void PndSttSkewStrawPzFinderAnalysisTask::DrawSkewTubeLines(PndSttHit* hit) {
	PndSttTube* myTube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
	Double_t tubeLengthHalf = myTube->GetHalfLength();
	TVector3 wireDirection = myTube->GetWireDirection();
	TVector3 tubePosition = myTube->GetPosition();
	TVector3 startPoint = tubePosition - (tubeLengthHalf * wireDirection);
	TVector3 endPoint = tubePosition + (tubeLengthHalf * wireDirection);

	TLine* tubeLine = new TLine(startPoint.X(), startPoint.Y(), endPoint.X(), endPoint.Y());
	tubeLine->Draw("SAME");
}

void PndSttSkewStrawPzFinderAnalysisTask::DrawHitIsochrone(PndSttHit* hit, Color_t color) {
	Double_t isor = hit->GetIsochrone();
	Double_t isox = hit->GetX();
	Double_t isoy = hit->GetY();
	TArc *circ = new TArc(isox, isoy, isor);
	circ->SetFillStyle(0);
	circ->SetLineColor(color);
	circ->Draw("SAME");
}

void PndSttSkewStrawPzFinderAnalysisTask::DrawHitSkewedIsochrone(PndSttHit* hit, Color_t color, Double_t beta, Double_t a, Double_t b) {
	Double_t isor = hit->GetIsochrone();
	Double_t isox = hit->GetX();
	Double_t isoy = hit->GetY();
	TEllipse *ell = new TEllipse(isox, isoy, a, b, 0, 360, -beta);
	ell->SetFillStyle(0);
	ell->SetLineColor(color);
	ell->Draw("SAME");
}

void PndSttSkewStrawPzFinderAnalysisTask::DrawTrackParams(PndTrack* track) {
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
void PndSttSkewStrawPzFinderAnalysisTask::DrawCombiLines(vector<vector< TVector2> > ZPhiPairVector){
	TVector2 v1, v2;
	double CosAngle, Angle;
	double x0, y0;
	TLine* ALine = new TLine();
	for (int i = 1; i < ZPhiPairVector.size()-1; i++){//Loop over straws
		for (int j = 0; j < ZPhiPairVector.at(i-1).size(); j++){//Loop over all Z-phi in straw i-1
			for (int k = 0; k < ZPhiPairVector.at(i).size(); k++){//Loop over all Z-phi in straw i
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

void PndSttSkewStrawPzFinderAnalysisTask::DrawCombiLinesResult(vector< TVector2> TrueZPhi){
	for (int i = 0; i < TrueZPhi.size()-1; i++){//Loop over straws
		TLine* ALine = new TLine(TrueZPhi.at(i+1).X(), TrueZPhi.at(i+1).Y(), TrueZPhi.at(i).X(), TrueZPhi.at(i).Y());
		ALine->Draw("SAME");
	}
}

void PndSttSkewStrawPzFinderAnalysisTask::DrawZPhiBase(vector<vector< TVector2> > ZPhiPairVector,
		vector< TVector2> TrueZPhi, Double_t slope, Double_t intercept,
		Double_t MCslope, Double_t MCintercept) {

	TCanvas *ZPhiCanvas = new TCanvas();
	ZPhiCanvas->SetTitle("ZPhiCanvas");
	ZPhiCanvas->SetCanvasSize(350, 350);//(250,500);

	//Print z-phi hits

	//Used for plotting in TGraph
	double x[ZPhiPairVector.size()*2], y[ZPhiPairVector.size()*2];
	int l=0;
	for (int j=0;j<ZPhiPairVector.size();j++) {
		for (int k=0;k<2;k++) {
			x[l] = ZPhiPairVector.at(j).at(k).X();
			y[l] = ZPhiPairVector.at(j).at(k).Y();
			l++;
		}
	}

	TGraph *ZPhiPoints = new TGraph(ZPhiPairVector.size()*2,x,y);
	ZPhiPoints->SetMarkerStyle(3);
	ZPhiPoints->SetMarkerColor(kRed);
	ZPhiPoints->SetMarkerSize(2);
	ZPhiPoints->SetDrawOption("AP");
	double xtrue[TrueZPhi.size()], ytrue[TrueZPhi.size()];
	for (Int_t j=0;j<TrueZPhi.size();j++) {
		xtrue[j] = TrueZPhi.at(j).X();
		ytrue[j] = TrueZPhi.at(j).Y();
	}

	TGraph *TrueZPhiPoints = new TGraph(TrueZPhi.size(),xtrue,ytrue);
	TrueZPhiPoints->SetMarkerStyle(4);
	TrueZPhiPoints->SetMarkerSize(2);
	TrueZPhiPoints->SetDrawOption("AP");
	TrueZPhiPoints->SetMarkerColor(kBlue);

	Double_t start = TrueZPhi.front().X();
	Double_t end = TrueZPhi.back().X();

	if (start > end) {
		Double_t temp = start;
		start = end;
		end = temp;
	}

	TF1 *fa = new TF1("fa","[0]*x + [1]",start,end);
	fa->SetParameter(0,slope);
	fa->SetParameter(1,intercept);

	TGraph* lineGraph = new TGraph(fa,"");
	lineGraph->SetLineColor(kBlue);

	TF1 *famc = new TF1("famc","[0]*x + [1]",start,end);
	famc->SetParameter(0,MCslope);
	famc->SetParameter(1,MCintercept);

	TGraph* MClineGraph = new TGraph(famc,"");
	MClineGraph->SetLineColor(kGreen);


	TMultiGraph *mg = new TMultiGraph();
	mg->SetTitle("");
	mg->Add(ZPhiPoints,"P");
	//mg->Add(TrueZPhiPoints,"P");
	//mg->Add(lineGraph);
	//mg->Add(MClineGraph);
	//mg->Draw("ap");
	mg->Draw("a");
	mg->GetXaxis()->SetTitle("z /cm");
	mg->GetXaxis()->SetTitleSize(0.06);
	mg->GetXaxis()->SetTitleOffset(0.5);
	mg->GetXaxis()->SetLabelOffset(0.);
	mg->GetXaxis()->SetLabelSize(0.055);
	mg->GetYaxis()->SetTitle("S /cm");//Change back to ("R*#phi /cm")
	mg->GetYaxis()->SetTitleSize(0.06);
	mg->GetYaxis()->SetTitleOffset(0.5);
	mg->GetYaxis()->SetLabelOffset(0);
	mg->GetYaxis()->SetLabelSize(0.055);

}
