#include "HistoGenerator.h"
#include "PDGMap.h"
#include "CChainGetter.h"
#include "TDirectory.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TH1F.h"
#include <iostream>
#include <assert.h>

using namespace std;

void HistoSet::AddHisto(const std::string SelectionSpec,const std::string &HistoName, TH1F *pHisto)
{
	std::map<std::string, TH1F*> &setP_ParticleHistos=ParticleHistos[SelectionSpec];
	setP_ParticleHistos[HistoName]=pHisto;
	cout << "Added " << SelectionSpec << "to ParticleHistos ";
	cout << "ParticleHistos.size() " << ParticleHistos.size() << endl;
}

const std::map<std::string, TH1F*> *HistoSet::GetParticleHistos(const std::string SelectionSpec) const
{
	if(ParticleHistos.find(SelectionSpec)!=ParticleHistos.end())	{
		return &(ParticleHistos.find(SelectionSpec))->second;
	}
	else	{
		cout << "no " << SelectionSpec << " found!" << endl;
		return NULL;
	}
}

void HistoGenerator::SetRange(const std::string &Range)
{
	fRange=Range;
}

void HistoGenerator::SetDrawCommand(const std::string &DrawCommand)
{
	fDrawCommand=DrawCommand;
}

//for every new Selection a new HistogramSet
void HistoGenerator::AddSelection(const std::string &Selection, const std::string &SelectionSpec)
{
	std::pair< std::string, std::string> sel;
	sel.first=Selection;
	sel.second=SelectionSpec;
	Selections.push_back(sel);
}

void HistoGenerator::SetHistoNameAddOn(const std::string &HistoNameAddOn, short int Color)
{
	fHistoNameAddOn=HistoNameAddOn;
	fColor=Color;
}

void HistoGenerator::FillHistos(const std::string HistogramName)
{
	if(fHistoSets.empty())	{
		return;
	}
	HistoSet *CurrentHistoSet=fHistoSets[fHistoSets.size()-1];
	vector<pair< string, string> >::const_iterator cit;
	for(cit=Selections.begin(); cit!=Selections.end(); ++cit)
	{
		string Selection=cit->first;
		string SelectionSpec=cit->second;
		string CompleteDrawCommand;
		string CompleteHistogramName=HistogramName + SelectionSpec + fHistoNameAddOn;
		CompleteDrawCommand = fDrawCommand +">>"+ CompleteHistogramName + fRange;	

		cout << CompleteDrawCommand << endl;
		cout << Selection << endl;
		TCanvas defaultCanvas;	//do not mess around in other canvases
		fChain->Draw(CompleteDrawCommand.c_str(), Selection.c_str());	
		TH1F *pHisto=(TH1F*)gDirectory->Get(CompleteHistogramName.c_str());	
		assert(pHisto);
		CurrentHistoSet->AddHisto(SelectionSpec,CompleteHistogramName,pHisto);
	}
}


//DrawR specifies histogram range
//DrawC specifies x:y
void HistoGenerator::Generate()
{		
	PDGMap Histos;
	HistoSet* curHistoSet=new HistoSet;
	curHistoSet->SetColor(fColor);
	fHistoSets.push_back(curHistoSet);
	
	//for every PDG Code a Chain is created, a histogram is drawn and the histogram is saved
	map<string,int>::const_iterator cit;
	for(cit=fChainFiles.begin(); cit!=fChainFiles.end(); cit++)
	{
		cout << "Loading Files in: " << (cit->first).c_str() << " (PDCode: " << cit->second << " )" << endl;
		CChainGetter chg;
		chg.SetFileExtension(fFileExtension.c_str());
		chg.SetFileListFile((cit->first).c_str()); 
		chg.ReadFileNames();
		TChain* pC=chg.GetChain();
		if(!pC)	{ 				
			cout << "Error loading Chain" << endl;
			continue;			
		}
		fChain=pC;
		FillHistos(Histos.GetParticleName(cit->second));
		pC->Delete();
	}
}

void HistoGenerator::AddChainFile(int PDGCode, const std::string &FileListFile)
{
	fChainFiles[FileListFile]=PDGCode;
}

void HistoGenerator::ReleaseChainFiles()
{
	fChainFiles.clear();
}

void HistoGenerator::Draw() const
{	
	if(fHistoSets.empty())	{
		return;
	}
	
	cout << "Selections.size() " << Selections.size() << endl;
	vector<pair< string, string> >::const_iterator cit;
	for(cit=Selections.begin(); cit!=Selections.end(); ++cit)
	{
			string SelectionSpec=cit->second;
			cout << "fHistoSets.size() " << fHistoSets.size() << endl;
			for(unsigned int i=0; i<fHistoSets.size(); ++i)
			{
				HistoSet *CurrentHistoSet=fHistoSets[i];
				std::string DrawOption;
				TCanvas *ParticleCanvas;
				if(i==0)	{
					ParticleCanvas=new TCanvas(SelectionSpec.c_str(), SelectionSpec.c_str());
					ParticleCanvas->Divide(2,3);
				}
				else	{
					DrawOption="SAME";
				}
				const std::map<std::string, TH1F*> *ParticleHistos =
				CurrentHistoSet->GetParticleHistos(SelectionSpec);
				if(ParticleHistos)	{
					std::map<std::string, TH1F*>::const_iterator Particles;
					int nSubpad=1;
					for(Particles=ParticleHistos->begin(); Particles!=ParticleHistos->end(); ++Particles)
					{
						cout << "Drawing " << Particles->first << " in Subpad " << nSubpad << endl;
						assert(nSubpad<8);
						ParticleCanvas->cd(nSubpad++);
						TH1F *currentHisto=Particles->second;
						if(currentHisto)	{
							currentHisto->SetLineColor(CurrentHistoSet->GetColor());
							currentHisto->Draw(DrawOption.c_str());
						}
		
					}
				}
			}
	}		
}
