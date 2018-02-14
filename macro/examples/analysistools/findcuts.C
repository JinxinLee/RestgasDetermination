// --------------------------------------------------------------------------------------
//
// ******** LOAD with '.L findcuts.C+' to use as function in CINT ********
//
// Tool to find good selection for signal vs bkg. Ranks variables according to maximum (S*ws)^2/(S*ws+B*wb) on optionally normalised distributions.
// Double click on a pad applies corresponding cut and re-iterates.
//
// USAGE: findcuts(TTree *t, TString ctlvar, TString sigcut, TString bnames, TString precut, int numvars, double ws, double wb, int norm)
//
// 	  t       : tree containing signal and background; if given as only argument, list of available branches are printed.
// 	  ctlvar  : control variable to check signal quality (e.g. invariant mass); displayed in upper left pad; excluded from branch list.
// 	  sigcut  : cut to isolate signal; background is selected with !(sigcut).
// 	  bnames  : blank separated list of branch variable names to be considered; can make use of name* / *name /*name*; !(*)name(*) excludes variables (default = '*').
// 	  precut  : cut to be applied before variable ranking is done (default = '').
// 	  numvars : number of variables to be displayed (default = 9).
// 	  ws, wb  : weight factors for signal & background (default = 1.0); are applied to spectra and calculations after optional normalisation.
// 	  norm    : normalisation of distributions before ranking (-> pdf) (default = 1.0); double click in control variable pad toggles and reruns.
//    bins    : number of histogram bins, precision depending on binning (default = 500)
//
// --------------------------------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TEventList.h"
#include "TDirectory.h"
#include "TLine.h"
#include "TArrow.h"
#include "TStyle.h"
#include "TObjArray.h"
#include "TRegexp.h"

typedef std::vector<TString> StrVec;

typedef std::map<double, TString> DblStrMap;
typedef std::map<double, TString>::iterator DblStrMapIt;

typedef std::map<TString, double> StrDblMap;

typedef std::map<TString, int> StrIntMap;
typedef std::map<TString, int>::iterator StrIntMapIt;

TCanvas *c1;                    // canvas to display variables
TEventList *els, *elb, *elpre;  // event lists for signal/background/all
TTree *t;                       // the TTree

int N0s, N0b;                   // initial signal/background count
int Ns, Nb;                     // signal/background count after preselection
double Wsig, Wbkg;              // signal/background weights for computing S^2/(S+B)
TString comtemplate, comcurrent;// template command string for the TExec; and current command string
bool normalize;                 // flag for possible normalisation
int BINS;                       // histogram bins

StrVec bnam;                    // vector containing the branch names to be analysed
DblStrMap qamap;                // maps QA value -> cut
StrDblMap sigeff, bkgeff;       // maps cut -> signal/background efficiency

