#ifndef RESOLUTION_HH
#define RESOLUTION_HH

#include <vector>
#include <string>
#include <ostream>
#include "Coordinates.h"
class TF1;
class TGraph;
class TGraph2D;



class Resolution
{
public:
	Resolution(const std::string &Name, const Color_t &color=kRed): m_ParticleName(Name), fColor(color)	{}
	~Resolution();
	void Fill(const std::string &ParticleName, double P, double Reso);
#if 0
	void Fill(Coordinates co, double Reso);
#endif
	void Fill(double P, double Reso);
	void Fill(double P, double Theta, double Reso);
	void Fill(double X, double Theta, TF1* FitFunc);
	void Fill(double X, TF1* FitFunc);
	
	double GetMean() const;
	TGraph *CreateGraph(int i=0) const;
	TGraph2D *CreateGraph2D(int i=0, int j=1) const;
	std::string GetParticleName()	const {return m_ParticleName;}
	bool IsFor(std::string ParticleName) const { return( ParticleName==m_ParticleName ); } 
	friend std::ostream& operator<< (std::ostream& s, const Resolution &me);
private:
	std::string m_ParticleName;
	Color_t fColor;
	std::vector<double> vReso;
	std::vector<Coordinates> vCoordinates;
};

#endif
