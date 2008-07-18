#ifndef SEPARATIONPOWER_HH
#define SEPARATIONPOWER_HH

#include <string>
#include <map>
#include <vector>

class TGraph;
class TF1;
class TH2F;

///\class SeparationPower 
///Separation Power for a specific Particle-Pair
///Function SeparationPower vs. X (e.g Momentum)
///
class SeparationPower
{
public:
	SeparationPower(const std::string &Name1,const std::string &Name2, const Color_t &color=kRed) : ParticleName1(Name1), ParticleName2(Name2), fColor(color)	{}
	~SeparationPower() {}
	
	//!for every Momentum, there is a gaus - fits for one particle species 
	void FillSeparationPower(const std::map<Double_t, std::map<std::string, TF1*> > &FitMap);
	
	//!2 gaus functions at a certain momentum
	void FillSeparationPower(TF1 *FitFunction1, TF1 *FitFunction2, Double_t X);
	
	//!2 times mean and sigma at certain value X
	void FillSeparationPower(Double_t Mean1, Double_t Mean2, Double_t Sigma1, Double_t Sigma2, Double_t X);
	
	//! Seppo-Value at certain value X
	void FillSeparationPower(Double_t SepPo,Double_t X);
	
	
	bool IsFor(const std::string &ParticleName) const { return( ParticleName==ParticleName1 || ParticleName==ParticleName2 ); } 
	bool IsFor(const std::string &ParticleName1,const std::string &ParticleName2)	const { return( IsFor(ParticleName1) && IsFor(ParticleName2) ); } 
	std::string GetTheOtherParticle(const std::string &ParticleName) const;
	void GetParticleNames(std::string &particleName1, std::string &particleName2) const {particleName1=ParticleName1; particleName2=ParticleName2;} 
	
	//! Fill a Histogram with the values of this class
	void FillHistogram(TH2F  *pHisto);
	
	//! Create a Graph from the values of this class
	TGraph *CreateGraph() const;
	
	bool CheckSanity()	const {	return(vSepPo.size() == vP.size()); }
	int Size() const {	return(vSepPo.size());	} 
	double GetMean() const;
	double GetMax() const; 
	
private:
	std::string ParticleName1;	
	std::string ParticleName2;
	Color_t fColor;	 
	
	std::vector<Double_t> vP;		//!< x-values	they have to be sorted! 
	std::vector<Double_t> vSepPo;	 	//!< y-values  
};

#endif
