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

void analyse_etac1(TString fname="ntp5_etac1_A.root", TString cut="chi24c<50", int sigmode=300, TString tit="")
{
	//---------------------------------------------------------------------------------------------------------------------------------
	//Processing analyse_etac1_slc.C("ntp5_slc_etac1_A.root","chi24c<50",300, "Setup A, #bar{p}p#rightarrow#tilde{#eta_{c1}}#eta")...
	//---------------------------------------------------------------------------------------------------------------------------------
	 //i  |    S0    |     B0     |     S    |    B   |    fS    |    fB     |    S*   |    B*   |
	//----+----------+------------+----------+--------+----------+-----------+---------+---------+
	 //0  |  100000  |   2.691 G  |   17743  |    17  |  0.0098  |  95.2150  |    173  |   1619  |
	 //8  |  100000  |   2.690 G  |     142  |     4  |  0.0098  |  95.2268  |      1  |    381  |
	//---------------------------------------------------------------------------------------------------------------------------------
	//Processing analyse_etac1_slc.C("ntp5_slc_etac1_B.root","chi24c<50",320, "Setup B, #bar{p}p#rightarrow#tilde{#eta_{c1}}#eta")...
	//----+----------+------------+----------+--------+----------+-----------+---------+---------+
	 //0  |  100000  |   2.675 G  |    3503  |    11  |  0.0098  |  95.7917  |     34  |   1054  |
	 //8  |  100000  |   2.819 G  |       2  |     1  |  0.0098  |  90.8956  |      0  |     91  |
	//######################################################################
	//---------------------------------------------------------------------------------------------------------------------------------
	//Processing analyse_etac1.C("ntp5_etac1_A.root","chi24c<50",300, "Setup A, #bar{p}p#rightarrow#tilde{#eta_{c1}}#eta")...
	//---------------------------------------------------------------------------------------------------------------------------------
	 //i  |    S0    |     B0     |     S    |    B   |    fS    |    fB     |    S*   |    B*   |
	//----+----------+------------+----------+--------+----------+-----------+---------+---------+
	 //0  |  100000  |   2.866 G  |   17744  |    12  |  0.0098  |  89.3904  |    173  |   1073  |
	 //7  |  100000  |   2.706 G  |     131  |     1  |  0.0098  |  94.6622  |      1  |     95  |
	//---------------------------------------------------------------------------------------------------------------------------------
	//Processing analyse_etac1.C("ntp5_etac1_B.root","chi24c<50",320, "Setup B, #bar{p}p#rightarrow#tilde{#eta_{c1}}#eta")...
	//----+----------+------------+----------+--------+----------+-----------+---------+---------+
	 //0  |  100000  |   2.931 G  |    3617  |    14  |  0.0098  |  87.4188  |     35  |   1224  |
	 //7  |  100000  |   2.739 G  |       5  |     1  |  0.0098  |  93.5508  |      0  |     94  |
 
 

	//data/DPMetac1_A_EMC1      ( 179/ 199 files) =   2866079091
	//data/DPMetac1_A_EMC2      ( 173/ 198 files) =   2770144980
	//data/DPMetac1_A_EMC3      ( 166/ 196 files) =   2658679173
	//data/DPMetac1_A_EMC4      ( 178/ 198 files) =   2850295200
	//data/DPMetac1_A_EMC5      ( 179/ 200 files) =   2866373874
	//data/DPMetac1_A_EMC6      ( 169/ 198 files) =   2706157924
	//data/DPMetac1_A_EMC7      ( 175/ 197 files) =   2801993946
	//data/DPMetac1_A_EMC8      ( 169/ 199 files) =   2706466096
	//--------------------
	//data/DPMetac1_B_EMC1      ( 183/ 198 files) =   2930718917
	//data/DPMetac1_B_EMC2      ( 168/ 199 files) =   2690367302
	//data/DPMetac1_B_EMC3      ( 167/ 197 files) =   2674423426
	//data/DPMetac1_B_EMC4      ( 177/ 200 files) =   2834563180
	//data/DPMetac1_B_EMC5      ( 174/ 199 files) =   2786239006
	//data/DPMetac1_B_EMC6      ( 169/ 198 files) =   2706374195
	//data/DPMetac1_B_EMC7      ( 173/ 198 files) =   2770320696
	//data/DPMetac1_B_EMC8      ( 171/ 198 files) =   2738620309



	std::map<long int,long int> evcnts = 
                           { {1300, 2866079091}, {1301, 2770144980}, {1302, 2658679173}, {1303, 2850295200}, {1304, 2866373874}, {1305, 2706157924}, {1306, 2801993946}, {1307, 2706466096},    // Setup A, pbp -> J/psi (-> e+ e-) pi+ pi-
		                     {1320, 2930718917}, {1321, 2690367302}, {1322, 2674423426}, {1323, 2834563180}, {1324, 2786239006}, {1325, 2706374195}, {1326, 2770320696}, {1327, 2738620309}
		                     };
		 
	// we merge the values at 0% and 100% from both studies by hand...
	// **** from phi study
	//data/DPMetac1_A_SLC1      ( 168/ 200 files) =   2690752186
	//data/DPMetac1_A_SLC9      ( 168/ 200 files) =   2690419363
	//--------------------
	//data/DPMetac1_B_SLC1      ( 167/ 200 files) =   2674554579
	//data/DPMetac1_B_SLC9      ( 176/ 200 files) =   2818616809
	
	evcnts[1300] += 2690752186;
	evcnts[1320] += 2690419363;
	evcnts[1307] += 2674554579;
	evcnts[1327] += 2818616809;
	
	std::map<long int,long int> mrgB = 
                           { {1300, 17+12},  {1307, 4+1},   
		                     {1320, 11+14},  {1327, 1+1}
		                     };
	
		                 
	//for ( auto x:evcnts) cout <<"mode "<<x.first<<" : "<<x.second<<endl;
	//cout <<endl;
		                 
	int bkgmode = sigmode+1000;
	//if (sigmode%100>19) bkgmode+=1;
	
	TFile *f = new TFile(fname);
	TTree *t = (TTree*)f->Get("ntp5");
	
	TFile fana("ana_etac.root","UPDATE");

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
	//TF1 *f1=new TF1("f1","0.5*[0]*(1.0-TMath::Erf((x-[1])/[2]))+pol2(3)",0,100.);	
	//f1->SetParLimits(2,15,300);	
	
	// -------------------------------------------------------------------
	// Supermodul              |  1  |  2  |  3  |  4  |  5  |  6  |  7  |
	// num Alveolen in theta   |  1  |  3  |  3  |  3  |  3  |  3  |  2  |
	// coverage from 22 to     |140.0|133.4|113.8| 94.1| 74.4| 54.8| 35.1|
	// -------------------------------------------------------------------

	// missing fraction in [%] of EMC = [ 1.0 - (tht_max - 22°)/118° ]* 100
	double emc_rmv[8] =    {   0.,   6.,  23.,  39.,  56.,  72.,  89., 100.};

	//TString lab[4] = { ";supermodules missing;S/B", ";supermodules missing;significance [#sigma]", ";supermodules missing;signal efficiency [%]", ";supermodules missing;background efficiency [%]"};
	TString lab[4] = {  ";EMC missing (polar #theta) [%];signal efficiency [%]", ";EMC missing (polar #theta) [%];background efficiency [%]", ";EMC missing (polar #theta) [%];S/B", ";EMC missing (polar #theta) [%];significance [#sigma]"};
	
	
	TCanvas *c1 = new TCanvas("c1","c1",1000,800);
	c1->Divide(2,2,0.0001,0.0001);
	
	double sig_S = 20;
	double sig_B = 42e6;
	
	double Lint  = 1220*5;
	double fBR   = 0.06*0.339*0.394;  // BR_J * BR_chic * BR_eta
	
	double S_dat = sig_S * fBR * Lint;
	double B_dat = sig_B * Lint;
		
	cout <<"S:B = "<<S_dat/B_dat<<"  mode_S="<<sigmode<<"  mode_B="<<bkgmode<<endl; 
	
	printf (" i  |    S0    |     B0     |     S    |    B   |    fS    |    fB     |    S*   |    B*   |\n");
	printf ("----+----------+------------+----------+--------+----------+-----------+---------+---------+\n");
	for (int i=0;i<8;++i)
	{
		long int S0 = 1e5;
		long int B0 = evcnts[sigmode+i+1000];
	
		
		double fS = S_dat/S0;
		double fB = B_dat/B0;
		
		TString sigcut = Form("%s && mode==%d && xmct", cut.Data(), sigmode+i);
		TString bkgcut = Form("%s && mode==%d", cut.Data(), bkgmode+i);
		
		
		double S  = (double) cntEvt(t, sigcut);
		double B  = (double) (mrgB.find(sigmode+i+1000) == mrgB.end() ? cntEvt(t, bkgcut)+1 : mrgB[sigmode+i+1000]);
		double dS = sqrt(S);
		double dB = sqrt(B);

		printf("%2d  |  %6ld  |  %6.3f G  |  %6d  |  %4d  |  %6.4f  |  %6.4f  |  %5.0f  |  %5.0f  |\n", i, S0, (double)B0/1e9, (int)S, (int)B, fS, fB, S*fS, B*fB);
		//cout <<i<<" : " <<"S0 = "<<S0<<"  B0 = "<<B0<<" : S="<<S<<"  B="<<B<<"  -->  S*"<<fS<<"="<<S*fS<<"  B*"<<fB<<"="<<B*fB<<endl;
		
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
	
	double hmaxy[4] = {20., 1e-5, 0.25, 7.};
	
	for (int i=0;i<4;++i) 
	{
		c1->cd(i+1);
		gPad->SetTopMargin(0.10);
		h[i] = createHistoGraph(g[i],tit+lab[i]);
		h[i]->SetMaximum(hmaxy[i]);
		h[i]->Draw();
		g[i]->Draw("P same");
		
		f1->SetParameters(TMath::MaxElement(g[i]->GetN(), g[i]->GetY()),70, 10, TMath::MinElement(g[i]->GetN(), g[i]->GetY()));
		if (i==1)
		{
			f1->SetParameters(TMath::MaxElement(g[i]->GetN(), g[i]->GetY()),120, -10, TMath::MinElement(g[i]->GetN(), g[i]->GetY()));
			f1->SetParLimits(1,110,200);
		}	
		
		
		//if (i<3) 
		if (i>0) g[i]->Fit("f1","q");
		else g[i]->Fit("f1","q","",0,95);
		//else g[i]->Fit("f2");
		g[i]->SetName(plotnam[i]+"_"+((TString)fname(5,7)));
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


/*
Found 244 files with pattern data/DPMetac1_A_EMC1 containing event info Sum of events = 781589138
Found 243 files with pattern data/DPMetac1_A_EMC2 containing event info Sum of events = 778264034
Found 247 files with pattern data/DPMetac1_A_EMC3 containing event info Sum of events = 791030134
Found 244 files with pattern data/DPMetac1_A_EMC4 containing event info Sum of events = 781662143
Found 248 files with pattern data/DPMetac1_A_EMC5 containing event info Sum of events = 794307759
Found 243 files with pattern data/DPMetac1_A_EMC6 containing event info Sum of events = 778257467
Found 242 files with pattern data/DPMetac1_A_EMC7 containing event info Sum of events = 775037895
Found 242 files with pattern data/DPMetac1_A_EMC8 containing event info Sum of events = 775095496
--------------------
Found 241 files with pattern data/DPMetac1_B_EMC1 containing event info Sum of events = 771776262
Found 242 files with pattern data/DPMetac1_B_EMC2 containing event info Sum of events = 775016383
Found 244 files with pattern data/DPMetac1_B_EMC3 containing event info Sum of events = 781427165
Found 235 files with pattern data/DPMetac1_B_EMC4 containing event info Sum of events = 752754673
Found 247 files with pattern data/DPMetac1_B_EMC5 containing event info Sum of events = 791054235
Found 242 files with pattern data/DPMetac1_B_EMC6 containing event info Sum of events = 775064262
Found 241 files with pattern data/DPMetac1_B_EMC7 containing event info Sum of events = 771961964
Found 244 files with pattern data/DPMetac1_B_EMC8 containing event info Sum of events = 781628321

		std::map<long int,long int> evcnts = 
                           { {1300,781589138}, {1301,778264034}, {1302,791030134},  {1303,781662143}, {1304,794307759}, {1305,778257467}, {1306,775037895}, {1307,775095496},    // Setup A, pbp -> J/psi (-> e+ e-) pi+ pi-
		                     {1320,771776262}, {1321,775016383}, {1322,781427165}, {1323,752754673}, {1324,791054235}, {1325,775064262}, {1326,771961964},  {1327,781628321}
		                     };
*/	

	//data/DPMetac1_A_EMC1      ( 451/ 504 files) =   4096546385
	//data/DPMetac1_A_EMC2      ( 460/ 500 files) =   4252979964
	//data/DPMetac1_A_EMC3      ( 464/ 506 files) =   4266061680
	//data/DPMetac1_A_EMC4      ( 458/ 500 files) =   4208139218
	//data/DPMetac1_A_EMC5      ( 467/ 500 files) =   4301265578
	//data/DPMetac1_A_EMC6      ( 459/ 497 files) =   4236964259
	//data/DPMetac1_A_EMC7      ( 456/ 499 files) =   4202381786
	//data/DPMetac1_A_EMC8      ( 458/ 499 files) =   4233827498
	//--------------------
	//data/DPMetac1_B_EMC1      ( 455/ 498 files) =   4198373087
	//data/DPMetac1_B_EMC2      ( 454/ 499 files) =   4169903068
	//data/DPMetac1_B_EMC3      ( 459/ 499 files) =   4224726395
	//data/DPMetac1_B_EMC4      ( 451/ 497 files) =   4211893563
	//data/DPMetac1_B_EMC5      ( 462/ 498 files) =   4234207999
	//data/DPMetac1_B_EMC6      ( 455/ 499 files) =   4185698000
	//data/DPMetac1_B_EMC7      ( 452/ 500 files) =   4150810055
	//data/DPMetac1_B_EMC8      ( 459/ 500 files) =   4224406655

		//std::map<long int,long int> evcnts = 
                           //{ {1300, 4096546385}, {1301, 4252979964}, {1302, 4266061680},  {1303, 4208139218}, {1304, 4301265578}, {1305, 4236964259}, {1306, 4202381786}, {1307, 4233827498},    // Setup A, pbp -> J/psi (-> e+ e-) pi+ pi-
		                     //{1320, 4198373087}, {1321, 4169903068}, {1322, 4224726395}, {1323, 4211893563}, {1324, 4234207999}, {1325, 4185698000}, {1326, 4150810055},  {1327, 4224406655}
		                     //};


	//data/DPMetac1_A_EMC1      ( 630/ 699 files) =   6962622485
	//data/DPMetac1_A_EMC2      ( 633/ 695 files) =   7023109828
	//data/DPMetac1_A_EMC3      ( 631/ 696 files) =   6940762184
	//data/DPMetac1_A_EMC4      ( 636/ 698 files) =   7058415189
	//data/DPMetac1_A_EMC5      ( 646/ 700 files) =   7167639452
	//data/DPMetac1_A_EMC6      ( 628/ 695 files) =   6943108546
	//data/DPMetac1_A_EMC7      ( 628/ 692 files) =   6956329888
	//data/DPMetac1_A_EMC8      ( 625/ 694 files) =   6908323139
	//--------------------
	//data/DPMetac1_B_EMC1      ( 637/ 695 files) =   7113112437
	//data/DPMetac1_B_EMC2      ( 622/ 698 files) =   6860231634
	//data/DPMetac1_B_EMC3      ( 626/ 696 files) =   6899184216
	//data/DPMetac1_B_EMC4      ( 628/ 697 files) =   7046479715
	//data/DPMetac1_B_EMC5      ( 636/ 697 files) =   7020440435
	//data/DPMetac1_B_EMC6      ( 624/ 697 files) =   6892077970
	//data/DPMetac1_B_EMC7      ( 625/ 698 files) =   6921142848
	//data/DPMetac1_B_EMC8      ( 627/ 695 files) =   6914980193

		//std::map<long int,long int> evcnts = 
                           //{ {1300, 6962622485}, {1301, 7023109828}, {1302, 6940762184}, {1303, 7058415189}, {1304, 7167639452}, {1305, 6943108546}, {1306, 6956329888}, {1307, 6908323139},    // Setup A, pbp -> J/psi (-> e+ e-) pi+ pi-
		                     //{1320, 7113112437}, {1321, 6860231634}, {1322, 6899184216}, {1323, 7046479715}, {1324, 7020440435}, {1325, 6892077970}, {1326, 6921142848}, {1327, 6914980193}
		                     //};
