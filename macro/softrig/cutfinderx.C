#include <algorithm>
#include <map>
#include <utility>
#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TString.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TEventList.h"
#include "TDirectory.h"
#include "TLine.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TObjArray.h"
#include "TPRegexp.h"
#include "TRegexp.h"
#include "TGraph.h"

#define MAX 1000
#define BINS 100
#define NCAN 3

typedef std::vector<pair<double, int> > ValueMap;
typedef std::map<int, int> CountMap;

CountMap evcnt, evcntrec[10];
std::map<TString, TString> mctvar;

double signi[MAX];
int idx[MAX];
TString vars[MAX];
TString cuts[MAX];
double cut[MAX];
TGraph grl[MAX];
TGraph grr[MAX];

double minh[MAX], maxh[MAX];

double N0_sig, N0_bg;
double Nsigev, Nbgev;
bool dstarmode;

Float_t fbranch[MAX];
Int_t   ibranch[MAX];
Bool_t  bbranch[MAX];
Int_t ev, run, mode, rec, nbranch;

// ---------------------------------------------------------------

int gettype(TTree *t, TString varname)
{
	TString leaftype = t->GetLeaf(varname)->GetTypeName();
	
	if (leaftype=="Float_t")	 return 0; 
	else if (leaftype=="Int_t")	 return 1;  
	else if (leaftype=="Bool_t") return 2;
	
	return -1;
}

// ---------------------------------------------------------------

int init(TTree *t)
{
	t->SetBranchAddress("ev",&ev);
	t->SetBranchAddress("run",&run);
	t->SetBranchAddress("mode",&mode);
	t->SetBranchAddress("recmode",&rec);
	
	TObjArray* branches = t->GetListOfBranches();	
	
	nbranch = 0;
	
	for(int i=0; i<=branches->GetLast(); ++i)
	{
		TBranch* branch = (TBranch*)branches->UncheckedAt(i);
		TString v=branch->GetName(); 

		if ( v=="ev" || v=="mode" || v=="run" || v=="nsig" ) continue;
		if ( v.Contains("pdg") || v.Contains("beam") || v.Contains("mct") ) continue;
		if ( v.BeginsWith("t") && v!="thr") continue;
		if ( v.EndsWith("vx") || v.EndsWith("vy") || v.EndsWith("vz") || v.EndsWith("pocmag")) continue;

		bool ok=false;
		
		if ( v.BeginsWith("es") || v=="mmiss" || v.EndsWith("d0m") ) ok=true;
		if ( v.EndsWith("p") || v.EndsWith("tht") || v.EndsWith("pcm") || v.EndsWith("thtcm") || v.EndsWith("pt") ) ok=true;
		if ( ok || v.EndsWith("ang") || v.Contains("poc") ) ok=true;
		if ( ok || v.Contains("pid") || v.Contains("min") || v.Contains("max") || v.Contains("sum") || v.Contains("fw") ) ok=true;
		if ( ok || v.EndsWith("sph") || v.EndsWith("apl") || v.EndsWith("pla") || v.EndsWith("thr") || v.EndsWith("cir") ) ok=true;
		
		if ( v=="xmdif" && dstarmode ) ok = true;
		if ( v=="xd0m" && dstarmode ) ok = false;
		
		if (!ok) continue;
		
		Int_t dtype=gettype(t, v);
		switch (dtype)
		{
			case 0: t->SetBranchAddress(v, &(fbranch[nbranch])); break;
			case 1: t->SetBranchAddress(v, &(ibranch[nbranch])); break;
			case 2: t->SetBranchAddress(v, &(bbranch[nbranch])); break;
		}
		
		vars[nbranch] = v;
		nbranch++;
	}
	
	return nbranch;
}

// ---------------------------------------------------------------

bool mycompare(int i, int j)
{
  return signi[i]>signi[j];
}

// ---------------------------------------------------------------

int uid(int ev, int run, int mode)
{
	return ev+10000*run+(((mode/100)%10)*20+mode%10)*100000;
}

// ---------------------------------------------------------------

