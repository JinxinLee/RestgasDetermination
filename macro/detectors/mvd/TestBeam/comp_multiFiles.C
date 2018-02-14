#include <iostream>
#include <utility>
#include <algorithm>

using std::cout;
using std::endl;

//* comp_multiFiles.C is a macro to combine identical histograms or graphs from different files in combined histograms.
//* The macro needs an input file (default is "inputFiles.txt") which contains the following information
//* Per line: - file name (optional with complete path) as string which contains the histograms to combine
//*           - identifier used in TLegend of histograms as string
//*           - scaling of histograms as double (to overlay the histograms they can be scaled to match to the same basis)
//*                                             (the macro takes the maximum number and scales all other histograms with maxScale/individualScale)
//* The second input parameter of the macro is the number of histograms to be shown on one canvas (default 9). The position of the canvas is the automatically calculated
//* and new canvasses are automatically created

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

int comp_multiFiles(std::string listOfFiles_File = "inputFiles.txt", int picpercan = 9) {

	std::vector<TCanvas*> canvasses;
	std::string filter = "";

	std::vector<fileData> fileNames = GetFileNames(listOfFiles_File);
	std::vector<TFile*> files;
	for (auto name : fileNames){
		files.push_back(new TFile(name.fFileName.c_str(), "READ"));
	}

	auto maxScalingFactor = std::max_element(fileNames.begin(), fileNames.end(), [](fileData const& lhs, fileData  const& rhs){return lhs.fScalingFactor < rhs.fScalingFactor;});
	double maxScaling = maxScalingFactor->fScalingFactor;

	std::cout << "Max Scaling Factor: " << maxScalingFactor->fScalingFactor << std::endl;
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
				h->Scale(maxScaling/fileNames[0].fScalingFactor);
				h->SetLineWidth(2);
				std::vector<TH1*> histos;
				histos.push_back(h);
				for (int i = 1; i < files.size(); i++){
					TH1* currentHisto = (TH1*)files[i]->Get(name);
					currentHisto->Scale(maxScaling/fileNames[i].fScalingFactor);
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
