#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
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
#include "TEventList.h"

using std::cout;
using std::endl;

TRandom3      fRand;
TLorentzVector fIni;

// *** some program parameters 
const int		fMAX	   = 100;  		// *** max tracks per event
int 			nbins 		= 300;      // *** bins of mass histograms
double 			low  		= 0.0;      // *** lower edge of histo
double 			high 		= 6.;      // *** higher edge of histo (will be modified acc. to resonance mass)

std::map<int, int> evcnt;
std::map<int, int> treeidx;
std::map<TString, TString> cuts;
std::map<TString, TString> modes;
std::map<TString, TString> mctvar;

TString ntpname[10]  = {"nphi","nlam","n2e","njpsi1","njpsi2","netac2","nd01","ndpm1","nds1","nlamc"};
TString plotname[10] = {"phim","lamm","eem","jpsim","jpsim","etacm","d0m","dpmm","dsm","lamcm"};

// *** setup  style
void setStyle()
{
	gStyle->SetPadTopMargin(0.13);
	gStyle->SetPadBottomMargin(0.18);
	gStyle->SetPadLeftMargin(0.2);
	gStyle->SetPadRightMargin(0.016);

	gStyle->SetLabelSize(0.076,"X");
	gStyle->SetLabelSize(0.07,"Y");

	gStyle->SetStatY(0.87);
	gStyle->SetStatX(0.986);
	gStyle->SetStatW(0.4);
	gStyle->SetStatH(0.12);
	gStyle->SetOptStat("e"); // only entries
	
	gStyle->SetTitleH(0.1);
	gStyle->SetTitleX(0.08);
	gStyle->SetTitleOffset(1.1,"x");
	gStyle->SetTitleXSize(0.076);
	
	gStyle->SetNdivisions(505);
}

