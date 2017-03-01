#include <vector>
#include <fstream>
#include <iostream>

#include "TChain.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TString.h"
#include "TRegexp.h"
#include "TLine.h"
#include "TArrow.h"

using std::cout;
using std::endl;

typedef std::vector<TString> StrArr;
typedef std::map<TString,int> VarHash;
typedef std::map<TString, std::vector<double> > VarCutMap;
typedef std::map<TString, std::vector<double> >::iterator VarCutMapIt;

StrArr varlist;
VarHash vhash;

VarCutMap lecutmap, gecutmap;

int SplitString(TString s, TString delim, StrArr &toks)
{
	toks.clear();
	
	TObjArray *tok = s.Tokenize(delim);
	int N = tok->GetEntries();	

	for (int i=0;i<N;++i) 
	{
		TString token = ((TObjString*)tok->At(i))->String();
		token.ReplaceAll("\t","");
		token = token.Strip(TString::kBoth);
		toks.push_back(token);
	}
	return N;
}

bool readConfig(TString fname, int mode)
{
	ifstream file(fname.Data());
	if (!file.is_open()) return false;

	varlist.clear();
	vhash.clear();
	lecutmap.clear();
	gecutmap.clear();

	char line[1500];

	StrArr sar1, sar2;
	
	// loop through file line by line
	while (!file.eof())
	{
		file.getline(line,1499);
		TString sline(line);
		
		// remove tabs everywhere
		sline.ReplaceAll("\t","");
		// if lines contains a '#' somewhere, cut string from that position (=comment)
		if (sline.Contains("#")) sline = sline(0,sline.Index("#")-1);
		// remove whitespace at begin and end
		sline = sline.Strip(TString::kBoth);
		
		if (sline=="") continue;
		
		SplitString(sline,":",sar1);
		
		int cmode  = (sar1[0].Atoi())%1000;
		double sqs = (sar1[0].Atof())/100000.;
		sqs = int(sqs*100)/100.;

		if (cmode != mode) continue;
		
		cout <<sline<<endl;
		
		SplitString(sar1[1],"&&",sar2);
		
		for (int i=0;i<sar2.size();++i)
		{
			TString var=sar2[i], cut=sar2[i];
			cut.ReplaceAll("=","");
			double gecut=-99999., lecut=-99999.;
			
			if (sar2[i].Contains(">")) 
			{
				var=sar2[i](0,sar2[i].Index(">"));
				gecut = TString(cut(cut.Index(">")+1,1000)).Atof();
			}
			
			else if (sar2[i].Contains("<")) 
			{
				var=sar2[i](0,sar2[i].Index("<"));
				lecut = TString(cut(cut.Index("<")+1,1000)).Atof();
			}

			//cout <<sqs<<" "<<"cut:"<<cut<<" le:"<<lecut<<" ge:"<<gecut<<endl;	
			
			if ( lecutmap.find(var)==lecutmap.end() && var!="tag" ) 
			{
				varlist.push_back(var); 
				//vhash[var]=1;
				std::vector<double> t;
				lecutmap[var] = t;
				gecutmap[var] = t;
			}
			
			if (lecut>-99999) {lecutmap[var].push_back(sqs); lecutmap[var].push_back(lecut);}
			if (gecut>-99999) {gecutmap[var].push_back(sqs); gecutmap[var].push_back(gecut);}
		}
	}
	
	file.close();
	
	return true;
}

int inspectmode(int mode, TString opt="cont", TString selcfg = "../../softrig/selection_fsim_dec2014.cfg")
{
	TString path="/data.local1/kgoetzen/data/softrig/fsim/merge4/shr/"; 
	
	TChain n(TString::Format("n%3d",mode));
	n.Add(TString::Format("%sM*_n%3d_*root",path.Data(),mode));

	TCanvas *c1=new TCanvas("c1","c1",10,10,800,600);

	if (!readConfig(selcfg,mode)) return;
	
	cout <<"vars("<<mode<<")"<<endl;
	
	TLine l1;
	l1.SetLineWidth(2);
	l1.SetLineColor(2);

	TArrow ar1;
	ar1.SetAngle(40);
	ar1.SetLineWidth(2);
	ar1.SetLineColor(2);
	ar1.SetFillColor(2);
	
	for (int i=0;i<varlist.size();++i) 
	{
		TString var = varlist[i];
		cout <<var<<""<<endl;
		
		double min = n.GetMinimum(var), max = n.GetMaximum(var), d=max-min;
		
		min-=0.05*d;
		max+=0.05*d;
		
		TH2F *h=new TH2F("h",var+" (mode "+TString::Format("%d",mode)+")",360,2.2,5.8,200,min,max);
		h->SetStats(0);
		h->SetXTitle("E_{cm} [GeV]");
		h->SetYTitle(var);
		
		n.Draw(var+":mode/1e8-0.13+recmode*0.01>>h","xmct");
		n.Draw(var+":mode/1e5+0.05>>+h","mode%1000==900");
		n.Draw(var+":mode/1e5+0.03>>+h","mode%1000==900");
		n.Draw(var+":mode/1e5+0.04>>+h","mode%1000==900");
		n.Draw(var+":mode/1e5+0.06>>+h","mode%1000==900");

		h->SetContour(50);
		h->Draw(opt);
		
		for (int j=0;j<lecutmap[var].size();j+=2) 
		{
			double sqs = lecutmap[var][j], val = lecutmap[var][j+1];
			printf("  sqs=%4.2f : %s<%f\n", sqs, var.Data(), val);
			l1.DrawLine(sqs-0.15, val, sqs+0.15, val);
			ar1.DrawArrow(sqs,val,sqs,val-d*0.05,0.015,"|>");
		}
		
		for (int j=0;j<gecutmap[var].size();j+=2) 
		{
			double sqs = gecutmap[var][j], val = gecutmap[var][j+1];
			printf("  sqs=%4.2f : %s>%f\n", sqs, var.Data(), val);
			l1.DrawLine(sqs-0.15, val, sqs+0.15, val);
			ar1.DrawArrow(sqs,val,sqs,val+d*0.05,0.015,"|>");
		}
		
		/*	
		TH2F *h=new TH2F("h",var,70,2.3,5.8,200,min,max);
		h->SetStats(0);
		
		n.Draw(var+":mode/1e8+0.05>>h","xmct",opt);
		n.Draw(var+":mode/1e5-0.05>>+h","mode%1000==900",opt);

		h->SetContour(50);
		
		*/
		c1->Update();
		c1->SaveAs(TString::Format("fig/mode%d_%s.gif",mode,var.Data()));
		
		
	}
	cout <<endl;
	
	
  return 0;
}
