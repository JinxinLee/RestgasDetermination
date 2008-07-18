#ifndef FITSERVICES_H
#define FITSERVICES_H

#include <string>
#include <vector>
#include "PndTpcDEDXFits.h"
class TH2F;
class TH1F;
class GausFitCenter;
//class PndTpcDEDXFits;

class FitServices
{
public:
	FitServices();
	~FitServices();
	/// add a histogram DEDX:P for one Particle
	void SetParticleHistogram(TH2F *ParticleHisto);
	
	/// the Bethe Bloch Minimum
	double GetBBMinimum();
	void DrawBBFit() const;

private:
	TH2F *fParticleHisto;
	GausFitCenter *ParticleFitCreator;
	PndTpcDEDXFits BBFits;
	
	std::vector<double> vP_set;
	bool IsInVector(double momentum, const std::vector<double> &vP_set) const;
	void SetMomenta();
	void InitGausFitCenter();
	void PrepareGausFitHistogram(double P_set, const std::string &strName, TH1F *pHisto) const;
	
	double BinToMomentum(TH2F *h, Int_t bin) const;
};

#endif