int countEvents(TTree *t, TEventList &el)
{
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("ev",1);
	t->SetBranchStatus("run",1);
	t->SetBranchStatus("mode",1);
	t->SetBranchStatus("recmode",1);
		
	evcnt.clear();
	for (int j=0;j<10;++j) evcntrec[j].clear();
	
	for (int i=0;i<el.GetN();++i)
	{
		t->GetEntry(el.GetEntry(i));
		evcnt[uid(ev,run,mode)]+=1;
		if (rec<10) evcntrec[rec][uid(ev,run,mode)]+=1;
	}
	t->SetBranchStatus("*",1);

	return evcnt.size();
}

// ---------------------------------------------------------------

void makeMaps(TTree *t, TString varname, TEventList &els, TEventList &elb, ValueMap &sig, ValueMap &bg, int id)
{
	int i;
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("ev",1);
	t->SetBranchStatus("mode",1);
	t->SetBranchStatus("run",1);
	t->SetBranchStatus(varname,1);
	
	Float_t var;
	Int_t dtype=gettype(t, varname);
	
	sig.clear();
	bg.clear();
	double min = 1e9, max=-1e9;

	// prepare the pairs of variable value, eventnumber for signal and count signals
	for (i=0;i<els.GetN();++i)
	{
		t->GetEntry(els.GetEntry(i));
		switch (dtype) {case 2: var = (Float_t)bbranch[id]; break; case 1: var = (Float_t) ibranch[id]; break; default: var=fbranch[id]; }
		
		sig.push_back(std::make_pair(var, uid(ev,run,mode)  ));
		if (var>max) max = var;
		if (var<min) min = var;
	}
		
	// prepare the pairs of variable value, eventnumber for background and count background
	for (i=0;i<elb.GetN();++i)
	{
		t->GetEntry(elb.GetEntry(i));
		switch (dtype) {case 2: var = (Float_t)bbranch[id]; break; case 1: var = (Float_t) ibranch[id]; break; default: var=fbranch[id]; }
		
		bg.push_back(std::make_pair(var, uid(ev,run,mode) ));
		if (var>max) max = var;
		if (var<min) min = var;
	}
	
	if (max>25) max=25;
	minh[id]=min-(max-min)*0.05; 
	maxh[id]=max+(max-min)*0.05;
}

// ---------------------------------------------------------------

double bestEffEvt(TTree *t, TString varname, TEventList &els, TEventList &elb, double &bestcut, double supr, int id)
{	
	CountMap bgcnt, sigcnt, sigcnt2;	
	ValueMap sigvals, bgvals;
	
	int Nbgval = elb.GetN();
	int dtype = gettype(t,varname);
	
	makeMaps(t, varname, els, elb, sigvals, bgvals, id);
	
	
	// sort signals by first element = variable value
	sort(bgvals.begin(), bgvals.end());

	int i=0;
	bgcnt.clear();
	// find cut for current efficiency requirement left cut
	while ( (bgcnt.size()/(double)Nbgev)<(1-supr) && i<Nbgval ) bgcnt[bgvals[i++].second]+=1;
	double leftcut = bgvals[i].first;
	
	bgcnt.clear();
	i=bgvals.size()-1;
	// find cut for current efficiency requirement right cut
	while ( (bgcnt.size()/(double)Nbgev)<(1-supr) && i>=0) bgcnt[bgvals[i--].second]+=1; 
	double rightcut = bgvals[i].first;
	
	sigcnt.clear();
	sigcnt2.clear();
	
	for (i=0;i<sigvals.size();++i)
	{
		if (sigvals[i].first<leftcut)  sigcnt[sigvals[i].second]+=1;
		if (sigvals[i].first>rightcut) sigcnt2[sigvals[i].second]+=1;
	}
	
	t->SetBranchStatus("*",1);
		
	double lefteff  = sigcnt.size()/Nsigev;
	double righteff = sigcnt2.size()/Nsigev;

	cout <<varname<<" "<<flush;
	
	bestcut = rightcut;
	if (lefteff>righteff) 
	{
		if (dtype==0) cuts[id] = TString::Format("%s<%.3f",varname.Data(),leftcut);
		else cuts[id] = TString::Format("%s<=%.0f",varname.Data(),leftcut);

		bestcut = leftcut;
		return lefteff;
	}

	if (dtype==0) cuts[id] = TString::Format("%s>%.3f",varname.Data(),rightcut);
	else cuts[id] = TString::Format("%s>=%.0f",varname.Data(),rightcut);

	return righteff;
}

