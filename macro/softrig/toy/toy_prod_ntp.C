//#include "SimpleCand.C"
#include <iostream>
#include <vector>
#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TParticle.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TStyle.h"
#include "TNtuple.h"
#include "TLatex.h"
#include "TVector.h"
#include "TMatrixD.h"
#include "TSystem.h"
#include "TMatrixDEigen.h"
#include "TMath.h"

#include "EventShape.h"


using std::cout;
using std::endl;

TRandom3      fRand;
TLorentzVector fIni;

// *** some program parameters
const int		fMAX	   = 100;  		// *** max tracks per event
int 			nbins 		= 300;      // *** bins of mass histograms
double 			low  		= 0.0;      // *** lower edge of histo
double 			high 		= 5.0;      // *** higher edge of histo (will be modified acc. to resonance mass)

// *** some contants for neutrals
const double    fNeutEff   = 0.9;      // *** efficiency for neutrals
const double    fdE        = 0.03;      // *** neutral energy resolution
const double    fNdtht     = 0.003;     // *** neutrals tht resolution
const double    fNdphi     = 0.003;     // *** neutrals phi resolution
const double    fNeutThrsh = 0.02;		// *** energy threshold of neutrals

// *** constants for particles
int				fPi0Pdg	   = 111;		// *** PDG code for pi0
double		 	fPi0Mass   = 0.13497;    // *** center of mass cut window
double			fPi0win	   = 0.020;

int				fKsPdg	   = 310;		// *** PDG code for pi0
double		 	fKsMass   = 0.497614;  // *** center of mass cut window
double			fKswin	   = 0.025;

int             fDPdg      = 411;         // *** PDG code to match D
double		 	fDMass     = 1.8693;      // *** center of mass cut window
double			fDsig      = 0.88;        // *** slope of linear width function
double			fDwin		=0.25;

int             fD0Pdg     = 421;         // *** PDG code to match D0
double		 	fD0Mass    = 1.8645;      // *** center of mass cut window
double			fD0sig     = 1.04;        // *** slope of linear width function
double			fD0win		=0.25;

int             fJpsiPdg   = 443;         // *** PDG code to match J/psi
double		 	fJMass     = 3.09687;        // *** center of mass cut window
double			fJsig      = 2.24;        // *** slope of linear width function
double			fJsig3pi   = 1.5;        // *** slope of linear width function for 3pi mode
double			fJ3pioff   = 0.027;        // *** offset of linear function for 3pi mode
double			fJwin		=0.7;

int             fLamPdg    = 3122;         // *** PDG code to match Lam
double		 	fLamMass   = 1.115684;        // *** center of mass cut window
double			fLamsig    = 0.08;        // *** slope of linear width function
double			fLamwin		=0.2;

int             fDsPdg     = 431;         // *** PDG code to match Ds
double		 	fDsMass    = 1.9686;        // *** center of mass cut window
double			fDssig     = 0.75;        // *** slope of linear width function
double			fDswin		=0.2;

int             fPhiPdg     = 333;         // *** PDG code to match Phi
double		 	fPhiMass    = 1.0195;        // *** center of mass cut window
double			fPhisig     = 0.15;        // *** slope of linear width function
double			fPhiwin		=0.2;

int             fEtacPdg     = 441;         // *** PDG code to match etac
double		 	fEtacMass    = 2.9798;        // *** center of mass cut window
double			fEtacsig     = 0.15;        // *** slope of linear width function
double			fEtacwin      =0.15;

int             fLamcPdg     = 4122;         // *** PDG code to match Lambda_c
double		 	fLamcMass    = 2.2849;        // *** center of mass cut window
double			fLamcsig     = 0.15;        // *** slope of linear width function
double			fLamcwin	  =0.2;


// *** for event shape variables (global)
double			_sph, _pla, _apl, _thr;
TVector3		_sphAx, _secAx, _thrAx;

// *** max order for Fox-Wolfram Moments
const int    	_nfwmom = 5;
// *** result container for computation (global)
double 			_foxwolfR[_nfwmom];

// *** pid probability table; will be modified according to purity parameter value
// ***  true pid:        e   mu    pi    K   p      classified as [%]
double pidtab [25] = {  80,   5,   5,   5,   5,    //  e
                        5,  80,   5,   5,   5,    // mu
                        5,   5,  80,   5,   5,    // pi
                        5,   5,   5,  80,   5,    // K
                        5,   5,   5,   5,  80
                     };  // p

// *** table determined by Donghee 11/2012 for P>0.1
double pidreal [25] = {  95,  6,  10,   7,  10,    //  e
                         5,  86,  27,   6,   7,    // mu
                         9,  29,  85,  18,  16,    // pi
                         9,  21,  40,  67,  24,    // K
                         12, 12,  20,  16,  90
                      };  // p

// *** global stable particle masses used in pid selection; set in init()
double mE, mMu, mPi, mK, mP, mPi0;

TF1 *f_res=0;

// *** this map contains mapping from PDG code to indices 0...4 (for pidtab)
// *** 11 -> 0, 13 -> 1, 211 -> 2, 321 -> 3, 2212 -> 4
std::map<int,int> pdgidx;

TDatabasePDG *pdg_db;

typedef std::vector<SimpleCand> CandList;

// *** all global particle lists we need to do combinatorics
CandList mclist, mcFSlist, recolist, recocmslist;

// *** the global pid lists
CandList eplus, eminus, muplus, muminus, piplus, piminus, kplus, kminus, pplus, pminus, gam;

// *** boost full list with -boost and store in boostlist
void boostList(CandList &list, CandList &boostlist, TVector3 boost)
{
	int N=list.size();
	boostlist.clear();

    for (unsigned int i=0;i<N;i++)
    {
        SimpleCand c=list[i];
		TLorentzVector lv(c.P4());
        lv.Boost(-boost);
		c.SetP4(lv);
		boostlist.push_back(c);
    }
}

