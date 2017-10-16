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

void analyse_J(TString fname="ntp1_Jee_A.root", TString cut="xd0d0pide>0.5&&xd0d1pide>0.5&&chi24c<50", int sigmode=100, TString tit="")
{
/*
Found 421 files with pattern data/DPMJee_A_EMC1 containing event info Sum of events = 24342651889
Found 412 files with pattern data/DPMJee_A_EMC2 containing event info Sum of events = 23723523588
Found 392 files with pattern data/DPMJee_A_EMC3 containing event info Sum of events = 21921465124
Found 382 files with pattern data/DPMJee_A_EMC4 containing event info Sum of events = 21192161015
Found 379 files with pattern data/DPMJee_A_EMC5 containing event info Sum of events = 20899111775
Found 382 files with pattern data/DPMJee_A_EMC6 containing event info Sum of events = 21264224163
Found 391 files with pattern data/DPMJee_A_EMC7 containing event info Sum of events = 21918977223
Found 371 files with pattern data/DPMJee_A_EMC8 containing event info Sum of events = 20327238649
--------------------
Found 383 files with pattern data/DPMJee_B_EMC1 containing event info Sum of events = 21266925718
Found 388 files with pattern data/DPMJee_B_EMC2 containing event info Sum of events = 21721036293
Found 381 files with pattern data/DPMJee_B_EMC3 containing event info Sum of events = 21144322234
Found 373 files with pattern data/DPMJee_B_EMC4 containing event info Sum of events = 20364376890
Found 379 files with pattern data/DPMJee_B_EMC5 containing event info Sum of events = 20899648526
Found 387 files with pattern data/DPMJee_B_EMC6 containing event info Sum of events = 21667979271
Found 393 files with pattern data/DPMJee_B_EMC7 containing event info Sum of events = 22088200372
Found 383 files with pattern data/DPMJee_B_EMC8 containing event info Sum of events = 21308984620


//Found 389 files with pattern data/DPMJee_A_EMC1 containing event info Sum of events = 21720072869
//Found 386 files with pattern data/DPMJee_A_EMC2 containing event info Sum of events = 21593645357
//Found 335 files with pattern data/DPMJee_A_EMC3 containing event info Sum of events = 17249102681
//Found 332 files with pattern data/DPMJee_A_EMC4 containing event info Sum of events = 17094178566
//Found 320 files with pattern data/DPMJee_A_EMC5 containing event info Sum of events = 16064391492
//Found 328 files with pattern data/DPMJee_A_EMC6 containing event info Sum of events = 16840054994
//Found 339 files with pattern data/DPMJee_A_EMC7 containing event info Sum of events = 17659373515
//Found 320 files with pattern data/DPMJee_A_EMC8 containing event info Sum of events = 16148632260
//--------------------
//Found 332 files with pattern data/DPMJee_B_EMC1 containing event info Sum of events = 17087475331
//Found 331 files with pattern data/DPMJee_B_EMC2 containing event info Sum of events = 17047402087
//Found 323 files with pattern data/DPMJee_B_EMC3 containing event info Sum of events = 16391336239
//Found 330 files with pattern data/DPMJee_B_EMC4 containing event info Sum of events = 16841138917
//Found 323 files with pattern data/DPMJee_B_EMC5 containing event info Sum of events = 16310034426
//Found 327 files with pattern data/DPMJee_B_EMC6 containing event info Sum of events = 16750649874
//Found 346 files with pattern data/DPMJee_B_EMC7 containing event info Sum of events = 18236171676
//Found 327 files with pattern data/DPMJee_B_EMC8 containing event info Sum of events = 16721268259

////Found 244 files with pattern data/DPMJee_A_EMC1 Sum of events = 9998354512
////Found 240 files with pattern data/DPMJee_A_EMC2 Sum of events = 9833039530
////Found 245 files with pattern data/DPMJee_A_EMC3 Sum of events = 9999171875
////Found 244 files with pattern data/DPMJee_A_EMC4 Sum of events = 9963726052
////Found 241 files with pattern data/DPMJee_A_EMC5 Sum of events = 9836541126
////Found 241 files with pattern data/DPMJee_A_EMC6 Sum of events = 9789924201
////Found 243 files with pattern data/DPMJee_A_EMC7 Sum of events = 9955996421
////Found 244 files with pattern data/DPMJee_A_EMC8 Sum of events = 10000749142
////--------------------
////Found 240 files with pattern data/DPMJee_B_EMC1 Sum of events = 9793726135
////Found 242 files with pattern data/DPMJee_B_EMC2 Sum of events = 9915026700
////Found 239 files with pattern data/DPMJee_B_EMC3 Sum of events = 9792482283
////Found 246 files with pattern data/DPMJee_B_EMC4 Sum of events = 10081159515
////Found 244 files with pattern data/DPMJee_B_EMC5 Sum of events = 9998543976
////Found 241 files with pattern data/DPMJee_B_EMC6 Sum of events = 9869599812
////Found 242 files with pattern data/DPMJee_B_EMC7 Sum of events = 9916468291
////Found 243 files with pattern data/DPMJee_B_EMC8 Sum of events = 9959400420
--------------------
--------------------
Found 238 files with pattern data/DPMJmm_A_EMC1 Sum of events = 8821819045
Found 242 files with pattern data/DPMJmm_A_EMC2 Sum of events = 9010185209
Found 214 files with pattern data/DPMJmm_A_EMC3 Sum of events = 7969422935
Found 218 files with pattern data/DPMJmm_A_EMC4 Sum of events = 8081829652
Found 208 files with pattern data/DPMJmm_A_EMC5 Sum of events = 7707163073
Found 203 files with pattern data/DPMJmm_A_EMC6 Sum of events = 7482700194
Found 209 files with pattern data/DPMJmm_A_EMC7 Sum of events = 7781565342
Found 214 files with pattern data/DPMJmm_A_EMC8 Sum of events = 7967077007
--------------------
Found 221 files with pattern data/DPMJmm_B_EMC1 Sum of events = 8229011119
Found 208 files with pattern data/DPMJmm_B_EMC2 Sum of events = 7705315505
Found 217 files with pattern data/DPMJmm_B_EMC3 Sum of events = 8043282415
Found 210 files with pattern data/DPMJmm_B_EMC4 Sum of events = 7780855820
Found 222 files with pattern data/DPMJmm_B_EMC5 Sum of events = 8230265044
Found 215 files with pattern data/DPMJmm_B_EMC6 Sum of events = 8005672265
Found 199 files with pattern data/DPMJmm_B_EMC7 Sum of events = 7407676699
Found 213 files with pattern data/DPMJmm_B_EMC8 Sum of events = 7932457375
*/	
	
	std::map<long int,long int> evcnts = 
                           { {1100,24342651889}, {1101,23723523588}, {1102,21921465124},  {1103,21192161015}, {1104,20899111775}, {1105,21264224163}, {1106,21918977223}, {1107,20327238649},    // Setup A, pbp -> J/psi (-> e+ e-) pi+ pi-
		                     {1120,21266925718}, {1121,21721036293}, {1122,21144322234}, {1123,20364376890}, {1124,20899648526}, {1125,21667979271}, {1126,22088200372},  {1127,21308984620},    // Setup B, pbp -> J/psi (-> e+ e-) pi+ pi-  //FIXME modes 1120 ...
		                 
		                     {1200,8821819045}, {1201,9010185209}, {1202,7969422935},  {1203,8081829652}, {1204,7707163073}, {1205,7482700194}, {1206,7781565342},  {1207,7967077007},    // Setup A, pbp -> J/psi (-> mu+ mu-) pi+ pi- 
		                     {1220,8229011119}, {1221,7705315505}, {1222,8043282415},  {1223,7780855820}, {1224,8230265044}, {1225,8005672265}, {1226,7407676699},  {1227,7932457375}     // Setup B, pbp -> J/psi (-> mu+ mu-) pi+ pi-  //FIXME modes 1220...
		                     };
		 
		                 
	//for ( auto x:evcnts) cout <<"mode "<<x.first<<" : "<<x.second<<endl;
	//cout <<endl;
		                 
	int bkgmode = sigmode+1000;
	//if (sigmode%100>19) bkgmode+=1;
	
	TFile *f = new TFile(fname);
	TTree *t = (TTree*)f->Get("ntp1");
	
	TFile fana("anaJ.root","UPDATE");

	TGraphErrors *g[4];
	g[0] = new TGraphErrors(8);
	g[1] = new TGraphErrors(8);
	g[2] = new TGraphErrors(8); 
	g[3] = new TGraphErrors(8); 
	
	confgraph(g[0], "signal to noise");
	confgraph(g[1], "significance");
	confgraph(g[2], "signal efficiency");
	confgraph(g[3], "background efficiency");
	
	TF1 *f1=new TF1("f1","0.5*[0]*(1.0-TMath::Erf((x-[1])/[2]))+[3]",0,100.);	
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
	
	double sig_S = 50;
	double sig_B = 46e6;
	
	double Lint  = 1170*2;
	double fBR   = 0.05*0.06;
	
	double S_dat = sig_S * fBR * Lint;
	double B_dat = sig_B * Lint;
		
	cout <<"S:B = "<<S_dat/B_dat<<"  mode_S="<<sigmode<<"  mode_B="<<bkgmode<<endl; 
	
	for (int i=0;i<8;++i)
	{
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
		
		g[2]->SetPoint(i, emc_rmv[i], SN);
		g[2]->SetPointError(i, 0, dSN);
		
		
		double Z  = S*fS/sqrt(S*fS+B*fB);
		double dZ = 0.5 * sqrt( ((fS*fS*S+2*fB*fS*B)*(fS*fS*S+2*fB*fS*B)*S + fS*fS*fB*fB*S*S*B)/pow(fS*S+fB*B,3)) ;//0.5 * sqrt( (pow(2*B*fB + S*fS,2)*S + fS*fS*S*S*B)/pow(fS*S+fB*B,3));
		
		g[3]->SetPoint(i, emc_rmv[i], Z);
		g[3]->SetPointError(i, 0, dZ);
		
		
		double effS  = S/S0;
		double deffS = effS*sqrt( 1./S + 1./S0 ); 
		
		g[0]->SetPoint(i, emc_rmv[i], effS*100.);
		g[0]->SetPointError(i, 0, deffS*100.);
		
		
		double effB  = B/B0;
		double deffB = effB*sqrt( 1./B + 1./B0 ); 
		
		g[1]->SetPoint(i, emc_rmv[i], effB*100.);
		g[1]->SetPointError(i, 0, deffB*100.);
	}	
	
	TH1F *h[4];
	
	gStyle->SetOptFit(0);
	gStyle->SetOptStat(0);
	
	TString plotnam[4]={"effs","effb","sn","sign"};
	
	for (int i=0;i<4;++i) 
	{
		c1->cd(i+1);
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
		g[i]->SetName(plotnam[i]+"_"+((TString)fname(5,5)));
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