// ---------------------------------------------------------------

double bestSuppressionEvt(TTree *t, TString varname, TEventList &els, TEventList &elb, double &bestcut, double eff, int id)
{	
	CountMap bgcnt, bgcnt2, sigcnt;	
	ValueMap sigvals, bgvals;
	
	int Nsigval = els.GetN();
	int dtype = gettype(t,varname);
	
	makeMaps(t, varname, els, elb, sigvals, bgvals, id);
	
	// sort signals by first element = variable value
	sort(sigvals.begin(), sigvals.end());

	int i=0;
	sigcnt.clear();
	// find cut for current efficiency requirement left cut
	while ( (sigcnt.size()/(double)Nsigev)<eff && i<Nsigval ) sigcnt[sigvals[i++].second]+=1;
	double leftcut = sigvals[i].first;
	
	sigcnt.clear();
	i=sigvals.size()-1;
	// find cut for current efficiency requirement right cut
	while ( (sigcnt.size()/(double)Nsigev)<eff && i>=0) sigcnt[sigvals[i--].second]+=1; 
	double rightcut = sigvals[i].first;
	
	bgcnt.clear();
	bgcnt2.clear();
	
	for (i=0;i<bgvals.size();++i)
	{
		if (bgvals[i].first<=leftcut)  bgcnt[bgvals[i].second];
		if (bgvals[i].first>=rightcut) bgcnt2[bgvals[i].second];
	}
	
	t->SetBranchStatus("*",1);
	
	
	double leftsupr = (Nbgev-bgcnt.size())/Nbgev;
	double rightsupr = (Nbgev-bgcnt2.size())/Nbgev;

	cout <<varname<<" "<<flush;
	
	if (leftcut!=leftcut || rightcut!=rightcut) return 0;
	
	bestcut = rightcut;
	if (leftsupr>rightsupr) 
	{
		if (dtype==0) cuts[id] = TString::Format("%s<%.3f",varname.Data(),leftcut);
		else cuts[id] = TString::Format("%s<=%.0f",varname.Data(),leftcut);
		
		bestcut = leftcut;
		return leftsupr;
	}
	
	if (dtype==0) cuts[id] = TString::Format("%s>%.3f",varname.Data(),rightcut);
	else cuts[id] = TString::Format("%s>=%.0f",varname.Data(),rightcut);

	return rightsupr;
}
// ---------------------------------------------------------------

