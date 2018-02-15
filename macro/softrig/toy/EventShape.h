#include "TLorentzVector.h"
#include "TVector3.h"
#include "SimpleCand.C"
#include "TMatrixD.h"
#include "TMatrixDEigen.h"

#define FWMAX 6 // maximum Fox Wolfram moment

class EventShape {
public:
	EventShape(const CandList &l, TLorentzVector cms);	
	~EventShape(){};

	// ******* multiplicities
	int NParticles() const {return fN;}			// number of particle candidates
	int NCharged() const {return fnChrg;}		// number of charged candidates
	int NNeutral() const  {return fnNeut;}		// number of neutral candidates
	
	// ******* maxima of momenta
	double PmaxLab() const  {return fpmaxlab;}		// max momentum in lab system
	double PmaxCms() const {return fpmaxcms;}		// max momentum im cms system
	double PminLab() const  {return fpminlab;}		// min momentum in lab system
	double PminCms() const {return fpmincms;}		// min momentum im cms system
	double Ptmax() const {return fptmax;}			// max pt (same for lab and cms)
	double Ptmin() const {return fptmin;}			// min pt (same for lab and cms)
	double PRapmax() const {return fprapmax;}		// max pseudorapidity (lab)
	
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
	
	double Thrust();
	TVector3 ThrustVector();
	
private:
	
	void ComputeSphericity();				// compute sph, apl, pla
	double Eps(const TVector3 v1, const TVector3 v2) {return (v1*v2)>0. ? 1. : -1.;}  // aux for Thrust
	double Legendre( int l, double x );		// Legendre function; auxilliary for Fox Wolfram moments
	
