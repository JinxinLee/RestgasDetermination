#include "TMVA/Factory.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TRegexp.h"
#include "TEventList.h"
#include "TLeaf.h"

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <utility>


int gettype(TTree *t, TString varname)
{
	if (t->GetBranch(varname)==0) return -1;
	
	TString leaftype = t->GetLeaf(varname)->GetTypeName();
	
	if (leaftype=="Float_t")	 return 0; 
	else if (leaftype=="Int_t")	 return 1;  
	else if (leaftype=="Bool_t") return 2;
	
	return -1;
}

// ---------------------------------------------------------------

int SplitString(TString s, TString delim, TString *toks, int maxtoks)
{
	TObjArray *tok = s.Tokenize(delim);
	int N = tok->GetEntries();	
	for (int i=0;i<N;++i) 
		if (i<maxtoks) 
		{
			toks[i] = ((TObjString*)tok->At(i))->String();
			toks[i].ReplaceAll("\t","");
			toks[i] = toks[i].Strip(TString::kBoth);
		}
	return N;
}

// ---------------------------------------------------------------
// for a string containing a cut, returns only the variables used,
// separated by blanks
TString getFromCut(TString vars)
{
	TString toks[50];
	int n=SplitString(vars, "&&", toks, 50);
	TRegexp rvar("[_a-zA-Z][_a-zA-Z0-9]*");
	
	TString res=" ";
	
	for (int i=0;i<n;++i)
	{
		TString v = toks[i](rvar);
		if (v!="")
		{
			if (res.Contains(" "+v+" ")) continue;
			res+=v+" ";
		}
	}
	
	res = res.Strip(TString::kBoth);
	
	return res;
}

// ---------------------------------------------------------------

void TMVATrainer(TString fname="", TString treename="", TString sigcut="", TString vars="", TString precut="")
{
	if ( fname=="" || treename=="" || sigcut=="" || vars=="" ) 
	{
		cout << "USAGE:\n";
		cout << "TMVATrainer.C( <input>, <tree>, <sigcut>, <vars>, [precut] )\n\n";
		cout << "   <input>   : input file name containing TTree <tree>\n";
		cout << "   <tree>    : name of the TTree containing signal and background\n";
		cout << "   <sigcut>  : cut separating signal from background -> bgcut = !(sigcut)\n";
		cout << "   <vars>    : list with variables for training\n";
		cout << "   [precut]  : optional precut before training; has to be applied also before testing!'\n\n";
		return;
	}
	
	if (vars.Contains("&&")) vars = getFromCut(vars);
	cout <<"Vars : "<<vars<<endl;
	
	TString bkgcut="!("+sigcut+")";
	
	if (precut!="")
	{
		cout <<"Precut : "<<precut<<endl;
		sigcut += "&&" + precut;
		bkgcut += "&&" + precut;
	}
	
	TFile *f = TFile::Open(fname);
	TTree *t =(TTree*) f->Get(treename);
	
	TString outfname = fname;
	outfname.ReplaceAll(".root","");
	TString tmvaname = outfname+"_"+treename;
	outfname=outfname+"_tmva.root";
	
	TFile* outputFile = TFile::Open(outfname, "RECREATE" );
	
	TMVA::Factory *factory = new TMVA::Factory( tmvaname, outputFile, "Silent:!V:Transformations=I;N;D");

	TString toks[30];
	int N = SplitString(vars," ",toks,30);
	
	for (int i=0;i<N;++i)
	{
		int btype = gettype(t, toks[i]);
		if (btype==0 || btype==1)
			factory->AddVariable(toks[i], btype==0?'F':'I');
	}   
	
	factory->AddTree( t, "Signal",     1.0, sigcut.Data());
	factory->AddTree( t, "Background", 1.0, bkgcut.Data());
	
	int nsig = t->GetEntries(sigcut);
	int nbkg = t->GetEntries(bkgcut);
	
	factory->PrepareTrainingAndTestTree( "", int(nsig*0.8), int(nbkg*0.8), int(nsig*0.19), int(nbkg*0.19)); 

//  	factory->BookMethod( TMVA::Types::kLikelihood, "Likelihood","!V:NAvEvtPerBin=50" );
//     factory->BookMethod( TMVA::Types::kMLP, "MLP", "!V:NCycles=50:HiddenLayers=10,10:TestRate=5" );

	// new verion 4.2.0
//	factory->BookMethod( TMVA::Types::kBDT, "BDT", "!V:nTrees=400:BoostType=AdaBoost:nCuts=10:MaxDepth=3:UseFisherCuts:DoPreselection" );
	// old verion 4.1.3
	factory->BookMethod( TMVA::Types::kBDT, "BDT", "!V:nTrees=400:BoostType=AdaBoost:nCuts=10:NNodesMax=10" );
	
	factory->TrainAllMethods();  
	//factory->TestAllMethods();
	//factory->EvaluateAllMethods();
	
	outputFile->Close();
	delete factory;
}
