#ifndef PDGMAP_HH
#define PDGMAP_HH
#include <map>
#include <string>


class PDGMap	{
public:
	PDGMap();
	std::map<int, std::string> Particles;
	void FillPDGMap();
	void FillPDGMapV2();
	void FillPDGMapV3();
	void FillScaledV3();
	void FillScaled();
	void Print() const;
	
	short int GetSeparationPowerColor(int pdgCode1, int pdgCode2) const;
	short int GetSeparationPowerColor(const std::string &ParticleName1, const std::string &ParticleName2) const;
	short int GetParticleColor(int pdgCode) const;
	short int GetParticleColor(const std::string &ParticleName) const;
	std::string GetParticleName(int PDGCode) const;
	std::string GetSummaryName();
private:
	void FillParticleColors();
	void FillSeparationPowerColors();
	
	std::map<int, short int> Colors;
	std::map<std::pair <int, int> , short int> SeppoColors;
};

#endif

