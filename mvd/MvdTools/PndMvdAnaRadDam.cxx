/*
 * PndMvdAnaRadDam.cxx
 *
 *  Created on: Feb 2, 2009
 *      Author: stockman
 */

#include "PndMvdAnaRadDam.h"
#include "TList.h"
#include "TCollection.h"
#include "TObject.h"

#include<iostream>

PndMvdAnaRadDam::PndMvdAnaRadDam()
{
	// TODO Auto-generated constructor stub

}

PndMvdAnaRadDam::~PndMvdAnaRadDam()
{
	// TODO Auto-generated destructor stub
}

void PndMvdAnaRadDam::AnalyzeFiles()
{
	for (unsigned int i = 0; i < fFileList.size(); i++){
		TFile* f = fFileList[i];
		TList* l = f->GetListOfKeys();
		TIter iter(l);
		TObject* ob;
		while(ob = iter()){
			if (TString(ob->GetName()).Contains("1_1")){
				TH2D* histo = (TH2D*)(f->Get(ob->GetName()));
				AddHisto(histo);
			}
		}
		f->Close();
	}
}

void PndMvdAnaRadDam::AddHisto(TH2D* histo)
{
	if(fHistoMap[histo->GetName()] == 0)
		fHistoMap[histo->GetName()] = new TH2D(*histo);
	else fHistoMap[histo->GetName()]->Add(histo);
}

TH2* PndMvdAnaRadDam::GetHisto(int i)
{
	std::map<TString, TH2D*>::iterator iter = fHistoMap.begin();
	for (int j = 0; j < i; j++)iter++;
	return iter->second;
}

TH2* PndMvdAnaRadDam::GetHistoByName(TString name)
{
	return fHistoMap[name];
}

void PndMvdAnaRadDam::SaveHistos(TString fileName)
{
	TFile f(fileName,"RECREATE");
	//f.Dump();
//	f.Write();
	for (std::map<TString, TH2D*>::const_iterator iter = fHistoMap.begin(); iter != fHistoMap.end(); iter++){
		//std::cout << iter->first << std::endl;
		iter->second->Write();
	}
}
