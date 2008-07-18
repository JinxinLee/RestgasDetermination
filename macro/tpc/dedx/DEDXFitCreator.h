#ifndef DEDXFITCREATOR_HH
#define DEDXFITCREATOR_HH

#include <vector>
#include <map>
#include <string>

class TF1;
class PDGMap;
class PndTpcDEDXFits;

class DEDXFitCreator	{

public:
	DEDXFitCreator();
	~DEDXFitCreator();
	
	//Makes the fits and stores the fitfunctions
	void CreateFits();
	
	//get mimimum for scaling histograms
	double GetMinimum() const;
	
	//Draws the desired Scatterplot with all fitfunction stored in vFits
	void DrawSummary();
	
	//Draws histograms and profiles for single particles in order to check the fits 
	void traceDrawing();
	void PrintChi2();
	PndTpcDEDXFits *GetFits() const { return p_Fits; } 

private:
	std::vector<TF1*> vFits;
	std::map<std::string, TF1*> FitMap;
	PDGMap *p_PDGMap;
	PndTpcDEDXFits *p_Fits;
};
#endif
