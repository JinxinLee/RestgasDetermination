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
#include <fstream>
#include "TLine.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TObjArray.h"
#include "TPRegexp.h"
#include "TRegexp.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"

#include <map>
#include <utility>
#include <algorithm>

typedef std::map<int, int> CountMap;

std::map<int, TString> cuts, files;
std::map<int, double> effs, effb;
std::vector<int> modes;

CountMap gevtcnt, evtcnt, evcntrec[10];

Int_t ev, run, mode, rec, nbranch;

// ---------------------------------------------------------------

void init(TTree *t)
{
	t->SetBranchAddress("ev",&ev);
	t->SetBranchAddress("run",&run);
	t->SetBranchAddress("mode",&mode);
	t->SetBranchAddress("recmode",&rec);
}

// ---------------------------------------------------------------

int uid(int lev, int lrun, int lmode)
{
	return lev+10000*lrun+(((lmode/100)%10)*20+lmode%10)*100000;
}

// ---------------------------------------------------------------

int countEvents(TTree *t, TEventList &el)
{
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("ev",1);
	t->SetBranchStatus("run",1);
	t->SetBranchStatus("mode",1);
	t->SetBranchStatus("recmode",1);
		
	evtcnt.clear();
	for (int j=0;j<10;++j) evcntrec[j].clear();
	
	for (int i=0;i<el.GetN();++i)
	{
		t->GetEntry(el.GetEntry(i));
		
		evtcnt[uid(ev,run,mode)]  += 1;
		gevtcnt[uid(ev,run,mode)] += 1;
		
		if (rec<10) evcntrec[rec][uid(ev,run,mode)]+=1;
	}
	t->SetBranchStatus("*",1);

	return evtcnt.size();
}

// ---------------------------------------------------------------

int readmodes(TString fname)
{
	ifstream f(fname.Data());
	if (!f.is_open()) 
	{
		cout <<"Cannot read file "<<fname.Data()<<endl;
		return 0;
	}

	TString sline;
	int cnt = 0;
	
	TRegexp reffs("eff_s = [0-9]+.[0-9]+");
	TRegexp reffb("eff_b = [0-9]+.[0-9]+");
	
	modes.clear();
	
	// loop through file line by line
	while (!f.eof())
	{
		sline.ReadLine(f);
		sline = sline.Strip(TString::kBoth);
		
		if (sline=="") continue;
		
		//cout <<sline<<endl;
		
		if (sline.BeginsWith("#"))
		{
			int lmode = TString(sline(2,6)).Atoi();
			if (sline(reffs)!="")
				effs[lmode] = TString(TString(sline(reffs))(8,100)).Atof();
			if (sline(reffb)!="")
				effb[lmode] = TString(TString(sline(reffb))(8,100)).Atof();
			//cout <<lmode<<" "<<effs[lmode]<<" "<<effb[lmode]<<endl;
		}
		else
		{
			int lmode = TString(sline(0,6)).Atoi();
			modes.push_back(lmode);
			cuts[lmode] = sline(9,500);	
			//cout <<lmode<<" "<<cuts[lmode].Data()<<endl;
			cnt++;
		}
	}
	
	f.close();
	
	return cnt;
}

void readdir()
{
	TSystemDirectory dir("*.root",".");

	TList *list = dir.GetListOfFiles();

	int nent = list->GetEntries();
	
	TRegexp reg("^M[0-9]+_n[0-9]+.+root");

	for (int i=0; i<nent; ++i)
	{
		TString fname = ((TSystemFile*) list->At(i))->GetName();
		TString mpat = fname(reg);
		
		if (mpat)
		{
			int lmode = TString(mpat(1,3)+mpat(6,3)).Atoi();
			files[lmode] = fname;
		}
	}
}
// ---------------------------------------------------------------


int simubg(TString cfgfile)
{
	int nmodes = readmodes(cfgfile);
	readdir();
	
	if (nmodes==0) return;
	
	TRegexp rtname("n[0-9][0-9][0-9]");
	
	double sumeffb=0., geffb=0.;
	
	TCanvas *c1=new TCanvas("c1","c1",5,5,1800,800);
	c1->Divide(10,6);
	
	gevtcnt.clear();
	
	for( int i=0; i<modes.size(); ++i)
	{
		int lmode = modes[i];
		
		sumeffb += effb[lmode];
		
		TFile f(files[lmode],"READ");
		TString tname = files[lmode](rtname);
		TTree *t = (TTree*) f.Get(tname);
		
		if (t) 
		{
			init(t);
			TString cut = cuts[lmode]+"&&mode%1000==900";
			TEventList el("el");
			t->Draw(">>el",cut);
			
			//cout <<"el:"<<el.GetN()<<" ev:"<<countEvents(t, el)<<" ";
			countEvents(t, el);
			geffb = (double)gevtcnt.size()/3e4;
			
			t->Draw(">>el",cuts[lmode]);
			t->SetEventList(&el);
			
			c1->cd(i%60+1);
			t->SetLineColor(1); t->Draw("xm");
			t->SetLineColor(2); t->Draw("xm","xmct","same");
			t->SetLineColor(4); t->Draw("xm","mode%1000==900","same");
			c1->Update();
			TLatex lat; lat.SetTextSize(0.09);
			lat.DrawLatex(gPad->GetUxmin()+(gPad->GetUxmax()-gPad->GetUxmin())*0.05,gPad->GetUymax()*0.85,TString::Format("(%02d) %d",i+1,lmode%1000));
			lat.DrawLatex(gPad->GetUxmin()+(gPad->GetUxmax()-gPad->GetUxmin())*0.05,gPad->GetUymax()*0.73,TString::Format("S:%4.2f%%",effs[lmode]));
			lat.DrawLatex(gPad->GetUxmin()+(gPad->GetUxmax()-gPad->GetUxmin())*0.05,gPad->GetUymax()*0.61,TString::Format("B:%4.3f%%",effb[lmode]));
			c1->Update();
		}
		
		printf("(%2d) %d (%4.1f, %6.3f; %6.3f; %6.3f) %s : %s\n",i+1, lmode, effs[lmode], effb[lmode], sumeffb, geffb, files[lmode].Data(), cuts[lmode].Data());  
	}
	
	printf("\nSUM(effb) = %6.3f%%   SIMU(effb) = %6.3f%%\n",sumeffb, geffb);
  return 0;
}



