#ifndef GAUSFITCENTERINITIATOR_H
#define GAUSFITCENTERINITIATOR_H

#include <vector>

class TH2F;
class TH1F;
class GausFitCenter;
class PDGMap;

class GausFitCenterInitiator
{
public:
	GausFitCenterInitiator();
	~GausFitCenterInitiator();
	void AddParticleHistogram(TH2F *ParticleHisto)	{fParticleHistos.push_back(ParticleHisto);}
	void InitGausFitCenter();
	GausFitCenter *GetGausFitCenter() const { return ParticleFitCreator;	}
	void SetMomenta();
	void SetMomentaV2();
	void SetPDGMap(PDGMap *pPDGMap);
private:
	std::vector<TH2F *> fParticleHistos;
	std::vector<double> vP_set;
	GausFitCenter *ParticleFitCreator;
	
	bool IsInVector(double momentum, const std::vector<double> &vP_set) const;
	double BinToMomentum(TH2F *h, int bin) const;
	void PrepareGausFitHistogram(double P_set, const std::string &strName, TH1F *pHisto) const;
};

#endif