// -------------------------------------------
// Routine to print out the usage of this tool
void FCPrintInfo()
{
	cout <<"\nfindcuts: Tool to find good selection for signal vs bkg. Ranks variables according to maximum (S*ws)^2/(S*ws+B*wb) on optionally normalised distributions.\n";
	cout <<"          Double click on a pad applies corresponding cut and re-iterates.\n\n";
	cout <<"USAGE:\nfindcuts(TTree *t, TString ctlvar, TString sigcut, TString bnames, TString precut, int numvars, double qaopt, double ws, double wb, int norm, int bins)\n"; 
	cout <<"  t       : tree containing signal and background; if given as only argument, list of available branches are printed.\n";
	cout <<"  ctlvar  : control variable to check signal quality (e.g. invariant mass); displayed in upper left pad; excluded from branch list.\n";
	cout <<"  sigcut  : cut to isolate signal; background is selected with !(sigcut).\n";
	cout <<"  bnames  : blank separated list of branch/alias variable names to be considered; can make use of name* / *name /*name*; !(*)name(*) excludes variables (default = \"*\").\n";
	cout <<"  precut  : cut to be applied before variable ranking is done (default = \"\").\n";
	cout <<"  qaopt   : optimisation mode; qaopt=0 : best S/sqrt(S+B); -1<qaopt<0 : best signal eff for fixed bkg reduction; 0<qaopt<1 : best bkg reduction for fixed signal eff. (default = 0).\n";
	cout <<"  numvars : number of variables to be displayed (default = 9).\n";
	cout <<"  ws, wb  : weight factors for signal & background (default = 1.0); are applied to spectra and calculations after optional normalisation.\n";
	cout <<"  norm    : normalisation of distributions before ranking (-> pdf) (default = 1); double click in control variable pad toggles 'norm' and reruns.\n";
	cout <<"  bins    : number of histogram bins, precision depending on binning (default = 500).\n\n";
	cout <<"Example 0: Print all available branches/aliases in tree:\n";
	cout <<"  root [0] findcuts(mytree)\n\n"; 
	cout <<"Example 1: Analyse all branches in TTree 'mytree', control variable 'mass', and signal cut 'issignal':\n";
	cout <<"  root [0] findcuts(mytree, \"mass\", \"issignal\")\n\n";
	cout <<"Example 2: Analyse all branches expect 'evcnt', 'run', those ending with 'px', 'py', or 'pz' or containing the word 'fit', after precut 'goodtracks>6&&goodphotons>2'\n";
	cout <<"  root [0] findcuts(mytree, \"mass\", \"issignal\", \"* !evcnt !run !*p[xyz] !*fit*\", \"goodtracks>6&&goodphotons>2\")\n\n";
	return;
}

// -------------------------------------------
// Splits a TString into vector of strings; separation character contained in delim 
int FCSplitString(TString s, TString delim, StrVec &toks)
{
	toks.clear();
	TObjArray *tok = s.Tokenize(delim);
	int N = tok->GetEntries();	
	for (int i=0;i<N;++i) 
	{
		TString token = (((TObjString*)tok->At(i))->String()).Strip(TString::kBoth);
		toks.push_back(token);
	}
	return toks.size();
}