	std::vector<TLorentzVector> fLabList;	// List of 4-vectors in lab frame
	std::vector<TLorentzVector> fCmsList;	// List of 4-vectors in cms frame
	std::vector<int>			fCharge;	// List of charges of particles
	
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

// ************************************ IMPLEMENTATION

EventShape::EventShape(const CandList &l, TLorentzVector cms) : 
  fnChrg(0), fnNeut(0), fN(0), fpmaxlab(0.), fpmaxcms(0.),fpminlab(0.), fpmincms(0.), fptmax(0.), fptmin(0.),
  fptsumlab(0.),fneutetsumlab(0.),fneutesumlab(0.),fchrgptsumlab(0.),fchrgpsumlab(0.),
  fptsumcms(0.),fneutetsumcms(0.),fneutesumcms(0.),fchrgptsumcms(0.),fchrgpsumcms(0.),
  fsph(-1.), fapl(-1.), fpla(-1.), fcir(-1.), fFWready(false), fthr(-1.)
{
  int i;

  // initialize more complex things
  fThrVect.SetXYZ(0.,0.,0.);
  fBoost=cms.BoostVector();  
  for (i=0;i<=FWMAX;++i) fFWmom[i]=0.;
  
  
  double pmax=0., ptmax=0., pmaxcms=0.;
  double pmin=1000., ptmin=1000., pmincms=1000.;
  double prapmax=0.;
  
  fLabList.clear();
  fCmsList.clear();
  
  for (i=0;i<l.size();++i)
  {
	TLorentzVector lv(l[i].P4());
	int chrg(l[i].Charge());

	fN++;
	
	// cache multiplicities
	if (chrg==0) fnNeut++;
	else fnChrg++;
	
	// cache unboosted 4-vectors
	fLabList.push_back(lv);
	// cache charges
	fCharge.push_back(chrg);
	
	// sum momentum variables (lab)
	fptsumlab += lv.Pt();
	if (chrg==0)
	{
	  fneutetsumlab += lv.Pt();  
	  fneutesumlab	+= lv.E();
	}
	else
	{
	  fchrgptsumlab	+= lv.Pt();
	  fchrgpsumlab	+= lv.P();
	}
	
	// cache maximum momenta in lab
	if (lv.P()>pmax) pmax=lv.P();
	if (lv.Pt()>ptmax) ptmax=lv.Pt();
	
	// cache minimum momenta in lab
	if (lv.P()<pmin) pmin=lv.P();
	if (lv.Pt()<ptmin) ptmin=lv.Pt();
	
	// cache pseudorapidity
	if (lv.PseudoRapidity()>prapmax) prapmax = lv.PseudoRapidity();
	
	// cache boosted vectors
	lv.Boost(-fBoost);
	fCmsList.push_back(lv);

	// sum momentum variables (cms)
	fptsumcms += lv.Pt();
	if (chrg==0)
	{
	  fneutetsumcms += lv.Pt();  
	  fneutesumcms	+= lv.E();
	}
	else
	{
	  fchrgptsumcms	+= lv.Pt();
	  fchrgpsumcms	+= lv.P();
	}
	
	// cache maximum momenta in cms
	if (lv.P()>pmaxcms) pmaxcms=lv.P();
	// cache minimum momenta in cms
	if (lv.P()<pmincms) pmincms=lv.P();
  }
  
  fpmaxlab = pmax;
  fptmax   = ptmax;
  fpmaxcms = pmaxcms;
  fpminlab = pmin;
  fptmin   = ptmin;
  fpmincms = pmincms;
  fprapmax = prapmax;
}

// ----------------------------

void EventShape::ComputeSphericity()
{
  if( fN==0 ) return;
  
  double stot=0, sxx=0, sxy=0, sxz=0, syy=0, syz=0, szz=0;	
  int i;
  
  for (i=0;i<fN;++i)
  {
	TVector3 v(fCmsList[i].Vect());
	
	sxx += v.X()*v.X(); sxy += v.X()*v.Y(); sxz += v.X()*v.Z();
	syy += v.Y()*v.Y(); syz += v.Y()*v.Z();
	szz += v.Z()*v.Z();
	
	stot += v.Mag2();
  }
  
  TMatrixD sm(3,3);
  sm(0,0) = sxx/stot; sm(0,1) = sxy/stot; sm(0,2) = sxz/stot; 
  sm(1,0) = sxy/stot; sm(1,1) = syy/stot; sm(1,2) = syz/stot; 
  sm(2,0) = sxz/stot; sm(2,1) = syz/stot; sm(2,2) = szz/stot; 
  
  TMatrixDEigen ei(sm);
  TMatrixD eiv=ei.GetEigenValues();
  
  fsph = 1.5 * (eiv(1,1) + eiv(2,2));		// 3/2 (lam_2+lam_3)
  fapl = 1.5 * eiv(2,2); 					// 3/2 lam_3
  fpla = eiv(1,1) - eiv(2,2);  				// lam_2-lam_3
  fcir = 2.*eiv(1,1)/(eiv(1,1)+eiv(0,0));	// 2* min(lam_1, lam_2)/(lam_1+lam_2)
}

// ---------------------------------

double EventShape::Sphericity()
{
  if (fsph<0.) ComputeSphericity();
  
  return fsph;
}

// ---------------------------------

double EventShape::Planarity()
{
  if (fpla<0.) ComputeSphericity();
  
  return fpla;
}

// ---------------------------------

double EventShape::Aplanarity()
{
  if (fapl<0.) ComputeSphericity();
  
  return fapl;
}

// ---------------------------------

double EventShape::Circularity()
{
  if (fcir<0.) ComputeSphericity();
  
  return fcir;
}

// ---------------------------------------
// use iterative formula for thrust vector
//
//             sum eps( n(j)*p_i) * p_i
//   n(j+1) = --------------------------
//            |        ''             | 
//
// with eps(x)=-1 for x<0 and =+1 for x>0

double EventShape::Thrust()
{
  // did we already compute?
  if (fthr>-1.) return fthr; 
  
  TVector3 n0(0,0,0);
  if( fN==0 ) return -1.;
  
  int i,j;
  double pmax=0;
  
  //find starting vector as maximum momentum vector
  for (i=0;i<fN;++i)
  {
	if (fCmsList[i].Vect().Mag()>pmax)
	{
	  n0=fCmsList[i].Vect();
	  pmax=fCmsList[i].Vect().Mag();
	}
  }
  
  TVector3 nnew(0,0,0);

  // find thrust axis (10 iterations)
  for (i=0;i<10;++i)
  {
	for (j=0;j<fN;++j) 
	  nnew += Eps(n0,fCmsList[j].Vect()) * fCmsList[j].Vect();
	
	n0=nnew.Unit();
  }
  
  fThrVect = n0;
  
  double thr=0, sum=0;
  for (i=0;i<fN;++i)
  {
	thr += fabs(fThrVect.Dot(fCmsList[i].Vect()));
	sum += fCmsList[i].Vect().Mag();
  }
  
  fthr = thr/sum;
  
  return fthr;
}

// ---------------------------------------

TVector3 EventShape::ThrustVector()
{
  if (fthr<0.) Thrust();
  
  return fThrVect;
}

// ---------------------------------------

double EventShape::Legendre( int l, double x )
{
    if (fabs(x)>1.) return -999.;

	if (l==0) return 1.;
	
    double pmm = 1.;
	double pmmp1 = x;

	if (l>1)
	{
		for(int ll=2; ll<=l; ll++)
		{
		  double pll = (x * (2 * ll - 1) * pmmp1 - (ll - 1) * pmm) / (ll);
		  pmm = pmmp1;
		  pmmp1 = pll;
		}
	}
   return pmmp1;
}

// ---------------------------------------

double EventShape::FoxWolfMomH(int order)
{
  if (order>FWMAX) return -1.;
 
  // already computed FW moments
  if (fFWready) return fFWmom[order];
  
  if( fN==0 ) return -1.;

  double s = 0.;
  int i,j,l;

  for (i=0; i<fN-1; ++i)
  {
     // this candidate's 3-momentum
	  TVector3 p1(fCmsList[i].Vect());
      double pmag1 = p1.Mag();

      // loop over other candidates, starting at the next one in the list
 	  
	  for (j=i+1; j<fN; ++j)
	  {
		// this candidate's 3-momentum
		TVector3 p2(fCmsList[j].Vect());
		double pmag2 = p2.Mag();

		// the cosine of the angle between the two candidates
		double cosPhi =  cos ( p1.Angle(p2) );

		// the contribution of this pair of track 
		// (note the factor 2 : the pair enters the sum twice)
		for( l=0; l<=FWMAX; l++ )
		  fFWmom[l] += 2 * pmag1 * pmag2 * Legendre( l, cosPhi );
		  
	  }
      // contribution of this track
      for( l=0; l<=FWMAX; l++ )
		fFWmom[l] += pmag1 * pmag1 * Legendre( l, 1. );
      
      // total energy
      s += fCmsList[i].Energy();
   }
  
  // well ...
  if( s<=0. ) return -1.;
  double s2=s*s;
  
  // normalize Fox Wolfram Moments
  for( i=0; i<=FWMAX; i++) fFWmom[i]/=s2 ;  
  
  fFWready = true;
  
  return fFWmom[order];
}


// ---------------------------------------

double EventShape::FoxWolfMomR(int order)
{
  return FoxWolfMomH(order)/FoxWolfMomH(0);
}

// ---------------------------------------
// ---------------------------------------
// ---------------------------------------

int EventShape::MultPminLab(double pmin)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fLabList[i].P()>pmin) cnt++;
 
  return cnt;
}