double bestCombiEvt(TTree *t, TString varname, TEventList &els, TEventList &elb, double &bestcut, double eff, int id)
{	
	CountMap bgcntl, bgcntr, sigcntl, sigcntr;	
	ValueMap sigvals, bgvals;
	
	int dtype = gettype(t,varname);
	int Ns = els.GetN(), Nb = elb.GetN();
	
	makeMaps(t, varname, els, elb, sigvals, bgvals, id);
	
	// sort signals by first element = variable value
	sort(sigvals.begin(), sigvals.end());
	sort(bgvals.begin(), bgvals.end());

	double step = (maxh[id]-minh[id])/(double)BINS;

	int isigl=0, ibgl = 0, isigr = sigvals.size()-1, ibgr = bgvals.size()-1;

	double leftcut = 0., rightcut = 0., qal = 1000., qar = 1000.;
	cout <<varname<<" "<<flush;

	for (int i=0; i<BINS; ++i)
	{
		double lcut = (i+1)*step + minh[id];
		double rcut = maxh[id] - (i+1)*step;
		
		while (sigvals[isigl].first<lcut && isigl<Ns) sigcntl[sigvals[isigl++].second]+=1;
		while (bgvals[ibgl].first<lcut && ibgl<Nb)   bgcntl[bgvals[ibgl++].second]+=1;
		
		while (sigvals[isigr].first>rcut && isigr>=0) sigcntr[sigvals[isigr--].second]+=1;
		while (bgvals[ibgr].first>rcut && ibgr>=0)   bgcntr[bgvals[ibgr--].second]+=1;
		
		double effl = (double) sigcntl.size()/Nsigev;
		double effr = (double) sigcntr.size()/Nsigev;
		double supl = 1. - (double) bgcntl.size()/Nbgev;
		double supr = 1. - (double) bgcntr.size()/Nbgev;
		
		double distl = sqrt((1.-effl)*(1.-effl) + (1.-supl)*(1.-supl));
		double distr = sqrt((1.-effr)*(1.-effr) + (1.-supr)*(1.-supr));
		
		printf("%8s (%3d) : left(%5.3f / %5.3f : %5.3f) right(%5.3f / %5.3f : %5.3f)\n", varname.Data(), i, effl, supl, distl, effr, supr, distr); 
		//cout <<i<<" "<<"el:"<<effl<<" eb:"<<effr<<" sl:"<<supl<<" sr:"<<supr<<" dl:"<<distl<<" dr:"<<distr<<endl;
		
		grl[id].SetPoint(i, effl, supl); 
		grr[id].SetPoint(i, effr, supr);
		
		if (distl<qal) {qal=distl; leftcut=lcut;}
		if (distr<qar) {qar=distr; rightcut=rcut;}
	}
	
	t->SetBranchStatus("*",1);
		
	if (leftcut!=leftcut || rightcut!=rightcut) return 0;
	
	bestcut = rightcut;
	if (qal<qar) 
	{
		if (dtype==0) cuts[id] = TString::Format("%s<%.3f",varname.Data(),leftcut);
		else cuts[id] = TString::Format("%s<=%.0f",varname.Data(),leftcut);
		
		bestcut = leftcut;
		return qal;
	}
	
	if (dtype==0) cuts[id] = TString::Format("%s>%.3f",varname.Data(),rightcut);
	else cuts[id] = TString::Format("%s>=%.0f",varname.Data(),rightcut);

	return qar;
}

// ---------------------------------------------------------------