// -------------------------------------------
// Prepares the list of branches under consideration
// bnames contains branch list with jokers like *name or !*name* et
// branch ctlvar will be enabled for plotting, but excluded from the ranking list
// also takes into account aliases of TTree
void FCPrepareTree(TString bnames, TString ctlvar)
{
	// Separate names of branches to be considered
	StrVec vecpatt, vecali;
	FCSplitString(bnames," ", vecpatt);
	
	// Enable/disable branches
	t->SetBranchStatus("*",0);
	bnam.clear();  // this array will contain the names of variables (branches and aliases) to be checked
	
	// ******** BRANCHES
	
	// Enable/disable branches according to list of patterns in vecpatt
	for (uint i=0;i<vecpatt.size();++i)
		if (vecpatt[i].BeginsWith("!")) t->SetBranchStatus(TString(vecpatt[i](1,1000)),0);
		else t->SetBranchStatus(vecpatt[i],1);
		
	// for preparing list disable ctlvar
	if (ctlvar!="") t->SetBranchStatus(ctlvar,0);	
	
	// Create a vector with all variables to be considered
	TObjArray* blist = t->GetListOfBranches();
	for (int i=0; i<=blist->GetLast(); ++i)
	{
		TString name = ((TBranch*)blist->UncheckedAt(i))->GetName();
		// Add branch name to list, excludes the control variable 
		if (t->GetBranchStatus(name)>0) bnam.push_back(name); 
	}
		
	// for sure enable branch with ctlvar
	if (ctlvar!="") t->SetBranchStatus(ctlvar,1);	
	
	// ******** ALIASES
	
	// now we still need to check the list of aliases and enable the branches connected 
	StrIntMap alimap;  // this serves as a marker map for the active aliases
	TList *alilist = t->GetListOfAliases();
	
	// Are there aliases defined?
	if (alilist)
	{	
		// loop through all patterns
		for (uint i=0;i<vecpatt.size();++i)
		{
			// first create the regular exp to identify the aliases matching the patterns
			TString patt = vecpatt[i];
			patt.ReplaceAll("*",".*");
			int toggle = 1; // switch the alias on (1) or off (0) ?
			if (patt.BeginsWith("!")) {toggle = 0; patt=patt(1,1000);}
			TRegexp regali(patt);
			
			// save the alias status (on or off) in the map
			// therefore loop through alias list and toggle all those to either on or off, which match the pattern
			for (int j=0; j<=alilist->GetLast(); ++j)
			{
				TString aliname = ((TNamed*)alilist->At(j))->GetName();
				if (aliname(regali)!="") alimap[aliname] = toggle;
			}
		}
		
		// loop through map to save the enabled aliases in the bnam vector
		StrIntMapIt it = alimap.begin();
		while (it != alimap.end()) 
		{
			if (it->second) { vecali.push_back(it->first); bnam.push_back(it->first); }
			it++;
		}
		
		// now we finally need to enable all branches connected to one of the enabled aliases
		for (uint i=0;i<vecali.size();++i)
		{
			// get the formula to the enabled alias
			TString fml = t->GetAlias(vecali[i]);
			//cout << "Alias "<<vecali[i]<<" -> "<<fml<<endl;
			TRegexp regvar("[A-Za-z_][A-Za-z_0-9]*");
			while (fml(regvar)!="")
			{
				TString sbr = fml(regvar);
				if (t->GetBranch(sbr)) {t->SetBranchStatus(sbr,1);}
				fml.Replace(fml.Index(sbr),sbr.Length(),"");
			}
		}
	}
	
	// ******** REPORT
	
	// Print out all branches found
	cout<<"**** Name pattern: \""<<bnames<<"\""<<endl;
	cout<<"**** "<<bnam.size()<<" branches/aliases found: \n";
	for (uint i=0;i<bnam.size();++i) 
		printf("%s  ",bnam[i].Data());

	cout <<endl;
}

// -------------------------------------------
// Determines the minimum and maximum value for variable named var for current selecion (for histograms)
bool FCFindLimits(TString var, double &min, double &max)
{
	// consider signal and background after precut
	t->SetEventList(elpre);
	// draw into a dummy histogram; this creates value vectors in the tree
	int nsel = t->Draw(var+">>hhh","","goff");
	t->SetEventList(0);
	
	if (nsel>500000) nsel=500000;
	TH1F *h = (TH1F*)gDirectory->Get("hhh");
	// if no entry left return
	if (h->GetEntries()==0) return false;
	
	// now determine the limits; therefore loop through TTree::GetV1() 
	// being a vector with the values of var
	min=100000., max=-100000.;
	double *x=t->GetV1();
	for (int i=0;i<nsel;++i)
	{
		if (x[i]>max) max=x[i];
		if (x[i]<min) min=x[i];
	}
	// increase the upper limit by a tiny value to include this largest value
	// in the uppermost bin, which has a high bin edge '< max', not '<= max'
	max+=1e-6;
	return true;
}

// -------------------------------------------
// Compute significance
double FCSfc(double S, double B)
{
	return S/sqrt(S+B);
}

