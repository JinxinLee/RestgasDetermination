#ifndef HISTOGRAMSET_H
#define HISTOGRAMSET_H

#include <map>
#include <string>

class TH2F;

class HistogramSet
{			//in a set of histograms all the histograms have the same number of bins, etc
public:		//every histogram is supposed to have a unique name
	HistogramSet();	
	~HistogramSet();
	bool Add(TH2F *pHisto,const std::string &HistogramName);
	void CreateSummary();
	bool GetNextParticleHistogram(TH2F *&pHisto);
	bool GetSummary(TH2F *&pHisto);
	//bool GetNext(TH2F *pHisto);
	bool IsAllowed(TH2F *pHisto);
	bool Has(const std::string &HistogramName);
	TH2F *Get(const std::string &HistogramName);
	HistogramSet *GetScaledHistogramSet(double ScalingFactor) const;
	
	static void GetHistoProperties(TH2F *histo, 	int &nbin_x, double &von_x, double &bis_x,
										int &nbin_y, double &von_y, double &bis_y	);
	void SetNameAddOn(const std::string NameAddOn)	{ fNameAddOn=NameAddOn;	}
private:
	std::map<std::string, TH2F*> fHistoMap;
	std::map<std::string, TH2F*>::iterator fIterator;
	TH2F *fSummary;
	std::string fNameAddOn;	//To ensure unique names
	
	double fVon_x;
	double fBis_x;
	int fNbin_x;
	double fVon_y;
	double fBis_y;
	int fNbin_y;
	
	bool ReturnSummary;
};



#endif 