// ---------------------------------------

int EventShape::MultPmaxLab(double pmax)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fLabList[i].P()<pmax) cnt++;
 
  return cnt;
}

// ---------------------------------------

int EventShape::MultPminCms(double pmin)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fCmsList[i].P()>pmin) cnt++;
 
  return cnt;
}

// ---------------------------------------

int EventShape::MultPmaxCms(double pmax)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fCmsList[i].P()<pmax) cnt++;
 
  return cnt;
}
// ---------------------------------------
// ---------------------------------------
// ---------------------------------------

int EventShape::MultPtminLab(double ptmin)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fLabList[i].Pt()>ptmin) cnt++;
 
  return cnt;
}

// ---------------------------------------

int EventShape::MultPtmaxLab(double ptmax)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fLabList[i].Pt()<ptmax) cnt++;
 
  return cnt;
}

// ---------------------------------------

int EventShape::MultPtminCms(double ptmin)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fCmsList[i].Pt()>ptmin) cnt++;
 
  return cnt;
}

// ---------------------------------------

int EventShape::MultPtmaxCms(double ptmax)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fCmsList[i].Pt()<ptmax) cnt++;
 
  return cnt;
}

// ---------------------------------------
// ---------------------------------------
// ---------------------------------------

int EventShape::MultNeutEminLab(double emin)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]==0 && fLabList[i].E()>emin) cnt++;
 
  return cnt;
}

// ---------------------------------------

int EventShape::MultNeutEmaxLab(double emax)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]==0 && fLabList[i].E()<emax) cnt++;
 
  return cnt;
}

// ---------------------------------------

