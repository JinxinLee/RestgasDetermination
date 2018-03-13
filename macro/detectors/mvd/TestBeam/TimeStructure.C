/*
 * TimeStructure.C
 *
 *  Created on: 04.09.2015
 *      Author: Stockmanns
 */

#include <map>
#include <vector>
#include <iomanip>
#include <iostream>

Int_t gEntryNr = 0;
TTree* t;
TClonesArray* mvdUnsorted = new TClonesArray("PndSdsDigiTopix4");
TClonesArray* mvdSorted = new TClonesArray("PndSdsDigiTopix4");
TH1D* h1Unsorted;
TH1D* h1Sorted;
TH1D* h1UnsortedDL;
TH1D* h1UnsortedTL;
TH1D* h1SortedDL;
TGraph *grTimeStampPos;
std::vector<Double_t> dataInStreamUnsorted;


void Init()
{
	TFile* f = new TFile("2014-10-27-00-06-02_50_MHz.root", "READ");
	t = (TTree*)f->Get("pndsim");
//	t->AddFriend("pndsim", "2014-10-27-00-06-02_50_MHz_Filtered_PosCorrected_Residuals.root");
//	t->AddFriend("pndsim", "2014-10-27-00-06-02_50_MHz_Filtered_PosCorrected_Track.root");
//	t->AddFriend("pndsim", "2014-10-27-00-06-02_50_MHz_Filtered_TimeCorrectedSorted_Event.root");
//	t->StartViewer();
	t->SetBranchAddress("ToPix4Hits", &mvdUnsorted);
	t->SetBranchAddress("ToPix4HitsSorted", &mvdSorted);
	h1Unsorted = new TH1D("h1Unsorted", "Unsorted", 200000, -100000, 100000);
	h1Sorted = new TH1D("h1Sorted", "Sorted", 200000, -100000, 100000);
	h1UnsortedDL = new TH1D("h1UnsortedDL", "UnsortedDL", 200000, -100000, 100000);
	h1UnsortedTL = new TH1D("h1UnsortedTL", "UnsortedTL", 200000, -100000, 100000);
//	t->SetBranchAddress("MvdTrack", &trackArray);


//	mg->Add(gProjections, "Projected");
	t->GetEntry(0);
}


int TimeStructure(Int_t sensorId = 0, Bool_t allSensors = kFALSE)
{
	Init();
	int runIndexUnsorted = 0;
	int runIndexSorted = 0;

	std::ofstream unsorted ("unsorted3.txt", std::ofstream::out);
	std::ofstream sorted ("sorted3.txt", std::ofstream::out);
	double oldTimeUnsorted = 0;
	double oldTimeUnsortedDL = 0;
	double oldTimeUnsortedTL = 0;
	double oldTimeSorted = 0;

//	for (int i = 0; i < t->GetEntries(); i++){
	double timeDiff = 0;
	double timeDiffDL = 0;
	double timeDiffTL = 0;

//	for (int i = 0; i < t->GetEntries(); i++){
	for (int i = 333800; i < 344680; i++){
		t->GetEntry(i);
		for (int j = 0; j < mvdUnsorted->GetEntries(); j++){
			PndSdsDigiTopix4* unsortedHit = (PndSdsDigiTopix4*)mvdUnsorted->At(j);
			if (unsortedHit->GetSensorID() == sensorId || allSensors){
				timeDiff = unsortedHit->GetTimeStamp() - oldTimeUnsorted;
				timeDiffDL = unsortedHit->GetTimeStamp() - oldTimeUnsortedDL;
				timeDiffTL = unsortedHit->GetTimeStamp() - oldTimeUnsortedTL;

				unsorted << runIndexUnsorted++ << " , " << unsortedHit->GetSensorID() << " , " << std::setprecision(15) << unsortedHit->GetTimeStamp() << " , " << timeDiff << std::endl;
				h1Unsorted->Fill(timeDiff);
				h1UnsortedDL->Fill(timeDiffDL);
				h1UnsortedTL->Fill(timeDiffTL);
				oldTimeUnsortedTL = oldTimeUnsortedDL;
				oldTimeUnsortedDL = oldTimeUnsorted;
				oldTimeUnsorted = unsortedHit->GetTimeStamp();
				dataInStreamUnsorted.push_back(unsortedHit->GetTimeStamp());
			}
		}
		for (int k = 0; k < mvdSorted->GetEntries(); k++){
			PndSdsDigiTopix4* sortedHit = (PndSdsDigiTopix4*)mvdSorted->At(k);
			if (sortedHit->GetSensorID() == sensorId || allSensors){
				double timeDiffD = sortedHit->GetTimeStamp() - oldTimeSorted;
				sorted << runIndexSorted++ << " , " << std::setprecision(15) << sortedHit->GetTimeStamp() << " , " << timeDiffD << std::endl;
				h1Sorted->Fill(timeDiffD);
				oldTimeSorted = sortedHit->GetTimeStamp();
			}
		}
		if (i % 1000 == 0){
			std::cout << i << std::endl;
		}
	}
	unsorted.close();
	sorted.close();

	TCanvas* c1 = new TCanvas();
	c1->Divide(4,1);
	c1->cd(1);
	h1Unsorted->Draw();
	c1->cd(2);
	h1UnsortedDL->Draw();
	c1->cd(3);
	h1UnsortedTL->Draw();
	c1->cd(4);
	h1Sorted->Draw();
  return 0;
}
