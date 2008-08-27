#ifndef GAUSFITCENTER_H
#define GAUSFITCENTER_H

#include <map>
#include <string>
#include <vector>

class TGraphErrors;
class TGraph;

class TH1F;
class TF1;
class TH1;
class GausFit;
class PDGMap;
class PndTpcDEDXFits;

//a class for creating and managing gaus fits for dedx data
//can create and manage quantities derived from these gaus fits:
//resolution(particle, momentum), separation power(particle1, particle2, momentum)
//bethe bloch graphs(particle), bethe bloch fits(particle)
//
//of course the added histograms are supposed to have a distribution of truncated mean values
//that belong to a particle with one momentum
class GausFitCenter
{
public:	
	GausFitCenter();
	~GausFitCenter();
	void SetPDGMap(PDGMap *pPDGMap);

	//add a histogram for witch a gaus fit is done
	void AddGausHistogram(double p,const std::string &particleName, TH1F* projection);
	
	void CreateBBGraphs();
	TGraphErrors *GetBBGraph(const std::string &ParticleName) const;
	
	void CreateBBFits();
	TF1* GetBBFit(const std::string &ParticleName) const;
	
	void CreateResolutionGraphs();
	TGraph *GetResolutionGraph(const std::string &ParticleName) const;
	
	void CreateSeparationPowerGraphs();
	TGraph *GetSeparationPowerGraph(const std::string &ParticleName1, const std::string &ParticleName2) const;
	
	//Draw GausFits together with the histograms they where done with
	//for every momentum all the particles will be drawn in one canvas
	//with severel subpads, one subpad accomodates a summary
	void Draw() const;
	void PrintGausFitMap() const;
	
private:
	std::map<double, std::map< std::string, GausFit*> > fGausFitMap;
	std::map<double, TH1 *> fProjectionHistogram;	//these are the histograms the gaus fits were done on

	std::map<std::string, TGraph *> fResolutionGraphs;					
	std::map<std::pair < std::string, std::string >, TGraph *> fSeppoGraphs;  
	
	// a Bethe Bloch Graph for every Particle
	std::map<std::string,TGraphErrors *> fBBGraphs;
	//a Bethe Bloch Fit for every Particle
	std::map<std::string,TF1 *> fBBFit;
	
	PndTpcDEDXFits *BBFits;
	PDGMap *fPDGMap;
};

#endif
