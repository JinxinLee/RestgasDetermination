#include "PDGMap.h"
#include <iostream>
using namespace std;

PDGMap::PDGMap()
{
	FillParticleColors();
	FillSeparationPowerColors();
	FillPDGMapV2();
	//FillScaled();
}

void PDGMap::FillParticleColors()
{
	Colors[11]=kRed;
	Colors[211]=kGreen;
	Colors[2212]=kBlue;
	Colors[321]=kMagenta;
}

void PDGMap::FillSeparationPowerColors()
{
	{
		std::pair <int, int> ElectronPion;
		ElectronPion.first=11;
		ElectronPion.second=211;
		SeppoColors[ElectronPion]=kRed;
		
		std::pair <int, int> KaonPion;
		KaonPion.first=321;
		KaonPion.second=211;
		SeppoColors[KaonPion]=kMagenta;	
		
		std::pair <int, int> ProtonPion;
		ProtonPion.first=2212;
		ProtonPion.second=211;
		SeppoColors[ProtonPion]=kBlue;
		
		std::pair <int, int> KaonProton;
		KaonProton.first=321;
		KaonProton.second=2212;
		SeppoColors[KaonProton]=kGreen;
		
		std::pair <int, int> KaonElectron;
		KaonElectron.first=321;
		KaonElectron.second=11;
		SeppoColors[KaonElectron]=kYellow;
		
		std::pair <int, int> ProtonElectron;
		ProtonElectron.first=2212;
		ProtonElectron.second=11;
		SeppoColors[ProtonElectron]=kCyan;
	}	
	
	{
		std::pair <int, int> ElectronPion;
		ElectronPion.first=211;
		ElectronPion.second=11;
		SeppoColors[ElectronPion]=kRed;
		
		std::pair <int, int> KaonPion;
		KaonPion.first=211;
		KaonPion.second=321;
		SeppoColors[KaonPion]=kMagenta;	
		
		std::pair <int, int> ProtonPion;
		ProtonPion.first=211;
		ProtonPion.second=2212;
		SeppoColors[ProtonPion]=kBlue;
		
		std::pair <int, int> KaonProton;
		KaonProton.first=2212;
		KaonProton.second=321;
		SeppoColors[KaonProton]=kGreen;
		
		std::pair <int, int> KaonElectron;
		KaonElectron.first=11;
		KaonElectron.second=321;
		SeppoColors[KaonElectron]=kYellow;
		
		std::pair <int, int> ProtonElectron;
		ProtonElectron.first=11;
		ProtonElectron.second=2212;
		SeppoColors[ProtonElectron]=kCyan;
	}		
	
}

void PDGMap::FillPDGMap()
{	
	//Particles[111]="Pi_0";
	//Particles[-13]="Muon+";	
	//Particles[13]="Muon-";
	Particles[11]="e-";
	//Particles[14]="Neutrino";
	//Particles[22]="Gamma";
	Particles[211]="Pi+";
	Particles[2212]="p+";
	Particles[321]="K+";
	Particles[-11]="e+";
	Particles[0]="Summary";
}

void PDGMap::FillPDGMapV2()
{	//my histograms get these names, root uses them internally and strange things will haben if there is
	//a '+'
	//Particles[111]="Pi_0";
	//Particles[-13]="Muon+";	
	//Particles[13]="Muon-";
	Particles[11]="P11";
	//Particles[14]="Neutrino";
	//Particles[22]="Gamma";
	Particles[211]="P211";
	Particles[2212]="P2212";
	Particles[321]="P321";
	Particles[-11]="Pm11";
	Particles[0]="Summary";
}

void PDGMap::FillPDGMapV3()
{
	Particles.clear();
	Particles[11]="P11";
	Particles[211]="P211";
	Particles[2212]="P2212";
	Particles[321]="P321";
}

void PDGMap::FillScaledV3()
{
	Particles.clear();
	Particles[11]="P11scaled";
	Particles[211]="P211scaled";
	Particles[2212]="P2212scaled";
	Particles[321]="P321scaled";
}

void PDGMap::FillScaled()
{
	Particles[11]="P11scaled";
	Particles[211]="P211scaled";
	Particles[2212]="P2212scaled";
	Particles[321]="P321scaled";
	Particles[-11]="Pm11scaled";
	Particles[0]="Summaryscaled";
}

void PDGMap::Print() const
{
	std::map<int, std::string>::const_iterator cit;
	std::map<int, std::string>::const_iterator end=Particles.end();
	for(cit=Particles.begin();cit!=end;++cit)
	{
		cout << cit->second << " PDGCode: " << cit->first << endl;
	}
}

std::string PDGMap::GetSummaryName()
{
	return Particles[0];
}

short int PDGMap::GetParticleColor(Int_t pdgCode) const
{
	map<int, short int>::const_iterator found=Colors.find(pdgCode);
	if( found!=Colors.end() )	{
		short int color=found->second;
		return color;
	}
	return 1;	// 1 is black
}

short int PDGMap::GetSeparationPowerColor(int pdgCode1, int pdgCode2) const
{
	map<std::pair <int, int> , short int>::const_iterator found=SeppoColors.find( pair<int, int>(pdgCode1, pdgCode2) );
	if( found != SeppoColors.end() )	{
		short int color=found->second;
		return color;		
	}
	return 1;
}

short int PDGMap::GetParticleColor(const string &ParticleName) const
{
	map<int, string>::const_iterator end=Particles.end();
	map<int, string>::const_iterator cit;
	for(cit=Particles.begin();cit!=end;++cit)
	{
		if(cit->second==ParticleName)	{
			short int color=GetParticleColor(cit->first);
			return color;
		}
	}
	cout << "No Color found for Particle " << ParticleName << endl;
	return 1;
}

short int PDGMap::GetSeparationPowerColor(const string &ParticleName1, const string &ParticleName2) const
{
	map<int, string>::const_iterator end=Particles.end();
	map<int, string>::const_iterator cit;
	
	short int PDGCode1=0;
	short int PDGCode2=0;
	for(cit=Particles.begin();cit!=end;++cit)
	{
		if(cit->second==ParticleName1)	{
			PDGCode1=cit->first;
		}
		if(cit->second==ParticleName2)	{
			PDGCode2=cit->first;
		}
	}
	
	if( PDGCode1 && PDGCode2 )	{
			short int color=GetSeparationPowerColor(PDGCode1, PDGCode2);
			return color;
	}
	return 1;
}

std::string PDGMap::GetParticleName(int PDGCode) const
{
	if(Particles.find(PDGCode)!=Particles.end())	{
		return (Particles.find(PDGCode))->second;
	}
	else	{
		return "UnknownParticle";
	}
}

