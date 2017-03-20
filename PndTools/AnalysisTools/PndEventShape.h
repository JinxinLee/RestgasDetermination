#ifndef PNDEVENTSHAPE_H
#define PNDEVENTSHAPE_H 1

#include "TLorentzVector.h"
#include "TVector3.h"

#define FWMAX 6 // maximum Fox Wolfram moment

class RhoCandList;

class PndEventShape {
public:
	PndEventShape(RhoCandList &l, TLorentzVector cms, double neutMinE=0.0, double chrgMinP=0.0);	
	~PndEventShape(){};

	// ******* multiplicities
	int NParticles() const {return fN;}			// number of particle candidates
	int NCharged() const {return fnChrg;}		// number of charged candidates
	int NNeutral() const  {return fnNeut;}		// number of neutral candidates
	
	// ******* maxima of momenta
	double PmaxLab() const  {return fpmaxlab;}		  // max momentum in lab system
	double PmaxCms() const {return fpmaxcms;}		  // max momentum im cms system
	double PminLab() const  {return fpminlab;}		  // min momentum in lab system
	double PminCms() const {return fpmincms;}		  // min momentum im cms system
	double Ptmax() const {return fptmax;}			  // max pt (same for lab and cms)
	double Ptmin() const {return fptmin;}			  // min pt (same for lab and cms)
	double PRapmax() const {return fprapmax;}		  // max pseudorapidity (lab)	
	double EmaxNeutLab() const {return femaxneutlab;} // max neutral energy (lab)
	double EmaxNeutCms() const {return femaxneutcms;} // max neutral energy (cms)
	double PmaxChrgLab() const {return fpmaxchlab;}   // max charged momentum (lab)
	double PmaxChrgCms() const {return fpmaxchcms;}   // max charged momentum (cms)
	
	// ******* sum of energies/momenta (lab system)
	double PtSumLab() const {return fptsumlab;}				// sum of pt in (lab)
	double NeutEtSumLab() const {return fneutetsumlab;}		// sum of transvers energys of neutrals (lab)
	double NeutESumLab()  const {return fneutesumlab;}		// sum of energys of neutrals (lab)
	double ChrgPtSumLab() const {return fchrgptsumlab;}		// sum of pt of charged (lab)
	double ChrgPSumLab() const {return fchrgpsumlab;}		// sum of momenta of charged (lab)
	
	// ******* sum of energies/momenta (cms system)
	double PtSumCms() const {return fptsumcms;}				// sum of pt in (cms)
	double NeutEtSumCms() const {return fneutetsumcms;}		// sum of transvers energys of neutrals (cms)
	double NeutESumCms()  const {return fneutesumcms;}		// sum of energys of neutrals (cms)
	double ChrgPtSumCms() const {return fchrgptsumcms;}		// sum of pt of charged (cms)
	double ChrgPSumCms() const {return fchrgpsumcms;}		// sum of momenta of charged (cms)
	
	// ******* detector sepcific quantities
	double DetEmcSum() const {return fdetemcsum;}			// sum of EMC cluster energies
	double DetEmcMax() const {return fdetemcmax;}			// maximum of EMC cluster energies
	
	// ******* multiplicities with threshold
	int MultPminLab(double pmin);	// number of particles with p>pmin (lab frame)
	int MultPmaxLab(double pmax);	// number of particles with p<pmax (lab frame)
	int MultPminCms(double pmin);	// number of particles with p>pmin (cms frame)
	int MultPmaxCms(double pmax);	// number of particles with p<pmax (cms frame)
	
	// ******* multiplicities with threshold
	int MultPtminLab(double ptmin);	// number of particles with pt>pmin (lab frame)
	int MultPtmaxLab(double ptmax);	// number of particles with pt<pmax (lab frame)
	int MultPtminCms(double ptmin);	// number of particles with pt>pmin (cms frame)
	int MultPtmaxCms(double ptmax);	// number of particles with pt<pmax (cms frame)
	
	// ******* PID multiplicities with PID and momentum threshold
	int MultElectronPminLab(double prob, double pmin=0);	// number of electrons with p>pmin and PID prob>prob (lab frame)
	int MultMuonPminLab(double prob, double pmin=0);		// number of muons with p>pmin and PID prob>prob (lab frame)
	int MultPionPminLab(double prob, double pmin=0);		// number of pions with p>pmin and PID prob>prob (lab frame)
	int MultKaonPminLab(double prob, double pmin=0);		// number of kaons with p>pmin and PID prob>prob (lab frame)
	int MultProtonPminLab(double prob, double pmin=0);	// number of protons with p>pmin and PID prob>prob (lab frame)
	
	int MultElectronPminCms(double prob, double pmin=0);	// number of electrons with p>pmin and PID prob>prob (cms frame)
	int MultMuonPminCms(double prob, double pmin=0);		// number of muons with p>pmin and PID prob>prob (cms frame)
	int MultPionPminCms(double prob, double pmin=0);		// number of pions with p>pmin and PID prob>prob (cms frame)
	int MultKaonPminCms(double prob, double pmin=0);		// number of kaons with p>pmin and PID prob>prob (cms frame)
	int MultProtonPminCms(double prob, double pmin=0);	// number of protons with p>pmin and PID prob>prob (cms frame)
	
	// ******* neutrals multiplicities with threshold
	int MultNeutEminLab(double emin);	// number of neutrals with E>emin (lab frame)
	int MultNeutEmaxLab(double emax);	// number of neutrals with E<emax (lab frame)
	int MultNeutEminCms(double emin);	// number of neutrals with E>emin (cms frame)
	int MultNeutEmaxCms(double emax);	// number of neutrals with E<emax (cms frame)
	
