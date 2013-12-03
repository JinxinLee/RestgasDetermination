#include <map>
#include <vector>
#include <iostream>
#include "TFile.h"
#include "TSystemFile.h"
#include "TSystemDirectory.h"
#include "TList.h"
#include "TString.h"
#include "TTree.h"
#include "TEventList.h"

using std::cout;
using std::endl;

std::vector<TString> lfiles;
std::vector<TString> ntps;
std::map<TString, TString> cuts;
std::map<TString, TString> modes;

std::map<int, int> evcnt;


void init_cuts(TString energy)
{
	if (energy=="24")
	{
		cuts["nphi"] = "phipt>0.239374&&phid1pidpi<0.0114195&&npidkl>1.99";
		cuts["nlam"] = "fw1>0.2&&pla>0.15&&fw4>0.2&&prapmax<2.5";
	}
	else if (energy=="38")
	{
		cuts["nphi"]  = "phipcm>1.02899&&phipt>0.699287&&phid0pidk>0.40629";
		cuts["nlam"]  = "thr>0.908013&&lampt>0.803121&&prapmax<3";
		cuts["njpsi"] = "jpsipcm<0.7";
		cuts["nd01"]  = "d0pcm<0.386277&&d0d0pidk>0.661042";
		//cuts["nd02"]  = "d0d0pidk>0.9&&d0pcm<0.292463";
		//cuts["nd02"]  = "d0d0pidk>0.8&&d0pcm<0.402238";
		cuts["ndpm1"] = "dpmpcm<0.292203&&dpmd0pidk>0.630705";
		//cuts["ndpm2"] = "dpmd0pidk>0.792914&&dpmpcm<0.3";
		//cuts["ndpm3"] = "dpmpcm<0.381059&&prapmax<3";
		//cuts["ndpm4"] = "dpmpcm<0.4&&prapmax<3";
	}
	else if (energy=="45")
	{
		cuts["nphi"]  = "phipcm>1.376&&fw2>0.631154&&npidkl>1";
		cuts["nlam"]  = "fw2>0.603771&&lampt>0.881976&&fw2>0.8";
		cuts["njpsi"] = "npt10>1&&jpsid0pidpi<0.2&&jpsid1pidpi<0.2";
		cuts["nd01"]  = "npt10>0&&d0d0pidk>0.453262&&prapmax<3";
		//cuts["nd02"]  = "d0d0pidk>0.9&&d0pcm<0.292463";
		//cuts["nd02"]  = "d0d0pidk>0.8&&d0pcm<0.402238";
		cuts["ndpm1"] = "dpmd0pidk>0.5&&dpmpt>1&&prapmax<3&&dpmd0p>1.2";
		//cuts["ndpm2"] = "dpmd0pidk>0.792914&&dpmpcm<0.3";
		//cuts["ndpm3"] = "dpmpcm<0.381059&&prapmax<3";
		//cuts["ndpm4"] = "dpmpcm<0.4&&prapmax<3";
		cuts["nds1"]  = "npidkl>1&&dsd1pidk>0.5&&dsd0pidk>0.5&&dsd1p>1.6";
	}
	else if (energy=="50")
	{
		cuts["nphi"]   =  "phipcm>1.75195&&fw2>0.654609&&phid1pidk>0.292159";
		cuts["nlam"]   =  "fw2>0.8&&lampcm>2.15559&&lampt>1";
		cuts["njpsi"]  =  "jpsipcm<1.73971&&jpsid1pidpi<0.2";
		cuts["nd01"]   =  "d0d0pidk>0.394868&&prapmax<2.88343";
		cuts["ndpm1"]  =  "dpmd0pidk>0.4&&dpmpt>0.7&&prapmax<3";
		cuts["nds1"]  =   "npidkl>1&&dsd1pidk>0.642251&&dsd1p>1&&dsd0pidk>0.5";
		cuts["nlamc"] =  "abs(lamcpcm-1)<0.1&&lamcd1pidk>0.5&&lamcd0pidp>0.5";
		cuts["netac1"]=  "etacd1pidk>0.389332&&etacd0pidk>0.4";
	}
	else if (energy=="55")
	{
	}
}

