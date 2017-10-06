/*
 * PndSttCellTrackingTestTask.cxx
 *
 *  Created on: Nov 18, 2014
 *      Author: micpa904
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

#include "TGraph.h"
#include "TAxis.h"
#include "PndSttMapCreator.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "PndGeoSttPar.h"

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
	fCombiTrackCand = (TClonesArray*) ioman->GetObject("CombiTrackCand");//For new tracker
	//fCombiTrackCand = (TClonesArray*) ioman->GetObject("SttCellTrackCand");//For old Sttcelltrackfinder
	//fCombiTrackCand = (TClonesArray*) ioman->GetObject("SttCellTrackCandCorrected");//WTF is this?
	//fCombiTrack = (TClonesArray*) ioman->GetObject("SttCellTrack");//For old sttcelltrackfinder
	fCombiTrack = (TClonesArray*) ioman->GetObject("CombiTrack");//For new tracker
	fCombiRiemannTrack = (TClonesArray*) ioman->GetObject("CombiRiemannTrack");
	//fCorrectedCombiRiemannTrack = (TClonesArray*) ioman->GetObject("CombiRiemannTrackCorrected");
	//fCorrectedCombiTrackCand = (TClonesArray*) ioman->GetObject("CombiTrackCandCorrected");//For new tracker
	//fCorrectedCombiTrack = (TClonesArray*) ioman->GetObject("CombiTrackCorrected");//For new tracker


	fSTTHits = (TClonesArray*) ioman->GetObject("STTHit");
	if ( ! fSTTHits ) {
		cout << "-W- PndSttCellTrackingTestTask::Init: "
		<< "fSTTHits not instantiated, return!" << endl;
		return kERROR;
	}

	// get tube-array
	PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
	fTubeArray = mapper->FillTubeArray();
/*
	fCanvas = new TCanvas();
	fCanvas->SetName("fCanvas");
	fCanvas->SetGrid();
	fCanvas->SetCanvasSize(700, 700);
*/

	return kSUCCESS;
}







void PndSttCellTrackingTestTask::Exec(Option_t* opt) {

	FairRootManager* ioman = FairRootManager::Instance();
	FairEventHeader* myEventHeader = (FairEventHeader*) fEventHeader;
	Int_t eventNumber;
	eventNumber = myEventHeader->GetMCEntryNumber();

	// init fMapTubeIDToHits
	cout<<"event: "<<eventNumber<<endl;
	cout<<"fFirstRiemannTrack: "<<fFirstRiemannTrack->GetEntriesFast()<<endl;
	cout<<"fFirstTrackCand: "<<fFirstTrackCand->GetEntriesFast()<<endl;
	cout<<"fCombiRiemannTrack: "<<fCombiRiemannTrack->GetEntriesFast()<<endl;
	cout<<"fCombiTrackCand: "<<fCombiTrackCand->GetEntriesFast()<<endl;
	cout<<"fCombiTrack: "<<fCombiTrack->GetEntriesFast()<<endl;
	//cout<<"fCorrectedCombiRiemannTrack: "<<fCorrectedCombiRiemannTrack->GetEntriesFast()<<endl;
	//cout<<"fCorrectedCombiTrackCand: "<<fCorrectedCombiTrackCand->GetEntriesFast()<<endl;
	//cout<<"fCorrectedCombiTrack: "<<fCorrectedCombiTrack->GetEntriesFast()<<endl;
	PndSttHit* hit;

	for (int j = 0; j < fSTTHits->GetEntriesFast(); ++j) {
		hit = (PndSttHit*) fSTTHits->At(j);
		/*PndSttTube* myTube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
		TVector3 wireDirection = myTube->GetWireDirection();
		cout<<hit->GetTubeID()<<endl;*/
		/*
		cout<<myTube->IsSkew()<<" "<<hit->GetTubeID()<<endl;
		if(myTube->IsSkew()){
			cout<<"Wire direction x: "<<wireDirection.X()<<" y: "<<wireDirection.Y()<<" z: "<<wireDirection.Z()<<endl;
		}
		*/
		fMapTubeIDToHits[hit->GetTubeID()].push_back(
		hit->GetLink(0).GetIndex());
		fMapHitIndexToTubeID[hit->GetLink(0).GetIndex()] = hit->GetTubeID();
	}


	/*
	PndTrackCand* myTrackCand;
	int curHitIndex;
	for (int i = 0; i < fFirstTrackCand->GetEntriesFast(); ++i) {
		cout<<"New track!"<<endl;
		// get current TrackCand

		myTrackCand = (PndTrackCand*) fFirstTrackCand->At(i);
		// get all FairLinks of STTHits
		FairMultiLinkedData hitLinks = myTrackCand->GetLinksWithType(
		FairRootManager::Instance()->GetBranchId("STTHit"));

		// for each STTHit
		for (int j = 0; j < hitLinks.GetNLinks(); ++j) {

			// Get HitIndex -> get Tube -> Get Position
			curHitIndex = hitLinks.GetLink(j).GetIndex();
			PndSttTube* myTube = (PndSttTube*) fTubeArray->At(
			fMapHitIndexToTubeID[curHitIndex]);
			TVector3 wireDirection = myTube->GetWireDirection();
			if(myTube->IsSkew())
			{
				cout<<"Wire direction x: "<<wireDirection.X()<<" y: "<<wireDirection.Y()<<" z: "<<wireDirection.Z()<<endl;
			}

		}
	}*/

	//DrawFirstRiemannPlots(eventNumber);
	//DrawCombiRiemannPlots(eventNumber);
	ExtractSkewedHits(eventNumber);
}

