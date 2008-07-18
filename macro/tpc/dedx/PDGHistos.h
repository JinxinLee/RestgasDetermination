#ifndef PDGHISTOS_HH
#define PDGHISTOS_HH

#include <string>
#include <vector>
#include "HistogramSet.h"

class TH2F;
class PDGMap;

class PDGHistos	{
public:
	PDGHistos();
	~PDGHistos();
	bool Add(TH2F *pHisto, const std::string &HistogramName);
	void Save(const std::string &strPathFilename);
	//void CreateSummary();
	void Scale(double ScalingFactor);
	void Scale();
	std::string GetParticleName(Int_t PDGCode);
private:	
	PDGMap *p_PDGMap;
		
	std::vector<HistogramSet*> fHistoSets;
	HistogramSet* CurrentHistograms;
};

#endif