// -------------------------------------------
// The QA routine. It finds the best value ws*ws*S*S/(ws*S +wb*B) for cuts from left and right
// It stores the found cut strings in the qamap
double FCQaVar(TString var, double qaopt=0)
{
	double xmin, xmax;
	
	if (FCFindLimits(var, xmin, xmax))
	{
		TH1F h1("h1","",BINS,xmin,xmax);
		TH1F h2("h2","",BINS,xmin,xmax);

		// Fill histograms for var
		
		// signal histo
		t->SetEventList(els);
		t->Draw(var+">>h1","","goff");
		
		// background histogram
		t->SetEventList(elb);
		t->Draw(var+">>h2","","goff");
		
		// normalize histograms (if not significance optimisation, do anyways!)
		if (normalize || fabs(qaopt)>0.0001)
		{
			h1.Scale(1./h1.GetEntries());
			h2.Scale(1./h2.GetEntries());
		}
		
		// multiply histos with weights (only for significance optimisation)
		if (fabs(qaopt)<0.0001) {h1.Scale(Wsig); h2.Scale(Wbkg);}
		double h1ent = h1.Integral(), h2ent = h2.Integral();
		
		// some vars to store the best efficiencies, cuts, partial sums, etc
		double rsums=0., lsums=0., rsumb=0., lsumb=0.;
		double bestSr = 0., bestSl=0., bestCutl=0., bestCutr=0., besteffsl=0., besteffbl=0., besteffsr=0., besteffbr=0.;
		
		// integrate from both sides
		for (int i=1;i<=BINS;++i)
		{
			// sum the bin contents from left
			lsums+=h1.GetBinContent(i);   // signal
			lsumb+=h2.GetBinContent(i);   // background
			
			// sum the bin contents from right
			rsums+=h1.GetBinContent(BINS-i+1);  // signal
			rsumb+=h2.GetBinContent(BINS-i+1);  // background
			
			double Sl=0, Sr=0;
			// compute the significances; weights are already taken into account by scaling the histograms
			if (abs(qaopt)<1e-8) 
			{
				Sl = FCSfc(lsums,lsumb);
				Sr = FCSfc(rsums,rsumb);
			}
			// compute the signal efficiency for a certain background suppression
			else if (qaopt<0)
			{
				if (lsumb<=(1.+qaopt)) Sl = lsums;
				if (rsumb<=(1.+qaopt)) Sr = rsums;
			}
			// compute the signal efficiency for a certain background suppression
			else
			{
				if (lsums>=qaopt) Sl = 1.-lsumb;
				if (rsumb>=qaopt) Sr = 1.-rsumb;
			}
			
			// find the best ones
			if (Sl>bestSl) {bestSl=Sl; besteffsl=lsums/h1ent; besteffbl=lsumb/h2ent; bestCutl=h1.GetBinLowEdge(i+1);}
			if (Sr>bestSr) {bestSr=Sr; besteffsr=rsums/h1ent; besteffbr=rsumb/h2ent; bestCutr=h1.GetBinLowEdge(BINS-i+1);}
 		}
		
		// create cut strings
		TString lcut = TString::Format("%s<%f",var.Data(),bestCutl);
		TString rcut = TString::Format("%s>%f",var.Data(),bestCutr);
		
		// and store in map with 1/(QA value) as key (since map is sorted in increasing order)
		double invqal = 1./bestSl, invqar = 1./bestSr;
		
		// if a cut with the same qa exists, insignificantly increase 
		// current values slightly for being able to store them; otherwise map entries are replaced
		int maxsearch = 100;
		while (qamap.find(invqal)!=qamap.end() && --maxsearch>0) invqal = 1./(1./invqal+0.0001);
		
		maxsearch = 100;
		while (qamap.find(invqar)!=qamap.end() && --maxsearch>0) invqar = 1./(1./invqar+0.0001);
		
		// store the qa -> cut combination for left and right cut
		qamap[invqal] = lcut;
		qamap[invqar] = rcut;
		
		// store the corresponding efficiency values
		sigeff[lcut] = besteffsl; 
		bkgeff[lcut] = besteffbl;
		sigeff[rcut] = besteffsr; 
		bkgeff[rcut] = besteffbr;
	}

	return 0;
}