double getPos(TH1* h)
{
	return (h->GetBinCenter(h->GetNbinsX())-h->GetBinCenter(1))*0.05+h->GetBinCenter(1);
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

void init_modes()
{
	modes["nphi"] 	= "000";	
	modes["nlam"] 	= "400";
	modes["n2e"]  	= "300";
	modes["njpsi1"] = "200";
	modes["njpsi2"] = "201";
	modes["netac2"]	= "221";	
	modes["nd01"] 	= "100";
	modes["ndpm1"] 	= "120";	
	modes["nds1"] 	= "140";	
	modes["nlamc"] 	= "420";

	modes["DPM"] 	= "900";
	
	mctvar["nphi"] 	= "phi";	
	mctvar["nlam"] 	= "lam";
	mctvar["n2e"]  	= "ee";
	mctvar["njpsi1"]= "jpsi";
	mctvar["njpsi2"]= "jpsi";
	mctvar["netac2"]= "etac";	
	mctvar["nd01"] 	= "d0";
	mctvar["ndpm1"] = "dpm";	
	mctvar["nds1"] 	= "ds";	
	mctvar["nlamc"] = "lamc";
}

void init_cuts_1000(TString energy)
{
	if (energy=="24")
	{
		cuts["nphi"] 	= "eslnpidk>1&&phipcm>0.5&&phipcm<0.75";
		cuts["nlam"] 	= "lampcm>0.38&&lampcm<0.48&&eslnpidp>0&&lamp>0.6";
		cuts["n2e"] 	= "eslnpide>1";

	}
	else if (energy=="38")
	{
		cuts["nphi"]  	= "eslnpidk>1&&phipcm>1.4";
		cuts["nlam"]  	= "eslnpidp>0&&lampcm>1.25&&esthr>0.93&&lampt>0.75";
		cuts["n2e"]  	= "eslnpide>1";
		cuts["njpsi1"] 	= "eslnpide>1&&jpsipcm<0.9";
		cuts["njpsi2"] 	= "eslnpidmu>1&&jpsipcm<0.9";
		cuts["netac2"] 	= "eslnpidk>0&&eslnpidpi>2";
		cuts["nd01"]  	= "eslnpidk>0&&d0pcm<0.4&&d0d0pt>0.4";
		cuts["ndpm1"] 	= "eslnpidk>0&&dpmpcm<0.3";
		
	}
	else if (energy=="45")
	{
		cuts["nphi"]  	= "eslnpidk>1&&phipcm>1.55&&esthr>0.85";
		cuts["nlam"]  	= "esl1npidp>0&&esthr>0.93&&lampt>1";
		cuts["n2e"]  	= "eslnpide>1&&mmiss>-1";
		cuts["njpsi1"] 	= "eslnpide>1&&jpsipcm<1.8&&detemcmax>2.5";
		cuts["njpsi2"] 	= "eslnpidmu>1&&jpsipcm<1.5";
		cuts["netac2"] 	= "eslnpidk>0&&eslnpidpi>2&&etacd1pt>0.35&&etacd0pt>0.35";
		cuts["nd01"]  	= "eslnpidk>0&&d0pt>1&&mmiss>1.5&&d0d0pidk>0.25";
		cuts["ndpm1"] 	= "eslnpidk>0&&dpmpcm>1.1&&dpmpcm<1.5&&dpmpt>0.9&&dpmd0pidk>0.3";
		cuts["nds1"] 	= "eslnpidk>1&&dspcm>1&&dspcm<1.17&&dspt>0.5";
		
	}
	else if (energy=="55")
	{
		cuts["nphi"]  	= "eslnpidk>1&&phipcm>1.5&&esthr>0.89";
		cuts["nlam"]  	= "eslnpidp>0&&esthr>0.97&&lampcm>2.1&&lampt>1";
		cuts["n2e"]  	= "eslnpide>1&&mmiss>-1";
		cuts["njpsi1"] 	= "eslnpide>1&&jpsipcm<2.2&&detemcmax>4&&jpsid1tht>0.07";
		cuts["njpsi2"] 	= "eslnpidmu>1&&jpsipcm<2";
		cuts["netac2"] 	= "eslnpidk>0&&eslnpidpi>2&&esptmax>1.1&&esnpart<15";
		cuts["nd01"]  	= "eslnpidk>0&&d0pcm>1.9&&mmiss>1.5&&d0pt>1.6&&d0pcm<2.15";
		cuts["ndpm1"] 	= "eslnpidk>0&&dpmpcm>1.95&&dpmpcm<2.15&&dpmpt>0.8&&dpmd0pt>0.4";
		cuts["nds1"] 	= "eslnpidk>1&&dspcm>1.85&&dspcm<2.1&&dspt>1.3";
		cuts["nlamc"] 	= "eslnpidk>0&&eslnpidp>0&&lamcpt>.9&&lamcd0pidp>0.21&&lamcd0thtcm<2";
		
	}
	else
	{
		cuts["nphi"]  	= "1";
		cuts["nlam"]  	= "1";
		cuts["n2e"]  	= "1";
		cuts["njpsi1"] 	= "1";
		cuts["njpsi2"] 	= "1";
		cuts["netac2"] 	= "1";
		cuts["nd01"]  	= "1";
		cuts["ndpm1"] 	= "1";
		cuts["nds1"] 	= "1";
		cuts["nlamc"] 	= "1";
	}
		// factor 1000
/*		cuts["nphi"] 	= "fw1>0.0129387&&phipt>0.2";
		cuts["nlam"] 	= "fw3>0.2&&fw4>0.3&&sumpc>0.9";
		cuts["nee"] 	= "1";*/

		// factor 1000
/*		cuts["nphi"]  	= "phipcm>1.31489&&phipt>0.698733";
		cuts["nlam"]  	= "fw2>0.688083&&lampt>0.990166";
		cuts["nee"]  	= "1";
		cuts["njpsi1"] 	= "1";
		cuts["njpsi2"] 	= "1";
		cuts["netac"] 	= "ptmax>0.84&&sumptc>2.9";
		cuts["nd0"]  	= "d0pcm<0.35&&ptmax>0.8";
		cuts["ndpm"] 	= "dpcm<0.34&&ptmax>0.67&&fw1>0";*/
		
		// first attempt
/*		cuts["netac"] 	= "ptmax>0.7&&sumptc>2.7";
		cuts["nd0"]  	= "d0pcm<0.33&&ptmax>0.75";
		cuts["ndpm"] 	= "dpcm<0.31&&ptmax>0.6&&pmin>0.1";*/
		// factor 1000
// 		cuts["nphi"]  	= "phipcm>1.9&&phipt>1";
// 		cuts["nlam"]  	= "lampcm>1.8&&thr>0.95&&lampt>1";
// 		cuts["nee"]  	= "1";
// 		cuts["njpsi1"] 	= "sumpc>3.5";
// 		cuts["njpsi2"] 	= "sumpc>3.5";
// 		cuts["netac"] 	= "ptmax>1&&sumpc>4";
// 		cuts["nd0"]  	= "abs(d0pcm-1.24)<0.15&&ptmax>1.05&&sumptc>3.1";
// 		cuts["ndpm"] 	= "abs(dpcm-1.24)<0.12&&dpt>1&&sumptc>3";
// 		cuts["nds"] 	= "abs(dspcm-1.1)<0.09&&ptmax>0.93";

		// first attempt                       		cuts["nphi"]  	= "phipcm>1.9&&phipt>1";
/*		cuts["nd0"]  	= "abs(d0pcm-1.24)<0.15&&ptmax>1.2";
		cuts["ndpm"] 	= "abs(dpcm-1.24)<0.15&&dpt>1&&pmin>0.15";
		cuts["nds"] 	= "abs(dspcm-1.1)<0.1&&ptmax>0.8";*/
		// factor 1000
/*		cuts["nphi"]  	= "phipcm>2.4&&phipt>0.5";
		cuts["nlam"]  	= "lampcm>0.23&&thr>0.98&&lampt>1.2";
		cuts["nee"]  	= "1";
		cuts["njpsi1"] 	= "sumpc>4";
		cuts["njpsi2"] 	= "sumpc>4";
		cuts["netac"] 	= "ptmax>1.1&&sumpc>4.8";
		cuts["nd0"]  	= "ptmax>1&&d0pcm>1.8&&d0pt>1.5";
		cuts["ndpm"] 	= "ptmax>1&&dpcm>1.8&&dpt>1.5";
		cuts["nds"] 	= "dspcm>1.8&&ptmax>0.8&&dspt>1.2";
		cuts["nlamc"] 	= "abs(lamcpcm-1.53)<0.12&&lamcpt>1.3&&fw1>0";*/
		// first attempt
/*		cuts["nlamc"] 	= "abs(lamcpcm-1.53)<0.15&&lamcpt>1.3";*/
}
void init_cuts_95(TString energy)
{
	if (energy=="24")
	{
		cuts["nphi"] 	= "eslnpidk>1&&phipcm>0.45&&phipcm<0.78";
		cuts["nlam"] 	= "eslnpidp>0&&lampcm<0.55&&lampcm>0.25&&mmiss<1.2";
		cuts["n2e"] 	= "eslnpide>1&&mmiss>-1";

	}
	else if (energy=="38")
	{
		cuts["nphi"]  	= "eslnpidk>1&&phipcm>1.2";
		cuts["nlam"]  	= "esthr>0.85&&lampcm>1.15&&lampt>0.2&&eslnpidp>0";
		cuts["n2e"]  	= "eslnpide>1&&mmiss>-1";
		cuts["njpsi1"] 	= "eslnpide>1&&jpsipcm<1";
		cuts["njpsi2"] 	= "eslnpidmu>1&&jpsipcm<1";
		cuts["netac2"] 	= "etacpcm<1.25&&essumptc>1.75";
		cuts["nd01"]  	= "eslnpidk>0&&d0pcm<0.4&&d0d0pt>0.25";
		cuts["ndpm1"] 	= "eslnpidk>0";
		
	}
	else if (energy=="45")
	{
		cuts["nphi"]  	= "eslnpidk>1&&phipcm>1.55";
		cuts["nlam"]  	= "esthr>0.87&&eslnpidp>0&&lampcm>1.5";
		cuts["n2e"]  	= "eslnpide>1&&mmiss>-1";
		cuts["njpsi1"] 	= "eslnpide>1&&jpsipcm<1.8&&detemcmax>2.5";
		cuts["njpsi2"] 	= "eslnpidmu>1&&jpsipcm<1.5";
		cuts["netac2"] 	= "etacpcm<1.25&&etacd1p>0.25";
		cuts["nd01"]  	= "eslnpidk>0&&d0pcm<1.6&&d0pcm>0.8&&d0d1p<6";
		cuts["ndpm1"] 	= "eslnpidk>0";
		cuts["nds1"] 	= "eslnpidk>0&&dspcm>0.4&&dsp<8&&dsthtcm>0.5";
		
	}
	else if (energy=="55")
	{
		cuts["nphi"]  	= "eslnpidk>1&&phipcm>1.75&&phipt>0.5";
		cuts["nlam"]  	= "esthr>0.9&&lampcm>1&&mmiss<2.4";
		cuts["n2e"]  	= "eslnpide>0&&mmiss>-1";
		cuts["njpsi1"] 	= "eslnpide>1&&jpsipcm<1.9&&detemcmax>3.5";
		cuts["njpsi2"] 	= "eslnpidmu>1&&jpsipcm<2.5";
		cuts["netac2"] 	= "etacpcm<2&&etacd1p>0.4&&etacecm>2.85&&eslnpide<1";
		cuts["nd01"]  	= "eslnpidk>0&&d0pcm<2.3&&d0pcm>0.95";
		cuts["ndpm1"] 	= "eslnpidk>0";
		cuts["nds1"] 	= "eslnpidk>0&&dspcm>0.5&&dsthtcm>0.5";
		cuts["nlamc"] 	= "mmiss>2";
		
	}
	else
	{
		cuts["nphi"]  	= "1";
		cuts["nlam"]  	= "1";
		cuts["n2e"]  	= "1";
		cuts["njpsi1"] 	= "1";
		cuts["njpsi2"] 	= "1";
		cuts["netac2"] 	= "1";
		cuts["nd01"]  	= "1";
		cuts["ndpm1"] 	= "1";
		cuts["nds1"] 	= "1";
		cuts["nlamc"] 	= "1";
	}
}

int cnt_events(TChain* t, TString cut, bool globcnt=false)
{
	treeidx.clear();
	Float_t ev;
	t->Draw(">>el",cut);
	
	TEventList *el=(TEventList*)gDirectory->Get("el");

	t->SetBranchStatus("*",0); 
	t->SetBranchStatus("ev",1);
	t->SetBranchAddress("ev",&ev);
	
	std::map<int, int> sigcnt;
	for (int j=0;j<el->GetN();++j) 
	{
		t->GetEntry(el->GetEntry(j));
		int tidx = t->GetTreeNumber();
		sigcnt[(int)ev+tidx*1000]+=1;
		if (globcnt) evcnt[(int)ev+tidx*1000]+=1;
	}
	t->SetBranchStatus("*",1); 
	return sigcnt.size();
}

void fillHistos(TChain *t, TString var, TString cut, TString all, TString sel, TString sig)
{
	t->Project(all,var,cut);
	t->Project(sel,var,cut+" && tag");
	t->Project(sig,var,cut+" && "+mctvar[t->GetName()]+"mct");
	
}
double DbMass(TString name) 
{
	TDatabasePDG *fPdg = TDatabasePDG::Instance();
	if (fPdg->GetParticle(name)) return fPdg->GetParticle(name)->Mass();
}

void full_core_ntp(TString fname, int nev=50, bool tagonly=false)
{
	int i,j;
	nev*=1000;
	
	setStyle();	
	init_modes();
	
	TString mode(fname(fname.Last('/')+4,3));

	TString energy(fname(fname.Last('/')+2,2));
	double sqrts=energy.Atof();
	
	if (tagonly)
		init_cuts_1000("");
	else
		init_cuts_1000(energy);
	
	cout <<energy<<endl;
	
	
	//TFile *f=new TFile(fname);
	TChain *t[10];
	
	for (i=0;i<10;++i)
	{
		t[i]=new TChain(ntpname[i]);
		t[i]->Add(fname);
/*		t[i]=(TTree*)f->Get(ntpname[i]);
		if (t==0) cout <<"not found :"<<ntpname[i].Data()<<endl;*/
	}
	
	TCanvas *c2=new TCanvas("c2","c2",20,20,1800,800);
	c2->Divide(5,2,0.005,0.01);
	
	
	double PhiMass = DbMass("phi");
	
	double D0Mass  = DbMass("D0");
	double DpmMass = DbMass("D+");
	double DsMass  = DbMass("D_s+");
	
	double JpsiMass  = DbMass("J/psi");
	double EtacMass  = DbMass("eta_c");
	double Chic0Mass = DbMass("chi_0c");
	
	double LamMass  = DbMass("Lambda0");
	double LamcMass = DbMass("Lambda_c+");
	
	// *** resonances
	double fPhiQaMin = PhiMass - 0.1;		
	double fPhiQaMax = PhiMass + 0.1;		
	
	// *** open charm
	double fD0QaMin = D0Mass - 0.3;		
	double fD0QaMax = D0Mass + 0.3;		
	
	double fDpmQaMin = DpmMass - 0.3;		
	double fDpmQaMax = DpmMass + 0.3;		
	
	double fDsQaMin = DsMass - 0.25;		
	double fDsQaMax = DsMass + 0.25;		
	
	// *** baryons
	double fLamcQaMin = LamcMass - 0.2;		
	double fLamcQaMax = LamcMass + 0.2;		

	double fLamQaMin = LamMass - 0.15;		
	double fLamQaMax = LamMass + 0.15;		
	
	// *** charmonia
	double fEtacQaMin = EtacMass - 0.65;		
	double fEtacQaMax = EtacMass + 0.65;		
	
	double fJpsiQaMin = JpsiMass - 1.00;		
	double fJpsiQaMax = JpsiMass + 0.75;		
	
	
	// *** electro-magnetic 
	double fEcm = sqrts/10.;
	double f2eQaMin = fEcm - 1.5;		
	double f2eQaMax = fEcm + 1.0;		
	
	
	double low2=0.5;
	// ****** Jpsi -> ee
	TH1F *h_jpsi1=new TH1F("h_jpsi1","J/#psi #rightarrow e^{+}e^{-}",nbins,fJpsiQaMin,fJpsiQaMax);
	TH1F *h_jpsi1sel=new TH1F("h_jpsi1sel","J/psi",nbins,fJpsiQaMin,fJpsiQaMax);
	TH1F *h_jpsi1sig=new TH1F("h_jpsi1sig","J/psi",nbins,fJpsiQaMin,fJpsiQaMax);
	h_jpsi1sig->SetLineColor(2);
	configHisto(h_jpsi1sel);
	
	// ****** Jpsi -> mu mu
	TH1F *h_jpsi2=new TH1F("h_jpsi2","J/#psi #rightarrow #mu^{+}#mu^{-}",nbins,fJpsiQaMin,fJpsiQaMax);
	TH1F *h_jpsi2sel=new TH1F("h_jpsi2sel","J/psi",nbins,fJpsiQaMin,fJpsiQaMax);
	TH1F *h_jpsi2sig=new TH1F("h_jpsi2sig","J/psi",nbins,fJpsiQaMin,fJpsiQaMax);
	h_jpsi2sig->SetLineColor(2);
	configHisto(h_jpsi2sel);
	
	// ****** eta_c -> Ks K pi
	TH1F *h_etac=new TH1F("h_etac","#eta_{c} #rightarrow K_{S} K^{+} #pi^{-}",nbins,fEtacQaMin,fEtacQaMax);
	TH1F *h_etacsel=new TH1F("h_etacsel","etac",nbins,fEtacQaMin,fEtacQaMax);
	TH1F *h_etacsig=new TH1F("h_etacsig","etac",nbins,fEtacQaMin,fEtacQaMax);
	h_etacsig->SetLineColor(2);
	configHisto(h_etacsel);
	
	// ****** D0 -> K- pi+
	TH1F *h_d0=new TH1F("h_d0","D^{0} #rightarrow K^{-}#pi^{+}",nbins,fD0QaMin,fD0QaMax);
	TH1F *h_d0sel=new TH1F("h_d0sel","D0",nbins,fD0QaMin,fD0QaMax);
	TH1F *h_d0sig=new TH1F("h_d0sig","D0",nbins,fD0QaMin,fD0QaMax);;
	h_d0sig->SetLineColor(2);
	configHisto(h_d0sel);
		
	// ****** D+ -> K- pi+ pi+
	TH1F *h_dpm=new TH1F("h_dpm","D^{+} #rightarrow K^{-}#pi^{+}#pi^{+}",nbins,fDpmQaMin,fDpmQaMax);
	TH1F *h_dpmsel=new TH1F("h_dpmsel","D#pm",nbins,fDpmQaMin,fDpmQaMax);
	TH1F *h_dpmsig=new TH1F("h_dpmsig","D#pm",nbins,fDpmQaMin,fDpmQaMax);
	h_dpmsig->SetLineColor(2);
	configHisto(h_dpmsel);
		
	// ****** Ds+ -> K+ K- pi+
	TH1F *h_ds=new TH1F("h_ds","D_{s}^{+} #rightarrow K^{+}K^{-}#pi^{+}",nbins,fDsQaMin,fDsQaMax);
	TH1F *h_dssel=new TH1F("h_dssel","Ds",nbins,fDsQaMin,fDsQaMax);
	TH1F *h_dssig=new TH1F("h_dssig","Ds",nbins,fDsQaMin,fDsQaMax);
	h_dssig->SetLineColor(2);
	configHisto(h_dssel);
	
	// ****** phi -> K+ K-
	TH1F *h_phi=new TH1F("h_phi","#phi #rightarrow K^{+}K^{-}",nbins,fPhiQaMin,fPhiQaMax);
	TH1F *h_phisel=new TH1F("h_phisel","Phi",nbins,fPhiQaMin,fPhiQaMax);
	TH1F *h_phisig=new TH1F("h_phisig","Phi",nbins,fPhiQaMin,fPhiQaMax);
	h_phisig->SetLineColor(2);
	configHisto(h_phisel);
	
	// ****** Lambda_c -> p K- pi+
	TH1F *h_lamc=new TH1F("h_lamc","#Lambda_{c} #rightarrow pK^{-}#pi^{+}",nbins,fLamcQaMin,fLamcQaMax);
	TH1F *h_lamcsel=new TH1F("h_lamcsel","Lam_c",nbins,fLamcQaMin,fLamcQaMax);
	TH1F *h_lamcsig=new TH1F("h_lamcsig","Lam_c",nbins,fLamcQaMin,fLamcQaMax);
	h_lamcsig->SetLineColor(2);
	configHisto(h_lamcsel);
	
	// ****** Lambda -> p pi-
	TH1F *h_lam=new TH1F("h_lam","#Lambda #rightarrow p#pi^{-}",nbins,fLamQaMin,fLamQaMax);
	TH1F *h_lamsel=new TH1F("h_lamsel","Lam",nbins,fLamQaMin,fLamQaMax);
	TH1F *h_lamsig=new TH1F("h_lamsig","Lam",nbins,fLamQaMin,fLamQaMax);
	h_lamsig->SetLineColor(2);
	configHisto(h_lamsel);
	
	// ****** pp -> e+ e-
	TH1F *h_ee=new TH1F("h_ee","p#bar{p} #rightarrow e^{+}e^{-}",nbins,f2eQaMin,f2eQaMax);
	TH1F *h_eesel=new TH1F("h_eesel","ee",nbins,f2eQaMin,f2eQaMax);
	TH1F *h_eesig=new TH1F("h_eesig","ee",nbins,f2eQaMin,f2eQaMax);
	h_eesig->SetLineColor(2);
	configHisto(h_eesel);
	
	h_jpsi1->SetXTitle("m(e^{+}e^{-}) [GeV/c^{2}]");
	h_jpsi2->SetXTitle("m(#mu^{+}#mu^{-}) [GeV/c^{2}]");
	h_etac->SetXTitle("m(K_{S} K^{+}#pi^{-}) [GeV/c^{2}]");

	h_d0->SetXTitle("m(K^{-}#pi^{+}) [GeV/c^{2}]");

	h_dpm->SetXTitle("m(K^{-}#pi^{+}#pi^{+}) [GeV/c^{2}]");

	h_ds->SetXTitle("m(K^{+}K^{-}#pi^{+}) [GeV/c^{2}]");

	h_phi->SetXTitle("m(K^{+}K^{-}) [GeV/c^{2}]");
	h_lamc->SetXTitle("m(pK^{-}#pi^{+}) [GeV/c^{2}]");
	h_lam->SetXTitle("m(p#pi^{-}) [GeV/c^{2}]");
	h_ee->SetXTitle("m(e^{+}e^{-}) [GeV/c^{2}]");
	
	// *** make histos
	fillHistos(t[0], "phim", cuts["nphi"], "h_phi", "h_phisel", "h_phisig");
	fillHistos(t[1], "lamm", cuts["nlam"], "h_lam", "h_lamsel", "h_lamsig");
	fillHistos(t[2], "eem",  cuts["n2e"], "h_ee",  "h_eesel",  "h_eesig");
	
	if (sqrts>24)
	{
		fillHistos(t[3], "jpsim", cuts["njpsi1"], "h_jpsi1", "h_jpsi1sel", "h_jpsi1sig");
		fillHistos(t[4], "jpsim", cuts["njpsi2"], "h_jpsi2", "h_jpsi2sel", "h_jpsi2sig");
		fillHistos(t[5], "etacm",  cuts["netac2"],  "h_etac",  "h_etacsel",  "h_etacsig");
		fillHistos(t[6], "d0m",    cuts["nd01"],    "h_d0",    "h_d0sel",    "h_d0sig");
		fillHistos(t[7], "dpmm",     cuts["ndpm1"],   "h_dpm",   "h_dpmsel",   "h_dpmsig");
	}
	
	if (sqrts>38)
		fillHistos(t[8], "dsm", cuts["nds1"], "h_ds", "h_dssel", "h_dssig");
	
	if (sqrts>45)
		fillHistos(t[9], "lamcm", cuts["nlamc"], "h_lamc", "h_lamcsel", "h_lamcsig");
	
	// event counts
	
	double channelcount[10]={0,0,0,0,0,0,0,0,0,0};
	double eff[10]={0,0,0,0,0,0,0,0,0,0};
	
	evcnt.clear();
	for (i=0;i<3;++i)
	{
		channelcount[i]=cnt_events(t[i], cuts[ntpname[i]]+"&&tag", true);
		cout <<i<< "  "<<channelcount[i]<<endl;
	}
	
	if (sqrts>24)
	for (i=3;i<8;++i)
	{
		channelcount[i]=cnt_events(t[i], cuts[ntpname[i]]+"&&tag", true);
		cout <<i<< "  "<<channelcount[i]<<endl;
	}
	if (sqrts>38)
	for (i=8;i<9;++i)
	{
		channelcount[i]=cnt_events(t[i], cuts[ntpname[i]]+"&&tag", true);
		cout <<i<< "  "<<channelcount[i]<<endl;
	}
	if (sqrts>45)
	for (i=9;i<10;++i)
	{
		channelcount[i]=cnt_events(t[i], cuts[ntpname[i]]+"&&tag", true);
		cout <<i<< "  "<<channelcount[i]<<endl;
	}
	
	double evCnt = evcnt.size();
	for (i=0;i<10;++i) eff[i]=channelcount[i]/nev*100.;
	
	cout <<"all:"<<evCnt<<endl;
	
	// ** draw the plots
	TLatex latex;
	latex.SetTextSize(0.08);
	char tmp[200];
	double offset = 0;//0.1;
	double posx = 0.24;
	double posy = 0.79;
	
// 	// *** plot all stuff and write out info
// 	c1->cd(1); h_pi0->Draw(); h_pi0sel->Draw("same"); h_pi0sig->Draw("same");
// 	c1->cd(2); h_ks->Draw(); h_kssel->Draw("same"); h_kssig->Draw("same");

	c2->cd(2); h_phi->Draw(); h_phisel->Draw("same");   h_phisig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[0]);
	latex.DrawLatexNDC(posx,posy,tmp);
	
	c2->cd(1); h_ee->Draw(); h_eesel->Draw("same"); h_eesig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[2]);
	latex.DrawLatexNDC(posx,posy,tmp);
	
	c2->cd(9); h_lam->Draw(); h_lamsel->Draw("same"); h_lamsig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[1]);
	latex.DrawLatexNDC(posx,posy,tmp);
	
	c2->cd(3); h_etac->Draw(); h_etacsel->Draw("same"); h_etacsig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[5]);
	latex.DrawLatexNDC(posx,posy,tmp);
	
	c2->cd(4); h_jpsi1->Draw(); h_jpsi1sel->Draw("same"); h_jpsi1sig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[3]);
	latex.DrawLatexNDC(posx,posy,tmp);
	
	c2->cd(5); h_jpsi2->Draw(); h_jpsi2sel->Draw("same"); h_jpsi2sig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[4]);
	latex.DrawLatexNDC(posx,posy,tmp);
	
	c2->cd(7); h_dpm->Draw();  h_dpmsel->Draw("same");  h_dpmsig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[7]);
	latex.DrawLatexNDC(posx,posy,tmp);
	
	c2->cd(6); h_d0->Draw();   h_d0sel->Draw("same");   h_d0sig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[6]);
	latex.DrawLatexNDC(posx,posy,tmp);

	c2->cd(8); h_ds->Draw();   h_dssel->Draw("same");   h_dssig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[8]);
	latex.DrawLatexNDC(posx,posy,tmp);
	
	c2->cd(10); h_lamc->Draw(); h_lamcsel->Draw("same"); h_lamcsig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[9]);
	latex.DrawLatexNDC(posx,posy,tmp);
	
	// total efficiency in first plot (phi)
	c2->cd(5);
	posy = h_jpsi2->GetMaximum()*1.12;
	if (posy==0) posy=1.12;
	sprintf(tmp,"(#epsilon_{t} = %4.1f%%)",double(evCnt)/double(nev)*100.);
	latex.SetTextColor(kMagenta+2);
	latex.DrawLatex(3.0,posy,tmp);
	
	c2->cd();
		
	//TString plot(fname(fname.Last('/')+1,fname.Last('.')-fname.Last('/')-1));
//fname(pos1+1,pos2-1));
    TString plot(fname(fname.Last('/')+1,6));
	if (!tagonly) plot+="_opt_supr";
	//plot="figfull/"+plot+".pdf";
	
	//cout <<plot<<endl;
	
	c2->SaveAs("figfull_v2/full_"+plot+".gif");
	c2->SaveAs("figfull_v2/full_"+plot+".pdf");
	
	std::ofstream ofs;
	
	ofs.open("full_supr1000.txt", std::ofstream::out | std::ofstream::app);
	TString result=TString::Format("%s & %3.1f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f \\\\\n",
								   mode.Data(), sqrts/10., eff[2], eff[0], eff[5], eff[3], eff[4], eff[6], eff[7], eff[8], eff[1], eff[9], evCnt/nev*100);
	//cout <<result;							
	ofs <<result;
	ofs.close();
}