	// ******* charged multiplicities with threshold
	int MultChrgPminLab(double pmin);	// number of charged with p>pmin (lab frame)
	int MultChrgPmaxLab(double pmax);	// number of charged with p<pmax (lab frame)
	int MultChrgPminCms(double pmin);	// number of charged with p>pmin (cms frame)
	int MultChrgPmaxCms(double pmax);	// number of charged with p<pmax (cms frame)

	// ******* sums with threshold
	double SumPminLab(double pmin);		// sum of momenta with p>pmin (lab frame)
	double SumPmaxLab(double pmax);		// sum of momenta with p<pmax (lab frame)
	double SumPminCms(double pmin);		// sum of momenta with p>pmin (cms frame)
	double SumPmaxCms(double pmax);		// sum of momenta with p<pmax (cms frame)
	
	// ******* sums with threshold
	double SumPtminLab(double ptmin);		// sum of pt with pt>pmin (lab frame)
	double SumPtmaxLab(double ptmax);		// sum of pt with pt<pmax (lab frame)
	double SumPtminCms(double ptmin);		// sum of pt with pt>pmin (cms frame)
	double SumPtmaxCms(double ptmax);		// sum of pt with pt<pmax (cms frame)
	
	// ******* neutrals sums with threshold
	double SumNeutEminLab(double emin);	// sum of energies of neutrals with E>emin (lab frame)
	double SumNeutEmaxLab(double emax);	// sum of energies of neutrals with E<emax (lab frame)
	double SumNeutEminCms(double emin);	// sum of energies of neutrals with E>emin (cms frame)
	double SumNeutEmaxCms(double emax);	// sum of energies of neutrals with E<emax (cms frame)
	
	// ******* charged sums with threshold
	double SumChrgPminLab(double pmin);	// sum of momenta of charged with p>pmin (lab frame)
	double SumChrgPmaxLab(double pmax);	// sum of momenta of charged with p<pmax (lab frame)
	double SumChrgPminCms(double pmin);	// sum of momenta of charged with p>pmin (cms frame)
	double SumChrgPmaxCms(double pmax);	// sum of momenta of charged with p<pmax (cms frame)

	// ******* shape variables
	double Sphericity();			// Sphericity
	double Aplanarity();			// Aplanarity
	double Planarity();				// Planarity
	double Circularity();			// Cirularity
	
	double FoxWolfMomH(int order);	// Fox Wolfram moment absolute H_i
	double FoxWolfMomR(int order);  // Fox Wolfram moment relative R_i = H_i/H_0
	
	double Thrust(int Nmax=4);      // Thrust, with 0.5 < thr < 1.0
	TVector3 ThrustVector();        // Direction of thrust vector
	
private:
	
	void ComputeSphericity();				// compute sph, apl, pla
	double Eps(const TVector3 v1, const TVector3 v2) {return (v1*v2)>0. ? 1. : -1.;}  // aux for Thrust
	double Legendre( int l, double x );		// Legendre function; auxilliary for Fox Wolfram moments
	static bool   CmpTVect3Mag(TVector3 v1, TVector3 v2) { return (v1.Mag()<v2.Mag()); }
	
	std::vector<TLorentzVector> fLabList;	// List of 4-vectors in lab frame
	std::vector<TLorentzVector> fCmsList;	// List of 4-vectors in cms frame
	std::vector<int>			fCharge;	// List of charges of particles
	std::vector<double>			fElProb;	// List of electron probabilities
	std::vector<double>			fMuProb;	// List of muon probabilities
	std::vector<double>			fPiProb;	// List of pion probabilities
	std::vector<double>			fKaProb;	// List of kaon probabilities
	std::vector<double>			fPrProb;	// List of proton probabilities
	
	int    fnChrg;				// number of charged particles
	int    fnNeut;				// number of neutral particles
	int    fN;					// number of particles
	
	double fpmaxlab;			// maximum momentum lab frame
	double fpmaxcms;			// maximum momentum cms frame
	double fpminlab;			// minimum momentum lab frame
	double fpmincms;			// minimum momentum cms frame
	double fptmax;				// maximum transvers momentum
	double fptmin;				// minimum transvers momentum
	double fprapmax;			// maximum pseudorapidity
	double femaxneutlab;		// max neutral energy (lab)
	double femaxneutcms;		// max neutral energy (cms)
	double fpmaxchlab;			// max charged momentum (lab)
	double fpmaxchcms;			// max charged momentum (cms)

	double fdetemcsum;			// sum of EMC cluster energies
	double fdetemcmax;			// maximum of EMC cluster energies

	double fptsumlab;			// sum of pt in (lab)
	double fneutetsumlab;		// sum of transvers energys of neutrals (lab)
	double fneutesumlab;		// sum of energys of neutrals (lab)
	double fchrgptsumlab;		// sum of pt of charged (lab)
	double fchrgpsumlab;		// sum of momenta of charged (lab)
	
	double fptsumcms;			// sum of pt in (lab)
	double fneutetsumcms;		// sum of transvers energys of neutrals (lab)
	double fneutesumcms;		// sum of energys of neutrals (lab)
	double fchrgptsumcms;		// sum of pt of charged (lab)
	double fchrgpsumcms;		// sum of momenta of charged (lab)
	
	double fsph;				// Sphericity
	double fapl;				// Aplanarity
	double fpla;				// Planarity
	double fcir;				// Circularity
  
	double fFWmom[FWMAX];		// Fox Wolfram moments up to FWMAX
	bool   fFWready;			// did we compute FW moments?
  
	double fthr;				// thrust
	TVector3 fThrVect;			// direction of thrust
	TVector3 fBoost;			// boost vector to go to requested frame
	

};

#endif
