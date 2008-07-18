#include "PndTpcDEDXFits.h"
#include "TF1.h"
#include "TProfile.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include "assert.h"

using namespace std;

double Res::GetResolution(double AddCoord)
{
	double old_min_distance=100000.;
	assert(m_Resolution.size()==m_Coordinates.size());
	double min_distance=0.;
	double ret=0.;
	unsigned int size=m_Resolution.size();
	for(unsigned int i=0; i<size; ++i)
	{
		min_distance=std::abs(m_Coordinates[i]-AddCoord);
		if(min_distance < old_min_distance)	{ 
			old_min_distance=min_distance;
			ret=m_Resolution[i];
		}
	}
	return ret;
}

std::ostream& operator<<(std::ostream &s, const Res &me)
{
	assert(me.m_Resolution.size()==me.m_Coordinates.size());
	unsigned int size=me.m_Resolution.size();
	for(unsigned int i=0; i<size; ++i)
	{
		s << "(" << me.m_Coordinates[i] << "," << me.m_Resolution[i] << ") ";
	}
	return s;
}

double PndTpcDEDXFits::dedxfunc(double *x, double *par)
{
	Double_t dedx;	
	//dedx=par[0] + par[1]/(x[0]*x[0]) + (par[2]/(x[0]*x[0]))*log(x[0]*x[0]);	//3 parameters	
	dedx=par[0] + par[1]/(x[0]) + (par[2]/(x[0]))*log(x[0]*x[0]);	//this is a good one to get the high P raising branch
	//dedx=par[0] + par[1]/(x[0]*x[0]) + (par[2]/(x[0]*x[0]))*log(x[0]*x[0]);
	return dedx;
}

double PndTpcDEDXFits::dedxfunc_e(double *, double *par)
{
	Double_t dedx;
	dedx=par[0]; //+ par[1]*x[0];
	return dedx;
}

void PndTpcDEDXFits::CreateFitForProfile(TProfile *pPro, const string &name)
{
	TF1 *fitFcn=CreateFunction(name); 
	Int_t nResult=pPro->Fit(name.c_str(), "WLLNOR");
	//W: i ignore the errors 
	//|-/> (perhaps it would work, too if i only ignore bins with one single entry, or make the bin size bigger
	//LL: loglikelihood 
	//NO: i don' t want to draw this action here
	//R: use Ranges of function
	if(nResult!=0)	{	//only if fit is succesfull
		cout << "PndTpcDEDXFits: Fit " << name << "is not okay!" << endl;
		return;
	}
	FitFunctions[name]=fitFcn;
}

void PndTpcDEDXFits::CreateFitForGraph(TGraphErrors *pGraph, const std::string &name)
{
	TF1 *fitFcn=CreateFunction(name);
	Int_t nResult=pGraph->Fit(name.c_str(), "WLLNOR");
	if(nResult!=0)	{	//only if fit is succesfull
		cout << "PndTpcDEDXFits: Fit " << name << "is not okay!" << endl;
		return;
	}
	FitFunctions[name]=fitFcn;
}

void PndTpcDEDXFits::SetAdditionalCoordinate(char Specifier, double Coordinate)
{
	AddCoord=Coordinate;
	Spec=Specifier;
}

void PndTpcDEDXFits::SetAdditionalCoordinateBB(char Specifier, double Coordinate)
{
	AddCoordBB=Coordinate;
	SpecBB=Specifier;
}

void PndTpcDEDXFits::SaveFitParameters(const string &filename) const
{
	ofstream ofs;
	ofs.open (filename.c_str(), ios_base::app);
	if(!ofs)	{
		cout << "Error opening output file!" << endl;
		return;
	}
	ofs << *this;
	ofs.close();
}

void PndTpcDEDXFits::Print()
{
	cout << *this << endl;
}

void PndTpcDEDXFits::LoadFile(const string &filename)
{
	string line;
	ifstream ifs (filename.c_str());
	if(!ifs)	{
		cout << "PndTpcDEDXFits: Error opening PID-file(1)!" << endl;
		return;
	}
	string ParticleName;
	Double_t Par0=0., Par1=0., Par2=0.;
	char B='l';
	char ReadSpecBB='N';
	double ReadAddCoordBB=0.;
	// read a line using the extraction operator
	while(ifs >> B >> ReadSpecBB >> ReadAddCoordBB >> ParticleName >> Par0 >> Par1 >> Par2) {
		if(B=='B' && ReadSpecBB ==SpecBB)	{
			cout << "[ "
			<< B << ' '
			<< SpecBB << ' '
			<< AddCoordBB << ' '
			<< ParticleName << ' '
			<< Par0 << ' '
			<< Par1 << ' '
			<< Par2 << " ] " << endl;
			TF1 *fitFcn=CreateFunction(ParticleName);
			fitFcn->SetParameter(0,Par0);
			fitFcn->SetParameter(1,Par1);
			fitFcn->SetParameter(2,Par2);
			FitFunctions[ParticleName]=fitFcn;
			AddCoordBB=ReadAddCoordBB;
		}
	}	
	ifs.close();
}