void PndSttCellTrackingTestTask::Finish() {

}

void PndSttCellTrackingTestTask::ExtractSkewedHits(Int_t eventNumber) {
	PndTrackCand* myTrackCand;
	int curHitIndex;
	int hitCounter = 0; // for counting the points of TGraph
	PndRiemannTrack* myRiemannTrack;
	double r, x, y;
	TVector3 wireDirection;
	TCanvas *c1 = new TCanvas();
	c1->SetCanvasSize(1000, 1000);
	Double_t w = gPad->GetWw()*gPad->GetAbsWNDC();
	Double_t h = gPad->GetWh()*gPad->GetAbsHNDC();
	Double_t xmin = -50;
	Double_t xmax = 50;
	Double_t ymin = -50;
	//Double_t ymax = ((xmax-xmin)*h/w)-150;
	Double_t ymax = 50;
	c1->SetFixedAspectRatio();
	c1->Range(xmin,ymin,xmax,ymax);
	/* Draw points of all STTHits of each TrackCand.
	* Hits of skewed tubes were ignored.*/
	//cout<<fCombiTrackCand->GetEntriesFast()<<endl;
	for (int i = 0; i < fCombiTrackCand->GetEntriesFast(); ++i) {//fCombiTrackCand
		// get current TrackCand
		myTrackCand = (PndTrackCand*) fCombiTrackCand->At(i);//fCombiTrackCand
		std::vector< PndTrackCandHit > tracklist = myTrackCand->GetSortedHits();
		//std::vector< PndSttHit* > hiterinos;
		//cout<<"Track number: "<<i<<endl;
		for (int j = 0; j < tracklist.size(); ++j)
		{
			PndSttHit* hit = (PndSttHit*) fSTTHits->At(tracklist.at(j).GetHitId());
			PndSttTube* myTube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
			//hiterinos.push_back((PndSttHit*) fSTTHits->At(tracklist.at(j).GetHitId()));
			wireDirection = myTube->GetWireDirection();

			r = hit->GetIsochrone();
			x = hit->GetX();
			y = hit->GetY();
			//cout<<"Isochrone r: "<<r<<" TubeID: "<<hit->GetTubeID()<<" Direction: "<<wireDirection.X()<<" "<<wireDirection.Y()<<" "<<wireDirection.Z()<<endl;
			if (myTube->IsSkew()) {
			TArc *circ = new TArc(x, y, r);
				circ->SetFillStyle(0);
				circ->SetLineColor(4);
				circ->Draw("SAME");
			}
		}
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
	}

	cout<<"Riemann circle parameters: "<<endl;
	for (int i = 0; i < fCombiRiemannTrack->GetEntriesFast(); ++i) {
		myRiemannTrack = (PndRiemannTrack*) (fCombiRiemannTrack->At(i));
		// get radius + position of center
		r = myRiemannTrack->r();
		x = myRiemannTrack->orig()[0];
		y = myRiemannTrack->orig()[1];
		//cout<<"r: "<<r<<" x: "<<x<<" y: "<<y<<" Pt: "<<myRiemannTrack->Pt(2.)<<endl;
		TArc *circ = new TArc(x, y, r);
		circ->SetFillStyle(0);
		circ->Draw("SAME");
	}
	//Mark origin
	TLine* xLine = new TLine(-50, 0, 50, 0);
	TLine* yLine = new TLine(0, 50, 0, -50);
	xLine->Draw("SAME");
	yLine->Draw("SAME");
}

















































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
void PndSttCellTrackingTestTask::DrawFirstRiemannPlots(Int_t eventNumber) {
	//  PndMCResult myResultHitToPoint = fMCMatch->GetMCInfo("STTHit", "STTPoint");
	PndTrackCand* myTrackCand;
	int curHitIndex;

	fCanvas->Clear();
	fCanvas->SetTitle(
	"STTHits and circle fits for event #"
	+ TString::Itoa(eventNumber, 10));

	TGraph* graph = new TGraph();
	int hitCounter = 0; // for counting points of TGraph

	graph->SetPoint(0,0,0); // only for debugging!!!

	// Draw points of all STTHits of each TrackCand.
	// Hits of skewed tubes were ignored.
	cout<<fFirstTrackCand->GetEntriesFast()<<endl;
	for (int i = 0; i < fFirstTrackCand->GetEntriesFast(); ++i) {
		// get current TrackCand
		myTrackCand = (PndTrackCand*) fFirstTrackCand->At(i);
		// get all FairLinks of STTHits
		FairMultiLinkedData hitLinks = myTrackCand->GetLinksWithType(
		FairRootManager::Instance()->GetBranchId("STTHit"));


		// for each STTHit
		for (int j = 0; j < hitLinks.GetNLinks(); ++j) {

			// Get HitIndex -> get Tube -> Get Position
			curHitIndex = hitLinks.GetLink(j).GetIndex();
			PndSttTube* tube = (PndSttTube*) fTubeArray->At(
			fMapHitIndexToTubeID[curHitIndex]);
			TVector3 pos = tube->GetPosition();

			//ignore skewed straw tubes
			if (!tube->IsSkew()) {
				graph->SetPoint(hitCounter, pos[0], pos[1]);
				++hitCounter;
			}
		}
	}

	// draw graph
	graph->Draw("SAMEA*");
	graph->GetXaxis()->SetLimits(-50, 50);
	graph->GetYaxis()->SetRangeUser(-50, 50);

	// draw circles of riemannTracks

	PndRiemannTrack* myRiemannTrack;
	double r, x, y;

	// for each riemannTrack
	for (int i = 0; i < fFirstRiemannTrack->GetEntriesFast(); ++i) {
		myRiemannTrack = (PndRiemannTrack*) fFirstRiemannTrack->At(i);

		// get radius + position of center
		r = myRiemannTrack->r();
		x = myRiemannTrack->orig()[0];
		y = myRiemannTrack->orig()[1];

		// draw circle
		TArc* circle = new TArc(x, y, r);
		circle->SetFillStyle(0);
		circle->Draw("SAME");
	}

	// mark center of STT
	TLine* xLine = new TLine(-50, 0, 50, 0);
	TLine* yLine = new TLine(0, 50, 0, -50);
	xLine->Draw("SAME");
	yLine->Draw("SAME");

	// save plot
	//fCanvas->Print("data/pandaspace/SttCellTrackFinder" + TString::Itoa(eventNumber, 10)+ ".pdf", "pdf");
}//End PndSttCellTrackingTestTask::DrawFirstRiemannPlots
*/

