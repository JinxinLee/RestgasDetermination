/*
 * PndMvdAnaRadDam.h
 *
 *  Created on: Feb 2, 2009
 *      Author: stockman
 */

#ifndef PNDMVDANARADDAM_H
#define PNDMVDANARADDAM_H

#include "TFile.h"
#include "TString.h"
#include "TGeoMatrix.h"
#include "TProfile2D.h"

#include <vector>
#include <map>

class PndMvdAnaRadDam
{
public:
	PndMvdAnaRadDam();
	virtual ~PndMvdAnaRadDam();

	void AddFile(TString fileName){AddFile(new TFile(fileName));}
	void AddFile(TFile* f){if (f!=0){fFileList.push_back(f);}}
	int GetNHistos(){return fHistoMap.size();}
	TH2* GetHisto(int i);
	TH2* GetHistoByName(TString name);

	Int_t GetNGoodFiles(){return fCountGoodFiles;}

	void SaveHistos(TString fileName);

	void AnalyzeFiles();



private:
	Int_t fCountGoodFiles;
	std::vector<TFile*> fFileList;
	std::map<TString,TProfile2D*> fHistoMap;

	void AddHisto(TProfile2D* histo);
};

#endif /* PNDMVDANARADDAM_H */
