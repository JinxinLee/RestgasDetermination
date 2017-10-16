// --------------------------------------------------------------------
// counts different events for a cut
long int cntEvt(TTree *t1, TString cut, TString var="ev")
{
	float fbev;
	int   ibev;
	
	TBranch *br = t1->GetBranch(var);
	if (br==0) return 0;
	
	TString tit = br->GetTitle(); // should end with '/I' or '/F'
	
	t1->SetEventList(0);
	t1->Draw(">>el",cut);
	TEventList *el=(TEventList*)gROOT->FindObject("el");
	
	t1->SetBranchStatus("*",0);
	t1->SetBranchStatus(var,1);
	
	// int or float branch
	bool isint=tit.EndsWith("/I");
	
	if (isint)	
		t1->SetBranchAddress(var,&ibev);
	else
		t1->SetBranchAddress(var,&fbev);
	
	// last and current event number; counter
	int lev = -1, cev=-1, cnt=0;
	for (int i=0;i<el->GetN();++i)
	{
		t1->GetEvent(el->GetEntry(i));
		
		cev = isint ? ibev : fbev;
		
		if (lev!=cev) cnt++;
		lev = cev;
	}
	
	t1->SetBranchStatus("*",1);
	
	return cnt;
}

// --------------------------------------------------------------------

void confgraph(TGraph *g, TString tit, int col=1, int marker=20)
{
	g->GetHistogram()->SetTitle(tit);
	g->GetHistogram()->SetMinimum(0);
	g->SetLineColor(col);
	g->SetMarkerColor(col);
	g->SetMarkerStyle(marker);
	g->SetMarkerSize(1);
	g->SetLineWidth(2);
}

// --------------------------------------------------------------------

TH1F* createHistoGraph(TGraph *g, TString tit="", double xmin=0, double xmax=0)
{
	static int cnt=0;
	if (tit=="") tit=g->GetTitle();
	
	double ymax  = TMath::MaxElement(g->GetN(), g->GetY());
	double dymax = g->GetErrorY(TMath::LocMax(g->GetN(), g->GetEY()));
	
	if (xmin>=xmax)
	{
		xmin = TMath::MinElement(g->GetN(), g->GetX());
		xmax = TMath::MaxElement(g->GetN(), g->GetX());
	}	
	TH1F *h=new TH1F(Form("h%03d",cnt++),tit,g->GetN(),xmin-0.5*(xmax-xmin)/g->GetN(), xmax+0.5*(xmax-xmin)/g->GetN());
	
	h->GetXaxis()->SetLabelSize(0.05);
	h->GetXaxis()->SetTitleSize(0.05);
	h->GetYaxis()->SetLabelSize(0.05);
	h->GetYaxis()->SetTitleSize(0.05);
	h->GetYaxis()->SetTitleOffset(1.4);
	
	h->SetMaximum((ymax+dymax)*1.05);
	h->SetMinimum(0);
	
	h->SetStats(0);
	
	return h;
}

// --------------------------------------------------------------------