double PndTpcDEDXFits::GetResolution(std::string ParticleName, double AdditionalCoordinate)
{
	if(Resos.find(ParticleName)==Resos.end())	{
		return 0.;
	}
	double R=(Resos[ParticleName].GetResolution(AdditionalCoordinate));
	return R;
}

bool PndTpcDEDXFits::GetInfo(std::string ParticleName, double P, double measuredDEDX, double &Probability, double AddC)
{
	if(FitFunctions.find(ParticleName)==FitFunctions.end())	{
		//cout << "No BetheBloch Fit available for Particle " << ParticleName << "!" << endl;
		Probability=0.;
		return false;
	}
	if(Resos.find(ParticleName)==Resos.end())	{
		//cout << "No Resolution available for Particle " << ParticleName << "!" << endl;
		Probability=0.;
		return false;
	}
	double DEDX=(FitFunctions[ParticleName])->Eval(P);
	//cout << "DEDX: " << DEDX << endl;
	double R=(Resos[ParticleName].GetResolution(AddC));
	double Sigma=R*DEDX;
	
	//convert distance DEDX-measuredDEDX into probabilty, 0=very close, 1=far away  
	double Prob=TMath::Erf( (std::abs(DEDX-measuredDEDX) / (TMath::Sqrt(2.)*Sigma)) );
	
	//reverse
	Probability=1.-Prob;
	if(Probability ==0.)	{
		return true;
	}
	
	double SumOfProbabilities;
	GetSumOfProp( P, measuredDEDX, SumOfProbabilities, AddC );
	assert(SumOfProbabilities);
	Probability=Probability/SumOfProbabilities;
	return true;
}

bool PndTpcDEDXFits::GetSumOfProp(double P, double measuredDEDX, double &SumOfProbabilities, double AddC)
{
	SumOfProbabilities=0.;
	std::map< std::string, TF1 *>::const_iterator cit;
	std::map< std::string, TF1 *>::const_iterator end=FitFunctions.end();
	for(cit=FitFunctions.begin(); cit!=end; ++cit)
	{
		string ParticleName=cit->first;
		if(Resos.find(ParticleName)!=Resos.end())	{
			double DEDX=(FitFunctions[ParticleName])->Eval(P);
			double R=(Resos[ParticleName].GetResolution(AddC));
			double Sigma=R*DEDX;
			double Prob=TMath::Erf( (std::abs(DEDX-measuredDEDX) / (TMath::Sqrt(2.)*Sigma)) );
			double Probability=1.-Prob;
			SumOfProbabilities+=Probability;
		}
	}
}

void  PndTpcDEDXFits::LoadResolution(const std::string &filename)
{
	string line;
	ifstream ifs (filename.c_str());
	if(!ifs)	{
		cout << "PndTpcDEDXFits: Error opening PID-file!" << endl;
	}
	string ParticleName;
	Double_t Resolution;
	char R='l';
	char ReadSpec='N';
	double ReadAddCoord=0.;
	
	while(ifs >> R  >> ReadSpec >> ReadAddCoord >> ParticleName >> Resolution) {
		if(R=='R' && ReadSpec==Spec )	{
			cout << R << " " << Spec << " " << AddCoord  << " "
			<< ParticleName << " " << Resolution << endl;
			Resos[ParticleName].SetResolution(Resolution, ReadAddCoord);
			AddCoord=ReadAddCoord;
		}
	}
}


TF1* PndTpcDEDXFits::CreateFunction(const string &name)
{
	Double_t min=0.03; //1/x + log x => exclude zero
	Double_t max=3.;
	Int_t nParameters=3;
	//string newname=name+FunctionNameAddOn;	//if i use better names than "p+" i can use this
	string newname=name;
	cout << "CreateFunction with name " << newname << endl;
	TF1 *fitFcn = new TF1(newname.c_str(), this, &PndTpcDEDXFits::dedxfunc, min, max, nParameters,"PndTpcDEDXFits","dedxfunc");	
	assert(fitFcn);
	return fitFcn;		
}

TF1* PndTpcDEDXFits::GetFunction(const std::string &name) const
{
	cout << "Request for Function ---" << name << "---" << endl;
	if(FitFunctions.find(name)!=FitFunctions.end())	{
		return (FitFunctions.find(name))->second;
	}
	else {
		cout << "Returning NULL!" << endl;
		return NULL;
	}
}

std::ostream& operator<< (std::ostream& s, const PndTpcDEDXFits& me){
	map< string, TF1 *>::const_iterator cit;
	for(cit=me.FitFunctions.begin();cit!=me.FitFunctions.end(); cit++)
	{
	s << "B"<<me.SpecBB<<" "<<me.AddCoord<<" "<< cit->first;
		for (int i=0; i<(cit->second)->GetNpar(); i++)
		{
			s << " " << (cit->second)->GetParameter(i);
		}
		s << "\n";
	}	
	std::map< std::string, Res >::const_iterator cjt;
	for(cjt=me.Resos.begin();cjt!=me.Resos.end();++cjt)
	{
	s << "R"<<me.Spec<<" "<< cjt->first << " " << cjt->second << "\n";	
	}
	return s;
}
