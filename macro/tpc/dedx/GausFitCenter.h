#ifndef GAUSFITCENTER_H
#define GAUSFITCENTER_H

#include <map>
#include <string>

class TGraphErrors;
class TGraph;

class TH1F;
class TF1;
class TH1;
class GausFit;
class PDGMap;
class PndTpcDEDXFits;

class GausFitCenter
{
public:	
	GausFitCenter();
	~GausFitCenter();
	void SetPDGMap(PDGMap *pPDGMap);
	void AddGausHistogram(double p,const std::string &particleName, TH1F* projection);
	
	void CreateBBGraphs();
	TGraphErrors *GetBBGraph(const std::string &ParticleName) const;
	
	void CreateBBFits();
	TF1* GetBBFit(const std::string &ParticleName) const;
	
	void CreateResolutionGraphs();
	TGraph *GetResolutionGraph(const std::string &ParticleName) const;
	
	void CreateSeparationPowerGraphs();
	TGraph *GetSeparationPowerGraph(const std::string &ParticleName1, const std::string &ParticleName2) const;
	
	void Draw() const;	//das ist der gewöhnliche TraceDrawing Output
	void PrintGausFitMap() const;
	
private:
	std::map<double, std::map< std::string, GausFit*> > fGausFitMap;
	std::map<double, TH1 *> fProjectionHistogram;	//needed for traceDrawing	

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