void analyse_phi_base(TString fname="ntp1_2phi_A.root", TString cut="abs(f4cxd0m+f4cxd1m-2.15)<0.15&&abs(f4cxd0m-1.02)<0.012&&abs(f4cxd1m-1.02)<0.012&chi24c<50", int sigmode=0, TString tit="")
{
/*
Found 553 files with pattern data/DPM2phi_A_EMC1 containing event info Sum of events = 859179654
Found 327 files with pattern data/DPM2phi_A_EMC5 containing event info Sum of events = 508041001
Found 294 files with pattern data/DPM2phi_A_EMC8 containing event info Sum of events = 456811794
Found 308 files with pattern data/DPM2phi_B_EMC1 containing event info Sum of events = 478549190
Found 346 files with pattern data/DPM2phi_B_EMC5 containing event info Sum of events = 537555935
Found 541 files with pattern data/DPM2phi_B_EMC8 containing event info Sum of events = 840537554
*/	
	
	std::map<long int,long int> evcnts = 
                           { {1000,859179654}, {1001,1}, {1002,1}, {1003,1}, {1004,508041001}, {1005,1}, {1006,1}, {1007,456811794},    // Setup A, pbp -> 2phi
		                     {1020,478549190}, {1021,1}, {1022,1}, {1023,1}, {1024,537555935}, {1025,1}, {1026,1}, {1027,840537554}     // Setup B, pbp -> 2phi
		                     };
		 
		                 
	//for ( auto x:evcnts) cout <<"mode "<<x.first<<" : "<<x.second<<endl;
	//cout <<endl;
		           
	if (cut=="") cut="1";
		                 
	int bkgmode = sigmode+1000;
	//if (sigmode%100>19) bkgmode+=1;
	
	TFile *f = new TFile(fname);
	TTree *t = (TTree*)f->Get("ntp1");
	
	TFile fana("anaPhi.root","UPDATE");

	TGraphErrors *g[4];
	g[0] = new TGraphErrors(3);
	g[1] = new TGraphErrors(3);
	g[2] = new TGraphErrors(3); 
	g[3] = new TGraphErrors(3); 
	
	confgraph(g[0], "signal to noise");
	confgraph(g[1], "significance");
	confgraph(g[2], "signal efficiency");
	confgraph(g[3], "background efficiency");
	
	TF1 *f1=new TF1("f1","pol0",0,100.);	
	f1->SetParLimits(2,15,100);	
	
	// -------------------------------------------------------------------
	// Supermodul              |  1  |  2  |  3  |  4  |  5  |  6  |  7  |
	// num Alveolen in theta   |  1  |  3  |  3  |  3  |  3  |  3  |  2  |
	// coverage from 22 to     |140.0|133.4|113.8| 94.1| 74.4| 54.8| 35.1|
	// -------------------------------------------------------------------

	// missing fraction in [%] of EMC = [ 1.0 - (tht_max - 22°)/118° ]* 100
	double emc_rmv[8] =    {   0.,   6.,  23.,  39.,  56.,  72.,  89., 100.};

	//TString lab[4] = { ";supermodules missing;S/B", ";supermodules missing;significance [#sigma]", ";supermodules missing;signal efficiency [%]", ";supermodules missing;background efficiency [%]"};
	TString lab[4] = {  ";EMC missing [%];signal efficiency [%]", ";EMC missing [%];background efficiency [%]", ";EMC missing [%];S/B", ";EMC missing [%];significance [#sigma]"};
	
	
	TCanvas *c1 = new TCanvas("c1","c1",1000,800);
	c1->Divide(2,2,0.0001,0.0001);
	
	double sig_S = 100;
	double sig_B = 60e6;
	
	double Lint  = 788*0.5;
	double fBR   = 0.489*0.489;
	
	double S_dat = sig_S * fBR * Lint;
	double B_dat = sig_B * Lint;
		
	cout <<"S:B = "<<S_dat/B_dat<<"  mode_S="<<sigmode<<"  mode_B="<<bkgmode<<endl; 
	
	int idxx[3] = {0, 4, 7};
	
	for (int jj=0;jj<3;++jj)
	{
		int i=idxx[jj];
		
		long int S0 = 1e5;
		long int B0 = evcnts[sigmode+i+1000];
	
		cout <<"S0 = "<<S0<<"  B0 = "<<B0<<endl;
		
		double fS = S_dat/S0;
		double fB = B_dat/B0;
		
		TString sigcut = Form("%s && mode==%d", cut.Data(), sigmode+i);
		TString bkgcut = Form("%s && mode==%d", cut.Data(), bkgmode+i);
		
		
		double S  = (double) cntEvt(t, sigcut);
		double B  = (double) cntEvt(t, bkgcut);
		double dS = sqrt(S);
		double dB = sqrt(B);

		cout <<i<<":  S="<<S<<"  B="<<B<<"  -->  S*"<<fS<<"="<<S*fS<<"  B*"<<fB<<"="<<B*fB<<endl;
		
		double SN  = S*fS/(B*fB);
		double dSN = SN*sqrt(dS*dS/(S*S) + dB*dB/(B*B));
		
		g[2]->SetPoint(jj, emc_rmv[i], SN);
		g[2]->SetPointError(jj, 0, dSN);
		
		
		double Z  = S*fS/sqrt(S*fS+B*fB);
		double dZ = 0.5 * sqrt( ((fS*fS*S+2*fB*fS*B)*(fS*fS*S+2*fB*fS*B)*S + fS*fS*fB*fB*S*S*B)/pow(fS*S+fB*B,3)) ;//0.5 * sqrt( (pow(2*B*fB + S*fS,2)*S + fS*fS*S*S*B)/pow(fS*S+fB*B,3));
		
		g[3]->SetPoint(jj, emc_rmv[i], Z);
		g[3]->SetPointError(jj, 0, dZ);
		
		
		double effS  = S/S0;
		double deffS = effS*sqrt( 1./S + 1./S0 ); 
		
		g[0]->SetPoint(jj, emc_rmv[i], effS*100.);
		g[0]->SetPointError(jj, 0, deffS*100.);
		
		
		double effB  = B/B0;
		double deffB = effB*sqrt( 1./B + 1./B0 ); 
		
		g[1]->SetPoint(jj, emc_rmv[i], effB*100.);
		g[1]->SetPointError(jj, 0, deffB*100.);
	}	
	
	TH1F *h[4];
	
	gStyle->SetOptFit(0);
	gStyle->SetOptStat(0);
	
	TString plotnam[4]={"effs","effb","sn","sign"};
	
	for (int i=0;i<4;++i) 
	{
		c1->cd(i+1);
		gPad->SetTopMargin(0.10);
		h[i] = createHistoGraph(g[i],tit+lab[i]);
		h[i]->Draw();
		g[i]->Draw("P same");
		
		f1->SetParameters(TMath::MaxElement(g[i]->GetN(), g[i]->GetY()),70, 10, TMath::MinElement(g[i]->GetN(), g[i]->GetY()));
		if (i==1)
		{
			f1->SetParameters(TMath::MaxElement(g[i]->GetN(), g[i]->GetY()),120, -10, TMath::MinElement(g[i]->GetN(), g[i]->GetY()));
			f1->SetParLimits(1,110,200);
		}	
		
		
		//if (i<3) 
		g[i]->Fit("f1","q");
		//else g[i]->Fit("f2");
		g[i]->SetName(plotnam[i]+"_"+((TString)fname(6,5)));
		g[i]->Write();
	}
	
	fname.ReplaceAll(".root","");
	cut.ReplaceAll("&&","_AND_");
	cut.ReplaceAll("||","_OR_");
	cut.ReplaceAll("!","_NOT_");
	cut.ReplaceAll(">","_lg_");
	cut.ReplaceAll("<","_sm_");
	cut.ReplaceAll(".","_");
	
	c1->SaveAs(Form("fig/%s__%03d__%s.gif",fname.Data(), sigmode, cut.Data()));
	c1->SaveAs(Form("fig/%s__%03d__%s.C",fname.Data(), sigmode, cut.Data()));
	
	fana.Close();
}