void cutfinderx(TString fname, TString precut="", double supr=0.95, int evmult=10000, double norm=1.0, int n0s=-1)
{
	gStyle->SetTitleX(0.2);
	gStyle->SetTitleY(0.993);
	gStyle->SetTitleH(0.07);
	gStyle->SetPadTopMargin(0.08);
	gStyle->SetTitleBorderSize(0);
	gStyle->SetOptStat(0);
	
	TString tagcut = "tag";
	
	TRegexp regntp("n[0-9]+");
	TRegexp regS("[0-9]+S");
	TRegexp regB("[0-9]+B");
	
	TString s=fname(regS);
	s = s(0,s.Length()-1);
	TString b=fname(regB);
	b = b(0,b.Length()-1);
	
	TString ntp = fname(regntp);
	TString smode = ntp(1,ntp.Length());
	int mode = smode.Atoi();
	dstarmode = (mode>=110 && mode<=119) || (mode>=130 && mode<=138) || (mode>=150 && mode<=151);
	
	if (n0s<0) n0s = s.Atoi();
	int n0b = b.Atoi();
	
	N0_sig = n0s*evmult;
	N0_bg = n0b*evmult;
	
	cout <<"tree '"<<ntp.Data()<<"' with N_sig = "<<N0_sig<<", N_bg = "<<N0_bg<<endl;

	int i,j;
	
	TFile *f=new TFile(fname,"READ");
	TTree *t=(TTree*)f->Get(ntp);
	
	init(t);
	
	TEventList els("els"), elsall("elsall"), elb("elb"), elball("elball");
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,1800,550);
	c1->Divide(7,3);
	
	if (precut=="") precut = tagcut;
	else precut = tagcut+"&&"+precut;
	
	TString sigcut = "xmct";
	TString bgcut  = "!xmct";
	
	t->Draw(">>elsall",tagcut+"&&"+sigcut);
	t->Draw(">>elball",tagcut+"&&"+bgcut);

	if (precut!="") 
	{
		sigcut+="&&"+precut;
		bgcut +="&&"+precut;
	}
	
	cout <<sigcut <<" "<<bgcut<<endl;
	t->Draw(">>els",sigcut);
	t->Draw(">>elb",bgcut);
	
	float nsig = countEvents(t,elsall);
	float nbg = countEvents(t,elball);
	Nbgev = countEvents(t, elb);
	Nsigev = countEvents(t, els);
	
	cout <<"SIG EVT: "<<Nsigev<<" ev  "<<els.GetN()<<" cn   BG: "<<Nbgev<<" ev  "<<elb.GetN()<<" cn"<<endl;

	int cnt=1;
	
	for(i=0;i<nbranch;++i) { idx[i]=i; signi[i]=0.; grl[i].Set(BINS); grr[i].Set(BINS);}
	
	for(i=0; i<nbranch; ++i)
	{
		if (supr<0)
			signi[i] = bestEffEvt(t, vars[i], els, elb, cut[i], -supr, i);
		else if (supr>0)
			signi[i] = bestSuppressionEvt(t, vars[i], els, elb, cut[i], supr, i);	
		else 
			signi[i] = bestCombiEvt(t, vars[i], els, elb, cut[i], supr, i);	
			
	}
	cout <<endl;
	 	
	cout <<"\n\nBEST 20 vars:"<<endl<<endl;
 	
	std::vector<int> myidx (idx, idx+nbranch);	
	
	std::sort(myidx.begin(), myidx.end(), mycompare);

	TLine l;
	l.SetLineColor(6);
	l.SetLineStyle(2);
	l.SetLineWidth(2);
	TLatex lt;
	lt.SetTextSize(0.06);
	TString target="supr";
	if (supr<0) target="eff";
	if (supr==0) target="qa";
	
	for (j=0;j<21;++j)
	{		
		i=myidx[j];

		if (j<20)
		{
			printf("%2d) %-15s : %4s = %5.3f   cut = %.4f ( %s )\n",j,vars[i].Data(), target.Data(), signi[i], cut[i], cuts[i].Data());

			TH1F h1("h1",vars[i],BINS,minh[i],maxh[i]);
			TH1F h2("h2",vars[i],BINS,minh[i],maxh[i]);
			h2.SetLineColor(2);
		
			t->SetEventList(&els);
			t->Project("h1",vars[i]);
			t->SetEventList(&elb);
			t->Project("h2",vars[i]);
			
			h1.Scale(1.0/h1.Integral());
			h2.Scale(1.0/h2.Integral());
			h1.SetTitleSize(0.05);
			
			c1->cd(j+1);
			double maxi = h1.GetMaximum();
			if (h2.GetMaximum()>maxi ) maxi = h2.GetMaximum();
			maxi*=1.1;
			h1.SetMaximum(maxi);
			h2.SetMaximum(maxi);
						
			h1.DrawNormalized();
			h2.DrawNormalized("same");

			double axmin = h1.GetXaxis()->GetXmin(),axmax = h1.GetXaxis()->GetXmax(); 
			
			l.DrawLine(cut[i],0, cut[i], maxi*0.9);
			
			lt.DrawLatex(axmin+(axmax-axmin)*0.6,1.01*maxi,TString::Format("%s = %6.4f",target.Data(),signi[i]));

		}
		else
		{
			TH1F hr("hr","recmodes",10,0,10);
 			c1->cd(21);
			
			for (int k=0;k<10;++k) hr.SetBinContent(k+1,(double) evcntrec[k].size()/evmult*norm);
			
			hr.DrawCopy();
		}		
	}
	
	//t->Draw(">>els",sigcut+"&&"+cuts[myidx[0]]);
	
		
	c1->Update();
	
	cout <<"\nCUT     : "<<precut.Data()<<endl;
	cout <<"SIG EVT: "<<Nsigev<<" ev  "<<els.GetN()<<" cn   BG: "<<Nbgev<<" ev  "<<elb.GetN()<<" cn"<<endl;

	printf("SIG EFF : %6.1f%%    BG EFF : %7.3f%%\n",Nsigev/N0_sig*100.,Nbgev/N0_bg*100.);
	printf("SIG REL : %6.1f%%    BG REL : %7.3f%%\n\n",Nsigev/nsig*100.,Nbgev/nbg*100.);
	
	cout<<"Recoil : "; for (int k=0;k<10;++k) printf("    %02d ",k);cout <<endl;
	cout<<"Eff    : "; for (int k=0;k<10;++k) printf("%6.1f%%",(double) evcntrec[k].size()/evmult*norm*100.);cout <<endl<<endl;

	t->SetEventList(0);
}
