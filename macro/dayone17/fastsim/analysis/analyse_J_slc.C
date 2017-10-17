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

void analyse_J_slc(TString fname="ntp1_slc_Jee_A.root", TString cut="xd0d0pide>0.5&&xd0d1pide>0.5&&chi24c<50", int sigmode=100, TString tit="")
{

	std::map<long int,long int> evcnts = 
                           //{ {1100,24342651889}, {1101,23723523588}, {1102,21921465124},  {1103,21192161015}, {1104,20899111775}, {1105,21264224163}, {1106,21918977223}, {1107,20327238649},    // Setup A, pbp -> J/psi (-> e+ e-) pi+ pi-
		                     //{1120,21266925718}, {1121,21721036293}, {1122,21144322234}, {1123,20364376890}, {1124,20899648526}, {1125,21667979271}, {1126,22088200372},  {1127,21308984620},    // Setup B, pbp -> J/psi (-> e+ e-) pi+ pi-  //FIXME modes 1120 ...
		                 
		                     //{1200,8821819045}, {1201,9010185209}, {1202,7969422935},  {1203,8081829652}, {1204,7707163073}, {1205,7482700194}, {1206,7781565342},  {1207,7967077007},    // Setup A, pbp -> J/psi (-> mu+ mu-) pi+ pi- 
		                     //{1220,8229011119}, {1221,7705315505}, {1222,8043282415},  {1223,7780855820}, {1224,8230265044}, {1225,8005672265}, {1226,7407676699},  {1227,7932457375}     // Setup B, pbp -> J/psi (-> mu+ mu-) pi+ pi-  //FIXME modes 1220...
		                     //};
                           { {1100, 15245616034}, {1101, 15245616034}, {1102, 15245616034},  {1103, 15245616034}, {1104, 15245616034}, {1105, 15245616034}, {1106, 7000000000}, {1107, 1},    // Setup A, pbp -> J/psi (-> e+ e-) pi+ pi-
		                     {1120, 1}, {1121, 1}, {1122, 1}, {1123,1}, {1124,1}, {1125,1}, {1126,1},  {1127,1},    // Setup B, pbp -> J/psi (-> e+ e-) pi+ pi-  //FIXME modes 1120 ...
		                 
		                     {1200, 1}, {1201, 1}, {1202, 1},  {1203, 1}, {1204, 1}, {1205, 1}, {1206, 1},  {1207, 1},    // Setup A, pbp -> J/psi (-> mu+ mu-) pi+ pi- 
		                     {1220, 1}, {1221, 1}, {1222, 1},  {1223, 1}, {1224, 1}, {1225, 1}, {1226, 1},  {1227, 1}     // Setup B, pbp -> J/psi (-> mu+ mu-) pi+ pi-  //FIXME modes 1220...
		                     };
		 
		                 
	//for ( auto x:evcnts) cout <<"mode "<<x.first<<" : "<<x.second<<endl;
	//cout <<endl;
		                 
	int bkgmode = sigmode+1000;
	//if (sigmode%100>19) bkgmode+=1;
	
	TFile *f = new TFile(fname);
	TTree *t = (TTree*)f->Get("ntp1");
	
	TFile fana("anaJ_slc.root","UPDATE");
	
	int Nred = 7;
	
	TGraphErrors *g[4];
	g[0] = new TGraphErrors(Nred);
	g[1] = new TGraphErrors(Nred);
	g[2] = new TGraphErrors(Nred); 
	g[3] = new TGraphErrors(Nred); 
	
	confgraph(g[0], "signal to noise");
	confgraph(g[1], "significance");
	confgraph(g[2], "signal efficiency");
	confgraph(g[3], "background efficiency");
	
	TF1 *f1[4];
	
	f1[0]=new TF1("f0","0.5*[0]*(1.0-TMath::Erf((x-[1])/[2]))+[3]",0,100.);	
	f1[0]->SetParameters(10,70, 10, 5);
	
	f1[1]=new TF1("f1","0.5*[0]*(1.0-TMath::Erf((x-[1])/[2]))+[3]",0,100.);	
	f1[1]->SetParameters(1e-7, 120, -10, 1e-8);
	f1[1]->SetParLimits(1,20,200);

	f1[2]=new TF1("f2","expo(0)+pol1(3)",0,100.);	
	f1[2]->SetParameters(1,1,1,1,1,1);
	
	f1[3]=new TF1("f3","expo(0)+expo(3)",0,100.);	
	f1[3]->SetParameters(1,1,1,1,1);
	

	//f1->SetParLimits(2,15,100);	
	//TF1 *f1=new TF1("f1","pol2(0)",0,100.);
	//f1->SetParameters(1,1,1);	
	
	// -------------------------------------------------------------------
	// 8x2 phi slices, symmetric
	// -------------------------------------------------------------------

	// missing fraction in [%] of EMC = [ 1.0 - (tht_max - 22°)/118° ]* 100
	double emc_rmv[7] =    {   0.,   12.5,  25.,  37.5,  50.,  62.5, 75.};//, 87.5 };

	//TString lab[4] = { ";supermodules missing;S/B", ";supermodules missing;significance [#sigma]", ";supermodules missing;signal efficiency [%]", ";supermodules missing;background efficiency [%]"};
	TString lab[4] = {  ";EMC missing [%];signal efficiency (#phi) [%]", ";EMC missing (#phi) [%];background efficiency [%]", ";EMC missing (#phi) [%];S/B", ";EMC missing (#phi) [%];significance [#sigma]"};
	
	
	TCanvas *c1 = new TCanvas("c1","c1",1000,800);
	c1->Divide(2,2,0.0001,0.0001);
	
	double sig_S = 50;
	double sig_B = 46e6;
	
	double Lint  = 1170*2;
	double fBR   = 0.05*0.06;
	
	double S_dat = sig_S * fBR * Lint;
	double B_dat = sig_B * Lint;
		
	cout <<"S:B = "<<S_dat/B_dat<<"  mode_S="<<sigmode<<"  mode_B="<<bkgmode<<endl; 
	
	for (int i=0;i<Nred;++i)
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
		
		//f1[i]->SetParameters(TMath::MaxElement(g[i]->GetN(), g[i]->GetY()),70, 10, TMath::MinElement(g[i]->GetN(), g[i]->GetY()));
		if (i==1)
		{
			f1[i]->SetParameters(TMath::MaxElement(g[i]->GetN(), g[i]->GetY()),120, -10, TMath::MinElement(g[i]->GetN(), g[i]->GetY()));
			f1[i]->SetParLimits(1,20,200);
		}	
		
		g[i]->Fit(Form("f%d",i),"q");
			
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