// *** aux for thrust; eps(x)=sign(x)
double eps(TVector3 v1, TVector3 v2)
{
  double prod = v1.Dot(v2);
  return prod/fabs(prod);
}

// *** routine to compute thrust vector and thrust
double compThrust(CandList& list, TVector3 &axis)
{
  TVector3 n0;
  int N=list.size();
  
  int i,j;
  double pmax=0;
  
  //find starting vector as maximum momentum vector
  for (i=0;i<N;++i)
  {
	if (list[i].P4().Vect().Mag()>pmax)
	{
	  n0=list[i].P4().Vect();
	  pmax=list[i].P4().Vect().Mag();
	}
  }
  //n0=list[0].P4().Vect();
  
  TVector3 nnew(0,0,0);
  // find thrust axis (10 iterations)
  for (i=0;i<10;++i)
  {
	for (j=0;j<N;++j)
	{
	  //nnew+=eps(n0,list[j].P4().Vect())*list[j].P4().Vect();
	  nnew+=n0*list[j].P4().Vect()>0 ? list[j].P4().Vect() : -list[j].P4().Vect();
	}
	n0=nnew.Unit();
  }
  
  axis = n0;
  double thr=0, sum=0;
  for (i=0;i<N;++i)
  {
	thr+=fabs(axis.Dot(list[i].P4().Vect()));
	sum+=list[i].P4().Vect().Mag();
  }
  return thr/sum;
}

