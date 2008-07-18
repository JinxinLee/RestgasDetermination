#ifndef DISTRIBANA_HH
#define DISTRIBANA_HH

#include <vector>
#include <string>
#include <map>

class TH1F;
class TF1;
class TGraphErrors;
class PndTpcDEDXFits;

//TODO: find out which of this functions are called from outside
		// -> make the others private
//TODO: this class does to many things
		// -> group them together to get an overview

class DEDXDistribana	{
public:
DEDXDistribana();
~DEDXDistribana();

//--- gaus fits
// for every momentum in vP_set
// for every Particle given by p_PDGMap;
// functions are listed in the sequence in which they should by called

	// set the minimum number of entrys which a histogram has to have to be fitted with a gaus function
	void SetMinEntries(Int_t minEntries) { m_MinEntries=minEntries; }

	// create for every Particle and Momentum a Projection
	// so there is something to fit
	void FillDEdxDistributions( std::map<string, Double_t> &Statistic);
	void FillDEdxDistributions( );
	
	void CreateDeDxDistributionGausFits();

	//see what has been done
	void DrawDeDxDistribution();
	void PrintChi2();
	
//--- bethe bloch fits
// an AddOn to the name of the BetheBloch Fits, so that i can see more than one set of fits
	void SetFunctionNameAddOn(const std::string &AddOn);
	void CreateGraphs();
	void CreateBBFits();
	void traceDrawing() const;
	void DrawDEDXSummary() const;
	void PrintBBFitChi2();

//***********************


private:

//--- gaus fits
	//Something the user can set
	Int_t m_MinEntries;
	
	//gets a Histogram with one single gaus distribution
	void DeDxFromExistingHistogram(TH1F *pHisto,std::string strName,Double_t P);
		// |-> gives a name to the histogram according to particle and momentum
		void PrepareHistogram(TH1F *pHisto,std::string strName, Double_t P_set);
		// the proberties of the single gaus histograms
		Int_t m_SumBins;
		double m_SumMin;
		double m_SumMax;
			// |-> for every momentum a summary which shows all particles together
			//      	this function creates a histogram with the proberties of the 
			//		single gaus histograms, if strName=="Summary"
			void CreateDeDxDistributionSummary(std::string strName, Double_t P);
	// gives access to a single gaus histogram
	TH1F* GetHistogram(std::string strName, Double_t P_set);
	
	// Creates a gaus fits for a Momentum P_set
	void CreateDeDxDistributionGausFits(Double_t P_set, std::map<std::string,TF1*> &ParticleFits);
	// |-> for every momentum there is a fit for the particles	
	std::map<Double_t, std::map<std::string, TF1*> > FitMap; 
		//|-> is used by ResoHandler and SeppoHandler to fill themeselves
		void GetFitMap(std::map<Double_t, std::map<std::string, TF1*> > &Map) const;
	void DrawDeDxDistribution(Double_t P_set, std::map<std::string, TF1*> &ParticleFits);

	
//knows about the Histograms==Particles that are to be treated with
PDGMap *p_PDGMap;
std::vector<Double_t> vP_set; //momenta for which gaus fits are made


//fits and graphs , i don' t know
PndTpcDEDXFits *p_Fits;
PndTpcDEDXFits *GetFits() const { return p_Fits; }
std::map<std::string,TGraphErrors *> vGraph;

//void CreateSummaries();
void FillSummary(TH1F *pFill, Double_t P_set);

// a couple of helper functions
bool IsInVector(Double_t momentum, const std::vector<Double_t> &vP_set) const;
Double_t BinToMomentum(TH2F *h, Int_t bin) const;
void SetMomenta();


//void CreateDeDxDistribution(const std::vector<Double_t> &vP_set);
//void CreateDeDxDistribution(Double_t P_set);
//void Fill(string strName, Double_t input, Double_t P, const vector<Double_t> vP_set, Double_t tolerance=0.0);
//bool Fill(string strName, Double_t input, Double_t P,const Double_t P_set, Double_t tolerance=0.0);
};

#endif