// -------------------------------------------
// Initialize style and find TCanvas if existing
// and set the event lists
void FCInit(int numvars)
{
	gStyle->SetTitleFontSize(0.08);
    gStyle->SetPadTopMargin(0.08);
	gStyle->SetOptStat(0);
	
	t->SetEventList(0);
	t->SetBranchStatus("*",1);

	// configure the TCanvas division and sizes of histos
	double plotdim = 250;
	if (numvars>11) plotdim=200;
	int cx = numvars+1;
	if (numvars>4)  cx=(numvars+2)/2;
	if (numvars>11) cx=5;
	if (numvars>14) cx=6;
	if (numvars>17) cx=7;
	
	int cy = (numvars+1)/cx;
	if ((numvars+1)%cx) cy+=1;
	
	// refetch canvas or create a new one
	c1=(TCanvas*)gROOT->FindObject("c1");	
	if (c1==0) c1 = new TCanvas("c1","c1",5,5,1000,650);
	c1->Clear();
	c1->Divide(cx, cy);
	c1->SetWindowSize(cx*plotdim, cy*plotdim*1.1);
	
	// refetch event lists if existing or create new ones
	els = (TEventList*)gROOT->FindObject("els");
	if (els==0) els = new TEventList("els");
	
	elb = (TEventList*)gROOT->FindObject("elb");
	if (elb==0) elb = new TEventList("elb");

	elpre = (TEventList*)gROOT->FindObject("elpre");
	if (elpre==0) elpre = new TEventList("elpre");
}

// -------------------------------------------
// Displays variable in pad #numpad in standard canvas
void FCDrawVariable(TString var, int numpad, int norm=1, TString cut="")
{
	double xmin, xmax;
	
	c1->cd(numpad);
	
	// if pad #1 (control variable), background color is set differently 
	if (numpad==1)
	{
		gPad->SetFrameFillColor(18);
		TString com = TString::Format(comcurrent.Data(),!normalize); // toggle normalize
		gPad->AddExec("ex1","if (gPad->GetEvent()==kButton1Double) { cout<<endl<<\" -> Toggle normalize setting.\"<<endl;"+com+";}");
	}
	
	// for all other pads a TExec is added which when clicked applies
	// the corresponding cut 
	if (numpad!=1)
	{
		// create the command string from the template string
		TString com = comtemplate;
		// isolate the part  "var>" or "var<" from the full cut string; 
		// this is to replace a previous cut on identical variable and direction (<,>)  
		TString cutpart = cut(0,max(cut.Index(">"),cut.Index("<"))+1);
		TRegexp r1(cutpart+"[-]*[0-9]*.[0-9]*");
		TString oldcut = com(r1);
		
		// if there was no such identical variable cut, just replace the 
		// keyword 'NEWCUT' in template with the new cut string
		if (oldcut=="")	com.ReplaceAll("NEWCUT",cut);
		else  // otherwise replace old cut with new cut and remove 'NEWCUT' or '&&NEWCUT' from template
		{
			com.ReplaceAll(oldcut,cut);
			com.ReplaceAll("&&NEWCUT","");
			com.ReplaceAll("NEWCUT","");
		}
		// finally add TExec to the current pad
		gPad->AddExec("ex1","if (gPad->GetEvent()==kButton1Double) { cout<<endl<<\" -> Applying '"+cut+"'\"<<endl;"+com+";}");
	}
	
	// set title for histogram
	TString title = cut;
	if (title=="") title=var;
	
	// prepare line and arrow for drawing cut 
	TLine ln;
	ln.SetLineColor(6);	ln.SetLineStyle(7); ln.SetLineWidth(2);
	
	TArrow arr;
	arr.SetAngle(40); arr.SetLineWidth(2); arr.SetLineColor(6); arr.SetFillColor(6);
	
	bool drawcut = (cut!="");
	int cutdir = cut.Contains("<") ? -1 : 1 ;
	
	// determind value of cut for displaying the line and arrow
	double cutval = drawcut ? TString(cut(max(cut.Index(">"),cut.Index("<"))+1,1000)).Atof() : -999.;
		
	if (FCFindLimits(var, xmin, xmax))
	{
		// histograms for signal and background
		TH1F h1("h1",title,BINS,xmin,xmax);
		TH1F h2("h2",title,BINS,xmin,xmax);
		// and the sum
		TH1F hs("hs",title,BINS,xmin,xmax);
		
		h1.SetLineColor(4);
		h2.SetLineColor(2);
		hs.SetLineColor(1);
		
		// fill histograms
		t->SetEventList(els);
		t->Draw(var+">>h1");
		t->SetEventList(elb);
		t->Draw(var+">>h2");
		
		// otherwise normalize histos
		if (norm)
		{
			h1.Scale(1./h1.GetEntries());
			h2.Scale(1./h2.GetEntries());
		}
		h1.Scale(Wsig); h2.Scale(Wbkg);
		
		// if we want no normalization fill a summed histogram
		if (!norm) {hs.Add(&h1);hs.Add(&h2);}
		
		// set maximum of both histos to display full y-range
		double maxi = 1.05*max(h1.GetMaximum(), h2.GetMaximum());
		if (!norm) maxi = hs.GetMaximum()*1.05;
		h1.SetMaximum(maxi);
		h2.SetMaximum(maxi);
		
		if (!norm) hs.DrawCopy();
		h1.DrawCopy(norm?"":"same");
		h2.DrawCopy("same");
		
		// draw the cut with direction arrow if requested
		if (drawcut) 
		{
			ln.DrawLine(cutval,0,cutval,maxi*0.9);
			arr.DrawArrow(cutval,maxi*0.9,cutval+0.1*(xmax-xmin)*cutdir,maxi*0.9,0.01,"|>");
		}
	}
	
	c1->cd();
}

