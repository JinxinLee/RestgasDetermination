#include <algorithm>
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TString.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TEventList.h"
#include "TDirectory.h"
#include <iostream>
#include "TLine.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TObjArray.h"
#include "TPRegexp.h"
#include "TRegexp.h"
#include <map>
#include <utility>
#include <algorithm>

#define MAX 1000
#define BINS 500
#define NCAN 3
#define MAXVARS 8  //maximum allowed number of variables for one selection
#define MAXCUTS 12  //maximum allowed number of cuts for one selection

typedef std::vector<pair<double, int> > ValueMap;
typedef std::map<int, int> CountMap;

CountMap evcnt, evcntrec[10];
std::map<TString, TString> mctvar;

TString vars[MAX];
TString cuts[MAX];
double cut[MAX];
double qual[MAX];
int idx[MAX];

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

bool mycompare(int i, int j)
{
  return qual[i]>qual[j];
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

int uid(int lev, int lrun, int lmode)
{
	return lev+10000*lrun+(((lmode/100)%10)*20+lmode%10)*100000;
}

// ---------------------------------------------------------------

int countEvents(TTree *t, TEventList &el)
{
	t->SetEventList(0);
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

	// prepare the pairs of variable value, eventnumber for signal and count signals
	for (i=0;i<els.GetN();++i)
	{
		t->GetEntry(els.GetEntry(i));
		switch (dtype) {case 2: var = (Float_t)bbranch[id]; break; case 1: var = (Float_t) ibranch[id]; break; default: var=fbranch[id]; }
		
		sig.push_back(std::make_pair(var, uid(ev,run,mode)  ));
	}
		
	// prepare the pairs of variable value, eventnumber for background and count background
	for (i=0;i<elb.GetN();++i)
	{
		t->GetEntry(elb.GetEntry(i));
		switch (dtype) {case 2: var = (Float_t)bbranch[id]; break; case 1: var = (Float_t) ibranch[id]; break; default: var=fbranch[id]; }
		
		bg.push_back(std::make_pair(var, uid(ev,run,mode) ));
	}
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
	
	for (i=0;i<(int)sigvals.size();++i)
	{
		if (sigvals[i].first<=leftcut)  sigcnt[sigvals[i].second]+=1;
		if (sigvals[i].first>=rightcut) sigcnt2[sigvals[i].second]+=1;
	}
	
	t->SetBranchStatus("*",1);
		
	double lefteff  = sigcnt.size()/Nsigev;
	double righteff = sigcnt2.size()/Nsigev;

	//cout <<varname<<" "<<flush;
	
	TString prec="%.4f";
	
	bestcut = rightcut;
	if (lefteff>righteff) 
	{
		bestcut = leftcut;
		
		if (bestcut<1e-3 || fabs(1.-bestcut)<1e-3) prec="%.6f";
		if (dtype==0) cuts[id] = TString::Format(TString("%s<="+prec).Data(),varname.Data(),leftcut);
		else cuts[id] = TString::Format("%s<=%.1f",varname.Data(),leftcut);
		
		qual[id] = lefteff;

		return lefteff;
	}
	
	if (bestcut<1e-3 || fabs(1.-bestcut)<1e-3) prec="%.6f";

	if (dtype==0) cuts[id] = TString::Format(TString("%s>="+prec).Data(),varname.Data(),rightcut);
	else cuts[id] = TString::Format("%s>=%.1f",varname.Data(),rightcut);

	qual[id] = righteff;

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
	
	for (i=0;i<(int)bgvals.size();++i)
	{
		if (bgvals[i].first<=leftcut)  bgcnt[bgvals[i].second];
		if (bgvals[i].first>=rightcut) bgcnt2[bgvals[i].second];
	}
	
	t->SetBranchStatus("*",1);
	
	
	double leftsupr = (Nbgev-bgcnt.size())/Nbgev;
	double rightsupr = (Nbgev-bgcnt2.size())/Nbgev;

	//cout <<varname<<" "<<flush;
	
	if (leftcut!=leftcut || rightcut!=rightcut) return 0;
	
	TString prec="%.4f";
	
	bestcut = rightcut;
	if (leftsupr>rightsupr) 
	{
		bestcut = leftcut;
		
		if (bestcut<1e-3 || fabs(1.-bestcut)<1e-3) prec="%.6f";
		if (dtype==0) cuts[id] = TString::Format(TString("%s<="+prec).Data(),varname.Data(),leftcut);
		else cuts[id] = TString::Format("%s<=%.1f",varname.Data(),leftcut);
		
		qual[id] = leftsupr;

		return leftsupr;
	}
	
	if (bestcut<1e-3 || fabs(1.-bestcut)<1e-3) prec="%.6f";

	if (dtype==0) cuts[id] = TString::Format(TString("%s>="+prec).Data(),varname.Data(),rightcut);
	else cuts[id] = TString::Format("%s>=%.1f",varname.Data(),rightcut);

	qual[id] = rightsupr;

	return rightsupr;
}

// ---------------------------------------------------------------

int findcut(TTree *t, TEventList &els, TEventList &elb, double supr, double &bestqa)
{
	int i;
	
	bestqa = -999.;
	
	TString bestcut="";
	
	int bestid = -1;
	
	//cout <<"000/000"<<flush;

	for(i=0; i<nbranch; ++i)
	{
		idx[i] = i;
		
		if (i%(nbranch/10)==0){ cout <<"#"<<flush;}
//		printf("\b\b\b\b\b\b\b%03d/%03d",i+1,nbranch);
//		fflush(stdout);
		
		double qa=0;
		
		if (supr<0)	qa = bestEffEvt(t, vars[i], els, elb, cut[i], -supr, i);
		else        qa = bestSuppressionEvt(t, vars[i], els, elb, cut[i], supr, i);	
		
		if (qa>bestqa && fabs(cut[i])>1e-5 && fabs(cut[i]-1.0)>1e-5) 
		{
			bestqa  = qa;
			bestcut = cuts[i];
			bestid = i;
		}
	}
	
	return bestid;
}

// ---------------------------------------------------------------
int countVars(TString s)
{
	int cnt = 0;
	TString svar="";
	TRegexp rvar("-[a-zA-Z_][a-zA-Z0-9_]+-");
	
	TObjArray *tok = s.Tokenize("&&");
	int N = tok->GetEntries();	
	for (int i=0;i<N;++i) 
	{
		if (i<100) 
		{
			TString toks = ((TObjString*)tok->At(i))->String();
			toks.ReplaceAll("\t","");
			toks = toks.Strip(TString::kBoth);
			if (toks.Index("<")>0) toks = toks(0,toks.Index("<"));
			if (toks.Index(">")>0) toks = toks(0,toks.Index(">"));
			svar += "-"+toks+"-";
		}
	}
	while (svar(rvar)!="") {svar.ReplaceAll(svar(rvar),""); cnt++;}
	return cnt;
}

// ---------------------------------------------------------------
int countCuts(TString s)
{
	TObjArray *tok = s.Tokenize("&&");
	return tok->GetEntries();	
}

// ---------------------------------------------------------------

int autocutx(TString fname, TString precut="", double supr=0.95, double target=0.0001, double mineff = 0.1, double minreleff = 0.0, int evmult=10000, double norm=1.0, int n0s=-1)
{
// 	gStyle->SetTitleX(0.2);
// 	gStyle->SetTitleY(0.993);
// 	gStyle->SetTitleH(0.07);
// 	gStyle->SetPadTopMargin(0.08);
// 	gStyle->SetTitleBorderSize(0);
	gStyle->SetOptStat(0);
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,1800,550);
	c1->Divide(7,2);
	
	TString tagcut = "tag";
	
	TRegexp ren("M[0-9][0-9][0-9]"); 
	TRegexp regntp("n[0-9]+");
	TRegexp regS("[0-9]+S");
	TRegexp regB("[0-9]+B");
	
	TString en = fname(ren);  en = en(1,5); 
	TString s  = fname(regS); s  = s(0,s.Length()-1);
	TString b  = fname(regB); b  = b(0,b.Length()-1);
	
	TString ntp   = fname(regntp);
	TString smode = ntp(1,ntp.Length());
	int dmode     = smode.Atoi()/10;
	dstarmode     = (dmode==11 || dmode==13 || dmode==15);
	
	if (dmode>=10 && dmode<20) norm = 0.5;
	
	if (dmode/10 == 6) norm = 0.2;
	
	if (n0s<0) n0s = s.Atoi();
	int n0b = b.Atoi();
	
	N0_sig = n0s*evmult;
	N0_bg = n0b*evmult;
	
	cout <<"tree '"<<ntp.Data()<<"' with N_sig = "<<N0_sig<<", N_bg = "<<N0_bg<<endl;

	TFile *f=new TFile(fname,"READ");
	TTree *t=(TTree*)f->Get(ntp);
	
	init(t);
	
	TEventList elsall("elsall"), elball("elball");
	TEventList els("els"), elb("elb");
	
	if (precut=="") precut = tagcut;
	else precut = tagcut+"&&"+precut;
	
	TString sigcut = "xmct";
	TString bgcut  = "!xmct";
	
	t->Draw(">>elsall",tagcut+"&&"+sigcut);
	t->Draw(">>elball",tagcut+"&&"+bgcut);
	float nsig = countEvents(t,elsall);
	float nbg = countEvents(t,elball);
	
	double beff=1., seff=1., rseff=1.;
	
	TRegexp rnum("[\\-]*[0-9]+\\.[0-9]+$");
	int cnt=0;
	
	while (cnt<150 && beff>target && seff>=mineff && rseff>=minreleff) 
	{		
		c1->cd(cnt%7+1); 
		t->SetLineColor(1);	t->Draw("xm",precut);
		t->SetLineColor(2);	t->Draw("xm",precut+"&&"+sigcut,"same");
		t->SetLineColor(4);	t->Draw("xm",precut+"&&"+bgcut,"same");
		
		c1->Update();
				
		t->Draw(">>els",sigcut+"&&"+precut);
		t->Draw(">>elb",bgcut+"&&"+precut);

		Nbgev  = countEvents(t, elb);
		Nsigev = countEvents(t, els);
		
	   	beff  = Nbgev/N0_bg;
	   	seff  = Nsigev/N0_sig;
		rseff = Nsigev/nsig;
		
		printf("S=%6d  B=%6d (%.2f): ",(int)Nsigev, (int)Nbgev, supr);
		
		// stop if target reached or signal efficiency too small
		if (beff<=target || seff<mineff || rseff<minreleff) continue;
		
		double qa=0.;
		
		int bestid = findcut(t, els, elb, supr, qa);
		
		TString bestcut = cuts[bestid];
		
		std::vector<int> myidx (idx, idx+nbranch);	
		std::sort(myidx.begin(), myidx.end(), mycompare);	
		
		if (precut.Length()>3 && precut.Index(vars[bestid]+">")<0 && precut.Index(vars[bestid]+"<")<0)
		{
			int i = 0, ncuts = countCuts(precut);
			
			while (i<nbranch && precut.Index(vars[idx[i]]+">")<0 && precut.Index(vars[idx[i]]+"<")<0) ++i;
			
			if (qual[idx[i]]/qa>0.8 ) bestcut = cuts[idx[i]];
		}		
		
		TString thenum = bestcut(rnum);
		double cutval = thenum.Atof();
		TString thevar = bestcut(0,bestcut.Index(thenum));
		TString thebarevar = thevar;
		thebarevar.ReplaceAll(">","");
		thebarevar.ReplaceAll("<","");
		thebarevar.ReplaceAll("=","");
				
		TRegexp rcut(thevar+"[\\-]*[0-9]+\\.[0-9]+");
		TString theoldcut = precut(rcut);
		
		c1->cd(cnt%7+8);
		
		t->SetLineColor(1);	t->Draw(thebarevar,precut);
		t->SetLineColor(2);	t->Draw(thebarevar,precut+"&&"+sigcut,"same");
		t->SetLineColor(4);	t->Draw(thebarevar,precut+"&&"+bgcut,"same");
		c1->Update();
		TLine l; l.SetLineColor(6); l.SetLineWidth(2);
		l.DrawLine(cutval,0,cutval,gPad->GetUymax()*0.9);
		
		c1->Update();
		
		if (theoldcut!="") 
			precut.ReplaceAll(theoldcut,bestcut);
		else
			precut += "&&"+bestcut;		
			
		cout <<" : "<<precut<<endl;
		cnt++;
	}
	
	cout <<"\nCUT : "<<en<<smode<<" : "<<precut.Data()<<endl;
	cout <<"SIG EVT: "<<Nsigev<<" ev    BG: "<<Nbgev<<" ev"<<endl;

	printf("SIG EFF : %6.1f%%    BG EFF : %7.3f%%\n",Nsigev/N0_sig*100.,Nbgev/N0_bg*100.);
	printf("SIG REL : %6.1f%%    BG REL : %7.3f%%\n\n",Nsigev/nsig*100.,Nbgev/nbg*100.);

	cout<<"Recoil : "; for (int k=0;k<10;++k) printf("    %02d ",k);cout <<endl;
	cout<<"Eff    : "; for (int k=0;k<10;++k) printf("%6.1f%%",(double) evcntrec[k].size()/evmult*norm*100.);cout <<endl<<endl;

	t->SetEventList(0);
  return 0;
}
