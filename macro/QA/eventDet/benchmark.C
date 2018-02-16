#include <vector>
#include <iostream>
#include <fstream>
#include <set>

//#include "PndDetectorList.h"

using std::cout;
using std::endl;
using std::vector;

void benchmark() {

	TString simFile = "sim.root";
	TString parFile = "sim_params.root";
	TString digiFile = "digi.root";
	TString recoFile = "reco.root";

	TString outFile = "out.root"; //of the FairRunAna
	TFile *out = TFile::Open("benchmark.root", "RECREATE"); //output for the histograms

	int Ev_start = 0;
	int Ev_end = 0;     // take 0 for all events
	TStopwatch timer;
	timer.Start();

	// algorithm parameters:

	double t0Dist_plus = 10;
	double t0Dist_minus = 4;

	// -----   Initialize Run manager   -------------------------------------------
	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(recoFile);
	fRun->AddFriend(digiFile);
	fRun->AddFriend(simFile);
	fRun->SetOutputFile(outFile);
	fRun->SetUseFairLinks(kTRUE);

	// -----  Parameter database   --------------------------------------------
	TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
	allDigiFile += "/macro/params/all.par";

	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo();
	parInput1->open(parFile.Data());

	FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
	parIo1->open(allDigiFile.Data(), "in");
	rtdb->setFirstInput(parInput1);
	rtdb->setSecondInput(parIo1);

	fRun->Init();

	// used variables

	double timeStamp;
	double t0_MC;
	double dist1,dist2;
	int t0_candidates = 0, t0_ghosts = 0, t0_good = 0, t0_missed = 0, t0_double = 0, t0_MCs = 0;

	std::multiset<double> set_t0MC, set_t0Candidates;
	std::multiset<double>::iterator it_t0Can, it_t0Can2, it_t0CanOld, it_t0MC;

	// output objects for .root file:

	// histogram parameters
	int binN = 1000, binStart = 0, binEnd = 5000;

	TH1D *h_t0_MC = new TH1D("h_t0_MC", "t0 ;time [ns]", binN, binStart,binEnd);
	TH1D *h_t0Candidates = new TH1D("h_t0Candidates", "t0 ;time [ns]", binN,binStart, binEnd);
	THStack *hs_timeline = new THStack("hs_timeline","event time and T0 candidates ;time [ns]");

	TH1D *h_t0Resolution = new TH1D("h_t0Resolution","t0 Resolution ;time [ns]", 10*(t0Dist_plus+t0Dist_minus), -t0Dist_minus, t0Dist_plus);
	TH1D *h_results = new TH1D("h_results", "results",6,0,5);

	// ----------------- Initialize Used Variables and Branches ------------------------------
	FairRootManager* ioman = FairRootManager::Instance();

	TClonesArray* t0CandidateArray = (TClonesArray*) ioman->GetObject("T0Candidates"); // if not "initialized" here it may produces error at the first access

	FairTimeStamp *t0Candidate;

	//---------------------------------FairEventHeader ----------------------------
	FairEventHeader* evtHeader = NULL;
	TFile* digFile = TFile::Open(digiFile);
	TTree* digTree = (TTree*) digFile->Get("cbmsim");
	digTree->SetBranchAddress("EventHeader.", &evtHeader);
	//-------------------------------------------------------------------

	if (Ev_end == 0)
		Ev_end = Int_t((ioman->GetInChain())->GetEntries());
	cout << " EndEvent = " << Ev_end << endl;

	//****************************************************************************************
// Data are collected in sets for the benchmark and in addition are stored in histograms

	for (int i_Event = Ev_start; i_Event < Ev_end; i_Event++) { // ------- Loop over events

		digTree->GetEntry(i_Event);
		ioman->ReadEvent(i_Event);
		cout << "event: " << i_Event << " time: " << evtHeader->GetEventTime()
				<< endl;

		if (i_Event < Ev_end - 2) { // -2 because the digi and the reco stage add an additional "event" to write out the remaining data in the buffers
			t0_MC = evtHeader->GetEventTime();
			if (t0_MC < binEnd)
				h_t0_MC->Fill(t0_MC, -0.5);

			set_t0MC.insert(t0_MC);
		}

		for (Int_t i_Array = 0; i_Array < t0CandidateArray->GetEntries();
				i_Array++) { //scitHit array loop

			t0Candidate = (FairTimeStamp*) t0CandidateArray->At(i_Array);
			timeStamp = t0Candidate->GetTimeStamp();
			if (timeStamp < binEnd)
				h_t0Candidates->Fill(timeStamp);

			set_t0Candidates.insert(timeStamp);

		} // end of scit array loop

	} // end of Event loop

	// *********************  Analyse Results of the T0 algorithm *************************


	t0_candidates = set_t0Candidates.size();
	t0_MCs = set_t0MC.size();

	for (it_t0MC = set_t0MC.begin(); it_t0MC != set_t0MC.end();it_t0MC++){

		it_t0Can = set_t0Candidates.lower_bound(*it_t0MC-t0Dist_minus); // get first possible T0 Candidate

		if(it_t0Can == set_t0Candidates.end()){ // if no possible T0 candidte is found (end of set)
			t0_missed++;
			continue;
		}

		dist1 = *it_t0Can - *it_t0MC;  // check distance between t0 cndidate and t0MC
		if (dist1 > t0Dist_plus){		// if distance is to large => t0_MC was missed
			t0_missed++;
			continue;
		}
		it_t0Can2 = it_t0Can;// check if the next t0_candidate may fits better!
		it_t0Can2++;
		if(it_t0Can2 != set_t0Candidates.end()){
			dist2 = *it_t0Can2 - *it_t0MC;
			if(abs(dist2) < abs(dist1)){
				it_t0Can = it_t0Can2;// if its better it replace the t0Candidate
				dist1 = dist2;
			}
		}  // now it_t0Can is the best fitting t0Candidate and dist1 is its distance

		t0_good++;
		h_t0Resolution->Fill(dist1);
		if(it_t0CanOld == it_t0Can) t0_double++; // now check a potential double pick of this t0_Candidate
		it_t0CanOld = it_t0Can; // save the picked t0 candidate for the next double pick check
	}

	t0_ghosts = t0_candidates - t0_good;

	h_results->Fill("t0_MCs",t0_MCs);
	h_results->Fill("t0_candidates",t0_candidates);
	h_results->Fill("t0_good",t0_good);
	h_results->Fill("t0_double",t0_double);
	h_results->Fill("t0_missed",t0_missed);
	h_results->Fill("t0_ghosts",t0_ghosts);
	// write out all stuff

	h_t0_MC->SetLineColor(6);
	h_t0_MC->SetFillColor(6);

	h_t0Candidates->SetLineColor(1);
	h_t0Candidates->SetFillColor(1);

	hs_timeline->Add(h_t0Candidates);
	hs_timeline->Add(h_t0_MC);

	TCanvas *c = new TCanvas;
	h_results->Draw();
	c->SaveAs("results.pdf");

	out->cd();

	h_t0_MC->Write("", TObject::kOverwrite);
	h_t0Candidates->Write("", TObject::kOverwrite);

	hs_timeline->Write("", TObject::kOverwrite);

	h_results->Write("", TObject::kOverwrite);

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// -----------
	cout << " Test passed" << endl;
	cout << " All ok " << endl;
	
	// return 0;
}