// -------------------------------------------
// The main function
// -------------------------------------------
int findcuts(TTree *theTree=0, TString ctlvar="", TString sigcut="", TString bnames="", TString precut="", int numvars=9, double qaopt=0., double ws=1., double wb=1., int norm=1, int bins=500)
{
	// if no argument given, print usage information
	if (theTree==0) {FCPrintInfo();return;} 
	t=theTree;
	
	// check for reasonable qaopt parameter -1<qaopt<1
	if (qaopt<-1) qaopt=-1;
	if (qaopt>1)  qaopt=1;
	cout <<"\nOptimising for ";
	if (qaopt<0) cout <<"best signal effciency for given background reduction of "<<fabs(qaopt)<<endl;
	if (qaopt>0) cout <<"best background reduction for given signal efficiency of "<<qaopt<<endl;
	else cout <<"best significance S/sqrt(S+B)"<<endl;
	
	// set defaults for some parameters
	if (numvars==0) numvars = 9;
	
	// print some status information about the TTree
	TString tname = t->GetName();
	cout <<"\nTTree           : "<<tname<<"\nEvents total    : "<<t->GetEntriesFast()<<"\nBranches total  : "<<t->GetNbranches()<<"\nAliases total   : ";
	cout<< (t->GetListOfAliases() ? t->GetListOfAliases()->GetSize() : 0) <<endl;
	
	// the current command string; will be printed at the end
	comcurrent = TString::Format("findcuts(%s,\"%s\",\"%s\",\"%s\",\"%s\",%d,%.3f,%.3f,%.3f,%%d,%d)",
										 tname.Data(),ctlvar.Data(), sigcut.Data(),bnames.Data(),precut.Data(),numvars, qaopt, ws, wb, bins);
	
	// The template string for pasting the new cut in
	comtemplate = TString::Format("findcuts(%s,\"%s\",\"%s\",\"%s\",\"%s%sNEWCUT\",%d,%.3f,%.3f,%.3f,%d,%d)",
								tname.Data(), ctlvar.Data(), sigcut.Data(),bnames.Data(),precut.Data(),precut==""?"":"&&",numvars, qaopt, ws, wb, norm, bins);
	
	// catch some special cases
	// if no sigcut is provided, only the list of branches is printed out
	bool printonly = (sigcut=="");
	
	// empty string for branch names defaults to all branches -> '*'
	if (bnames=="") bnames="*";
	if (precut=="") precut="1";
	Wsig = ws; Wbkg = wb;
	
	// rank with normalized spectra; else use real event numbers
	normalize = norm;
	BINS = bins;
	
	// prepare signal and background cut
	TString bkgcut = "!("+sigcut+")";
	TString sfull  = precut+"&&"+sigcut;
	TString bfull  = precut+"&&"+bkgcut;
		
	// prepare event lists for signal and background
	if (!printonly)
	{
		// refetch canvas and eventlists
		FCInit(numvars);
		
		t->Draw(">>els",sfull);
		t->Draw(">>elb",bfull);
		t->Draw(">>elpre",precut);
		
		Ns  = els->GetN();
		Nb  = elb->GetN();
		N0s = t->GetEntries(sigcut); 
		N0b = t->GetEntries(bkgcut); 
	
		cout <<"Events selected : "<<elpre->GetN()<<endl;
	}
	cout <<endl;
	
	// select variables and fill StrVec bnam with selected branch names
	// ctlvar will be excluded from the ranking
	FCPrepareTree(bnames,ctlvar);		
	
	if (printonly) return;
	
	if (t->GetBranch(ctlvar)==0) {cout <<"**** Unknown control variable: "<<ctlvar<<endl<<endl; return;}
	
	// Draw the control variable in 1st pad
	if (ctlvar!="") FCDrawVariable(ctlvar,1,0,"ctlvar: "+ctlvar);
	
	// if at least one event signal and background is left do ranking
	if (Nb>0 && Ns>0) 
	{
		// prepare the map for ranking
		qamap.clear();
		
		// print progress bar and prepare some variables
		cout <<"+-----------+-----------+\n";
		double ticks = 0; 
		
		// **** do the actual ranking procedure by analysing all variables
		for (uint i=0;i<bnam.size();++i) 
		{
			if (bnam[i]!=ctlvar) FCQaVar(bnam[i], qaopt);
			// print progress bar
			while ((double)i/(double)bnam.size()>ticks/25.) { cout <<"#"<<flush; ticks++; }
		}
		cout <<endl<<endl;
		
		// print out and display the results. qamap is sorted by its key being the qa value itself!
		DblStrMapIt it = qamap.begin();
		int cnt=0;
		
		while (it != qamap.end() && cnt++<numvars) 
		{
			double  qa = it->first;
			TString cut = it->second;
		
			if (sigeff[cut]<=bkgeff[cut]) continue;
		
			printf("(%2d) %-25s : qa = %6.3f   (eff_s = %5.3f, eff_b = %5.3f)\n", cnt, cut.Data(), 1./qa, sigeff[cut], bkgeff[cut]);
			
			// isolate the variable name from the cut and plot
			TString var = cut(0,max(cut.Index(">"),cut.Index("<")));				
			FCDrawVariable(var,cnt+1,normalize,cut);
			
			++it;
		}
	}
	else cout <<"\n**** No "<<(Nb==0?"background":"signal")<<" events left. No ranking possible."<<endl;
	
	t->SetBranchStatus("*",1);
	t->SetEventList(0);
	
	// the current command string; will be printed at the end
	cout <<"\nCommand:\n"<<TString::Format(comcurrent.Data(),normalize)<<endl;
	
	// print efficiency values
	double effs = (double)Ns/N0s, effb = (double)Nb/N0b;
	double qa  = normalize ? FCSfc(ws*effs, wb*effb) : FCSfc(ws*Ns, wb*Nb);
	double pur = normalize ? ws*effs/(ws*effs+wb*effb) : ws*Ns/(ws*Ns+wb*Nb);
	printf("\nEFF_S = %5.3f (%d/%d)   EFF_B = %5.3f (%d/%d)   PUR = %5.3f   S/sqrt(S+B) %s= %6.3f\n",effs, Ns, N0s, effb, Nb, N0b, pur, normalize?"norm'd ":"", qa);
	
	c1->cd(); c1->Update();
  return 0;
}

