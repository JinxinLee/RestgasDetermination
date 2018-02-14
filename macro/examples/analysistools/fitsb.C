#include "TStyle.h"
#include "TString.h"
#include "TH1F.h"
#include "TF1.h"
#include "TPaveStats.h"
#include "TLatex.h"
#include "TPad.h"
#include "TList.h"

const int maxdeg = 5;

int fitsb(TH1 *h=0, TString type="", TString parms="", double A=0., double min=-999., double max = -999.)
{
	gStyle->SetOptFit(112);
	int i;
	if (h==0) 
	{
		cout <<"\nFits a distribution consisting of signal peak and background; prints out the integral of the signal in the fit region\n\n";
		cout <<"USAGE:\nfitsb(TH1* h, <TString type>, <TString parms>, <double N>, <double min>, <double max>)\n"; 
		cout <<"  h       : histogram pointer\n";
		cout <<"  <type>  : default = \"gaus2\"; definition of fit function (gaus,bw,vgt,dgaus,bigaus,novo) and degree of bkg polyn (max=5, no number->no bkg, 8=argus fcn). E.g.: \"bw3\" = breit wigner + pol3\n";
		cout <<"            gaus = Gaussian; bw = Breit-Wigner; vgt = Voigt-Fcn; dgaus = Double Gaussian (common mean); bigaus = Bifurcated Gaussian; novo = Novosibirsk Fcn\n";
		cout <<"  <parms> : defauls extracted from histogram; string with initial fit parameters w/o amplitude param. E.g.: \"3.5,0.2,1,1,1\" -> mean=3.5, sigma=0.2, a0=1, a1=1, a2=1\n";
		cout <<"            a leading 'f' fixes the parameter. E.g.: \"f3.5,0.2\" -> mean=3.5 (fixed), sigma=0.2\n";
		cout <<"  <A>     : amplitude parameter; default = h.Entries()/3. Negative value fixes the parameter at -A.\n";
		cout <<"  <min>   : minimum of fit range; default = h min x\n";
		cout <<"  <max>   : maximum of fit range; default = h max x\n";
		return 0;
	} 

	h->Draw();

	if (type=="") type="gaus2";
	
	// bin width for amplitude param
	double w = h->GetBinWidth(1);
	
	// fit range; if min=max, fit range over the whole histogram
	if (min==-999.) min = h->GetXaxis()->GetXmin(); 
	if (max==-999.) max = h->GetXaxis()->GetXmax();

	if (min>max) {double tmp=min; min=max;max=tmp;}
	
	TString tmpl;
	int sigmode = 0;
	int paroff  = 3;  // first parameter offset for the bkg polynomial
	
	// set signal type
	if      (type.BeginsWith("gaus"))   sigmode = 0;  // gaussian
	else if (type.BeginsWith("bw"))     sigmode = 1;  // breit wigner
	else if (type.BeginsWith("vgt"))    sigmode = 2;  // voigtian
	else if (type.BeginsWith("dgaus"))  sigmode = 3;  // double gaussian
	else if (type.BeginsWith("bigaus")) sigmode = 4;  // bifurcated gaussian
	else if (type.BeginsWith("novo"))   sigmode = 5;  // novosibirsk function
	
	// set fit function template 
	if      (sigmode==0) tmpl = "%f*[0]/[2]/sqrt(2.0*pi)*exp(-0.5*((x-[1])/[2])^2)";  // Gaussian
//	else if (sigmode==1) tmpl = "%f*[0]*TMath::Voigt(x-[1],0,[2],4.)";                // Breit-Wigner via Voigt
	else if (sigmode==1) tmpl = "%f*[0]*[2]/(2.0*pi*((x-[1])^2+([2]/2)^2))";          // Breit-Wigner
	else if (sigmode==2) tmpl = "%f*[0]*TMath::Voigt(x-[1],[2],[3],4.)";              // Voigt
	else if (sigmode==3) tmpl = "%f*[0]*([4]/([4]+1.0)/[2]*exp(-0.5*((x-[1])/[2])^2)+1.0/([4]+1.0)/[3]*exp(-0.5*((x-[1])/[3])^2))/sqrt(2.0*pi)";  // 2 Gaussians with common mean (double gauss)
	else if (sigmode==4) tmpl = "%f*[0]*2./([2]+[3])/sqrt(2.0*pi)*exp(-0.5*(x-[1])^2/([2]*(x<[1])+[3]*(x>=[1]))^2)";                              // bifurcated Gaussian
	else if (sigmode==5) tmpl = "[0]*(([3]>0)*((x-[1])*[3]/[2]<1)*exp( -0.5*(log(1-((x-[1])*[3]/[2]<1)*(x-[1])*[3]/[2]))^2/(2./2.3548*sinh(0.5*[3]*2.3548))^2 - 0.5*[2]^2)+([3]<=0)*(1./[2]/sqrt(2.0*pi)*exp(-0.5*((x-[1])/[2])^2)))";            // Novisibirsk function
		
    if (sigmode==2 || sigmode==4 || sigmode==5) paroff = 4;
	if (sigmode==3) paroff = 5;

	// degree of bkg polynomial; 8+9 are argus fcn and truncated polynom
	int deg = -1; // no background fcn
	TString degs = type(type.Length()-1,1);  // last character of fcn string could be the digit for bkg polynomial
	TString nums="0123458";
	if (nums.Contains(degs)) deg = degs.Atoi();
	if (deg>maxdeg && deg<8) deg=maxdeg;
	
	// total number of params
	int partot = paroff+deg+1;
	if (deg==8) partot-=5;    // correction for number of parameters for abusing deg==8 (->9 parms) for argus fcn with only 4 parameters
	
	// set fit function TFormula
	TString fnc = TString::Format(tmpl.Data(), w);
	if (deg>=0&&deg<8) fnc+=TString::Format("+pol%d(%d)",deg, paroff);
	else if (deg==8) // ARGUS fcn bkg
	{
		fnc+=TString::Format("+(x<[%d])*[%d]*x*(abs(1-(x/[%d])^2))^[%d]*exp([%d]*(1-(x/[%d])^2))", paroff+1, paroff, paroff+1, paroff+2, paroff+3, paroff+1);
	}

	cout<<"\nFit fcn: " <<fnc<<endl<<endl;

	// set the parameter list from par string
	double prm[20]={0};       // the parameters
	double prmr[20]={0};      // possible second par values defining a range
	bool   fixparm[20]={0};   // flags for fixed parameter
	int pcnt = 0;
	if (parms!="") parms=parms+",";
	
	// set some defaults
	prm[0] = h->GetEntries()/3.;                   // N
	prm[1] = h->GetBinCenter(h->GetMaximumBin());  // mean estimate
	prm[2] = h->GetRMS()/2;                        // sigma/Gamma estimate
	if (sigmode>1)  prm[3] = prm[2]*2.;            // 2nd sigma/Gamma estimate
	if (sigmode==3) prm[4] = 1.0;                  // ratio sigma1/sigma2	
	
	if (deg<maxdeg+1) 
	{
		for (i=paroff;i<partot;++i) prm[i] = 1.0;      // bkg parms
	}
	else if (deg==8)
	{
		prm[paroff]   = h->GetMaximum()/5.;
		prm[paroff+1] = max;
		prm[paroff+2] = prm[paroff+3] = 0.5;
	}
	
	// set initial amplitude parameter
	if (A>0) prm[0] = A;
	// if A<0 the parameter value will be fixed to -A
	if (A<0) {prm[0] = -A; fixparm[0]=true;}
		
	// analyse parameter string
	while (parms!="")
	{
		TString sparm = parms(0,parms.Index(","));
		if (sparm!="") 
		{
			// fix this parameter?
			if (sparm.BeginsWith("f")) 
			{
				sparm=sparm(1,1000); // cut away the 'f'
				fixparm[1+pcnt] = true;
			}
			// ranged parameter in form 'r10.2|10.5'?
			if (sparm.BeginsWith("r"))
			{
// 				cout<<"ranged"<<endl;
				TString sparm2 = sparm(sparm.Index("|")+1,1000);
				sparm = sparm(1,sparm.Index("|"));
				prmr[1+pcnt]=sparm2.Atof();
// 				cout <<"parm2="<<prmr[1+pcnt];
			}
			prm[1+pcnt++] = sparm.Atof();
			parms = parms(parms.Index(",")+1,1000);
		}
	} 
	
	// setup the total fit fcn (signal + background)
	TF1 ff1("ff1", fnc);
	ff1.SetLineColor(4); ff1.SetNpx(500);  // some style setting

	// set the parameter names
	if (sigmode==0)      ff1.SetParNames("N","#mu","#sigma",                      "a_{0}","a_{1}","a_{2}","a_{3}","a_{4}","a_{5}");
	else if (sigmode==1) ff1.SetParNames("N","#mu","#Gamma",                      "a_{0}","a_{1}","a_{2}","a_{3}","a_{4}","a_{5}");
	else if (sigmode==2) ff1.SetParNames("N","#mu","#sigma","#Gamma",             "a_{0}","a_{1}","a_{2}","a_{3}","a_{4}","a_{5}");
	else if (sigmode==3) ff1.SetParNames("N","#mu","#sigma_{1}","#sigma_{2}","R", "a_{0}","a_{1}","a_{2}","a_{3}","a_{4}","a_{5}");
	else if (sigmode==4) ff1.SetParNames("N","#mu","#sigma_{1}","#sigma_{2}",     "a_{0}","a_{1}","a_{2}","a_{3}","a_{4}","a_{5}");
	else if (sigmode==5) ff1.SetParNames("A","#mu","#sigma","#tau",               "a_{0}","a_{1}","a_{2}","a_{3}","a_{4}","a_{5}");
	
	if (deg==8) 
	{
		ff1.SetParName(paroff  , "a");
		ff1.SetParName(paroff+1, "m_{0}");
		ff1.SetParName(paroff+2, "p");
		ff1.SetParName(paroff+3, "c");
	}
			
	// fix the parameters which were requested to be fixed
	for (i=0;i<partot;++i)
	{
		if (fixparm[i])
			ff1.FixParameter(i,prm[i]);
		else if (prmr[i]!=0)
			ff1.SetParLimits(i,prm[i],prmr[i]);
		else
			ff1.SetParameter(i,prm[i]);
	}
	
	// ***** do the fit
	h->Fit("ff1","q","",min, max);
	h->Fit("ff1","m","",min, max);
	
	// compute integral of total fit fcn in interval min ... max
	int integral = ff1.Integral(min,max)/w;
	int bkg = 0;
	
	// do we have a bkg polynomial? 
	if (deg>=0 && deg<=maxdeg)
	{
		// create pure bkg function
		TF1 *ff2=new TF1("ff2",TString::Format("pol%d",deg));
		ff2->SetLineColor(kRed+1); ff2->SetLineStyle(2); ff2->SetNpx(500);  // some style setting
		ff2->SetRange(min,max);
		
		// copy parameters from full fcn
		for (i=0;i<deg+1;++i) ff2->SetParameter(i,ff1.GetParameter(paroff+i));
		
		// add it to the histogram (so that it is shown when drawing the histogram)
		h->GetListOfFunctions()->Add(ff2);
		
		// compute bkg intergral
		bkg = ff2->Integral(min,max)/w;
		
		// #signals = #total - #bkg 
		if (bkg>0) integral -= bkg;
	}
	
	// argus bkg
	if (deg==8)
	{
		// create pure bkg function
		TF1 *ff2=new TF1("ff2","(x<[1])*[0]*x*(abs(1-(x/[1])^2))^[2]*exp([3]*(1-(x/[1])^2))");
		ff2->SetLineColor(kRed+1); ff2->SetLineStyle(2); ff2->SetNpx(500);  // some style setting
		ff2->SetRange(min,max);
		
		// copy parameters from full fcn
		for (i=0;i<4;++i) ff2->SetParameter(i,ff1.GetParameter(paroff+i));
		
		// add it to the histogram (so that it is shown when drawing the histogram)
		h->GetListOfFunctions()->Add(ff2);
		
		// compute bkg intergral
		bkg = ff2->Integral(min,max)/w;
		
		// #signals = #total - #bkg 
		if (bkg>0) integral -= bkg;
	 }
	
	// restyle the stats box to adapt different number of parameters
	TPaveStats *s = (TPaveStats*) gPad->GetPrimitive("stats");
	if (s)
	{
		s->SetX1NDC(0.65);
		s->SetY1NDC(gStyle->GetStatX()-0.032*(partot+4));
		s->SetTextSize(0.025); 
	}
	
	// write numbers of S and B to the histogram	
	TLatex ls,lb;
	double xmin = h->GetXaxis()->GetXmin(), xmax = h->GetXaxis()->GetXmax();
	ls.SetTextSize(0.045); ls.SetTextColor(4);
	ls.DrawLatex(xmin+(xmax-xmin)*0.03,h->GetMaximum()*0.95,TString::Format("S_{int} = %d",integral));
	if (deg>=0)
	{
		lb.SetTextSize(0.045); lb.SetTextColor(kRed+1);
		lb.DrawLatex(xmin+(xmax-xmin)*0.03,h->GetMaximum()*0.87,TString::Format("B_{int} = %d",bkg));
	}
	
	// compute S/N and significance
	double StoN = (double)integral/(double)bkg;
	double Sign = (double)integral/sqrt((double)bkg+(double)integral);
	
	// print out the var list string for easier copy paste
	cout <<endl<<"Var-list: \"";
	for (int i=1;i<partot;++i)
	{
		if (fixparm[i]) cout<<"f";
		cout << ff1.GetParameter(i);
		if (i<partot-1) cout<<",";
	}
	cout <<"\"";
	
	// print some info about S and B
	printf("\n\n[%.3f ... %.3f] : S = %d  B = %d  S/B = %.3f  S/sqrt(S+B) = %.4f\n", min, max, integral, bkg, StoN, Sign); 
	return integral;
}
