#ifndef FITSERVICES_H
#define FITSERVICES_H

#include <string>
#include <vector>
#include "PndTpcDEDXFits.h"
class TH2F;
class TH1F;
class GausFitCenterInitiator;
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

	void ClearMomenta();
	void AddMomentum(double P);

private:
	TH2F *fParticleHisto;
	PndTpcDEDXFits BBFits;
	GausFitCenterInitiator *ParticleFitCreatorOwner;
};

#endif
