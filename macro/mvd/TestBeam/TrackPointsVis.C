/*
 * TrackPointsVis.C
 *
 *  Created on: 04.09.2015
 *      Author: Stockmanns
 */

#include <map>

Int_t gEntryNr = 0;
TTree* t;
TClonesArray* mvdHitArray = new TClonesArray("PndSdsHit");
TClonesArray* mvdProjectedArray = new TClonesArray("PndSdsHit");
TClonesArray* trackArray = new TClonesArray("PndTrack");
TGraph* gHitPoints = new TGraph(4);
TGraph* gProjections = new TGraph(4);
TMultiGraph* mg = new TMultiGraph();

void Init()
{
	TFile* f = new TFile("2014-10-27-00-06-02_50_MHz_Filtered_PosCorrected_Event.root", "READ");
	t = (TTree*)f->Get("cbmsim");
	t->AddFriend("cbmsim", "2014-10-27-00-06-02_50_MHz_Filtered_PosCorrected_Residuals.root");
	t->AddFriend("cbmsim", "2014-10-27-00-06-02_50_MHz_Filtered_PosCorrected_Track.root");
	t->AddFriend("cbmsim", "2014-10-27-00-06-02_50_MHz_Filtered_TimeCorrectedSorted_Event.root");
	t->StartViewer();

	t->SetBranchAddress("MVDHitsPixelPosCorrected", &mvdHitArray);
	t->SetBranchAddress("MvdProjectedHit", &mvdProjectedArray);
	t->SetBranchAddress("MvdTrack", &trackArray);

	gHitPoints->SetLineColor(kRed);
	gHitPoints->SetMarkerStyle(2);
	gHitPoints->SetMarkerColor(kRed);

	gProjections->SetLineColor(kBlue);
	gProjections->SetMarkerStyle(5);
	gProjections->SetMarkerColor(kBlue);


//	mg->Add(gProjections, "Projected");
	mg->Add(gHitPoints, "Hits");
	t->GetEntry(0);
}

void SetEntryNr(Int_t nr){
	gEntryNr = nr;
	t->GetEntry(gEntryNr);
}

void Next(){
	Bool_t run = kTRUE;
	while(run){
		t->GetEntry(++gEntryNr);
		if (mvdProjectedArray->GetEntriesFast() > 0){
			run = kFALSE;
		}
	}
	std::cout << "NextEntry: " << gEntryNr << std::endl;
	Draw();
}

void Draw(){

	std::cout << "mvdHitArray: " << mvdHitArray->GetEntries() << " projectedHitArray " << mvdProjectedArray->GetEntries() << std::endl;

//	gHitPoints->Clear();
	ExtractData(mvdHitArray, gHitPoints);
	ExtractData(mvdProjectedArray, gProjections);
	//if (gHitPoints->GetN() != x.size()) gHitPoints->Expand(x.size());

	std::cout << "NHits: " << gHitPoints->GetN() << std::endl;
	gHitPoints->Draw("apl");
	gProjections->Draw("pl");
}

void ExtractData(TClonesArray* array, TGraph* g){
	std::map<Double_t, Double_t>  x;
	std::map<Double_t, Double_t> y;
	for (int i = 0; i < array->GetEntriesFast(); i++){
		PndSdsHit* myHit = (PndSdsHit*)array->At(i);
		x[myHit->GetZ()] = myHit->GetX();
		y[myHit->GetZ()] = myHit->GetY();
		std::cout << i  << " : (" << myHit->GetX() << "/" << myHit->GetY() << "/" << myHit->GetZ() << ")" << std::endl;
	}
	for (int j = 0; j < g->GetN(); j++) g->RemovePoint(j);

	std::map<Double_t, Double_t >::iterator yit = y.begin();
	int i = 0;
	for (std::map<Double_t, Double_t >::iterator xit = x.begin() ; xit != x.end(); xit++, yit++, i++){
		g->SetPoint(i, xit->second, yit->second);
	}
}

int TrackPointsVis()
{
	Init();
	Draw();
  return 0;
}
