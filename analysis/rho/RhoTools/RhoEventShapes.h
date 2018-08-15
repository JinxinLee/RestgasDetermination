#ifndef RHOEVENTSHAPES_H
#define RHOEVENTSHAPES_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoEventShapes                                                       //
//                                                                      //
// Functions to calculate the event & event shape variables             //
//                                                                      //
// Author List:                                                         //
// Klaus Goetzen, GSI, May 2013                                         //
// Ralf Kliemt, HIM/GSI May.2013                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMatrixD.h"
#include "TMatrixDEigen.h"
#include "RhoCandList.h"

#define FWMAX 6 // maximum Fox Wolfram moment

class RhoEventShapes {
public:
	RhoEventShapes(RhoCandList &l, TLorentzVector cms);	
	virtual ~RhoEventShapes(){};

	// ******* multiplicities
	int NParticles() const {return fN;}			// number of particle candidates
	int NCharged() const {return fnChrg;}		// number of charged candidates
	int NNeutral() const  {return fnNeut;}		// number of neutral candidates
	
	// ******* maxima of momenta
	double PmaxLab() const  {return fpmaxlab;}		// max momentum in lab system
	double PmaxCms() const {return fpmaxcms;}		// max momentum im cms system
	double Ptmax() const {return fptmax;}			// max pt (same for lab and cms)
	
	// ******* sum of energies/momenta (lab system)
	double PtSumLab() const {return fptsumlab;}				// sum of pt in (lab)
	double NeutEtSumLab() const {return fneutetsumlab;}		// sum of transvers energys of neutrals (lab)
	double NeutESumLab()  const {return fneutesumlab;}		// sum of energys of neutrals (lab)
	double ChrgPtSumLab() const {return fchrgptsumlab;}		// sum of pt of charged (lab)
	double ChrgPSumLab() const {return fchrgpsumlab;}		// sum of momenta of charged (lab)
	
	// ******* sum of energies/momenta (cms system)
	double PtSumCms() const {return fptsumcms;}				// sum of pt in (lab)
	double NeutEtSumCms() const {return fneutetsumcms;}		// sum of transvers energys of neutrals (lab)
	double NeutESumCms()  const {return fneutesumcms;}		// sum of energys of neutrals (lab)
	double ChrgPtSumCms() const {return fchrgptsumcms;}		// sum of pt of charged (lab)
	double ChrgPSumCms() const {return fchrgpsumcms;}		// sum of momenta of charged (lab)
	
	// ******* multiplicities with threshold
	int MultPminLab(double pmin);	// number of particles with p>pmin (lab frame)
	int MultPmaxLab(double pmax);	// number of particles with p<pmax (lab frame)
	int MultPminCms(double pmin);	// number of particles with p>pmin (cms frame)
	int MultPmaxCms(double pmax);	// number of particles with p<pmax (cms frame)

	// ******* shape variables
	double Sphericity();			// Sphericity
	double Aplanarity();			// Aplanarity
	double Planarity();				// Planarity
	
	double FoxWolfMomH(int order);	// Fox Wolfram moment absolute H_i
	double FoxWolfMomR(int order);  // Fox Wolfram moment relative R_i = H_i/H_0
	
	double Thrust();
	TVector3 ThrustVector();
	
private:
	
	void ComputeSphericity();				// compute sph, apl, pla
	double Eps(const TVector3 v1, const TVector3 v2) {return (v1*v2)>0. ? 1. : -1.;}  // aux for Thrust
	double Legendre( int l, double x );		// Legendre function; auxilliary for Fox Wolfram moments
	
	std::vector<TLorentzVector> fLabList;	//!<! List of 4-vectors in lab frame
	std::vector<TLorentzVector> fCmsList;	//!<! List of 4-vectors in cms frame
	std::vector<int>			fCharge;	//!<! List of charges of particles
	
	int    fnChrg;				// number of charged particles
	int    fnNeut;				// number of neutral particles
	int    fN;					// number of particles
	
	double fpmaxlab;			// maximum momentum lab frame
	double fpmaxcms;			// maximum momentum cms frame
	double fptmax;				// maximum transvers momentum

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
  
	double fFWmom[FWMAX];		// Fox Wolfram moments up to FWMAX
	bool   fFWready;			// did we compute FW moments?
  
	double fthr;				// thrust
	TVector3 fThrVect;			// direction of thrust
	TVector3 fBoost;			// boost vector to go to requested frame

	bool fNeutralOnly;			// only compute for neutrals
	bool fChargedOnly;			// only compute for charged

  public:
    ClassDef(RhoEventShapes,1);

};
#endif
