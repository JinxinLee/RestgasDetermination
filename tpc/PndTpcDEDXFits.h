#ifndef DEDXFITS
#define DEDXFITS

#include <ostream>
#include <map>
#include <string>
#include <vector>
//#include "TObject.h"

class TProfile;
class TF1;
class TGraphErrors;

class Res
{
public:
	double GetResolution(double AddCoord);
	void SetResolution(double reso, double AddCoord)	{ m_Resolution.push_back(reso);
												  m_Coordinates.push_back(AddCoord);	}
	friend std::ostream& operator<<(std::ostream &s, const Res &me);											  	
private:
	std::vector<double> m_Resolution;
	std::vector<double> m_Coordinates;
};

class PndTpcDEDXFits
{
public:
	PndTpcDEDXFits():AddCoord(0.),Spec('N'),AddCoordBB(0.),SpecBB('N'),FunctionNameAddOn("fnc"){}
	~PndTpcDEDXFits(){}
	double dedxfunc_e(double *, double *par);
	double dedxfunc(double *x, double *par);
	void CreateFitForProfile(TProfile *pPro, const std::string &name);
	void CreateFitForGraph(TGraphErrors *pGraph, const std::string &name);
	TF1* GetFunction(const std::string &name) const;
	double GetResolution(std::string ParticleName, double AdditionalCoordinate);
	void SaveFitParameters(const std::string &filename) const;
	bool GetInfo(std::string ParticleName, double P, double measuredDEDX, double &Probability, double AddC=0.); 
	bool GetSumOfProp(double P, double measuredDEDX, double &SumOfProbabilities, double AddC);
	void LoadFile(const std::string &filename);
	void LoadResolution(const std::string &filename);
	void SetAdditionalCoordinate(char Specifier, double Coordinate);
	void SetAdditionalCoordinateBB(char Specifier, double Coordinate);
	void Print();
	void SetFunctionNameAddOn(const std::string &AddOn)	{ FunctionNameAddOn=AddOn;	}
	friend std::ostream& operator<<(std::ostream &s, const PndTpcDEDXFits &me);
private:
	std::map< std::string, TF1 *> FitFunctions;	//Fits for Particles
	std::map< std::string, Res > Resos;
	TF1* CreateFunction(const std::string &name);
	double AddCoord;
	char Spec;
	double AddCoordBB;
	char SpecBB;
	std::string FunctionNameAddOn;
};

#endif