void init_modes()
{
	modes["nphi"] = "00";	
	modes["nlam"] = "31";	
	modes["njpsi"] = "21";
	modes["nd01"] = "18";	modes["nd02"] = "182";	modes["nd03"] = "183";
	modes["ndpm1"] = "14";	modes["ndpm2"] = "142";	modes["ndpm3"] = "143";	modes["ndpm4"] = "144";
	modes["nds1"] = "17";	modes["nds2"] = "172";
	modes["nlamc"] = "34";
	modes["netac1"] = "70";	modes["netac2"] = "71";	modes["netac3"] = "72";	modes["netac4"] = "73";
	modes["nchic01"] = "80";modes["nchic02"] = "81";
	
	modes["DPM"] = "50";
}

void get_tuples(TString beg="")
{
   TSystemDirectory dir(".",".");
   TList *files = dir.GetListOfFiles();
   if (files) 
   {
      TSystemFile *file;
      TString fname;
      TIter next(files);
      while ((file=(TSystemFile*)next())) 
	  {
         fname = file->GetName();
         if (!file->IsDirectory() && fname.BeginsWith(beg) && fname.EndsWith(".root")) {
			TString nname=TString( fname(4,fname.Length()-9));
            cout<< "Found " << fname.Data() <<endl; 
			if (nname!="ntp") 
			{
				if (cuts.count(nname)!=0){
					ntps.push_back(nname);
					lfiles.push_back(fname);
				}
			}
         }
      }
   }
}

int get_N0(TString fname, TString bgcut)
{
	TFile *f=new TFile(fname,"READ");
	TTree *n = (TTree*) f->Get("ntp");
	
	int N0 = n->GetEntries(bgcut);
	f->Close();
	delete f;
	return N0;
}

int cnt_events(TTree* t, TString cut, bool globcnt=false)
{
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
		sigcnt[(int)ev]+=1;
		if (globcnt) evcnt[(int)ev]+=1;
	}
	t->SetBranchStatus("*",1); 
	return sigcnt.size();
}

void simu_tag(TString energy="24")
{
	init_cuts(energy);
	init_modes();
	
	TString bgcut = "mode=="+energy+modes["DPM"];

	TString pref = "M"+energy+"_";
	get_tuples(pref);
	
	cout <<"\nn-tuples and cuts used: "<<endl;
	for (uint i=0;i<ntps.size();++i) cout << lfiles[i]<<", "<<ntps[i]<<" : " <<cuts[ntps[i]]<<" && tag && "<<bgcut<<endl;
	cout <<endl;
	
	int N0 = get_N0(pref+"ntp.root",bgcut);
	
	cout <<"Total Bkg N0 = "<<N0<<endl;
		
	int sum = 0;
	
	for (uint i=0;i<ntps.size();++i) 
	{
		TString sigcut = "mode=="+energy+modes[ntps[i]];
		
		int sigN0 = get_N0(pref+"ntp.root",sigcut);

		TFile *f=new TFile(lfiles[i],"READ");
		TTree *t= (TTree*)f->Get(ntps[i]);
		
		TString bcut=cuts[ntps[i]]+" && tag && "+bgcut;
		TString scut=cuts[ntps[i]]+" && tag && "+sigcut;
		
		int nbg  = cnt_events(t, bcut, true);
		int nsig = cnt_events(t, scut);
					
		double eff_bg = (double)nbg/N0;
		double eff_sg = (double)nsig/sigN0;
		
		TString ccut = cuts[ntps[i]];
		ccut.ReplaceAll("&&"," && ");
		
		TString sout = TString::Format("%8s : BG = %5d/%6d = %7.5f  SIG = %5d/%6d = %7.5f   cut : %s\n",
									   ntps[i].Data(), nbg, N0, eff_bg, nsig, sigN0, eff_sg, ccut.Data());
		
		cout <<sout.Data();
		sum+=nbg;
	
		f->Close();
		delete f;
	}
	
	double eff = evcnt.size()/(double)N0;
	cout <<"\n\nN0_BG = "<<N0<<"   SUM = "<<sum<<"   SIMU TAG = "<<evcnt.size()<<"   ->   EFF = "<<eff<<"   SUPR-FAC = "<<1./eff<<endl;
	
/*   for ( std::map< int, int, std::less< int > >::const_iterator iter = evcnt.begin();
      iter != evcnt.end(); ++iter )
      if (iter->second>1) cout << iter->first << '\t' << iter->second << '\n';*/
	
}
















