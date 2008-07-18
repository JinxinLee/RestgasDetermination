#ifndef GAUSFIT_H
#define GAUSFIT_H

#include <string>

class TH1F;
class TF1;
class PDGMap;

class GausFit
{
public:
	GausFit(double P,const std::string &ParticleName, TH1F* projection);	
	~GausFit();
	void Fit(PDGMap *p_PDGMap=NULL);
	double GetMean() 	const	{ return fMean; }
	double GetSigma() 	const	{ return fSigma; }
	double GetP() 		const	{ return fP;	}
	unsigned int GetN() const	{ return fN; }
	
	// to draw them
	TH1F *GetProjection() 	const;
	TF1 *GetGausFit() 		const;
	double GetResolution() 	const;
	double GetSeparationPower(const GausFit &gausFit) const;
private:
	std::string fParticleName;
	TH1F *fProjection;	//the Projection the gaus fit is made from
	TF1 *fGausFit;
	double fSigma;
	double fMean;
	double fP;
	unsigned int fN;
	int fMinEntries;
	double fSumMax;
	
	void PrepareGausFit(TF1 *FitFcn) const;
};

#endif
