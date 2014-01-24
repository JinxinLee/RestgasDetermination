#include "EventShape.h"
#include "SimpleNtp.C"
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
#include "TGaxis.h"

using std::cout;
using std::endl;

TRandom3      fRand;
TLorentzVector fIni;

// *** some program parameters 
const int		fMAX	   = 100;  		// *** max tracks per event
int 			nbins 		= 300;      // *** bins of mass histograms
double 			low  		= 0.0;      // *** lower edge of histo
double 			high 		= 6.0;      // *** higher edge of histo (will be modified acc. to resonance mass)

// *** some contants for neutrals
const double    fNeutEff   = 0.95;      // *** efficiency for neutrals
const double    fdE        = 0.05;      // *** neutral energy resolution
const double    fNdtht     = 0.003;     // *** neutrals tht resolution
const double    fNdphi     = 0.003;     // *** neutrals phi resolution

// number of sigmas selection for pi0, KS
int nsigpiks = 5;
// number of sigmas selection for channels
int nsig     = 8;

// *** constants for particles
int				fPi0Pdg	   = 111;		// *** PDG code for pi0
double		 	fPi0Mass   = 0.13497;    // *** center of mass cut window
double			fPi0win	   = 0;
double			fPi0sig	   = 0.0055;

int				fKsPdg	   = 310;		// *** PDG code for pi0
double		 	fKsMass   = 0.497614;  // *** center of mass cut window
double			fKswin	   = 0.025;
double			fKssig	   = 0.012;

int             fDPdg      = 411;         // *** PDG code to match D
double		 	fDMass     = 1.8693;      // *** center of mass cut window
double			fDsig      = 0.045;        // *** sigma of peak
double			fDwin		=0.25;

int             fD0Pdg     = 421;         // *** PDG code to match D0
double		 	fD0Mass    = 1.8645;      // *** center of mass cut window
double			fD0sig     = 0.055;        // *** sigma of peak
double			fD0win		=0.25;

int             fJpsiPdg   = 443;         // *** PDG code to match J/psi
double		 	fJMass     = 3.09687;        // *** center of mass cut window
double			fJsig      = 0.11;        // *** sigma of peak
double			fJwin		=0.7;
	
int             feePdg    = 88880;         // *** PDG code to match pbarpSystem
double		 	feeMass   = 0;             // *** center of mass cut window, here = sqrt(s)
double			feesigconst = 0.0024;        // *** const of linear width function
double			feesigslope = 0.036;        // *** slope of linear width function
double			feewin	  = 0.;             // *** to be determined by function 

int             fLamPdg    = 3122;         // *** PDG code to match Lam
double		 	fLamMass   = 1.115684;        // *** center of mass cut window
double			fLamsig    = 0.0042;        // *** sigma of peak
double			fLamwin		=0.1;

int             fDsPdg     = 431;         // *** PDG code to match Ds
double		 	fDsMass    = 1.9686;        // *** center of mass cut window
double			fDssig     = 0.04;        // *** sigma of peak
double			fDswin		=0.2;
	
int             fPhiPdg     = 333;         // *** PDG code to match Phi
double		 	fPhiMass    = 1.0195;        // *** center of mass cut window
double			fPhisig     = 0.0065;        // *** sigma of peak
double			fPhiwin		=0.2;
	
int             fEtacPdg     = 441;         // *** PDG code to match etac
double		 	fEtacMass    = 2.9798;        // *** center of mass cut window
double			fEtacsig     = 0.08;        // ***  sigma of peak
double			fEtacwin      =0.15;
	
int             fLamcPdg     = 4122;         // *** PDG code to match Lambda_c
double		 	fLamcMass    = 2.2849;        // *** center of mass cut window
double			fLamcsig     = 0.035;        // *** sigma of peak
double			fLamcwin	  =0.2;

// *** pid probability table; will be modified according to purity parameter value
// ***  true pid:        e   mu    pi    K   p      classified as [%]    
double pidtab [25] = {  80,   5,   5,   5,   5,    //  e
                         5,  80,   5,   5,   5,    // mu       
                         5,   5,  80,   5,   5,    // pi
                         5,   5,   5,  80,   5,    // K
                         5,   5,   5,   5,  80 };  // p
						 
// *** table determined by Donghee 11/2012 for P>0.1						 
double pidreal [25] = {  95,  6,  10,   7,  10,    //  e
                         5,  86,  27,   6,   7,    // mu       
                         9,  29,  85,  18,  16,    // pi
                         9,  21,  40,  67,  24,    // K
                         12, 12,  20,  16,  90 };  // p
                         
// *** global stable particle masses used in pid selection; set in init()
double mE, mMu, mPi, mK, mP, mPi0;

TF1 *f_res=0;
int npi, nk, npr, ne, nmu, nlep, evtno, globmode;

// *** this map contains mapping from PDG code to indices 0...4 (for pidtab)
// *** 11 -> 0, 13 -> 1, 211 -> 2, 321 -> 3, 2212 -> 4
std::map<int,int> pdgidx;

TDatabasePDG *pdg_db;

typedef std::vector<SimpleCand> CandList;

// *** all global particle lists we need to do combinatorics
CandList mclist, mcFSlist, recolist,recocmslist;

// *** the global pid lists
CandList eplus, eminus, muplus, muminus, piplus, piminus, kplus, kminus, pplus, pminus, gam;

// *** setup  style
void setStyle()
{
	gStyle->SetPadTopMargin(0.13);
	gStyle->SetPadBottomMargin(0.18);
	gStyle->SetPadLeftMargin(0.2);
	gStyle->SetPadRightMargin(0.015);

	gStyle->SetLabelSize(0.075,"X");
	gStyle->SetLabelSize(0.07,"Y");

	gStyle->SetStatY(0.87);
	gStyle->SetStatX(0.985);
	gStyle->SetStatW(0.4);
	gStyle->SetStatH(0.12);
	gStyle->SetOptStat("e"); // only entries
	
	gStyle->SetTitleH(0.1);
	gStyle->SetTitleX(0.08);
	gStyle->SetTitleOffset(1.1,"x");
	gStyle->SetTitleXSize(0.075);
}

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

