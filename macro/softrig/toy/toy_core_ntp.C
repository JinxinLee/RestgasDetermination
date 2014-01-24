#include <iostream>
#include <fstream>
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
std::map<TString, TString> cuts;
std::map<TString, TString> modes;

TString ntpname[10]  = {"nphi","nlam","nee","njpsi1","njpsi2","netac","nd0","ndpm","nds","nlamc"};
TString plotname[10] = {"phim","lamm","eem","jpsi1m","jpsi2m","etacm","d0m","dm"  ,"dsm","lamcm"};

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
	modes["nee"]  	= "300";
	modes["njpsi1"] = "200";
	modes["njpsi2"] = "201";
	modes["netac"] 	= "221";	
	modes["nd0"] 	= "100";
	modes["ndpm"] 	= "120";	
	modes["nds"] 	= "140";	
	modes["nlamc"] 	= "420";

	modes["DPM"] 	= "900";
}

void init_cuts(TString energy)
{
	if (energy=="24")
	{
		cuts["nphi"] 	= "pmax<0.56&&phipcm>0.55&&phipcm<0.68";
		cuts["nlam"] 	= "lampcm<0.5&&fw1>0.16&&lampcm>0.39";
		cuts["nee"] 	= "1";

		// factor 1000
/*		cuts["nphi"] 	= "fw1>0.0129387&&phipt>0.2";
		cuts["nlam"] 	= "fw3>0.2&&fw4>0.3&&sumpc>0.9";
		cuts["nee"] 	= "1";*/
	}
	else if (energy=="38")
	{
		cuts["nphi"]  	= "thr>0.9&&phipcm>1.46";
		cuts["nlam"]  	= "thr>0.93&&thr<0.99&&fw5>0.21&&lampcm>1.38&&fw4>0.35";
		cuts["nee"]  	= "1";
		cuts["njpsi1"] 	= "1";
		cuts["njpsi2"] 	= "1";
		cuts["netac"] 	= "sumptc>1.68&&ptmax>0.54&&sumpc>2.9";
		cuts["nd0"]  	= "d0pcm<0.36&&pmax>0.8&&ptmax>0.42";
		cuts["ndpm"] 	= "dpcm<0.32&&dpcm>0.17&&ptmax>0.4&&dp>2.7";
		
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
	}
	else if (energy=="45")
	{
		cuts["nphi"]  	= "thr>0.94&&phipcm>1.85";
		cuts["nlam"]  	= "thr>0.95&&fw4>0.53&&pmax>1.1&&sumptc>0.7";
		cuts["nee"]  	= "1";
		cuts["njpsi1"] 	= "sumpc>3.5";
		cuts["njpsi2"] 	= "sumpc>3.5";
		cuts["netac"] 	= "sumptc>2.1&&ptmax>0.65&&sumpc>3.4";
		cuts["nd0"]  	= "ptmax>0.64&&d0pcm>0.95&&pmax>1&&sumpt>2&&d0pcm<1.4";
		cuts["ndpm"] 	= "dpcm>1.1&&dpcm<1.4&&ptmax>0.58";
		cuts["nds"] 	= "dspcm>0.9&&dspcm<1.35&&ptmax>0.55";
		
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
	}
	else if (energy=="55")
	{
		cuts["nphi"]  	= "thr>0.96&&phipcm>2.3";
		cuts["nlam"]  	= "thr>0.97&&pmax>1.6&&sumptc>1.4";
		cuts["nee"]  	= "1";
		cuts["njpsi1"] 	= "sumpc>4";
		cuts["njpsi2"] 	= "sumpc>4";
		cuts["netac"] 	= "ptmax>0.8&&sumptc>2.8&&pmax>1.15";
		cuts["nd0"]  	= "d0pcm>1.75&&ptmax>0.8&&sumpt>2.4";
		cuts["ndpm"] 	= "dpcm>1.8&&ptmax>0.68&&dpcm<2.3&&dpt>0.5";
		cuts["nds"] 	= "dspcm>1.75&&dspcm<2.2&&ptmax>0.6";
		cuts["nlamc"] 	= "lamcpcm>1.3&&lamcpcm<1.7&&ptmax>0.5&&fw1>-0.02";
		
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
	else
	{
		cuts["nphi"]  	= "1";
		cuts["nlam"]  	= "1";
		cuts["nee"]  	= "1";
		cuts["njpsi1"] 	= "1";
		cuts["njpsi2"] 	= "1";
		cuts["netac"] 	= "1";
		cuts["nd0"]  	= "1";
		cuts["ndpm"] 	= "1";
		cuts["nds"] 	= "1";
		cuts["nlamc"] 	= "1";
	}
}

int cnt_events(TTree* t, TString cut, bool globcnt=false)
{
	Float_t ev;
	t->Draw(">>el",cut);
	
	TEventList *el=(TEventList*)gDirectory->Get("el");

	t->SetBranchStatus("*",0); 
	t->SetBranchStatus("evt",1);
	t->SetBranchAddress("evt",&ev);
	
	std::map<int, int> sigcnt;
	for (int j=0;j<el->GetN();++j) 
	{
		t->GetEntry(el->GetEntry(j));
		sigcnt[(int)ev]+=1;
		if (globcnt) evcnt[(int)ev]+=1;
	}
	t->SetBranchStatus("*",1); 
	return sigcnt.size();
}

void fillHistos(TTree *t, TString var, TString cut, TString all, TString sel, TString sig)
{
	t->Project(all,var,cut);
	t->Project(sel,var,cut+" && tag");
	t->Project(sig,var,cut+" && mct");
	
}

void toy_core_ntp(TString fname, int nev=50, bool tagonly=false)
{
	int i,j;
	nev*=1000;
	
	setStyle();	
	init_modes();
	
	TString mode(fname(6,3));
	
	TString energy(fname(4,2));
	double sqrts=energy.Atof();
	
	if (tagonly)
		init_cuts("");
	else
		init_cuts(energy);
	
	
	TFile *f=new TFile(fname);
	TTree *t[10];
	
	for (i=0;i<10;++i)
	{
		t[i]=(TTree*)f->Get(ntpname[i]);
	}
	
	TCanvas *c2=new TCanvas("c2","c2",20,20,1800,800);
	c2->Divide(5,2,0.005,0.01);
	
	double low2=0.5;
	
	// ****** Jpsi -> ee
	TH1F *h_jpsi1=new TH1F("h_jpsi1","J/#psi #rightarrow e^{+}e^{-}",nbins,low,high);
	TH1F *h_jpsi1sel=new TH1F("h_jpsi1sel","J/psi",nbins,low,high);
	TH1F *h_jpsi1sig=new TH1F("h_jpsi1sig","J/psi",nbins,low,high);
	h_jpsi1sig->SetLineColor(2);
	configHisto(h_jpsi1sel);
	
	// ****** Jpsi -> mu mu
	TH1F *h_jpsi2=new TH1F("h_jpsi2","J/#psi #rightarrow #mu^{+}#mu^{-}",nbins,low,high);
	TH1F *h_jpsi2sel=new TH1F("h_jpsi2sel","J/psi",nbins,low,high);
	TH1F *h_jpsi2sig=new TH1F("h_jpsi2sig","J/psi",nbins,low,high);
	h_jpsi2sig->SetLineColor(2);
	configHisto(h_jpsi2sel);
	
	// ****** eta_c -> Ks K pi
	TH1F *h_etac=new TH1F("h_etac","#eta_{c} #rightarrow K_{S} K^{+} #pi^{-}",nbins,low,high);
	TH1F *h_etacsel=new TH1F("h_etacsel","etac",nbins,low,high);
	TH1F *h_etacsig=new TH1F("h_etacsig","etac",nbins,low,high);
	h_etacsig->SetLineColor(2);
	configHisto(h_etacsel);
	
	// ****** D0 -> K- pi+
	TH1F *h_d0=new TH1F("h_d0","D^{0} #rightarrow K^{-}#pi^{+}",nbins,low,high);
	TH1F *h_d0sel=new TH1F("h_d0sel","D0",nbins,low,high);
	TH1F *h_d0sig=new TH1F("h_d0sig","D0",nbins,low,high);
	h_d0sig->SetLineColor(2);
	configHisto(h_d0sel);
		
	// ****** D+ -> K- pi+ pi+
	TH1F *h_dpm=new TH1F("h_dpm","D^{+} #rightarrow K^{-}#pi^{+}#pi^{+}",nbins,low,high);
	TH1F *h_dpmsel=new TH1F("h_dpmsel","D#pm",nbins,low,high);
	TH1F *h_dpmsig=new TH1F("h_dpmsig","D#pm",nbins,low,high);
	h_dpmsig->SetLineColor(2);
	configHisto(h_dpmsel);
		
	// ****** Ds+ -> K+ K- pi+
	TH1F *h_ds=new TH1F("h_ds","D_{s}^{+} #rightarrow K^{+}K^{-}#pi^{+}",nbins,low,high);
	TH1F *h_dssel=new TH1F("h_dssel","Ds",nbins,low,high);
	TH1F *h_dssig=new TH1F("h_dssig","Ds",nbins,low,high);
	h_dssig->SetLineColor(2);
	configHisto(h_dssel);
	
	// ****** phi -> K+ K-
	TH1F *h_phi=new TH1F("h_phi","#phi #rightarrow K^{+}K^{-}",nbins,low2,high/2);
	TH1F *h_phisel=new TH1F("h_phisel","Phi",nbins,low2,high/2);
	TH1F *h_phisig=new TH1F("h_phisig","Phi",nbins,low2,high/2);
	h_phisig->SetLineColor(2);
	configHisto(h_phisel);
	
	// ****** Lambda_c -> p K- pi+
	TH1F *h_lamc=new TH1F("h_lamc","#Lambda_{c} #rightarrow pK^{-}#pi^{+}",nbins,low,high);
	TH1F *h_lamcsel=new TH1F("h_lamcsel","Lam_c",nbins,low,high);
	TH1F *h_lamcsig=new TH1F("h_lamcsig","Lam_c",nbins,low,high);
	h_lamcsig->SetLineColor(2);
	configHisto(h_lamcsel);
	
	// ****** Lambda -> p pi-
	TH1F *h_lam=new TH1F("h_lam","#Lambda #rightarrow p#pi^{-}",nbins,low2,high/2);
	TH1F *h_lamsel=new TH1F("h_lamsel","Lam",nbins,low2,high/2);
	TH1F *h_lamsig=new TH1F("h_lamsig","Lam",nbins,low2,high/2);
	h_lamsig->SetLineColor(2);
	configHisto(h_lamsel);
	
	// ****** pp -> e+ e-
	TH1F *h_ee=new TH1F("h_ee","p#bar{p} #rightarrow e^{+}e^{-}",nbins,low,high*1.2);
	TH1F *h_eesel=new TH1F("h_eesel","ee",nbins,low,high*1.2);
	TH1F *h_eesig=new TH1F("h_eesig","ee",nbins,low,high*1.2);
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
	fillHistos(t[2], "eem",  cuts["nee"], "h_ee",  "h_eesel",  "h_eesig");
	
	if (sqrts>24)
	{
		fillHistos(t[3], "jpsi1m", cuts["njpsi1"], "h_jpsi1", "h_jpsi1sel", "h_jpsi1sig");
		fillHistos(t[4], "jpsi2m", cuts["njpsi2"], "h_jpsi2", "h_jpsi2sel", "h_jpsi2sig");
		fillHistos(t[5], "etacm",  cuts["netac"],  "h_etac",  "h_etacsel",  "h_etacsig");
		fillHistos(t[6], "d0m",    cuts["nd0"],    "h_d0",    "h_d0sel",    "h_d0sig");
		fillHistos(t[7], "dm",     cuts["ndpm"],   "h_dpm",   "h_dpmsel",   "h_dpmsig");
	}
	
	if (sqrts>38)
		fillHistos(t[8], "dsm", cuts["nds"], "h_ds", "h_dssel", "h_dssig");
	
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
	double offset = 0.1;
	double pos=3.3, pos2=1.85;
	double hfac = 0.75;
	
// 	// *** plot all stuff and write out info
// 	c1->cd(1); h_pi0->Draw(); h_pi0sel->Draw("same"); h_pi0sig->Draw("same");
// 	c1->cd(2); h_ks->Draw(); h_kssel->Draw("same"); h_kssig->Draw("same");

	c2->cd(2); h_phi->Draw(); h_phisel->Draw("same");   h_phisig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[0]);
	latex.DrawLatex(pos2,h_phi->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(1); h_ee->Draw(); h_eesel->Draw("same"); h_eesig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[2]);
	latex.DrawLatex(pos*1.2,h_ee->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(9); h_lam->Draw(); h_lamsel->Draw("same"); h_lamsig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[1]);
	latex.DrawLatex(pos2,h_lam->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(3); h_etac->Draw(); h_etacsel->Draw("same"); h_etacsig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[5]);
	latex.DrawLatex(pos,h_etac->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(4); h_jpsi1->Draw(); h_jpsi1sel->Draw("same"); h_jpsi1sig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[3]);
	latex.DrawLatex(pos,h_jpsi1->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(5); h_jpsi2->Draw(); h_jpsi2sel->Draw("same"); h_jpsi2sig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[4]);
	latex.DrawLatex(pos,h_jpsi2->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(7); h_dpm->Draw();  h_dpmsel->Draw("same");  h_dpmsig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[7]);
	latex.DrawLatex(pos,h_dpm->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(6); h_d0->Draw();   h_d0sel->Draw("same");   h_d0sig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[6]);
	latex.DrawLatex(pos,h_d0->GetMaximum()*hfac+offset,tmp);

	c2->cd(8); h_ds->Draw();   h_dssel->Draw("same");   h_dssig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[8]);
	latex.DrawLatex(pos,h_ds->GetMaximum()*hfac+offset,tmp);
	
	c2->cd(10); h_lamc->Draw(); h_lamcsel->Draw("same"); h_lamcsig->Draw("same");
	sprintf(tmp,"#epsilon = %4.1f%%",eff[9]);
	latex.DrawLatex(pos,h_lamc->GetMaximum()*hfac+offset,tmp);
	
	// total efficiency in first plot (phi)
	c2->cd(5);
	double posy = h_jpsi2->GetMaximum()*1.12;
	if (posy==0) posy=1.12;
	sprintf(tmp,"(#epsilon_{t} = %4.1f%%)",double(evCnt)/double(nev)*100.);
	latex.SetTextColor(kMagenta+2);
	latex.DrawLatex(3.0,posy,tmp);
	
	c2->cd();
	
	TString plot(fname(0,9));
	if (!tagonly) plot+="_opt";
	
	c2->SaveAs("fig/"+plot+"_eff90.gif");
	c2->SaveAs("fig/"+plot+"_eff90.pdf");
	
	std::ofstream ofs;
	
	ofs.open("test_eff90.txt", std::ofstream::out | std::ofstream::app);
	TString result=TString::Format("%s & %3.1f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f & %6.2f \\\\\n",
								   mode.Data(), sqrts/10., eff[2], eff[0], eff[5], eff[3], eff[4], eff[6], eff[7], eff[8], eff[1], eff[9], evCnt/nev*100);
	//cout <<result;							
	ofs <<result;
	ofs.close();
}


