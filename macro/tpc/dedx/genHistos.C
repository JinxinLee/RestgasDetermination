#include <iostream>
#include <sstream>
#include <map>

#include "TChain.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TObject.h"
#include "TDirectory.h"

#include "PDGHistos.h"
#include "CChainGetter.h"
#include "genHistos.h"
#include "DEDXFitCreator.h"
#include "PndTpcDEDXFits.h"

using namespace std;

void GetChainFiles(string Dir, string JobSumName, map<string, int> &vChainFiles);
//void GetTreesFile(string Dir, string JobSumName, string &FileListFile);

//this is a compiled script
//will be called by acallgenHistos, that loads the necessary librarys
///generates a vector where files are stored that belong to a Particle
///assembles the draw command
//it should be in a macro
bool callgenHistos(string Dir, string JobSumName, string Prefix, double from, double to, string FileExtension=".reco.root")
{
	std::cout << "...callgenHistos!" << std::endl; 
	map<string,int> ChainFiles;
	GetChainFiles(Dir,JobSumName,ChainFiles);
	
	string myFunction;
	if(Prefix=="DEDX")	{
		myFunction="TruncateAndMean";
	}
	else if(Prefix=="DEDXALT")	{
		myFunction="TruncateAndMeanAlternative";
	}
	else	{
		myFunction="TruncateAndMeanAlternative";
		Prefix="DEDXALT";
	}
	
	ostringstream FunctionCall;
	FunctionCall << myFunction << "(" << from << "," << to << ")";
	string myFunctionCall=FunctionCall.str();
	
	
	if(myFunction=="TruncateAndMeanAlternative")	{
		Prefix="DEDXALT";
	}
	else if(myFunction=="TruncateAndMean")	{
		Prefix="DEDX";
	}
	
	string DrawR;
	string myObject;
	string myFactor;
	string Selection;	//the selection P>1.5 is too exclude mal fitted tracks
	if(FileExtension==".raw.root")	{
		myObject="PndTpcRawDEdx";
		//DrawR = "(50, 0, 1, 1300, -2, 11)";
		DrawR = "(300, 0, 3, 1000, 0, 25)";
		myFactor="*1000*1000";
		Selection=myObject + ".fP>0.15 && " + myObject + ".fTrackNr==0 && " + myObject + "." + myFunctionCall+">0";
	}
	else	{
		myObject="PndTpcRawDEdxReco";
		Selection=myObject + ".fP>0.15 && " + myObject + ".fMotherID==-1 && " + myObject + "." + myFunctionCall+">0";
		//DrawR = "(50,0,  1, 1300, -2000, 11000)";
		//DrawR = "(60, 0.15, 0.45, 1300, -2000, 11000)";
		DrawR = "(300, 0, 3, 1300, -2000, 1700000)";
	}
	
	ostringstream AssembleInfoString;
	AssembleInfoString << from << "-" << to;
	string InfoString=Prefix+AssembleInfoString.str();
	
	string SaveTo=Dir + InfoString + myObject + "_" + JobSumName + "_" + "scaledHistos.root"; //TODO - tollerer Namen
	cout << "Saving Histos to " << SaveTo.c_str() << endl;
	
	ostringstream DrawC;
	DrawC << myObject << "." << myFunctionCall << myFactor << ":" << myObject << ".fP";
	cout << "Joice: " << DrawC.str().c_str() << endl;
	cout << "Selection: " << Selection << endl;
	
	genHistos( DrawC.str(), DrawR , Selection, ChainFiles, SaveTo.c_str(), FileExtension );
	
	return true;
}

//will be called by callgenHistos
//generate histograms from trees in TxtFiles and save them to SaveToFile
void FillHistos(string DrawCommand, string Range, string Selection, TChain *pChain, string HistogramName, PDGHistos &Histos)
{
	string CompleteDrawCommand;
	CompleteDrawCommand = DrawCommand +">>"+ HistogramName + Range;	
	cout << CompleteDrawCommand << endl;
	cout << Selection << endl;
	pChain->Draw(CompleteDrawCommand.c_str(), Selection.c_str());	
	TH2F *pHisto=(TH2F*)gDirectory->Get(HistogramName.c_str());	
	Histos.Add(pHisto,HistogramName);
	pHisto=NULL;
}

//DrawR specifies histogram range
//DrawC specifies x:y
void genHistos( string DrawC, string DrawR, string Selection, map<string,int> ChainFiles, string SaveToFile, string FileExtension )
{		
	PDGHistos Histos;

	//for every PDG Code a Chain is created, a histogram is drawn and the histogram is saved
	map<string,int>::const_iterator cit;
	for(cit=ChainFiles.begin(); cit!=ChainFiles.end(); cit++)
	{
		cout << "Loading Files in: " << (cit->first).c_str() << " (PDCode: " << cit->second << " )" << endl;
		CChainGetter chg;
		chg.SetFileExtension(FileExtension.c_str());
		chg.SetFileListFile((cit->first).c_str()); 
		chg.ReadFileNames();
		TChain* pC=chg.GetChain();
		if(!pC)	{ 				
			cout << "Error loading Chain" << endl;
			continue;			
		}
		FillHistos(DrawC, DrawR, Selection,pC,Histos.GetParticleName(cit->second), Histos);
		pC->Delete();
	}
	Histos.Scale();
	Histos.Save(SaveToFile.c_str());
}

//knows how files look like, checks if files exist and gives back a vector with the names of the valid files
void GetChainFiles(string Dir, string JobSumName, map<string, int> &vChainFiles)
{
	string AddOn="Gesamt";
	//.gen files have to be generated first by the AtJobEnd Script  
	//these files contain all the jobs that belong to a particle 
	string file1=Dir+JobSumName+AddOn+"P321.gen";
	cout << file1.c_str() << endl;
	vChainFiles[file1.c_str()]=321;
	
	string file2=Dir+JobSumName+AddOn+"P2212.gen";
	cout << file2.c_str() << endl;	
	vChainFiles[file2.c_str()]=2212;
	
	string file3=Dir+JobSumName+AddOn+"P211.gen";
	cout << file3.c_str() << endl;	
	vChainFiles[file3.c_str()]=211;
	
	string file4=Dir+JobSumName+AddOn+"P11.gen";
	cout << file4.c_str() << endl;	
	vChainFiles[file4.c_str()]=11;	
	
}

/*
void GetTreesFile(string Dir, string JobSumName, string &FileListFile)
{
	FileListFile=Dir+JobSumName+"ALL"+".gen";
}
*/
