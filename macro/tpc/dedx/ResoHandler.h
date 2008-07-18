#ifndef RESOHANDLER_HH
#define RESOHANDLER_HH

#include <map>
#include <vector>
#include <string>
#include "TObject.h"

class TF1;
class Resolution;
class PDGMap;
class DEDXDistribana;

class ResoHandler
{
public:
	ResoHandler();
	//ResoHandler(DEDXDistribana *distribution);
	~ResoHandler();
	void Fill(std::map<Double_t, std::map<std::string, TF1*> > &FitMap);
	void Fill(std::map<Double_t, std::map<std::string, TF1*> > &FitMap, double Theta);
	void Fill(DEDXDistribana *distribution);
	Resolution *GetResolution(const std::string &ParticleName) const;
	void Draw() const;
	void Save(const std::string &filename) const;
	void SetAdditionalCoordinate(char Specifier, double Coordinate);
private:
	std::vector<Resolution*> vResos;
	PDGMap *p_PDGMap;
	double AddCoord;
	char Spec;
};

#endif