/*
void PndSttCellTrackingTestTask::DrawCombiRiemannPlots(Int_t eventNumber) {
	PndTrackCand* myTrackCand;
	int curHitIndex;
	fCanvas->Clear();
	fCanvas->SetTitle(
	"STTHits and circle fits for event #" + TString::Itoa(eventNumber, 10));
	TGraph* graph = new TGraph();
	int hitCounter = 0; // for counting the points of TGraph

	// Draw points of all STTHits of each TrackCand.
	// Hits of skewed tubes were ignored.
	for (int i = 0; i < fCombiTrackCand->GetEntriesFast(); ++i) {

		// get current TrackCand
		myTrackCand = (PndTrackCand*) fCombiTrackCand->At(i);

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
				graph->SetPoint(hitCounter, pos[0], pos[1]);
				++hitCounter;
			}
		}
	}

	// draw graph
	graph->Draw("SAMEA*");
	graph->GetXaxis()->SetLimits(-50, 50);
	graph->GetYaxis()->SetRangeUser(-50, 50);


	//  PndTrack *myTrack;
	//
	//  for (int i = 0; i < fCombiTrack->GetEntriesFast(); ++i) {
	//    myTrack = (PndTrack*) (fCombiTrack->At(i));
	//
	//  }

	PndRiemannTrack* myRiemannTrack;
	Double_t r, x, y;
	for (int i = 0; i < fCombiRiemannTrack->GetEntriesFast(); ++i) {
		myRiemannTrack = (PndRiemannTrack*) (fCombiRiemannTrack->At(i));
		// get radius + position of center
		r = myRiemannTrack->r();
		x = myRiemannTrack->orig()[0];
		y = myRiemannTrack->orig()[1];
		// draw circle
		TArc* circle = new TArc(x, y, r);
		circle->SetFillStyle(0);
		circle->SetLineColor(6);
		circle->Draw("SAME");
	}
	for (int i = 0; i < fCorrectedCombiRiemannTrack->GetEntriesFast(); ++i) {
		myRiemannTrack = (PndRiemannTrack*) (fCorrectedCombiRiemannTrack->At(i));
		// get radius + position of center
		r = myRiemannTrack->r();
		x = myRiemannTrack->orig()[0];
		y = myRiemannTrack->orig()[1];
		cout<<"r: "<<r<<" x: "<<x<<" y: "<<y<<" Pt: "<<myRiemannTrack->Pt(2.)<<endl;
		// draw circle
		TArc* circle = new TArc(x, y, r);
		circle->SetFillStyle(0);
		circle->SetLineColor(4);
		circle->Draw("SAME");
	}
	//  for (int i = 0; i < fCombiTrack->GetEntriesFast(); ++i) {
	//
	//  }
	// marl center of STT
	TLine* xLine = new TLine(-50, 0, 50, 0);
	TLine* yLine = new TLine(0, 50, 0, -50);
	xLine->Draw("SAME");
	yLine->Draw("SAME");
	// save plot
	//fCanvas->Print("data/pandaspace/SttCellTrackFinder" + TString::Itoa(eventNumber, 10) + ".pdf","pdf");
}//End PndSttCellTrackingTestTask::DrawCombiRiemannPlots
*/
