#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TKey.h"
#include <iostream>
#include "TH1F.h"
#include "TRandom.h"

using std::cout;
using std::endl;

TCanvas* createCanvas(int picpercan){
	double division = TMath::Sqrt((double)picpercan);
	int first = ceil(division);
	int second = round(division);
	cout << "createCanvas: " << picpercan << " : " << first << "/" << second << endl;
	TCanvas* newCan = new TCanvas();
	newCan->Divide(first, second);
	return newCan;
}

int comp_recoqa(TString fn = "QA_histograms.root", TString fn2 =
		"QA_histograms_test.root", int picpercan = 9, double minP = 0.03, int minev = 3,
		int maxfail = 3) {
	// fn   -> red
	// fn2 -> blue
	std::vector<TCanvas*> canvasses;
	TCanvas *c1 = new TCanvas();
	c1->Divide(6, 6);
	bool fTest = kFALSE;

	TFile *f = new TFile(fn, "READ");
	if (!f->IsZombie()) {
		TFile *f2 = new TFile(fn2, "READ");

		TKey *key;
		TIter next(f->GetListOfKeys());

		int failcount = 0;
		Int_t yy = 0;
		Int_t actualCanvas = 0;
		Int_t actualPad = 0;
		while ((key = (TKey*) next())) {
			actualCanvas = TMath::Floor((double)yy / picpercan);
			actualPad = (yy % picpercan) + 1;
			cout << yy << " : " << actualCanvas << "/" << actualPad << endl;
			if (!(canvasses.size() > actualCanvas)){
				cout << "New canvas" << endl;
				canvasses.push_back(createCanvas(picpercan));
			}
			canvasses[actualCanvas]->cd(actualPad);
			TObject *obj = key->ReadObj();

			// only check TH1Fs
			if (!obj->InheritsFrom("TH1F"))
				continue;

			TString name = obj->GetName();
			TH1F* h = (TH1F*) obj;
			TH1F* h2 = (TH1F*) f2->Get(name);
			h->SetLineColor(2);
			h->SetLineWidth(2);
			h2->SetLineWidth(2);
			if (h->GetMaximum() < h2->GetMaximum()) {
				h2->Draw();
				h->Draw("same");
			} else {
				h->Draw();
				h2->Draw("same");
			}
			if (h->GetEntries() < minev) {
				cout << "Histogram (almost) empty : " << name << " \""
						<< h2->GetTitle() << "\":  N = " << h->GetEntries()
						<< endl;
				failcount++;
			} else {
				double P = h2->KolmogorovTest(h);
				if (P < minP) {
					cout << "Incompatible distribution: " << name << " \""
							<< h2->GetTitle() << "\":  P = " << P << endl;
					failcount++;
				}
			}
			yy++;
		}

		if (failcount < maxfail)
			fTest = kTRUE;

	}

	if (fTest) {
		cout << " Test passed" << endl;
		cout << " All ok " << endl;
	} else {
		cout << " Test Failed" << endl;
		cout << " Not Ok " << endl;
	}
	return 0;
}
