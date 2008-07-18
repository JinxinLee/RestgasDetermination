#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>

#include "TFile.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TROOT.h"
#include "TGraph2D.h"

#include "PDGMap.h"
#include "DEDXFitCreator.h"
#include "DEDXDistribana.h"
#include "seppo.h"
#include "Resolution.h"
#include "ResoHandler.h"
#include "SepPoHandler.h"
#include "SeparationPower.h"
#include "seppo.h"
#include "PndTpcDEDXFits.h"


using namespace std;


void SetCanvasTitleExtension(const string &CanvasTitleExt) 
{
	Int_t nCanvases=gROOT->GetListOfCanvases()->GetEntries();
	for(int i=0; i<nCanvases; i++)
	{
		TCanvas *cC=(TCanvas*)gROOT->GetListOfCanvases()->At(i);
		string title=cC->GetTitle();
		//look for Canvases that have no extension yet 
		if(title.find_first_of("----")==string::npos)	{
			string newTitle=title+"----"+CanvasTitleExt;
			cC->SetTitle(newTitle.c_str());
			cC->SetName(newTitle.c_str());
		}
	}
}

bool callrunSeppo(string Dir, string JobSumName, string FunctionSpec, double from, double to, string FileType="reco")
{	
	string myObject;
	if(FileType == "raw")	{
		myObject="PndTpcRawDEdx";
	}
	else	{
		myObject="PndTpcRawDEdxReco";
	}

	ostringstream AssembleInfoString;
	AssembleInfoString << from << "-" << to;
	string InfoString=FunctionSpec+AssembleInfoString.str();
	string SeppoHisto=InfoString + myObject + "_" + JobSumName + "_" + "scaledHistos.root"; //TODO
	string SeppoFile=Dir+SeppoHisto.c_str();
	cout << "Calculating SeparationPower for File: " << SeppoFile.c_str() << endl; 		
	seppo(SeppoFile.c_str(), 1);
	
	string CanvasTitleExt=myObject+InfoString;
	SetCanvasTitleExtension(CanvasTitleExt);
	
	return true;
}

void WriteSummaryFile(const std::vector<string> &TheSummary, std::map<std::string, Double_t> &Statistic, string Filename)
{

//Write a grepable Summary File
	ofstream SummaryFile;
	SummaryFile.open("SeppoSummary.txt", ios_base::app);
	if(SummaryFile)	{
		SummaryFile << "###########################################################################################\n";
		SummaryFile << "###########################################################################################\n" << endl;
		string HistoName=Filename.substr(Filename.find_last_of("/")+1); 
		SummaryFile << Filename << ":    " << endl;
		{
			vector<string>::const_iterator ci;
			for(ci=TheSummary.begin(); ci!=TheSummary.end(); ci++)
			{
				SummaryFile << HistoName << ":   ";
				SummaryFile << (*ci) << endl;
			}
		}
		{
			map<string,Double_t>::const_iterator ci;
			for(ci=Statistic.begin();ci!=Statistic.end(); ci++)
			{
				SummaryFile << HistoName << ":   ";
				SummaryFile << ci->first << ": " << ci->second << " Entries" << endl; 
			}
		} 
		 //outfile.open ("test.txt", ofstream::out | ofstream::app);	
		 SummaryFile << "\n\n\n" << endl;
		 SummaryFile.close();
	}
   	else	{
		cout << "Error: SummaryFile could not be opened!!!!!!!!!!!!!!" << endl;
	}
	
}


PndTpcDEDXFits *MakeBBlochFits()
{
	DEDXFitCreator BB;
	BB.CreateFits();
	BB.traceDrawing();
	BB.DrawSummary();
	double min=BB.GetMinimum();
	cout << "Got scaling factor: " << min << endl;
	PndTpcDEDXFits * ret=BB.GetFits();	
	return ret;
}


//Gets a root file where histograms of the dedx-Distribution is stored
void seppo(string Filename, bool bShowDEDXDistribution)
{
	TFile *f = new TFile(Filename.c_str());
	assert(f);
	
	/*
	TH2F *h1 = (TH2F *) gROOT->FindObject("p+");
	if(h1)	{
		TCanvas *c1=new TCanvas("h1", "h1");
		h1->Draw();
	}
	TH2F *h2 = (TH2F *) gROOT->FindObject("Summary");
	h2->Draw();	
	h->SetShowProjectionY(1);
	f->Close();
	*/
	
	
	DEDXDistribana distribution;
	distribution.SetFunctionNameAddOn("_d");
	map<string, Double_t> Statistic;
	distribution.FillDEdxDistributions( Statistic);
	//MakeBBlochFits();
	distribution.CreateDeDxDistributionGausFits();
	
	distribution.CreateGraphs();
	distribution.CreateBBFits();
	distribution.traceDrawing();
	distribution.DrawDEDXSummary();
	
	
	
	SepPoHandler TheSep;
	ResoHandler TheReso;
	
	if(bShowDEDXDistribution)	{
		distribution.DrawDeDxDistribution();
	}
	map<Double_t, map<string, TF1*> > FitMap;
	distribution.GetFitMap(FitMap);
	TheSep.FillSeparationPower(FitMap);
	TheReso.Fill(FitMap);
	TheSep.Draw();
	TheReso.Draw();

	vector<string> TheSummary;
	TheSep.GetSummary(TheSummary);
	distribution.PrintChi2();
	distribution.PrintBBFitChi2();
	//WriteSummaryFile(TheSummary,Statistic, Filename);
}

void test2D()
{
	Resolution reso("test");
	TFile *f = new TFile("/nfs/data/panda-mc/noStrag/t40/t40/DEDX0-1PndTpcRawDEdxReco_t40_Histos.root");	
	//TH2F *h = (TH2F *) gROOT->FindObject("Pi+");
	
	DEDXDistribana distribution;
	map<string, Double_t> Statistic;
	distribution.FillDEdxDistributions(Statistic);
	
	ResoHandler TheReso;
	map<Double_t, map<string, TF1*> > FitMap;
	distribution.CreateDeDxDistributionGausFits();
	distribution.GetFitMap(FitMap);
	TheReso.Fill(FitMap, 40);
	f->Delete();
	
	f = new TFile("/nfs/data/panda-mc/noStrag/t60/t60/DEDX0-1PndTpcRawDEdxReco_t60_Histos.root");
	distribution.FillDEdxDistributions(Statistic);
	distribution.CreateDeDxDistributionGausFits();
	distribution.GetFitMap(FitMap);
	TheReso.Fill(FitMap, 60);
	f->Delete();
	
	f = new TFile("/nfs/data/panda-mc/noStrag/t80/t80/DEDX0-1PndTpcRawDEdxReco_t80_Histos.root");
	distribution.FillDEdxDistributions( Statistic);
	distribution.CreateDeDxDistributionGausFits();
	distribution.GetFitMap(FitMap);
	TheReso.Fill(FitMap, 80);
	f->Delete();	
	
	Resolution *pr=TheReso.GetResolution("K+");
	TGraph2D *pGraph2D=pr->CreateGraph2D();
	pGraph2D->Draw("surf1");
} 

