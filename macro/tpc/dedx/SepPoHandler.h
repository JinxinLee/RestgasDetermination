#ifndef SEPOHANDLER_HH
#define SEPOHANDLER_HH

#include <map>
#include <string>
#include <vector>

class TH2F;
class TCanvas;
class TF1;
class SeparationPower;
class PDGMap;

/// \class SepPoHandler
/// rules over the SeparationPower of specified particles
///

class SepPoHandler
{
public:
	SepPoHandler();
	~SepPoHandler();

	TH2F *CreateHistogram();
	void FillSeparationPower(std::map<Double_t, std::map<std::string, TF1*> > &FitMap);
	void Draw(const std::string &strName) const;
	void Draw() const;
	void GetSummary(std::string &summary) const;
	void GetSummary(std::vector<std::string> &summary) const;	
private:
	std::vector<SeparationPower*> vSepPos;
	Int_t nHistograms;
	PDGMap *p_PDGMap;
};

#endif