int EventShape::MultNeutEminCms(double emin)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]==0 && fCmsList[i].E()>emin) cnt++;
 
  return cnt;
}

// ---------------------------------------

int EventShape::MultNeutEmaxCms(double emax)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]==0 && fCmsList[i].E()<emax) cnt++;
 
  return cnt;
}

// ---------------------------------------
// ---------------------------------------
// ---------------------------------------

int EventShape::MultChrgPminLab(double pmin)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]!=0 && fLabList[i].P()>pmin) cnt++;
 
  return cnt;
}

// ---------------------------------------

int EventShape::MultChrgPmaxLab(double pmax)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]!=0 && fLabList[i].P()<pmax) cnt++;
 
  return cnt;
}

// ---------------------------------------

int EventShape::MultChrgPminCms(double pmin)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]!=0 && fCmsList[i].P()>pmin) cnt++;
 
  return cnt;
}

// ---------------------------------------

int EventShape::MultChrgPmaxCms(double pmax)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]!=0 && fCmsList[i].P()<pmax) cnt++;
 
  return cnt;
}

// ---------------------------------------
// ---------------------------------------
// ---------------------------------------

double EventShape::SumPminLab(double pmin)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fLabList[i].P()>pmin) sum+=fLabList[i].P();
 
  return sum;
}

// ---------------------------------------

double EventShape::SumPmaxLab(double pmax)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fLabList[i].P()<pmax) sum+=fLabList[i].P();
 
  return sum;
}

// ---------------------------------------

double EventShape::SumPminCms(double pmin)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fCmsList[i].P()>pmin) sum+=fLabList[i].P();
 
  return sum;
}

// ---------------------------------------

double EventShape::SumPmaxCms(double pmax)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fCmsList[i].P()<pmax) sum+=fLabList[i].P();
 
  return sum;
}

// ---------------------------------------
// ---------------------------------------
// ---------------------------------------

double EventShape::SumPtminLab(double ptmin)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fLabList[i].Pt()>ptmin) sum+=fLabList[i].Pt();
 
  return sum;
}

// ---------------------------------------

double EventShape::SumPtmaxLab(double ptmax)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fLabList[i].Pt()<ptmax) sum+=fLabList[i].Pt();
 
  return sum;
}

// ---------------------------------------

double EventShape::SumPtminCms(double ptmin)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fCmsList[i].Pt()>ptmin) sum+=fLabList[i].Pt();
 
  return sum;
}

// ---------------------------------------

double EventShape::SumPtmaxCms(double ptmax)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fCmsList[i].Pt()<ptmax) sum+=fLabList[i].Pt();
 
  return sum;
}

// ---------------------------------------
// ---------------------------------------
// ---------------------------------------

double EventShape::SumNeutEminLab(double emin)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]==0 && fLabList[i].E()>emin) sum+=fLabList[i].E();
 
  return sum;
}

// ---------------------------------------

double EventShape::SumNeutEmaxLab(double emax)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]==0 && fLabList[i].E()<emax) sum+=fLabList[i].E();
 
  return sum;
}

// ---------------------------------------

double EventShape::SumNeutEminCms(double emin)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]==0 && fCmsList[i].E()>emin) sum+=fLabList[i].E();
 
  return sum;
}

// ---------------------------------------

double EventShape::SumNeutEmaxCms(double emax)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]==0 && fCmsList[i].E()<emax) sum+=fLabList[i].E();
 
  return sum;
}

// ---------------------------------------
// ---------------------------------------
// ---------------------------------------

double EventShape::SumChrgPminLab(double pmin)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]!=0 && fLabList[i].P()>pmin) sum+=fLabList[i].P();
 
  return sum;
}

// ---------------------------------------

double EventShape::SumChrgPmaxLab(double pmax)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]!=0 && fLabList[i].P()<pmax) sum+=fLabList[i].P();
 
  return sum;
}

// ---------------------------------------

double EventShape::SumChrgPminCms(double pmin)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]!=0 && fCmsList[i].P()>pmin) sum+=fLabList[i].P();
 
  return sum;
}

// ---------------------------------------

double EventShape::SumChrgPmaxCms(double pmax)
{
  double sum=0;
  for (int i=0;i<fN;++i) 
	if (fCharge[i]!=0 && fCmsList[i].P()<pmax) sum+=fLabList[i].P();
 
  return sum;
}
