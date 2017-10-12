#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TKey.h"
#include <iostream>
#include "TH1F.h"
#include "TRandom.h"
#include <utility>

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

struct fileData {
	fileData(std::string fileName, std::string parameter, double scaling) : fFileName(fileName), fParameter(parameter), fScalingFactor(scaling) {};
	std::string fFileName;
	std::string fParameter;
	double fScalingFactor;
};

std::vector<fileData> GetFileNames(std::string inputFile)
{
	std::vector<fileData> listOfFiles;
	std::string fileName;
	std::string parameter;
	double scalingFactor;

	std::ifstream in(inputFile);
	while(in >> fileName >> parameter >> scalingFactor){
		listOfFiles.push_back(fileData(fileName, parameter, scalingFactor));
	}
	return listOfFiles;
}

int comp_multiFiles(std::string listOfFiles_File = "inputFiles.txt", int picpercan = 9, double minP = 0.03, int minev = 3,
		int maxfail = 3) {
	// fn   -> red
	// fn2 -> blue
	std::vector<TCanvas*> canvasses;
	std::string filter = "";
//	TCanvas *c1 = new TCanvas();
//	c1->Divide(6, 6);

	std::vector<fileData> fileNames = GetFileNames(listOfFiles_File);
	std::vector<TFile*> files;
	for (auto name : fileNames){
		files.push_back(new TFile(name.fFileName.c_str(), "READ"));
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

			TObject *obj = key->ReadObj();
			TString name = obj->GetName();
			std::cout << name << std::endl;
			if (!name.Contains(filter.c_str())) continue;

			actualCanvas = TMath::Floor((double)yy / picpercan);
			actualPad = (yy % picpercan) + 1;
			cout << yy << " : " << actualCanvas << "/" << actualPad << endl;
			if (!(canvasses.size() > actualCanvas)){
				cout << "New canvas" << endl;
				canvasses.push_back(createCanvas(picpercan));
			}
			canvasses[actualCanvas]->cd(actualPad);
			yy++;

			// only check TH1Fs
			if (obj->InheritsFrom("TH1")){
				TH1* h = (TH1*) obj;
				h->Scale(fileNames[0].fScalingFactor);
				h->SetLineWidth(2);
				std::vector<TH1*> histos;
				histos.push_back(h);
				for (int i = 1; i < files.size(); i++){
					TH1* currentHisto = (TH1*)files[i]->Get(name);
					currentHisto->Scale(fileNames[i].fScalingFactor);
					histos.push_back(currentHisto);
					currentHisto->SetLineColor(i);
					currentHisto->SetLineWidth(2);
				}
				int maxHisto = GetMaxHisto(histos);
				if (maxHisto > -1){
					TLegend* theLegend = new TLegend(0.5,0.55,0.9,0.75);
					histos[maxHisto]->Draw();
					theLegend->AddEntry(histos[maxHisto], fileNames[maxHisto].fParameter.c_str(),"l");
					for (int j = 0; j < histos.size(); j++){
						if (j != maxHisto){
							histos[j]->Draw("same");
							theLegend->AddEntry(histos[j], fileNames[j].fParameter.c_str(),"l");
						}
					}
					theLegend->Draw();
				}
			}
			if (obj->InheritsFrom("TGraph")){
				TString name = obj->GetName();
				TLegend* aLegend = new TLegend(0.5,0.55,0.9,0.75);
				TGraph* g = (TGraph*)obj;
				g->SetMarkerStyle(6);
				g->Draw("AL");
				aLegend->AddEntry(g, fileNames[0].fParameter.c_str(),"l");
				std::vector<TGraph*> graphs;
				graphs.push_back(g);
				for (int i = 1; i < files.size(); i++){
					TGraph* currentGraph = (TGraph*)files[i]->Get(name);
					graphs.push_back(currentGraph);
					currentGraph->SetMarkerStyle(6);
					currentGraph->SetMarkerColor(i+1);
					currentGraph->SetLineColor(i+1);
					currentGraph->Draw("L");
					aLegend->AddEntry(currentGraph, fileNames[i].fParameter.c_str(),"l");
				}

			}
		}
	}
	return 0;
}