// *** is there one mass combination in the window?
bool eventAccepted(CandList &l,  double m =0, double w=0)
{
	bool accepted = false;
	unsigned int i=0;
	
	while (!accepted && i<l.size())
	//for (unsigned int i=0;i<l.size();++i) 
	{
		double mass = l[i++].P4().M();
		if (mass>m-w && mass<m+w) accepted =true;
	}
	
	return accepted;
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
			c.AddDau(&(l1[i]));c.AddDau(&(l2[j]));
			c.SetPdg(matchPdg);
			
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
				c.SetEvtId(ev1);
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
				c.AddDau(&(l1[i1]));c.AddDau(&(l2[i2]));c.AddDau(&(l3[i3]));
				c.SetPdg(matchPdg);
				
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
					c.SetEvtId(ev1);
				}
				
				out.push_back(c);
			}
		}
	}
	
	return out.size();
}

int combine(CandList &l1, CandList &l2, CandList &l3, CandList &l4, CandList &out, int matchPdg=0)
{
	out.clear();
	
	bool same12 = (&l1 == &l2);
	bool same13 = (&l1 == &l3);
	bool same14 = (&l1 == &l4);
	
	bool same23 = (&l2 == &l3);
	bool same24 = (&l2 == &l4);
	
	bool same34 = (&l3 == &l4);
	
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
				
				for (unsigned int i4=0;i4<l4.size();++i4)
				{
					if ((same14 && i4<i1) || (same24 && i4<i2) || (same34 && i4<i3)) continue;
					
					if ( l1[i1].Marker() & l4[i4].Marker() ) continue;
					if ( l2[i2].Marker() & l4[i4].Marker() ) continue;
					if ( l3[i3].Marker() & l4[i4].Marker() ) continue;
					
					//if (l1[i1].EvtId()!=l2[i2].EvtId() || 
					//    l1[i1].EvtId()!=l3[i3].EvtId() ||
					//    l2[i2].EvtId()!=l3[i3].EvtId()   ) continue;     // *** tracks from different events
					
					SimpleCand c;
					c.SetP4( l1[i1].P4() + l2[i2].P4() + l3[i3].P4() + l4[i4].P4() );
					c.SetCharge( l1[i1].Charge() + l2[i2].Charge() + l3[i3].Charge() + l4[i4].Charge() );
					c.SetNFS( l1[i1].NFS() + l2[i2].NFS() + l3[i3].NFS() + l4[i4].NFS() );
					c.SetMarker( l1[i1].Marker() | l2[i2].Marker() | l3[i3].Marker() | l4[i4].Marker() );
					c.AddDau(&(l1[i1]));c.AddDau(&(l2[i2]));c.AddDau(&(l3[i3]));c.AddDau(&(l4[i4]));
					c.SetPdg(matchPdg);
					
					int m1 = l1[i1].MotherIdx();
					int m2 = l2[i2].MotherIdx();
					int m3 = l3[i3].MotherIdx();
					int m4 = l4[i4].MotherIdx();
					
					int ev1 = l1[i1].EvtId();
					int ev2 = l2[i2].EvtId();
					int ev3 = l3[i3].EvtId();
					int ev4 = l4[i4].EvtId();
					
					if ( ev1==ev2 && ev2==ev3 && ev3==ev4 &&			// *** particles are from same event
						m1>=0 && m1 == m2 && m2 == m3 && m3 == m4 &&	// *** there is a common mother of all particles
						(l1[i1].McPid()||l1[i1].Mct()) &&				// *** all particles match (FS: PID, Comp:mct)
						(l2[i2].McPid()||l2[i2].Mct()) &&				// *** "
						(l3[i3].McPid()||l3[i3].Mct()) &&				// *** "
						(l4[i4].McPid()||l4[i4].Mct()) &&				// *** "
								4==mclist[m1].NDau() &&					// *** mother has as many daughters as we combine particles
						matchPdg == mclist[m1].Pid() )					// *** mother has correct PDG
					{
						c.SetId(m1);
						c.SetMcPid();
						c.SetMct();
						c.SetMotherIdx(mclist[m1].MotherIdx());
						c.SetEvtId(ev1);
					}
					
					out.push_back(c);
				}
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
	
	unsigned int marker = 1;
	
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
			c.SetMarker(marker);
			c.SetPdg(211*c.Charge());
			marker *=2;
			reco.push_back(c);
		}
		else if (pid==22 && isDetected(fNeutEff))  // *** neutral
		{
			smearMom(c,fdE, fNdtht, fNdphi);
			c.SetMarker(marker);
			c.SetPdg(22);
			marker *=2;
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
	if (mass<0) mass = 0.139;

	for (unsigned int i=0;i<in.size();++i)
	{
		SimpleCand c=in[i];
		
		// select a specific charge in any case
		if (c.Charge()==chrg)
		{
			c.SetMass(mass);
			if (c.Pid()==pdg) c.SetMcPid();
			c.SetPdg(pdg);
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

void configHisto(TH1* h)
{
	h->SetFillStyle(3001);
	h->SetFillColor(602);
	h->SetLineStyle(3);
	h->SetLineColor(602);
/*	TGaxis *ax = (TGaxis*)h->GetYaxis();
	ax->SetMaxDigits(2);*/
}

void qaCand(TString pre, SimpleCand &c, SimpleNtp *n)
{
	TLorentzVector l = c.P4();
	TVector3 boost = fIni.BoostVector();
	TLorentzVector lb = l;
	lb.Boost(-boost);
	
	n->Column(pre+"px",  l.Px());
	n->Column(pre+"py",  l.Py());
	n->Column(pre+"pz",  l.Pz());
	n->Column(pre+"p",   l.P());
	n->Column(pre+"e",   l.E());
	n->Column(pre+"m",   l.M());
	n->Column(pre+"pdg", c.Pdg());
	n->Column(pre+"tpdg", c.Pid());
	n->Column(pre+"ch",  c.Charge());
	
	n->Column(pre+"pt",  l.Pt());
	n->Column(pre+"tht", l.Theta());
	n->Column(pre+"phi", l.Phi());
	n->Column(pre+"pcm", lb.P());
	
	n->Column(pre+"mct", c.Mct()||c.McPid() );
}

// int getDau( int idx, SimpleCand &c)
// {
// 	int num=0;
// 	int curridx=-1;
// 	unsigned int marker = c.Marker();
// 	
// 	while (curridx<idx)
// 	{
// 		while (!(marker&1) && num<32) {num++;marker>>=1;}
// 		curridx++;
// 	}
// 	return num;
// }
// 
// int countDau(SimpleCand &c)
// {
// 	int num=0;
// 	unsigned int marker = c.Marker();
// 	while (marker) {marker &= (marker-1); num++;}
// 	
// 	return num;
// }
// 
// int getDauIdx(SimpleCand &c, int* dauidx)
// {
// 	int num=0;
// 	int idx=0;
// 	unsigned int marker = c.Marker();
// 
// 	while (marker)
// 	{
// 		if (marker & 1) 
// 		{
// 			dauidx[num]=idx;
// 			num++;
// 		}
// 		marker >>= 1;
// 		idx++;
// 	}
// 	return num;
// }

void writeTuple(TString pre, CandList &l, EventShape &evsh, SimpleNtp *n, double mass, double win)
{
	int dauidx[32];
	for (int i=0;i<l.size();++i)
	{
		qaCand(pre,l[i], n);
		float tag = 0;
		float mct = l[i].Mct();
		
		if (fabs(l[i].P4().M()-mass)<win) tag=1.0;
		
		n->Column("tag",tag);
		n->Column("mct",mct);
	
		//int nDau = getDauIdx(l[i], dauidx);
		
		//cout <<"c("<<l[i].Marker()<<") "<<i<<" made from ";
		for (int j=0;j<l[i].NDau();++j)
		{
			//cout <<dauidx[j]<<"  ";
			//SimpleCand d = recolist[dauidx[j]];
			SimpleCand d=*(l[i].Dau(j));
			qaCand(TString::Format("%sd%d",pre.Data(),j),d, n);
		}
		//cout <<endl;
		
		n->Column("evt", evtno);
		n->Column("mode", globmode);
		
		// store eventshape
		n->Column("npart", evsh.NParticles());
		n->Column("nneut", evsh.NNeutral());
		n->Column("nchrg", evsh.NCharged());
		n->Column("sumpc", evsh.ChrgPSumCms());
		n->Column("sumpt", evsh.PtSumLab());
		n->Column("sumptc", evsh.ChrgPtSumLab());
		n->Column("pmaxl" ,evsh.PmaxLab());
		n->Column("pmax", evsh.PmaxCms());
		n->Column("pmin", evsh.PminCms());
		n->Column("ptmax",evsh.Ptmax());
		n->Column("thr",  evsh.Thrust());
		n->Column("pla",  evsh.Planarity());
		n->Column("apl",  evsh.Aplanarity());
		n->Column("sph",  evsh.Sphericity());
		n->Column("cir",  evsh.Circularity());
		n->Column("ne",   ne);
		n->Column("nmu",  nmu);
		n->Column("npi",  npi);
		n->Column("nk",   nk);
		n->Column("npr",  npr);
		n->Column("fw1", evsh.FoxWolfMomR(1));
		n->Column("fw2", evsh.FoxWolfMomR(2));
		n->Column("fw3", evsh.FoxWolfMomR(3));
		n->Column("fw4", evsh.FoxWolfMomR(4));
		n->Column("fw5", evsh.FoxWolfMomR(5));
		
		n->DumpData();
	}
}

void toy_core(TString fsig, int nev=2, double sqrts=3.77, bool simcut=true, bool evcut=false, double dp=0.05, 
			double trkeff=95., double pideff=95., double misid =5., double P_mix=0.00, bool writentp=false, int mode =9999)
{
	setStyle();
	
	globmode= mode;
	
	fRand.SetSeed(314);
	
	init(pideff, misid);
	
	int i,j,k;
	
	makeIni4Vector(fIni, sqrts);
	
	fIni.Print();
	
	fPi0win = nsigpiks*fPi0sig;
	fKswin  = nsigpiks*fKssig;
	
	fPhiwin  = nsig*fPhisig;	
	fDwin    = nsig*fDsig;
	fD0win   = nsig*fD0sig;
	fDswin   = nsig*fDssig;
	fJwin    = nsig*fJsig;	
	fEtacwin = nsig*fEtacsig;	
	feewin   = nsig*(feesigconst+sqrts*feesigslope);	
	fLamcwin = nsig*fLamcsig;	
	fLamwin  = nsig*fLamsig;	
	
	//gStyle->SetOptStat(1111);
	
	// *** open file and get tree
	TFile *f=new TFile(fsig,"READ");
	if (f->IsZombie()) 
	{
		cout <<"File not found:"<<fsig<<endl;
		return;
	}
	
	bool dpm=fsig.Contains("DPM"); // *** DPM events
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
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,600,300);
	c1->Divide(2,1);
	TCanvas *c2=new TCanvas("c2","c2",20,20,1800,800);
	c2->Divide(5,2,0.005,0.01);
	
/*	TCanvas *c3=new TCanvas("c3","c3",1010,10,600,600);
	c3->Divide(2,2);*/
	
//	TCanvas *c4=new TCanvas("c4","c4",1220,20,500,500);
	
	double min=0, max=6.0, min2=0.5;
	
	// *** create some histograms
	TH1F *h_mom= new TH1F("h_mom","Momenta",200,0,5);
	TH1F *h_momc= new TH1F("h_momc","Momenta charged",200,0,5);
	TH1F *h_momn= new TH1F("h_momn","Momenta neutral",200,0,5);
	TH1F *h_mult= new TH1F("h_mult","Multiplicity",50,-0.5,49.5);
	TH1F *h_multrec= new TH1F("h_multrec","Multiplicity reco",50,-0.5,49.5);
	TH1F *h_multc= new TH1F("h_multc","Multiplicity charged",30,-0.5,29.5);
	TH1F *h_multn= new TH1F("h_multn","Multiplicity neutral",50,-0.5,49.5);
	TH1F *h_fw1= new TH1F("h_fw1","Fox Wolfram R1",300,-1.1,1.1);
	TH1F *h_pmax= new TH1F("h_pmax","pmax cms",300,0,5);
	
	TH1F *h_mult_k = new TH1F("h_mult_k","Multiplicity Kaons",10,-0.5,9.5);
	TH1F *h_mult_lep = new TH1F("h_mult_lep","Multiplicity Leptons",10,-0.5,9.5);
	TH1F *h_sum_klep = new TH1F("h_sum_lep","Sum Kaons + Leptons",15,-0.5,14.5);
	TH2F *h_mult_klep=new TH2F("h_mult_klep","#Kaons vs. #Leptons",10,-0.5,9.5, 10, -0.5, 9.5);
	h_mult_klep->SetYTitle("#Leptons");
	h_mult_klep->SetXTitle("#Kaons");
	
	// ****** pi0
	TH1F *h_pi0=new TH1F("h_pi0","#pi^{0} #rightarrow #gamma#gamma",200,min,0.4);
	TH1F *h_pi0sel=new TH1F("h_pi0sel","Pi0",200,min,0.4);
	TH1F *h_pi0sig=new TH1F("h_pi0sig","Pi0",200,min,0.4);
	h_pi0sig->SetLineColor(2);
	configHisto(h_pi0sel);
	
	// ****** Ks
	TH1F *h_ks=new TH1F("h_ks","K_{S}^{0} #rightarrow #pi^{+}#pi^{-}",200,0.2,0.8);
	TH1F *h_kssel=new TH1F("h_kssel","Pi0",200,0.2,0.8);
	TH1F *h_kssig=new TH1F("h_kssig","Pi0",200,0.2,0.8);
	h_kssig->SetLineColor(2);
	configHisto(h_kssel);
	
	// ****** Jpsi -> ee
	TH1F *h_jpsi=new TH1F("h_jpsi","J/#psi #rightarrow e^{+}e^{-}",nbins,min,max);
	TH1F *h_jpsisel=new TH1F("h_jpsisel","J/psi",nbins,min,max);
	TH1F *h_jpsisig=new TH1F("h_jpsisig","J/psi",nbins,min,max);
	h_jpsisig->SetLineColor(2);
	configHisto(h_jpsisel);
	
	// ****** Jpsi -> mu mu
	TH1F *h_jpsi2=new TH1F("h_jpsi2","J/#psi #rightarrow #mu^{+}#mu^{-}",nbins,min,max);
	TH1F *h_jpsi2sel=new TH1F("h_jpsi2sel","J/psi",nbins,min,max);
	TH1F *h_jpsi2sig=new TH1F("h_jpsi2sig","J/psi",nbins,min,max);
	h_jpsi2sig->SetLineColor(2);
	configHisto(h_jpsi2sel);
	
	// ****** eta_c -> Ks K pi
	TH1F *h_etac=new TH1F("h_etac","#eta_{c} #rightarrow K_{S} K^{+} #pi^{-}",nbins,min,max);
	TH1F *h_etacsel=new TH1F("h_etacsel","etac",nbins,min,max);
	TH1F *h_etacsig=new TH1F("h_etacsig","etac",nbins,min,max);
	h_etacsig->SetLineColor(2);
	configHisto(h_etacsel);
	
	// ****** D0 -> K- pi+
	TH1F *h_d0=new TH1F("h_d0","D^{0} #rightarrow K^{-}#pi^{+}",nbins,min,max);
	TH1F *h_d0sel=new TH1F("h_d0sel","D0",nbins,min,max);
	TH1F *h_d0sig=new TH1F("h_d0sig","D0",nbins,min,max);
	h_d0sig->SetLineColor(2);
	configHisto(h_d0sel);
		
	// ****** D+ -> K- pi+ pi+
	TH1F *h_dpm=new TH1F("h_dpm","D^{+} #rightarrow K^{-}#pi^{+}#pi^{+}",nbins,min,max);
	TH1F *h_dpmsel=new TH1F("h_dpmsel","D#pm",nbins,min,max);
	TH1F *h_dpmsig=new TH1F("h_dpmsig","D#pm",nbins,min,max);
	h_dpmsig->SetLineColor(2);
	configHisto(h_dpmsel);
		
	// ****** Ds+ -> K+ K- pi+
	TH1F *h_ds=new TH1F("h_ds","D_{s}^{+} #rightarrow K^{+}K^{-}#pi^{+}",nbins,min,max);
	TH1F *h_dssel=new TH1F("h_dssel","Ds",nbins,min,max);
	TH1F *h_dssig=new TH1F("h_dssig","Ds",nbins,min,max);
	h_dssig->SetLineColor(2);
	configHisto(h_dssel);
	
	// ****** phi -> K+ K-
	TH1F *h_phi=new TH1F("h_phi","#phi #rightarrow K^{+}K^{-}",nbins,min2,max/2);
	TH1F *h_phisel=new TH1F("h_phisel","Phi",nbins,min2,max/2);
	TH1F *h_phisig=new TH1F("h_phisig","Phi",nbins,min2,max/2);
	h_phisig->SetLineColor(2);
	configHisto(h_phisel);
	
	// ****** Lambda_c -> p K- pi+
	TH1F *h_lamc=new TH1F("h_lamc","#Lambda_{c} #rightarrow pK^{-}#pi^{+}",nbins,min,max);
	TH1F *h_lamcsel=new TH1F("h_lamcsel","Lam_c",nbins,min,max);
	TH1F *h_lamcsig=new TH1F("h_lamcsig","Lam_c",nbins,min,max);
	h_lamcsig->SetLineColor(2);
	configHisto(h_lamcsel);
	
	// ****** Lambda -> p pi-
	TH1F *h_lam=new TH1F("h_lam","#Lambda #rightarrow p#pi^{-}",nbins,min2,max/2);
	TH1F *h_lamsel=new TH1F("h_lamsel","Lam",nbins,min2,max/2);
	TH1F *h_lamsig=new TH1F("h_lamsig","Lam",nbins,min2,max/2);
	h_lamsig->SetLineColor(2);
	configHisto(h_lamsel);
	
	// ****** pp -> e+ e-
	TH1F *h_ee=new TH1F("h_ee","p#bar{p} #rightarrow e^{+}e^{-}",nbins,min,max*1.2);
	TH1F *h_eesel=new TH1F("h_eesel","ee",nbins,min,max*1.2);
	TH1F *h_eesig=new TH1F("h_eesig","ee",nbins,min,max*1.2);
	h_eesig->SetLineColor(2);
	configHisto(h_eesel);
	
	
	h_jpsi->SetXTitle("m(e^{+}e^{-}) [GeV/c^{2}]");
	h_jpsi2->SetXTitle("m(#mu^{+}#mu^{-}) [GeV/c^{2}]");
	h_etac->SetXTitle("m(K_{S} K^{+}#pi^{-}) [GeV/c^{2}]");

	h_d0->SetXTitle("m(K^{-}#pi^{+}) [GeV/c^{2}]");

	h_dpm->SetXTitle("m(K^{-}#pi^{+}#pi^{+}) [GeV/c^{2}]");

	h_ds->SetXTitle("m(K^{+}K^{-}#pi^{+}) [GeV/c^{2}]");

	h_phi->SetXTitle("m(K^{+}K^{-}) [GeV/c^{2}]");
	h_lamc->SetXTitle("m(pK^{-}#pi^{+}) [GeV/c^{2}]");
	h_lam->SetXTitle("m(p#pi^{-}) [GeV/c^{2}]");
	h_ee->SetXTitle("m(e^{+}e^{-}) [GeV/c^{2}]");
	
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
	
	TFile *fn=0;
	
	TString filename(fsig(fsig.Index("/")+1,fsig.Length())+"_ntp.root");
	
	if (writentp) fn=new TFile(filename, "RECREATE");
	
	SimpleNtp *nphi		= new SimpleNtp("nphi","phi -> K+K-");
	SimpleNtp *nd0		= new SimpleNtp("nd0","D0 -> K+ pi+");
	SimpleNtp *ndpm		= new SimpleNtp("ndpm","D+ -> K- pi+ pi+");
	SimpleNtp *nds		= new SimpleNtp("nds","Ds -> K+ K- pi+");
	SimpleNtp *njpsi1	= new SimpleNtp("njpsi1","J/psi -> e+ e-");
	SimpleNtp *njpsi2	= new SimpleNtp("njpsi2","J/psi -> mu+ mu-");
	SimpleNtp *netac	= new SimpleNtp("netac","eta_c -> KS (pi+pi-) K- pi+");
	SimpleNtp *nee		= new SimpleNtp("nee","ppbar -> e+ e-");
	SimpleNtp *nlam		= new SimpleNtp("nlam","Lambda -> p pi-");
	SimpleNtp *nlamc	= new SimpleNtp("nlamc","Lambda_c -> p K- pi+");		
		
	int combCnt=0;  // how many combinatione are within our mass criterion
	int evCnt=0;    // how many _events_ (not combinations!) are within our mass criterion
	int evMultCut=0;
	
	// *** the event loop
	
	int mixevts=1;
	int currmix=0;
	
	int cntInLam=0;
	
	// how many events are tagged by individual algo?
	bool channelselect[10];
	double channelcount[10];
	TString channelname[10]={"Phi(KK)","pp->ee" ,"Lam(ppi)","etac(KS K pi)","J/psi(ee)","J/psi(mumu)","D+-(K2pi)","D0(Kpi)","Ds(KKpi)","Lamc(pKpi)"};
	
	for (i=0;i<10;++i) channelcount[i]=0.;
	
	if (fRand.Rndm()<P_mix) mixevts=2;
	
	for (i=0;i<nev;++i)
	{	
		evtno = i;
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
			int npi = 0;
			int nstab = 0;
			
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
				if (pdg==211) npi++;
				if (isFinalState(pdg)) nstab++; 
			}
			//if (npi!=4 || nstab!=npi ) good = false; 
		}
		
		if (!good) continue;
		
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
		
		h_mult->Fill(mcFSlist.size());
		
		// *** if mass cut is negative (= number of sigmas), compute absolute window size
		//if (mwin<0) mwidth=-mwin*(dp*mwsig+mwoff);
		
		// *** prepare the toy reco: smear momentum and apply track efficiency (both flat at the moment)
		makeRecoCands(mcFSlist, recolist, trkeff/100., dp );
		
		// *** the boost of the system
		TVector3 boost = fIni.BoostVector();
		boostList(recolist, recocmslist, boost);
		
		// *** Make Event Selection
		EventShape evsh(recolist, fIni);
/*		if (evcut)
		{
			if (evshape.ChrgPtSumLab()<1.5) continue;
		}*/
		
		h_multrec->Fill(recolist.size());
		//cout <<mcFSlist.size()<<"  "<<recolist.size()<<endl;
		
		// *** prepare all pid list according to the pidtable probabilities
		makePidSelection(recolist);
		
		npi = piplus.size()+piminus.size();
		nk = kplus.size()+kminus.size();
		npr = pplus.size()+pminus.size();
		ne = eplus.size()+eminus.size();
		nmu = muplus.size()+muminus.size();
		nlep = ne+nmu;
		
/*		int nk10 = 0;  // # kaons with p>1.0
		for (j=0;j<kplus.size();++j) if (kplus[j].P4().P()>1.0) nk10++;
		for (j=0;j<kminus.size();++j) if (kminus[j].P4().P()>1.0) nk10++;
		
		int npr10 = 0;  // # protons with p>1.0
		for (j=0;j<pplus.size();++j) if (pplus[j].P4().P()>1.0) npr10++;
		for (j=0;j<pminus.size();++j) if (pminus[j].P4().P()>1.0) npr10++;*/
		
		h_mult_k->Fill(nk);
		h_mult_lep->Fill(nlep);
		h_mult_klep->Fill(nk, nlep);
		h_sum_klep->Fill(nk + nlep);
		
					   				
		// *** the lists we need for combinatorics
		CandList comb1, comb2, comb3;

		CandList LPhi, LPi0, LKs;				// phi -> KK , pi0 -> gg (mass cut), KS -> pi+ pi- (mass cut)
		CandList LJpsiE, LJpsiMu;       		// J/psi -> ee/mu mu
		CandList LEtac, LEtac2;       		// etac
		CandList LDp, LDm, LDp2, LDm2, LDp3, LDm3, LDp4, LDm4;	// D+- -> K2pi, K2pipi0, KS pi pi0, KS pi pi pi
		CandList LDsp, LDsm, LDsp2, LDsm2;		// Ds -> KKpi, KKpipi0				
		CandList LD0, LD02, LD0b, LD02b, LD03, LD03b;	// D0 -> Kpi, Kpipi0, K3pi
		CandList LLamcp, LLamcm;				// Lc -> pKpi
		CandList LLam, LLamb;					// Lam -> ppi
		CandList Lee;					// Lam -> ppi
		
		int ncomb1=0, ncomb2=0;
		bool selected=false;
		
		for (j=0;j<13;++j) channelselect[j]=false;
				
		// Apply cut on kaon/lepton multiplicity
/*		if ( multcut>0 && (N_K+N_lep<multcut) )
		{
			mixevts=1;
			if (fRand.Rndm()<P_mix) mixevts=2;
			continue;
		}*/
		
		evMultCut++;

		bool accepted = false;
		
		//
		//  ********* pi0 / KS
		//
		// *** do combinatorics (pi0 -> gg) 
		combine(gam, gam, comb1, fPi0Pdg);
		fillHisto(comb1, h_pi0, 0, fPi0Mass, fPi0win, h_pi0sel, 0, h_pi0sig ); 
		selectMass(comb1, LPi0, fPi0Mass, fPi0win);
		
		// *** do combinatorics (pi0 -> gg) 
		combine(piplus, piminus, comb1, fKsPdg);
		fillHisto(comb1, h_ks, 0, fKsMass, fKswin, h_kssel, 0, h_kssig ); 
		selectMass(comb1, LKs, fKsMass, fKswin);
		

		// **** cache some eventshape vars
		int npart      = evsh.NParticles();
		int nchrg      = evsh.NCharged();
		int nneut      = evsh.NNeutral();
		int npi0       = LPi0.size();					// #pi0 candidates
		int nks0       = LKs.size();					// #KS0 candidates
		int npi005     = 0.0;
		int	npi010     = 0.0;
		int nks005     = 0.0;
		int nks010     = 0.0;
		int np05       = evsh.MultPminCms(0.5);
		int np10       = evsh.MultPminCms(1.0);

		double pmaxl   = evsh.PmaxLab();
		double pmax    = evsh.PmaxCms();
		double pmin    = evsh.PminCms();
		double ptmax   = evsh.Ptmax();
		
		double sumpc   = evsh.ChrgPSumCms();
		double sumpc05 = evsh.SumChrgPminCms(0.5);
		double sumpt   = evsh.PtSumLab();
		double sumptc  = evsh.ChrgPtSumLab();
		double sumptl  = sumpt;
		double sumen   = evsh.NeutESumCms();
		
		double fw1     = evsh.FoxWolfMomR(1);
		double fw2     = evsh.FoxWolfMomR(2);
		double fw3     = evsh.FoxWolfMomR(3);
		double fw4     = evsh.FoxWolfMomR(4);
		double fw5     = evsh.FoxWolfMomR(5);
		
		// count pi0 with threshold
		for (j=0; j<LPi0.size(); ++j) 
		{
			if (LPi0[j].P4().P()>0.5) npi005++;
			if (LPi0[j].P4().P()>1.0) npi010++;
		}
		// count KS with threshold
		for (j=0; j<LKs.size(); ++j) 
		{
			if (LKs[j].P4().P()>0.5) nks005++;
			if (LKs[j].P4().P()>1.0) nks010++;
		}
		
		h_fw1->Fill(fw2);
		h_pmax->Fill(pmax);

		
		// **** find out, whether event was accepted
		
		// ********* PHI -> K+ K-
		if ( !evcut 
		     || (sqrts==2.4  &&   nk>1 && npart<7 && pmax<0.648   )
			 || (sqrts==3.77 &&   nk>1 && fw2>0.5456 )
			 || (sqrts==4.28 &&   nk>1 && 1 )
			 || (sqrts==5.0  &&   nk>1 && 1 )
			 || (sqrts==5.5  &&   nk>1 && 1 )
			)
		{
		  // *** do combinatorics (phi -> K K) 
		  combine (kplus, kminus, LPhi, fPhiPdg); 
		  
		  accepted |= eventAccepted(LPhi,fPhiMass, fPhiwin);
		  
		  writeTuple("phi",LPhi,evsh, nphi,fPhiMass, fPhiwin);
		}


		// ********* LAMBDA -> p pi-
		if ( !evcut 
		     || (sqrts==2.4  &&   npart>3 && npr>0 && npi>0 && fw1>0.132 && fw4>0.2024 )
			 || (sqrts==3.77 &&   npart>3 && pmax>1.056 &&     fw1>0.0   && fw2>0.6248 && fw5>0.22  )
			 || (sqrts==4.28 &&   npart>3 && 1 )
			 || (sqrts==5.0  &&   npart>3 && 1 )
			 || (sqrts==5.5  &&   npart>3 && 1 )
			)
		{
		  // *** do combinatorics (Lam -> p pi) 
		  combine (pplus, piminus, LLam, fLamPdg); 
		  combine (pminus, piplus, LLamb, -fLamPdg); 		
		  
		  accepted |= eventAccepted(LLam, fLamMass, fLamwin);
		  accepted |= eventAccepted(LLamb, fLamMass, fLamwin);
		  
		  writeTuple("lam",LLam,evsh, nlam, fLamMass, fLamwin);
		  writeTuple("lam",LLamb,evsh, nlam, fLamMass, fLamwin);
		}

		// ********* pp -> e+ e-
		if ( !evcut 
			)
		{
		  // *** do combinatorics (e+ e-) 
		  combine(eminus, eplus, Lee, 88880);
		  
		  accepted |= eventAccepted(Lee,  fIni.M(), feewin);
		  
		  writeTuple("ee",Lee,evsh, nee,  fIni.M(), feewin);
		}

		// ********* ETAC
		if ( !evcut 
/*			 || (sqrts==3.77 &&  np10>1 && npi<4 && pmin<0.3 && sumpc>3.296 )
			 || (sqrts==4.28 &&  1 )
			 || (sqrts==5.0  &&  1 )
			 || (sqrts==5.5  &&  1 )*/
			)
		{
		  // *** do combinatorics (ETAC) 
		  combine(LKs, kplus, piminus, LEtac, fEtacPdg);
		  combine(LKs, kminus, piplus, LEtac2, fEtacPdg);
		  
		  accepted |= eventAccepted(LEtac,  fEtacMass, fEtacwin);
		  accepted |= eventAccepted(LEtac2,  fEtacMass, fEtacwin);
		  
		  writeTuple("etac",LEtac,evsh, netac,  fEtacMass, fEtacwin);
		  writeTuple("etac",LEtac2,evsh, netac,  fEtacMass, fEtacwin);
		}

		// ********* JPSI -> e+e-
		if ( !evcut 
			 || (sqrts==3.77 &&  np10>1 && npi<4 && pmin<0.3 && sumpc>3.296 )
			 || (sqrts==4.28 &&  1 )
			 || (sqrts==5.0  &&  1 )
			 || (sqrts==5.5  &&  1 )
			)
		{
		  // *** do combinatorics (J/psi -> ee) 
		  combine(eminus, eplus, LJpsiE, fJpsiPdg);
		  
		  accepted |= eventAccepted(LJpsiE,  fJMass, fJwin);

		  writeTuple("jpsi1",LJpsiE,evsh, njpsi1,  fJMass, fJwin);
		}
		
		// ********* JPSI -> mu+mu-
		if ( !evcut 
			 || (sqrts==3.77 &&  np10>1 && npi<4 && pmin<0.3 && sumpc>3.296 )
			 || (sqrts==4.28 &&  1 )
			 || (sqrts==5.0  &&  1 )
			 || (sqrts==5.5  &&  1 )
			)
		{
		  // *** do combinatorics (J/psi -> mu mu) 
		  combine(muminus, muplus, LJpsiMu, fJpsiPdg);
		  
		  accepted |= eventAccepted(LJpsiMu,  fJMass, fJwin);

		  writeTuple("jpsi2",LJpsiMu,evsh, njpsi2,  fJMass, fJwin);
		}
		
		
		// ********* D+ -> K- pi+ pi+
		if ( !evcut 
			 || (sqrts==3.77 &&   nk>0 && npart<12 && pmax<0.936 && sumpc>1.76 )
			 || (sqrts==4.28 &&   nk>0 && 1 )
			 || (sqrts==5.0  &&   nk>0 && 1 )
			 || (sqrts==5.5  &&   nk>0 && 1 )
			)
		{
		  // *** do combinatorics (D+- -> K pi pi) 
		  combine (piplus,  piplus,  kminus, LDp, fDPdg); 
		  combine (piminus, piminus, kplus,  LDm, -fDPdg); 
		  
		  accepted |= eventAccepted(LDp,fDMass, fDwin);
		  accepted |= eventAccepted(LDm,fDMass, fDwin);
		  
		  writeTuple("d",LDp,evsh, ndpm,fDMass, fDwin);
		  writeTuple("d",LDm,evsh, ndpm,fDMass, fDwin);
		}
		
		
		// ********* D0 -> K- pi+
		if ( !evcut 
			 || (sqrts==3.77 &&   nk>0 && sumpc05>3.18 && pmax>0.768 && pmax<1.056 && fw3<0.1848 )
			 || (sqrts==4.28 &&   nk>0 && 1 )
			 || (sqrts==5.0  &&   nk>0 && 1 )
			 || (sqrts==5.5  &&   nk>0 && 1 )
			)
		{
		  // *** do combinatorics (D0 -> K pi) 
		  combine (kplus, piminus, LD0, -fD0Pdg); 
		  combine (kminus, piplus, LD0b, fD0Pdg); 

		  accepted |= eventAccepted(LD0, fD0Mass, fD0win);
		  accepted |= eventAccepted(LD0b, fD0Mass, fD0win);
		  
		  writeTuple("d0",LD0,evsh, nd0, fD0Mass, fD0win);
		  writeTuple("d0",LD0b,evsh, nd0, fD0Mass, fD0win);
		}	


		// ********* DS -> K+ K- pi+
		if ( !evcut 
			 || (sqrts==4.28 &&   nk>1 && 1 )
			 || (sqrts==5.0  &&   nk>1 && 1 )
			 || (sqrts==5.5  &&   nk>1 && 1 )
			)
		{
		  // *** do combinatorics (Ds+- -> K K pi) 
		  combine (kplus, kminus, piplus, LDsp, fDsPdg); 
		  combine (kplus, kminus, piminus, LDsm, -fDsPdg); 

		  accepted |= eventAccepted(LDsp, fDsMass, fDswin);
		  accepted |= eventAccepted(LDsm, fDsMass, fDswin);
		  
		  writeTuple("ds",LDsp,evsh, nds, fDsMass, fDswin);
		  writeTuple("ds",LDsm,evsh, nds, fDsMass, fDswin);
		}


		// ********* LAMBDA_C -> p K- pi+
		if ( !evcut 
			 || (sqrts==5.0  &&  1 )
			 || (sqrts==5.5  &&  1 )
			)
		{
		  // *** do combinatorics (Lamc) 
		  combine (pplus, kminus, piplus, LLamcp, fLamcPdg); 
		  combine (pminus, kplus, piminus, LLamcm, -fLamcPdg); 		
		
		  accepted |= eventAccepted(LLamcp, fLamcMass, fLamcwin);
		  accepted |= eventAccepted(LLamcm, fLamcMass, fLamcwin);
		  
		  writeTuple("lamc",LLamcp,evsh, nlamc, fLamcMass, fLamcwin);
		  writeTuple("lamc",LLamcm,evsh, nlamc, fLamcMass, fLamcwin);
		}

		
		// ********* NOW do counting and drawing, ONLY for events which are accepted by any selector			
	    // {"Phi(KK)", "ee", "Lam(ppi)", "etac", "J/psi(ee)","J/psi(mumu)","D+-(K2pi)", "D0(Kpi)","Ds(KKpi)","Lamc(pKpi)"};
        //     0        1        2         3         4             5           6           7           8          9        
		if (!simcut || accepted)
		{
			// **** Phi
			if (sqrts>2.2)
			{
			  ncomb1 = fillHisto(LPhi, h_phi, 0, fPhiMass, fPhiwin, h_phisel, 0, h_phisig  ); 
			  combCnt += ncomb1;
			  if (ncomb1>0) {selected=true; channelselect[0]=true;}
			}
			
			// **** ee
			if (sqrts>2.2)
			{
			  ncomb1 = fillHisto(Lee, h_ee, 0, fIni.M(), feewin, h_eesel, 0, h_eesig  ); 
			  combCnt += ncomb1;
			  if (ncomb1>0) {selected=true; channelselect[1]=true;}
			}

			// **** Lam
			if (sqrts>2.2)
			{
			  ncomb1 = fillHisto(LLam, h_lam, 0, fLamMass, fLamwin, h_lamsel, 0, h_lamsig ); 
			  ncomb2 = fillHisto(LLamb, h_lam, 0, fLamMass, fLamwin, h_lamsel, 0, h_lamsig ); 
			  combCnt += ncomb1 + ncomb2;
			  if (ncomb1>0 || ncomb2>0) {selected=true; channelselect[2]=true;}
			}
			
			// **** etac
			if (sqrts>3.1)
			{
			  ncomb1 = fillHisto(LEtac, h_etac, 0, fEtacMass, fEtacwin, h_etacsel, 0, h_etacsig ); 
			  ncomb2 = fillHisto(LEtac2, h_etac, 0, fEtacMass, fEtacwin, h_etacsel, 0, h_etacsig ); 
			  combCnt += ncomb1 + ncomb2;
			  if (ncomb1>0 || ncomb2>0 ) {selected=true; channelselect[3]=true;}
			}
			
			// **** J/psi ee
			if (sqrts>3.1)
			{
			  ncomb1 = fillHisto(LJpsiE, h_jpsi, 0, fJMass, fJwin, h_jpsisel, 0, h_jpsisig ); 
			  combCnt += ncomb1;
			  if (ncomb1>0 ) {selected=true; channelselect[4]=true;}
			}
			
			// **** J/psi mumu
			if (sqrts>3.1)
			{
			  ncomb1 = fillHisto(LJpsiMu, h_jpsi2, 0, fJMass, fJwin, h_jpsi2sel, 0, h_jpsi2sig ); 
			  combCnt += ncomb1 ;
			  if (ncomb1>0 ) {selected=true; channelselect[5]=true;}
			}
			
			// **** Dp
			if (sqrts>3.75)
			{
			  ncomb1 = fillHisto(LDp, h_dpm, 0, fDMass, fDwin, h_dpmsel, 0, h_dpmsig ); 
			  ncomb2 = fillHisto(LDm, h_dpm, 0, fDMass, fDwin, h_dpmsel, 0, h_dpmsig ); 	
			  combCnt += ncomb1 +ncomb2;
			  if (ncomb1>0 || ncomb2>0) {selected=true; channelselect[6]=true;}		  
			}

			// **** D0 
			if (sqrts>3.75)
			{
			  ncomb1 = fillHisto(LD0, h_d0, 0, fD0Mass, fD0win, h_d0sel, 0, h_d0sig ); 
			  ncomb2 = fillHisto(LD0b, h_d0, 0, fD0Mass, fD0win, h_d0sel, 0, h_d0sig ); 	
			  combCnt += ncomb1 +ncomb2;
			  if (ncomb1>0 || ncomb2>0) {selected=true; channelselect[7]=true;}	  
			}

			// **** Ds
			if (sqrts>4.)
			{
			  ncomb1 = fillHisto(LDsp, h_ds, 0, fDsMass, fDswin, h_dssel, 0, h_dssig ); 
			  ncomb2 = fillHisto(LDsm, h_ds, 0, fDsMass, fDswin, h_dssel, 0, h_dssig ); 
			  combCnt += ncomb1 +ncomb2;
			  if (ncomb1>0 || ncomb2>0) {selected=true; channelselect[8]=true;}
			}
		
			// **** Lamc
			if (sqrts>4.57)
			{
			  ncomb1 = fillHisto(LLamcp, h_lamc, 0, fLamcMass, fLamcwin, h_lamcsel, 0, h_lamcsig ); 
			  ncomb2 = fillHisto(LLamcm, h_lamc, 0, fLamcMass, fLamcwin, h_lamcsel, 0, h_lamcsig ); 
			  combCnt += ncomb1 + ncomb2;
			  if (ncomb1>0 || ncomb2>0) {selected=true; channelselect[9]=true;}
			}
		}
	    // ****************
	    
		if (selected) evCnt+=mixevts; // *** if any combination is in window count event
		
		for (j=0;j<10;++j) if (channelselect[j]) channelcount[j]+=mixevts;
		
		//hmix->Fill(mixevts);
		// new round of mixing
		mixevts=1;
		if (fRand.Rndm()<P_mix) mixevts=2;
		
		//if (i<3) printAllLists();
	}
	//cout <<"cntInLam "<<cntInLam<<endl;
	cout<< "#Combinations:"<<combCnt<<"  #Events:"<<evCnt<<"  ";
	cout <<"Efficiency   (ev):"<<(double)evCnt/(double)nev<<endl;
	cout <<"Mult Cut Eff (ev):"<<(double)evMultCut/(double)nev<<endl;
	
	for (i=0;i<10;++i) 
	{
		channelcount[i]/=nev;
		cout <<channelname[i]<<":"<<channelcount[i]<<endl;
	}
	cout <<endl;
	
	TLatex latex;
	latex.SetTextSize(0.08);
	char tmp[200];
	double offset = 0.1;
	double pos=3.3, pos2=1.85;
	double hfac = 0.75;
	// *** plot all stuff and write out info
	c1->cd(1); h_pi0->Draw(); h_pi0sel->Draw("same"); h_pi0sig->Draw("same");
	c1->cd(2); h_ks->Draw(); h_kssel->Draw("same"); h_kssig->Draw("same");

	c2->cd(1); h_phi->Draw(); h_phisel->Draw("same");   h_phisig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",channelcount[0]*100.);
	latex.DrawLatex(pos2,h_phi->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(2); h_ee->Draw(); h_eesel->Draw("same"); h_eesig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",channelcount[1]*100.);
	latex.DrawLatex(pos*1.2,h_ee->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(3); h_lam->Draw(); h_lamsel->Draw("same"); h_lamsig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",channelcount[2]*100.);
	latex.DrawLatex(pos2,h_lam->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(4); h_etac->Draw(); h_etacsel->Draw("same"); h_etacsig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",channelcount[3]*100.);
	latex.DrawLatex(pos,h_etac->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(5); h_jpsi->Draw(); h_jpsisel->Draw("same"); h_jpsisig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",channelcount[4]*100.);
	latex.DrawLatex(pos,h_jpsi->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(6); h_jpsi2->Draw(); h_jpsi2sel->Draw("same"); h_jpsi2sig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",channelcount[5]*100.);
	latex.DrawLatex(pos,h_jpsi2->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(7); h_dpm->Draw();  h_dpmsel->Draw("same");  h_dpmsig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",channelcount[6]*100.);
	latex.DrawLatex(pos,h_dpm->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(8); h_d0->Draw();   h_d0sel->Draw("same");   h_d0sig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",channelcount[7]*100.);
	latex.DrawLatex(pos,h_d0->GetMaximum()*hfac+offset,tmp);

	c2->cd(9); h_ds->Draw();   h_dssel->Draw("same");   h_dssig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",channelcount[8]*100.);
	latex.DrawLatex(pos,h_ds->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(10); h_lamc->Draw(); h_lamcsel->Draw("same"); h_lamcsig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",channelcount[9]*100.);
	latex.DrawLatex(pos,h_lamc->GetMaximum()*hfac+offset,tmp);
	
	// total efficiency in first plot (phi)
	c2->cd(5);
	double posy = h_jpsi->GetMaximum()*1.12;
	if (posy=0) posy=1.12;
	sprintf(tmp,"(#epsilon_{t} = %4.1f%%)",double(evCnt)/double(nev)*100.);
	latex.SetTextColor(kMagenta+2);
	latex.DrawLatex(3.0,h_jpsi->GetMaximum()*1.12,tmp);

	
	//sprintf(tmp,"#epsilon = %4.1f%%",channelcount[6]*100.);
	//latex.DrawLatex(2.5,h_pi0->GetMaximum()*0.95,tmp);
	
/*	c2->cd();
	TPad *peff=new TPad("peff","peff",0.45,0.47,0.55,0.53);
	peff->Draw();
	peff->cd();
	sprintf(tmp,"#epsilon_{tot} = %4.1f%%",double(evCnt)/double(nev)*100.);
	latex.SetTextSize(0.5);
	latex.SetTextColor(2);
	latex.DrawLatex(0.1,0.3,tmp);
	*/
	c2->cd();
	
	h_mult_klep->Scale(1.0/nev);
	
	TString plotname(fsig(fsig.Index("/")+1,fsig.Length()-5));
	
	c2->SaveAs("fig/"+plotname+"_core.gif");
	c2->SaveAs("fig/"+plotname+"_core.pdf");
	
	if (writentp) 
	{
		fn->Write();
		fn->Close();
	}
	
}