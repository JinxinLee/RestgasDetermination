/*
 * PndMvdAnaRadDam.h
 *
 *  Created on: Feb 2, 2009
 *      Author: stockman
 */

#ifndef PNDMVDANARADDAM_H_
#define PNDMVDANARADDAM_H_

#include "TFile.h"
#include "TString.h"
#include "TGeoMatrix.h"
#include "TH2.h"

#include <vector>
#include <map>

class PndMvdAnaRadDam
{
public:
	PndMvdAnaRadDam();
	virtual ~PndMvdAnaRadDam();

	void AddFile(TString fileName){AddFile(new TFile(fileName));}
	void AddFile(TFile* f){fFileList.push_back(f);}
	int GetNHistos(){return fHistoMap.size();}
	TH2* GetHisto(int i);
	TH2* GetHistoByName(TString name);

	void SaveHistos(TString fileName);

	void AnalyzeFiles();



private:
	std::vector<TFile*> fFileList;
	std::map<TString,TH2D*> fHistoMap;

	void AddHisto(TH2D* histo);
};

#endif /* PNDMVDANARADDAM_H_ */