// *** Legendre Function; auxiliary function for Fox-Wolfram-Moments
double Legendre( int l, double x )
{
    assert(fabs(x) <= 1.);

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

// *** compute relative Fox-Wolfram-Moments R up to 4th order (see _nfwmom contant above)
void compFoxWolfMoms(CandList& list)
{
  int n=list.size();
  if( n==0 ) return;

  double s = 0.;
  double _sumarray[_nfwmom]={0,0,0,0};
  int i,j,l;

  for (i=0; i<n-1; ++i)
  {
     // this candidate's 3-momentum
	  TVector3 p1(list[i].P4().Vect());
      double pmag1 = p1.Mag();

      // loop over other candidates, starting at the next one in the list
 	  
	  for (j=i+1; j<n; ++j)
	  {
		// this candidate's 3-momentum
		TVector3 p2(list[j].P4().Vect());
		double pmag2 = p2.Mag();

		// the cosine of the angle between the two candidates
		double cosPhi =  cos ( p1.Angle(p2) );

		// the contribution of this pair of track 
		// (note the factor 2 : the pair enters the sum twice)
		for( l=0; l<_nfwmom; l++ )
		  _sumarray[l] += 2 * pmag1 * pmag2 * Legendre( l, cosPhi );
		  
	  }
      // contribution of this track
      for( l=0; l<_nfwmom; l++ )
		_sumarray[l] += pmag1 * pmag1 * Legendre( l, 1. );
      
      // total energy
      s += list[i].P4().Energy();
   }
  
  // well ...
  if( s<=0. ) return;
  double ssq=s*s;
  // normalize Fox Wolfram Moments
  for( i=0; i<_nfwmom; i++)
    _foxwolfR[i] = _sumarray[i]/ssq ;
}

// *** routine to compute sphericity, planarity and aplanarity
double compSphericity(CandList &list)
{
  int N=list.size();
  
  double stot=0, sxx=0, sxy=0, sxz=0, syy=0, syz=0, szz=0;	
  int i;
  
  for (i=0;i<N;++i)
  {
	TVector3 v(list[i].P4().Vect());
	
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
  
  _sph = 1.5 * (eiv(1,1) + eiv(2,2));
  _apl = 1.5 * eiv(2,2);
  _pla = eiv(1,1) - eiv(2,2);
  
  return _sph;
}

// *** check PDG code is final state particle
bool isFinalState(int n)
{
    n=abs(n);
    return (n==11 || n==13  || n==211 || n==321 || n==2212 || n==22);
}

// *** online momentum resoltion based on talk of Yutie Lang, PANDA CM 12/2011, TRK oder DAQ session
// *** values (pt[GeV/c], dp/p[%]): (1.0, 3.8) (2.0, 5.5) (5.0, 12.2)
// *** see init routine for function parameters
double dponline(TLorentzVector &v)
{
    return f_res->Eval(v.Pt())/100.;
}

// *** computes the pid table for given efficiency and purity
void makePidTable(double pideff, double misid)
{
    cout<< "Using PID table:"<<endl;

    if (pideff<0 && misid<0)
    {
        for (int i=0;i<5;++i)
        {
            for (int j=0;j<5;++j)
            {
                //pidtab[j+5*i] = pidreal[j+5*i];
                printf("%4.0f",pidtab[j+5*i]);
                //cout << pidtab[i+5*j]<<"  ";
            }
            cout<< endl;
        }
        return;
    }

    if (pideff<0)   pideff=0;
    if (pideff>100) pideff=100;
    if (misid>100) misid=100;

    // *** neff is the efficiency for particles of wrong type
    //double neff = pideff*(100-pidpur)/(4*pidpur);
    //if (neff>100) neff=100;

    for (int i=0;i<5;++i)
    {
        for (int j=0;j<5;++j)
        {
            if (i==j) pidtab[i+5*j] = pideff;
            else pidtab[i+5*j] = misid;
            cout << pidtab[i+5*j]<<"  ";
        }
        cout<< endl;
    }
}

// *** initialization of some globals
void init(double pideff, double misid)
{
    // *** setting the mapping of PDG code to tab index
    pdgidx.clear();
    pdgidx[11]=0;
    pdgidx[13]=1;
    pdgidx[211]=2;
    pdgidx[321]=3;
    pdgidx[2212]=4;

    pdg_db=TDatabasePDG::Instance();

    // *** create pid table
    makePidTable(pideff, misid);

    // *** gets particle masses from pdg database and set them globally
    mE  = pdg_db->GetParticle(11)->Mass();
    mMu = pdg_db->GetParticle(13)->Mass();
    mPi = pdg_db->GetParticle(211)->Mass();
    mK  = pdg_db->GetParticle(321)->Mass();
    mP  = pdg_db->GetParticle(2212)->Mass();

    mPi0= pdg_db->GetParticle(111)->Mass();

    // *** resolution fcn based on Yutie Lang results; see routine dponline above
    f_res=new TF1("f_res","pol2(0)");
    f_res->SetParameters(2.367,1.3,0.133);
    f_res->SetRange(0,10);
}

// *** prints single candidate of type SimpleCand
void printCand(SimpleCand c)
{
    cout <<c.Id()<<" pid:"<<c.Pid()<<" ("<<c.P4().X()<<","<<c.P4().Y()<<","<<c.P4().Z()<<";"<<c.P4().T()<<") ";
    cout <<c.Charge()<<" "<<c.P4().M();
    cout <<" moth:"<<c.MotherIdx()<<" nsib:"<<c.NSiblings()<<" nfs:"<<c.NFS();
    cout <<" mcpid:"<<c.McPid()<<" mct:"<<c.Mct()<<endl;
}

// *** prints list of candidates
void printList(CandList &l, TString tit="")
{
    if (tit!="") cout << tit<<endl;
    for (unsigned int j=0;j<l.size();++j) printCand(l[j]);
    cout <<endl;
}

// *** prints all global lists
void printAllLists()
{
    printList(mclist, "MC List");
    printList(recolist, "Reco List");

    printList(eplus, "e+ List");
    printList(eminus, "e- List");

    printList(muplus, "mu+ List");
    printList(muminus, "mu- List");

    printList(piplus, "pi+ List");
    printList(piminus, "pi- List");

    printList(kplus, "k+ List");
    printList(kminus, "k- List");

    printList(pplus, "p+ List");
    printList(pminus, "p- List");

    printList(gam, "gamma List");
}

// *** counts how many combinations (=composite particles in l) fullfill the mass criterion
int massCrit(CandList &l, double m, double w)
{
    int cnt = 0;
    for (unsigned int i=0;i<l.size();++i)
    {
        double mass=l[i].P4().M();
        if (mass>m-w && mass<m+w) cnt++;
    }

    return cnt;
}

// *** fill the mass histos
int fillHisto(CandList &l, TH1 *hall, TH1* hbg=0, double m =0, double w=0,  TH1* hallsel=0, TH1* hbgsel=0, TH1* hsig=0, TH1* hmm=0, TH2* hmvsmm=0)
{
    int cnt =0;
    for (unsigned int i=0;i<l.size();++i)
    {
        double mass = l[i].P4().M();
        double miss = (fIni-l[i].P4()).M();
        hall->Fill(mass);

        //missing mass
        if (hmm) hmm->Fill(miss);
        if (hmvsmm) hmvsmm->Fill(mass, miss);

        bool mct = l[i].Mct();
        if (hbg && !mct) hbg->Fill(mass);
        if (hsig && mct) hsig->Fill(mass);
        if (mass>m-w && mass<m+w)
        {
            cnt++;
            if (hallsel) hallsel->Fill(mass);
            if (hbgsel && !mct) hbgsel->Fill(mass);
        }
    }
    return cnt;
}

int combine(CandList &l1, CandList &l2, CandList &out, int matchPdg=0)
{
    out.clear();

    bool sameList = (&l1 == &l2);

    for (unsigned int i=0;i<l1.size();++i)
    {
        for (unsigned int j=0;j<l2.size();++j)
        {
            if (sameList && j<=i) continue;
            if (l1[i].Marker() & l2[j].Marker()) continue;  // *** overlap
            //if (l1[i].EvtId()!=l2[j].EvtId()) continue;     // *** tracks from different events
            SimpleCand c;
            c.SetP4(l1[i].P4()+l2[j].P4());
            c.SetCharge(l1[i].Charge()+l2[j].Charge());
            c.SetNFS(l1[i].NFS()+l2[j].NFS());
            c.SetMarker(l1[i].Marker() | l2[j].Marker());

            int m1 = l1[i].MotherIdx();
            int m2 = l2[j].MotherIdx();
            int ev1 = l1[i].EvtId();
            int ev2 = l2[j].EvtId();

            if ( ev1==ev2 &&							   // *** particles are from same event
                    m1>=0 && m1 == m2 &&                      // *** there is a common mother of all particles
                    (l1[i].McPid()||l1[i].Mct()) &&            // *** all particles match (FS: PID, Comp:mct)
                    (l2[j].McPid()||l2[j].Mct()) &&            // *** "
                    2==mclist[m1].NDau() &&            // *** mother has as many daughters as we combine particles
                    matchPdg == mclist[m1].Pid() )             // *** mother has correct PDG
            {
                c.SetId(m1);
                c.SetMcPid();
                c.SetMct();
                c.SetMotherIdx(mclist[m1].MotherIdx());
            }

            out.push_back(c);
        }
    }
    return out.size();
}

int combine(CandList &l1, CandList &l2, CandList &l3, CandList &out, int matchPdg=0)
{
    out.clear();

    bool same12 = (&l1 == &l2);
    bool same13 = (&l1 == &l3);
    bool same23 = (&l2 == &l3);

    for (unsigned int i1=0;i1<l1.size();++i1)
    {
        for (unsigned int i2=0;i2<l2.size();++i2)
        {
            if (same12 && i2<i1) continue;
            if (l1[i1].Marker() & l2[i2].Marker()) continue;

            for (unsigned int i3=0;i3<l3.size();++i3)
            {
                if ((same13 && i3<i1) || (same23 && i3<i2)) continue;

                if ( l1[i1].Marker() & l3[i3].Marker() ) continue;
                if ( l2[i2].Marker() & l3[i3].Marker() ) continue;

                //if (l1[i1].EvtId()!=l2[i2].EvtId() ||
                //    l1[i1].EvtId()!=l3[i3].EvtId() ||
                //    l2[i2].EvtId()!=l3[i3].EvtId()   ) continue;     // *** tracks from different events

                SimpleCand c;
                c.SetP4( l1[i1].P4() + l2[i2].P4() + l3[i3].P4());
                c.SetCharge( l1[i1].Charge() + l2[i2].Charge() + l3[i3].Charge() );
                c.SetNFS( l1[i1].NFS() + l2[i2].NFS() + l3[i3].NFS());
                c.SetMarker( l1[i1].Marker() | l2[i2].Marker() | l3[i3].Marker() );

                int m1 = l1[i1].MotherIdx();
                int m2 = l2[i2].MotherIdx();
                int m3 = l3[i3].MotherIdx();

                int ev1 = l1[i1].EvtId();
                int ev2 = l2[i2].EvtId();
                int ev3 = l3[i3].EvtId();

                if ( ev1==ev2 && ev2==ev3 &&				   // *** particles are from same event
                        m1>=0 && m1 == m2 && m2 == m3 &&          // *** there is a common mother of all particles
                        (l1[i1].McPid()||l1[i1].Mct()) &&          // *** all particles match (FS: PID, Comp:mct)
                        (l2[i2].McPid()||l2[i2].Mct()) &&          // *** "
                        (l3[i3].McPid()||l3[i3].Mct()) &&          // *** "
                        3==mclist[m1].NDau() &&            // *** mother has as many daughters as we combine particles
                        matchPdg == mclist[m1].Pid() )             // *** mother has correct PDG
                {
                    c.SetId(m1);
                    c.SetMcPid();
                    c.SetMct();
                    c.SetMotherIdx(mclist[m1].MotherIdx());
                }

                out.push_back(c);
            }
        }
    }

    return out.size();
}

void smearMom(SimpleCand &c, double dpr=0.05, double dtht=0.000, double dphi=0.000)
{
    TLorentzVector p4=c.P4();

    // *** use resolution function
    if (dpr<0) dpr = dponline(p4);

    double p    = p4.P();
    double m    = p4.M();

    double dp   = p*dpr;
    double newp = -1;

    if (dp==0) newp=p;
    while (newp<0) newp = p4.P() + fRand.Gaus(0.0,dp);

    p4.SetVectM( p4.Vect().Unit()*newp, m );

    double newtht = p4.Theta();
    if (dtht!=0) newtht += fRand.Gaus(0.,dtht);
    p4.SetTheta(newtht);

    double newphi = p4.Phi();
    if (dphi!=0) newphi += fRand.Gaus(0.,dphi);
    p4.SetPhi(newphi);

    c.SetP4(p4);

}

bool isDetected(double eff=0.90)
{
    // very simple efficiency cut; might be dependent on candidate itself later
    if (fRand.Rndm()<eff) return true;
    return false;
}

void makeRecoCands(CandList &mc, CandList &reco, double eff=0.90, double dp=0.05, double dtht=0.001, double dphi=0.001)
{
    reco.clear();

    for (unsigned int i=0;i<mc.size();i++)
    {
        SimpleCand c=mc[i];
        int pid = abs(c.Pid());
        if (!isFinalState(pid)) continue;

        if (pid!=22 && isDetected(eff))  // *** charged track
        {
            smearMom(c,dp,dtht,dphi);
			// set pion mass as default
			c.SetMass(0.13957);
            reco.push_back(c);
        }
        else if (pid==22 && isDetected(fNeutEff) && c.P4().E()>fNeutThrsh)  // *** neutral
        {
            smearMom(c,fdE, fNdtht, fNdphi);
            reco.push_back(c);
        }
    }
}

// **** A simple mass selector
void selectMass(CandList &in, CandList &out, double mmean=0, double mw=0)
{
    out.clear();

    for (unsigned int i=0;i<in.size();++i)
        if (fabs(in[i].P4().M()-mmean)<mw) out.push_back(in[i]);

}

// *** perform pid selection according to the pidtable
void select(CandList &in, CandList &out, int chrg=0, int pdg=0, double mass=0.139)
{
    out.clear();

    TParticlePDG *part = pdg_db->GetParticle(pdg);

    //pdg *= pdg<20 ? -chrg : chrg;

    if (part) mass = part->Mass();
    if (mass<0) mass = 0.13957;

    for (unsigned int i=0;i<in.size();++i)
    {
        SimpleCand c=in[i];

        // select a specific charge in any case
        if (c.Charge()==chrg)
        {
            c.SetMass(mass);
            if (c.Pid()==pdg) c.SetMcPid();
            if (pdg==0 || pdg==22) out.push_back(c); // do we want select a pdg code? 0=only charge
            else
            {
                // invalid pdg code?
                if (pdgidx.find(abs(pdg))==pdgidx.end()) continue;

                int selidx   = pdgidx[abs(pdg)];
                int partidx  = pdgidx[abs(c.Pid())];
                double prob  = pidtab[5*selidx + partidx]/100.;
                //cout <<pdg <<" "<<c.Pid()<<" : "<<selidx<<" "<<partidx<<" P:"<<prob<<endl;

                if (fRand.Rndm()<prob) out.push_back(c);
            }
        }
    }
}

void makePidSelection(CandList &l)
{
    // *** prepare globally defined PID lists
    select(l, eplus, 1, -11, mE);
    select(l, eminus, -1, 11, mE);

    select(l, muplus, 1, -13, mMu);
    select(l, muminus, -1, 13, mMu);

    select(l, piplus, 1, 211, mPi);
    select(l, piminus, -1, -211, mPi);

    select(l, kplus, 1, 321, mK);
    select(l, kminus, -1, -321, mK);

    select(l, pplus, 1, 2212, mP);
    select(l, pminus, -1, -2212, mP);

    select(l, gam, 0, 22, 0);
}

void makeIni4Vector(TLorentzVector &l, double s)
{
    double X = (s*s-2*mP*mP)/(2*mP);
    double p = sqrt(X*X - mP*mP);
    l.SetXYZM(0,0,p,s);
}


void softtrigger_kin5(TString fsig, int mode=0, double sqrts=3.77, int nev=1000, bool evcut=false, double dp=0.03,
                     double trkeff=100., double pideff=95., double misid =5., double P_mix=0.00 )
{
    fRand.SetSeed();

    init(pideff, misid);

    unsigned int i,j,k;

    makeIni4Vector(fIni, sqrts);

    //gStyle->SetOptStat(1111);

    // *** open file and get tree
    TFile *f=new TFile(fsig,"READ");
    if (f->IsZombie())
    {
        cout <<"File not found:"<<fsig<<endl;
        return;
    }

    bool dpm=fsig.Contains("DPM");            // *** DPM events
	bool bkg=dpm || fsig.Contains("BKG");     // *** Background events from EvtGen OR DPM events
    TString treename = dpm ? "data" : "ntp";  // *** set treename
    TTree *t=(TTree*)f->Get(treename);

    if (t==0)
    {
        cout <<"Tree '"<<treename<<"' not found in '"<<fsig<<"'."<<endl;
        return;
    }

    // *** how many events ?
    int Nevt = t->GetEntriesFast();
    if (nev==0) nev = Nevt;

    double mmean=0, mwsig=0, mwoff=0;
    int matchPDG=0;

    // *** choose histogram scale according to resonance
    high = mmean+2;

    double min=0, max=5.5;

    TString name = "ntp_";
    if (!bkg) name+="sig_mode_";
    else name+="bkg_mode_";
    name+=mode; name+=".root";
	
	TFile *ggg=new TFile(name, "RECREATE");

	//branches
    //                   0   1     2     3     4   5   6   7  8   9   10   11    12   13   14   15   16   17   18   19   20    21    22    23    24    25    26    27   28  29  30  31  32  33
	TString branches = "sig:mode:npart:nneut:nchrg:ne:nmu:npi:nk:npr:pmax:ptmax:np05:np10:np15:np20:np25:np30:np40:np50:npt05:npt10:npt15:npt20:npt25:npt30:npt40:npt50:sph:apl:pla:thr:sne:scp:";

    //                  34  35  36   37     38    39    40    41    42    43    44    45    46    47   48   49    50    51    52    53   54   55    56
	branches        += "fw1:fw2:fw3:pmaxl:ptmaxl:np05l:np10l:np15l:np20l:np25l:np30l:np40l:np50l:ne10:ne20:nmu10:nmu20:npi10:npi20:nk10:nk20:npr10:npr20:";
	
    //                   57     58     59     60     61      62      63    64    65     66    67  68  69  70   71    72     73      74      75      76      77       78      79
	branches        += "sumpt:sumptl:sumptc:sumptcl:sumetn:sumetnl:sumpc:sumpcl:sumen:sumenl:cir:fw4:fw5:pmin:ptmin:pminl:prapmax:sumpt05:sumpt10:sumpc05:sumpc10:sumpc05l:sumpc10l:";
    
	//                    80        81       82      83    84   85    86     87     88     89
	branches        += "sumen05:sumen10:sumen05l:sumen10l:npi0:nks0:npi005:nks005:npi010:nks010";
	
    TNtuple *ntpev = new TNtuple("ntpev","ntpev", branches);

    Float_t ntpval[100];

    // *** prepare the tree to loop through

    Int_t nTrk,Id[fMAX],DF[fMAX],DL[fMAX];
    Double_t px[fMAX],py[fMAX],pz[fMAX],E[fMAX],m[fMAX];
    TClonesArray* DpmPart=0;

    if (dpm)
    {
        DpmPart=new TClonesArray("TParticle",100);
        t->SetBranchAddress("Npart",&nTrk);
        t->SetBranchAddress("Particles", &DpmPart);
    }
    else
    {
        t->SetBranchAddress("nTrk",&nTrk);
        t->SetBranchAddress("Id",Id);
        t->SetBranchAddress("DF",DF);
        t->SetBranchAddress("DL",DL);
        t->SetBranchAddress("px",px);
        t->SetBranchAddress("py",py);
        t->SetBranchAddress("pz",pz);
        t->SetBranchAddress("E",E);
        t->SetBranchAddress("m",m);
    }


    // *** the event loop

    int mixevts=1;
    int currmix=0;

    // how many events are tagged by individual algo?
    bool channelselect[6];
    double channelcount[6];
    TString channelname[6]={"J/psi","D+-","Ds","Phi","D0","Lamc"};

    for (i=0;i<6;++i) channelcount[i]=0.;

    if (fRand.Rndm()<P_mix) mixevts=2;

    for (i=0;i<nev;++i)
    {
        t->GetEntry(i);

        if (i%5000==0) cout <<"ev "<<i<<endl;

        // *** read in from file the mc candidate
        if (0==currmix)
        {
            mclist.clear();   // *** contains all MC cands (also non-final-states)
            mcFSlist.clear(); // *** contains only final state MC cands
        }

        bool good = true;

        if (dpm)
        {
            for (j=0;j<nTrk;++j)
            {
                TParticle *pt=(TParticle*)DpmPart->At(j);
                Id[j] = pt->GetPdgCode();
                int pdg = abs(Id[j]);
                px[j] = pt->Px();
                py[j] = pt->Py();
                pz[j] = pt->Pz();
                E[j]  = pt->Energy();
                m[j]  = pt->GetMass();
                DF[j] = 0;
                DL[j] = -1;
            }
        }

		for (j=0;j<nTrk;++j)
        {
            int chrg = 0;
            int pdg = abs(Id[j]);
            TParticlePDG *p=pdg_db->GetParticle(Id[j]);
            if (p) chrg = p->Charge();
            if (abs(chrg)>1) chrg/=3;

            SimpleCand c(px[j], py[j], pz[j], E[j], Id[j], j, chrg);
            c.SetDau(DF[j], DL[j]);

            for (k=0;k<j;++k) if (j>=DF[k] && j<=DL[k])
                {
                    c.SetMotherIdx(k);
                    c.SetNSiblings(DL[k]-DF[k]+1);
                }

            c.SetEvtId(i);
			
            mclist.push_back(c);

            // final state particle?
            if (isFinalState(pdg)) mcFSlist.push_back(c);
        }

        if (++currmix<mixevts) continue;

        currmix=0;

		// *** the boost of the system
		TVector3 boost = fIni.BoostVector();

        // *** prepare the toy reco: smear momentum and apply track efficiency (both flat at the moment)
        makeRecoCands(mcFSlist, recolist, trkeff/100., dp );

/*		int nneut = 0;
		int nchrg = 0;

		for (j=0;j<recolist.size();++j)
		{
			int chrg = recolist[j].Charge();
			if (abs(chrg)==0) nneut++; else nchrg++;
		}
*/		
		boostList(recolist, recocmslist, boost);

        // *** prepare all pid list according to the pidtable probabilities
        makePidSelection(recolist);
		
		EventShape evs(recolist, fIni);
		
		// *** the lists we need for combinatorics
		CandList comb1;
		CandList LPi0, LKs;				// pi0 -> gg (mass cut), KS -> pi+ pi- (mass cut)
		
		// *** do combinatorics (pi0 -> gg) 
		combine(gam, gam, comb1, fPi0Pdg);
		//fillHisto(comb1, h_pi0, 0, fPi0Mass, fPi0win, h_pi0sel, 0, h_pi0sig ); 
		selectMass(comb1, LPi0, fPi0Mass, fPi0win);
		
		// *** do combinatorics (pi0 -> gg) 
		combine(piplus, piminus, comb1, fKsPdg);
		//fillHisto(comb1, h_ks, 0, fKsMass, fKswin, h_kssel, 0, h_kssig ); 
		selectMass(comb1, LKs, fKsMass, fKswin);
		
		// *** Make Event Selection
// 		if (evcut)
// 		{
// 			EventShape evshape(recolist, fIni);
// 			if (evshape.ChrgPtSumLab()<1.5) continue;
// 		}

        //        0   1     2     3     4   5   6   7  8  9   10   11    12   13   14   15   16   17   18   19   20    21    22    23    24    25    26    27   28  29  30  31  32  33
	    //      "sig:mode:npart:nneut:nchrg:ne:nmu:npi:nk:npr:pmax:ptmax:np05:np10:np15:np20:np25:np30:np40:np50:npt05:npt10:npt15:npt20:npt25:npt30:npt40:npt50:sph:apl:pla:thr:sne:scp";

        //       34  35  36   37     38    39    40    41    42    43    44    45    46    47   48   49    50    51    52    53   54   55    56
	    //      "fw1:fw2:fw3:pmaxl:ptmaxl:np05l:np10l:np15l:np20l:np25l:np30l:np40l:np50l:ne10:ne20:nmu10:nmu20:npi10:npi20:nk10:nk20:npr10:npr20");
        
        //        57     58     59     60     61      62      63    64    65     66    67  68  69  70   71    72     73      74      75      76      77       78      79
	    //      "sumpt:sumptl:sumptc:sumptcl:sumetn:sumetnl:sumpc:sumpcl:sumen:sumenl:cir:fw4:fw5:pmin:ptmin:pminl:prapmax:sumpt05:sumpt10:sumpc05:sumpc10:sumpc05l:sumpc10l:";
    
	    //       80        81       82      83    84   85    86     87     88     89
	    //   "sumen05:sumen10:sumen05l:sumen10l:npi0:nks0:npi005:nks005:npi010:nks010";
		
		ntpval[0]  = !bkg;                          // signal or background events
		ntpval[1]  = mode;                          // decay mode (parameter of script - set by hand)
		
		ntpval[2]  = evs.NParticles();              // #particles (multiplicity)
		ntpval[3]  = evs.NNeutral();                // #neutrals cands
		ntpval[4]  = evs.NCharged();                // #charged cands
		
		ntpval[5]  = eplus.size()+eminus.size();    // #electron candidates
		ntpval[6]  = muplus.size()+muminus.size();  // #muon candidates
		ntpval[7]  = piplus.size()+piminus.size();  // #pion candidates
		ntpval[8]  = kplus.size()+kminus.size();    // #kaon candidates
		ntpval[9]  = pplus.size()+pminus.size();    // #proton candidates
		ntpval[84] = LPi0.size();					// #pi0 candidates
		ntpval[85] = LKs.size();					// #KS0 candidates
		
		ntpval[37] = evs.PmaxLab();                 // maximum momentum (lab)
		ntpval[38] = evs.Ptmax();                   // maximum pt (lab)
		ntpval[72] = evs.PminLab();                 // minimum momentum (lab)
		ntpval[71] = evs.Ptmin();                   // minimum pt (lab)
		ntpval[73] = evs.PRapmax();                 // maximum pseudo rapidity (lab)
		
		ntpval[39] = evs.MultPminLab(0.5);          // #particles with p>0.5 GeV (lab)
		ntpval[40] = evs.MultPminLab(1.0);          // #particles with p>1.0 GeV (lab)
		ntpval[41] = evs.MultPminLab(1.5);          // #particles with p>1.5 GeV (lab)
		ntpval[42] = evs.MultPminLab(2.0);          // #particles with p>2.0 GeV (lab)
		ntpval[43] = evs.MultPminLab(2.5);          // #particles with p>2.5 GeV (lab)
		ntpval[44] = evs.MultPminLab(3.0);          // #particles with p>3.0 GeV (lab)
		ntpval[45] = evs.MultPminLab(4.0);          // #particles with p>4.0 GeV (lab)
		ntpval[46] = evs.MultPminLab(5.0);          // #particles with p>5.0 GeV (lab)
		
		ntpval[58] = evs.PtSumLab();				// sum pt of all particles (lab)
		ntpval[60] = evs.ChrgPtSumLab();			// sum pt of chrg particles (lab)
		ntpval[62] = evs.NeutEtSumLab();			// sum Et of neut particles (lab)
		ntpval[64] = evs.ChrgPSumLab();				// sum p of chrg particles (lab)
		ntpval[66] = evs.NeutESumLab();				// sum E of neut particles (lab)

		ntpval[78] = evs.SumChrgPminLab(0.5);		// sum p of charged particles with p>0.5 (lab)
		ntpval[79] = evs.SumChrgPminLab(1.0);		// sum p of charged particles with p>1.0 (lab)
		
		ntpval[82] = evs.SumNeutEminLab(0.5);		// sum E of neutral particles with E>0.5 (lab)
		ntpval[83] = evs.SumNeutEminLab(1.0);		// sum E of neutral particles with E>1.0 (lab)

		ntpval[58] = evs.PtSumLab();				// sum pt of all particles (lab)
		ntpval[74] = evs.SumPtminLab(0.5);			// sum pt of all particles with pt>0.5 GeV
		ntpval[75] = evs.SumPtminLab(1.0);			// sum pt of all particles with pt>1.0 GeV
					 
		// all following values are in center-of-momentum system
		ntpval[10] = evs.PmaxCms();                 // maximum momentum
		ntpval[11] = evs.Ptmax();                   // maximum pt
		ntpval[70] = evs.PminCms();                 // minimum momentum
		
		ntpval[12] = evs.MultPminCms(0.5);          // #particles with p>0.5 GeV
		ntpval[13] = evs.MultPminCms(1.0);          // #particles with p>1.0 GeV
		ntpval[14] = evs.MultPminCms(1.5);          // #particles with p>1.5 GeV
		ntpval[15] = evs.MultPminCms(2.0);          // #particles with p>2.0 GeV
		ntpval[16] = evs.MultPminCms(2.5);          // #particles with p>2.5 GeV
		ntpval[17] = evs.MultPminCms(3.0);          // #particles with p>3.0 GeV
		ntpval[18] = evs.MultPminCms(4.0);          // #particles with p>4.0 GeV
		ntpval[19] = evs.MultPminCms(5.0);          // #particles with p>5.0 GeV
				
		ntpval[57] = evs.PtSumCms();				// sum pt of all particles 
		ntpval[59] = evs.ChrgPtSumCms();			// sum pt of chrg particles 
		ntpval[61] = evs.NeutEtSumCms();			// sum Et of neut particles 
		ntpval[63] = evs.ChrgPSumCms();				// sum p of chrg particles
		ntpval[65] = evs.NeutESumCms();				// sum E of neut particles
		
		ntpval[76] = evs.SumChrgPminCms(0.5);		// sum p of charged particles with p>0.5 (cms)
		ntpval[77] = evs.SumChrgPminCms(1.0);		// sum p of charged particles with p>1.0 (cms)
		
		ntpval[80] = evs.SumNeutEminCms(0.5);		// sum E of neutral particles with E>0.5 (cms)
		ntpval[81] = evs.SumNeutEminCms(1.0);		// sum E of neutral particles with E>1.0 (cms)
		
		ntpval[20] = 0.0;                           // #particles with pt>0.5 GeV
		ntpval[21] = 0.0;                           // #particles with pt>1.0 GeV
		ntpval[22] = 0.0;                           // #particles with pt>1.5 GeV
		ntpval[23] = 0.0;                           // #particles with pt>2.0 GeV
		ntpval[24] = 0.0;                           // #particles with pt>2.5 GeV
		ntpval[25] = 0.0;                           // #particles with pt>3.0 GeV
		ntpval[26] = 0.0;                           // #particles with pt>4.0 GeV
		ntpval[27] = 0.0;                           // #particles with pt>5.0 GeV

		ntpval[47] = 0.0;							// #electrons with p>1.0
		ntpval[48] = 0.0;							// #electrons with p>2.0

		ntpval[49] = 0.0;							// #muons with p>1.0
		ntpval[50] = 0.0;							// #muons with p>2.0
		
		ntpval[51] = 0.0;							// #pions with p>1.0
		ntpval[52] = 0.0;							// #pions with p>2.0

		ntpval[53] = 0.0;							// #kaons with p>1.0
		ntpval[54] = 0.0;							// #kaons with p>2.0
		
		ntpval[55] = 0.0;							// #protons with p>1.0
		ntpval[56] = 0.0;							// #protons with p>2.0
		
		ntpval[86] = 0.0;							// #pi0s with p>0.5
		ntpval[88] = 0.0;							// #pi0s with p>1.0
		
		ntpval[87] = 0.0;							// #KSs with p>0.5
		ntpval[89] = 0.0;							// #KSs with p>1.0
		
		//compSphericity(recocmslist);
		
		//TVector3 thrAxis;
		//_thr = compThrust(recocmslist, thrAxis);
		
		// quantities based on momentum tensor
		ntpval[28] = evs.Sphericity();				// sphericity = 3/2 * (lam_2+lam_3)
		ntpval[29] = evs.Aplanarity();				// aplanarity = 3/2 * lam_3
		ntpval[30] = evs.Planarity();				// planarity  = lam_2 - lam_3
		ntpval[67] = evs.Circularity();				// circularity = 2 * min(lam_1, lam_2)/(lam_1 + lam_2)
		ntpval[31] = evs.Thrust();					// thrust
		
		//compFoxWolfMoms(recocmslist);
		
		ntpval[34] = evs.FoxWolfMomR(1);					// Fox Wolfram Moment R1 = H1/H0
		ntpval[35] = evs.FoxWolfMomR(2);					// Fox Wolfram Moment R2 = H2/H0
		ntpval[36] = evs.FoxWolfMomR(3);					// Fox Wolfram Moment R3 = H3/H0
		ntpval[68] = evs.FoxWolfMomR(4);					// Fox Wolfram Moment R4 = H4/H0
		ntpval[69] = evs.FoxWolfMomR(5);					// Fox Wolfram Moment R5 = H5/H0
		
		// count electrons
		for (j=0; j<eplus.size(); ++j) 
		{
			if (eplus[j].P4().P()>1.0) ntpval[47]+=1.0;
			if (eplus[j].P4().P()>2.0) ntpval[48]+=1.0;
		}
		for (j=0; j<eminus.size(); ++j) 
		{
			if (eminus[j].P4().P()>1.0) ntpval[47]+=1.0;
			if (eminus[j].P4().P()>2.0) ntpval[48]+=1.0;
		}
		
		// count muons
		for (j=0; j<muplus.size(); ++j) 
		{
			if (muplus[j].P4().P()>1.0) ntpval[49]+=1.0;
			if (muplus[j].P4().P()>2.0) ntpval[50]+=1.0;
		}
		for (j=0; j<muminus.size(); ++j) 
		{
			if (muminus[j].P4().P()>1.0) ntpval[49]+=1.0;
			if (muminus[j].P4().P()>2.0) ntpval[50]+=1.0;
		}
		
		// count pions
		for (j=0; j<piplus.size(); ++j) 
		{
			if (piplus[j].P4().P()>1.0) ntpval[51]+=1.0;
			if (piplus[j].P4().P()>2.0) ntpval[52]+=1.0;
		}
		for (j=0; j<piminus.size(); ++j) 
		{
			if (piminus[j].P4().P()>1.0) ntpval[51]+=1.0;
			if (piminus[j].P4().P()>2.0) ntpval[52]+=1.0;
		}
		
		// count kaons
		for (j=0; j<kplus.size(); ++j) 
		{
			if (kplus[j].P4().P()>1.0) ntpval[53]+=1.0;
			if (kplus[j].P4().P()>2.0) ntpval[54]+=1.0;
		}
		for (j=0; j<kminus.size(); ++j) 
		{
			if (kminus[j].P4().P()>1.0) ntpval[53]+=1.0;
			if (kminus[j].P4().P()>2.0) ntpval[54]+=1.0;
		}
		
		// count protons
		for (j=0; j<pplus.size(); ++j) 
		{
			if (pplus[j].P4().P()>1.0) ntpval[55]+=1.0;
			if (pplus[j].P4().P()>2.0) ntpval[56]+=1.0;
		}
		for (j=0; j<pminus.size(); ++j) 
		{
			if (pminus[j].P4().P()>1.0) ntpval[55]+=1.0;
			if (pminus[j].P4().P()>2.0) ntpval[56]+=1.0;
		}
		
		// count pi0
		for (j=0; j<LPi0.size(); ++j) 
		{
			if (LPi0[j].P4().P()>0.5) ntpval[86]+=1.0;
			if (LPi0[j].P4().P()>1.0) ntpval[88]+=1.0;
		}
		// count KS
		for (j=0; j<LKs.size(); ++j) 
		{
			if (LKs[j].P4().P()>0.5) ntpval[87]+=1.0;
			if (LKs[j].P4().P()>1.0) ntpval[89]+=1.0;
		}
		
		
		
		for (j=0;j<recolist.size();++j)
		{
			// *** candidate in lab system
			SimpleCand     cnd    = recolist[j];
			TLorentzVector lv     = cnd.P4();
			
			// *** candidate in cms system
			SimpleCand     cndcms = recocmslist[j];
			TLorentzVector lvcm   = cndcms.P4();
			
			// compute in LAB
			
			
			if (lvcm.Pt()>0.5) ntpval[20]+=1.0;
			if (lvcm.Pt()>1.0) ntpval[21]+=1.0;
			if (lvcm.Pt()>1.5) ntpval[22]+=1.0;
			if (lvcm.Pt()>2.0) ntpval[23]+=1.0;
			if (lvcm.Pt()>2.5) ntpval[24]+=1.0;
			if (lvcm.Pt()>3.0) ntpval[25]+=1.0;
			if (lvcm.Pt()>4.0) ntpval[26]+=1.0;
			if (lvcm.Pt()>5.0) ntpval[27]+=1.0;
			
        }

		ntpev->Fill(ntpval);

        mixevts=1;
        if (fRand.Rndm()<P_mix) mixevts=2;
    }

	
    ggg->cd();
    ntpev->Write();
    ggg->Close();

}
