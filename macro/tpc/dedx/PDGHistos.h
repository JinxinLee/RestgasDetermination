#ifndef PDGHISTOS_HH
#define PDGHISTOS_HH

#include <string>
#include <vector>
#include "HistogramSet.h"

class TH2F;
class PDGMap;

//add histograms which contain all information for a particle (dE/dx:P)
//will create a summary histogram which contains all particles and looks nice
//can created a scaled set of the histogram, if the pion minimum can be found
//WARNING: the fit is done with a set of hard coded momenta
//that means the fit seen with DEDXHistoProcessing may not be the fit here
//the momenta should have their own class and should be set globally in a macro
//so that they are the same for the histogram generation and the histogram processing
class PDGHistos	{
public:
	PDGHistos();
	~PDGHistos();
	bool Add(TH2F *pHisto, const std::string &HistogramName);
	void Save(const std::string &strPathFilename);
	void Scale(double ScalingFactor);
	void Scale();
	std::string GetParticleName(Int_t PDGCode);
private:	
	PDGMap *p_PDGMap;
		
	std::vector<HistogramSet*> fHistoSets;
	HistogramSet* CurrentHistograms;
};

#endif
