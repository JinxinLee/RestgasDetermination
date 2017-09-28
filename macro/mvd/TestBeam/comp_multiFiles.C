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
	newCan->Divide(second, first);
	return newCan;
}

int GetMaxHisto(std::vector<TH1*> histos){
	int index = -1;
	int oldMax = 0;
	for (int i = 0; i < histos.size(); i++){
		if (histos[i]->GetMaximum() > oldMax){
			oldMax = histos[i]->GetMaximum();
			index = i;
		}
	}
	return index;
}

std::vector<std::string> GetFileNames(std::string inputFile)
{
	std::vector<std::string> listOfFiles;
	std::string data;

	std::ifstream in(inputFile);
	while(in >> data){
		listOfFiles.push_back(data);
	}
	return listOfFiles;
}

int comp_multiFiles(std::string listOfFiles_File = "inputFiles.txt", int picpercan = 9, double minP = 0.03, int minev = 3,
		int maxfail = 3) {
	// fn   -> red
	// fn2 -> blue
	std::vector<TCanvas*> canvasses;
//	TCanvas *c1 = new TCanvas();
//	c1->Divide(6, 6);

	std::vector<std::string> fileNames = GetFileNames(listOfFiles_File);
	std::vector<TFile*> files;
	for (auto name : fileNames){
		files.push_back(new TFile(name.c_str(), "READ"));
	}

	if (files.size() < 2)
		return 1;
	TFile *f = files[0];
	if (!f->IsZombie()) {
		TKey *key;
		TIter next(f->GetListOfKeys());

		int failcount = 0;
		Int_t yy = 0;
		Int_t actualCanvas = 0;
		Int_t actualPad = 0;
		Int_t i = 0;
		while ((key = (TKey*) next())) {
			actualCanvas = TMath::Floor((double)yy / picpercan);
			actualPad = (yy % picpercan) + 1;
			cout << yy << " : " << actualCanvas << "/" << actualPad << endl;
			if (!(canvasses.size() > actualCanvas)){
				cout << "New canvas" << endl;
				canvasses.push_back(createCanvas(picpercan));
			}
			canvasses[actualCanvas]->cd(actualPad);
			yy++;
			TObject *obj = key->ReadObj();

			// only check TH1Fs
			if (!obj->InheritsFrom("TH1")){
				cout << "Wrong Inheritance" << endl;
				continue;
			}

			TString name = obj->GetName();
			std::cout << name << std::endl;
			//if (!name.Contains("fP")) continue;

			TH1* h = (TH1*) obj;
			h->SetLineWidth(2);
			std::vector<TH1*> histos;
			histos.push_back(h);
			for (int i = 1; i < files.size(); i++){
				TH1* currentHisto = (TH1*)files[i]->Get(name);
				histos.push_back(currentHisto);
				currentHisto->SetLineColor(i);
				currentHisto->SetLineWidth(2);
			}
			int maxHisto = GetMaxHisto(histos);
			if (maxHisto > -1){
				histos[maxHisto]->Draw();
				for (int j = 0; j < histos.size(); j++){
					if (j != maxHisto)
						histos[j]->Draw("same");
				}
			}
		}
	}
	return 0;
}
