
int cols[10] = {1,2,4,6,kOrange+5,9,kGreen+2, kBlue+3,kCyan-2,kMagenta+2};

// *** setup  style
void setStyle()
{
	gStyle->SetPadTopMargin(0.10);
	gStyle->SetPadBottomMargin(0.16);
	gStyle->SetPadLeftMargin(0.12);
	gStyle->SetPadRightMargin(0.05);

	gStyle->SetLabelSize(0.05,"X");
	gStyle->SetLabelSize(0.05,"Y");
	
	gStyle->SetTitleH(0.07);
	gStyle->SetTitleX(0.08);
	gStyle->SetTitleOffset(1.3,"x");
	gStyle->SetTitleXSize(0.055);
}
void renorm(double* a, int n=4)
{
	for (int i=0;i<n;++i) a[i]/=100.;
}

void config_graph(TGraph *g, int m=20, TString name="")
{
	g->SetMarkerStyle(m);
	g->SetMarkerColor(1);
	g->SetMarkerSize(1.2);
	g->SetLineWidth(2);
	g->SetLineColor(cols[m-20]);
	g->SetMarkerColor(cols[m-20]);
	g->SetName(name);
}

void overall_perf_toy()
{
	setStyle();
	
	TCanvas *cb1=new TCanvas("cb1","cb1",10,10,700,500);
	TCanvas *cb2=new TCanvas("cb2","cb2",30,30,700,500);
	TCanvas *cb3=new TCanvas("cb3","cb3",50,50,700,500);

	TCanvas *c2=new TCanvas("c2","c2",900,10,700,500);
	TCanvas *c3=new TCanvas("c3","c3",900,500,700,500);
	TCanvas *c4=new TCanvas("c4","c4",900,500,700,500);
	
	TH1F *hb1=new TH1F("hb1","Background fraction",100,2.0, 6.0);
	hb1->SetMaximum(0.3);
	hb1->SetMinimum(0);
	hb1->SetStats(0);
	hb1->SetXTitle("#sqrt{s} [GeV]");
	
	TH1F *hb2=new TH1F("hb2","Background fraction",100,2.0, 6.0);
	hb2->SetMaximum(1.0);
	hb2->SetMinimum(5e-4);
	hb2->SetStats(0);
	hb2->SetXTitle("#sqrt{s} [GeV]");
	
	TH1F *hb3=new TH1F("hb3","Background fraction",100,2.0, 6.0);
	hb3->SetMaximum(1.0);
	hb3->SetMinimum(5e-4);
	hb3->SetStats(0);
	hb2->SetXTitle("#sqrt{s} [GeV]");
	
	TH1F *h2=new TH1F("h2","Efficiencies - mass cut only",100,2.0, 6.0);
	h2->SetMaximum(1);
	h2->SetMinimum(0.6);
	h2->SetStats(0);
	h2->SetXTitle("#sqrt{s} [GeV]");
	
	TH1F *h3=new TH1F("h3","Efficiencies - all cuts (high suppression)",100,2.0, 6.0);
	h3->SetMaximum(1);
	h3->SetMinimum(0.0);
	h3->SetStats(0);
	h3->SetXTitle("#sqrt{s} [GeV]");
	
	TH1F *h4=new TH1F("h4","Efficiencies - all cuts (high efficiency)",100,2.0, 6.0);
	h4->SetMaximum(1);
	h4->SetMinimum(0.6);
	h4->SetStats(0);
	h4->SetXTitle("#sqrt{s} [GeV]");
	
	double E[4]={2.4, 3.77, 4.5, 5.5};
	double E2[3]={ 3.77, 4.5, 5.5};
	double E3[2]={4.5, 5.5};
	double E4[1]={ 5.5};
	
	// ****
	// *** Combinatorics and mass cut only
	// ****
//            e^+e^-      \phi    \eta_cJ/\psi(ee)J/\psi(\mu\mu)       D^0     D^\pm       D_s   \Lambda \Lambda_c       DPM
// 2.4  |     79.41     88.32      -         -         -         -         -         -        91.12      -         3.46 
// 3.77 |     80.28     91.16     87.41     79.88     81.49     85.16     81.87      -        91.12      -        11.70 
// 4.5  |     80.43     92.08     89.24     80.55     82.01     87.36     85.55     89.13     90.87      -        15.27 
// 5.5  |     80.88     91.81     89.14     80.84     82.78     87.04     85.54     90.38     91.05     89.06     21.93 

	double ee1[4] =  {79.41 ,  80.28  , 80.43,   80.88};
	double phi1[4] = {88.32 ,  91.16  , 92.08 ,  91.81 };
	double etac1[3] = {  87.41  , 89.24 ,  89.14 };
	double j11[3] = { 79.88  , 80.55  , 80.84};
	double j21[3] = { 81.49  , 82.01  , 82.78};
	double d01[3] = {85.16 ,  87.36  , 87.04 };
	double d1[3] = {81.87 ,  85.55 ,  85.54 };
	double ds1[2] = {89.13 ,  90.38 };
	double lam1[4] = {91.12 ,  91.12  , 90.87 ,  91.05 };
	double lamc1[1] = { 89.06 };
	double dpm1[4] = {3.46 ,  11.70 ,  15.27 ,  21.93};
		
	// ****
	// *** values for forced factor 1000 suppression
	// ****
//            e^+e^-      \phi    \eta_cJ/\psi(ee)J/\psi(\mu\mu)       D^0     D^\pm       D_s   \Lambda \Lambda_c       DPM
// 2.4  |     79.41     83.99      -         -         -         -         -         -        82.63      -         0.08 
// 3.77 |     80.27     85.58     39.49     79.17     80.90     47.31     25.66      -        82.30      -         0.10 
// 4.5  |     79.95     86.57     41.57     79.49     81.02     33.75     20.73     26.69     80.75      -         0.10 
// 5.5  |     80.37     87.18     56.96     77.49     79.96     61.01     55.85     57.79     82.53     37.52      0.09 

	double ee2[4] =  {79.41 ,  80.27  , 79.95  , 80.37};
	double phi2[4] = {83.99 ,85.58 ,86.57 ,87.18 };
	double etac2[3] = { 39.49, 41.57 , 56.96 };
	double j12[3] = {79.17 , 79.49 , 77.49 };
	double j22[3] = {80.90 ,81.02 , 79.96 };
	double d02[3] = {47.31 ,33.75 , 61.01};
	double d2[3] = { 25.66 ,20.73 ,55.85 };
	double ds2[2] = { 26.69 , 57.79 };
	double lam2[4] = { 82.63, 82.30,80.75 ,  82.53};
	double lamc2[1] = { 37.52 };
	double dpm2[4] = {0.08 ,0.10 , 0.10,0.09 };

	// ****
	// *** values for efficiency 90
	// ****
	
/*           e^+e^-      \phi    \eta_cJ/\psi(ee)J/\psi(\mu\mu)       D^0     D^\pm       D_s   \Lambda \Lambda_c       DPM
2.4  |     79.41     83.99      -         -         -         -         -         -        82.63      -         0.08 
3.77 |     80.27     86.13     68.33     79.17     80.90     76.69     72.00      -        82.30      -         0.72 
4.5  |     79.95     86.58     73.64     79.59     81.12     78.50     75.50     74.86     80.75      -         1.14 
5.5  |     80.37     87.18     70.01     77.58     80.03     78.40     74.48     74.16     82.53     72.29      0.98 */
	
	double ee3[4] =  {79.41 ,  80.27  , 78.95  , 80.37};
	double phi3[4] = { 83.99,86.13 , 86.58 , 87.18 };
	double etac3[3] = { 68.33 , 73.64, 70.0};
	double j13[3] = { 79.17, 79.59,77.58 };
	double j23[3] = { 80.90, 81.12, 80.03 };
	double d03[3] = {76.69  ,78.50  , 78.40 };
	double d3[3] = { 72.00 , 75.50,  74.48 };
	double ds3[2] = { 74.86, 74.16 };
	double lam3[4] = {82.63 , 82.30, 80.75,  82.53};
	double lamc3[1] = { 72.29 };
	double dpm3[4] = { 0.08, 0.72 ,1.14 , 0.98};
	
	renorm(dpm1);
	renorm(dpm2);
	renorm(dpm3);
	renorm(ee1);
	renorm(ee2);
	renorm(ee3);
	renorm(phi1);
	renorm(phi2);
	renorm(phi3);
	renorm(etac1,3);
	renorm(etac2,3);
	renorm(etac3,3);
	renorm(j11,3);
	renorm(j12,3);
	renorm(j13,3);
	renorm(j21,3);
	renorm(j22,3);
	renorm(j23,3);
	renorm(d01,3);
	renorm(d02,3);
	renorm(d03,3);
	renorm(d1,3);
	renorm(d2,3);
	renorm(d3,3);
	renorm(ds1,2);
	renorm(ds2,2);
	renorm(ds3,2);
	renorm(lam1);
	renorm(lam2);
	renorm(lam3);
	renorm(lamc1,1);
	renorm(lamc2,1);
	renorm(lamc3,1);
	
	TGraph *gdpm1=new TGraph(4,E,dpm1);
	TGraph *gdpm2=new TGraph(4,E,dpm2);
	TGraph *gdpm3=new TGraph(4,E,dpm3);
	
	TGraph *gee1=new TGraph(4,E,ee1);
	TGraph *gee2=new TGraph(4,E,ee2);
	TGraph *gee3=new TGraph(4,E,ee3);
	
	TGraph *gphi1=new TGraph(4,E,phi1);
	TGraph *gphi2=new TGraph(4,E,phi2);
	TGraph *gphi3=new TGraph(4,E,phi3);
	
	TGraph *get1=new TGraph(3,E2,etac1);
	TGraph *get2=new TGraph(3,E2,etac2);
	TGraph *get3=new TGraph(3,E2,etac3);
	
	TGraph *gj11=new TGraph(3,E2,j11);
	TGraph *gj12=new TGraph(3,E2,j12);
	TGraph *gj13=new TGraph(3,E2,j13);
	
	TGraph *gj21=new TGraph(3,E2,j21);
	TGraph *gj22=new TGraph(3,E2,j22);
	TGraph *gj23=new TGraph(3,E2,j23);
	
	TGraph *gd01=new TGraph(3,E2,d01);
	TGraph *gd02=new TGraph(3,E2,d02);
	TGraph *gd03=new TGraph(3,E2,d03);
	
	TGraph *gd1=new TGraph(3,E2,d1);
	TGraph *gd2=new TGraph(3,E2,d2);
	TGraph *gd3=new TGraph(3,E2,d3);
	
	TGraph *gds1=new TGraph(2,E3,ds1);
	TGraph *gds2=new TGraph(2,E3,ds2);
	TGraph *gds3=new TGraph(2,E3,ds3);
	
	TGraph *glam1=new TGraph(4,E,lam1);
	TGraph *glam2=new TGraph(4,E,lam2);
	TGraph *glam3=new TGraph(4,E,lam3);
	
	TGraph *glc1=new TGraph(1,E4,lamc1);
	TGraph *glc2=new TGraph(1,E4,lamc2);
	TGraph *glc3=new TGraph(1,E4,lamc3);
	
	
	config_graph(gdpm1,20, "gdpm1");config_graph(gdpm2,21, "gdpm2");
	config_graph(gee1,20, "gee1");config_graph(gee2,20, "gee2");
	config_graph(gphi1,21, "gphi1");config_graph(gphi2,21, "gphi2");
	config_graph(gj11,22 , "gj11");config_graph(gj12,22, "gj12");
	config_graph(gj21,23, "gj21");config_graph(gj22,23, "gj22");
	config_graph(get1,24, "get1");config_graph(get2,24, "get2");
	config_graph(gd01,25, "gd01");config_graph(gd02,25, "gd02");
	config_graph(gd1,26, "gd1");config_graph(gd2,26, "gd2");
	config_graph(gds1,27, "gds1");config_graph(gds2,27, "gds2");
	config_graph(glam1,28, "glam1");config_graph(glam2,28, "glam2");
	config_graph(glc1,29, "glc1");config_graph(glc2,29, "glc2");
	
	config_graph(gdpm3,21, "gdpm3");
	config_graph(gee3,20, "gee3");
	config_graph(gphi3,21, "gphi3");
	config_graph(gj13,22 , "gj13");
	config_graph(gj23,23, "gj23");
	config_graph(get3,24, "get3");
	config_graph(gd03,25, "gd03");
	config_graph(gd3,26, "gd3");
	config_graph(gds3,27, "gds3");
	config_graph(glam3,28, "glam31");
	config_graph(glc3,29, "glc3");
	
	glc1->SetMarkerSize(2);glc2->SetMarkerSize(2);glc3->SetMarkerSize(2);
	
	
	// ********** 
	// ********** background mass only
	// ********** 
	
	(cb1->cd())->SetLogy(0);
	gPad->SetGridx();
	gPad->SetGridy();
	
	hb1->Draw();
	gdpm1->Draw("LP same");
	//gdpm2->Draw("LP same");
	//TLegend *leg1 = new TLegend(0.12,0.75,0.58,0.9);
	TLegend *leg1 = new TLegend(0.12,0.83,0.58,0.9);
	leg1->AddEntry("gdpm1","mass cut only","lp");
	//leg1->AddEntry("gdpm2","all cuts (high suppression)","lp");
	leg1->Draw();
	
	cb1->SaveAs("figtoy_v2/Background_massonly.gif");
	cb1->SaveAs("figtoy_v2/Background_massonly.pdf");

	// ********** 
	// ********** background mass only + eff 90
	// ********** 
	
	(cb2->cd())->SetLogy(1);
	gPad->SetGridx();
	gPad->SetGridy();
	
	hb2->Draw();
	gdpm1->Draw("LP same");
	gdpm2->Draw("LP same");
	//TLegend *leg1 = new TLegend(0.12,0.75,0.58,0.9);
	TLegend *leg11 = new TLegend(0.12,0.75,0.62,0.9);
	leg11->AddEntry("gdpm1","mass cut only","lp");
	leg11->AddEntry("gdpm2","all cuts (high supression)","lp");
	leg11->Draw();
	
	cb2->SaveAs("figtoy_v2/Background_supr1000.gif");
	cb2->SaveAs("figtoy_v2/Background_supr1000.pdf");

	// ********** 
	// ********** background mass only + supr 1000
	// ********** 
	
	(cb3->cd())->SetLogy(1);
	gPad->SetGridx();
	gPad->SetGridy();
	
	hb3->Draw();
	gdpm1->Draw("LP same");
	gdpm3->Draw("LP same");
	//TLegend *leg1 = new TLegend(0.12,0.75,0.58,0.9);
	TLegend *leg12 = new TLegend(0.12,0.75,0.62,0.9);
	leg12->AddEntry("gdpm1","mass cut only","lp");
	leg12->AddEntry("gdpm3","all cuts (high efficiency)","lp");
	leg12->Draw();
	
	cb3->SaveAs("figtoy_v2/Background_eff90.gif");
	cb3->SaveAs("figtoy_v2/Background_eff90.pdf");

	// ********** 
	// ********** Efficiencies mass only
	// ********** 
	
	c2->cd();
	gPad->SetGridx();
	gPad->SetGridy();
	h2->Draw();
	gee1->Draw("LP same"); 
	gphi1->Draw("LP same");
	get1->Draw("LP same"); 
	gj11->Draw("LP same");
	gj21->Draw("LP same"); 
	gd01->Draw("LP same");
	gd1->Draw("LP same"); 
	gds1->Draw("LP same");
	glam1->Draw("LP same"); 
	glc1->Draw("LP same");
//	TLegend *leg2 = new TLegend(0.12,0.68,0.5,0.9);
	TLegend *leg2 = new TLegend(0.12,0.16,0.5,0.40);
	leg2->SetNColumns(3);
	leg2->AddEntry("glam1","#Lambda","lp");
	leg2->AddEntry("glc1","#Lambda_{c}","lp");
	leg2->AddEntry("gphi1","#phi","lp");
	leg2->AddEntry("gj11","J/#psi(2e)","lp");
	leg2->AddEntry("gj21","J/#psi(2#mu)","lp");
	leg2->AddEntry("get1","#eta_{c}","lp");
	leg2->AddEntry("gd01","D^{0}","lp");
	leg2->AddEntry("gd1","D^{#pm}","lp");
	leg2->AddEntry("gds1","D_{s}","lp");
	leg2->AddEntry("gee1","e+e-","lp");
	leg2->Draw();
	c2->SaveAs("figtoy_v2/Efficiency_sum_mass_only.gif");
	c2->SaveAs("figtoy_v2/Efficiency_sum_mass_only.pdf");
	
	// ********** 
	// ********** Efficiencies eff 90
	// ********** 
	
	c3->cd();
	gPad->SetGridx();
	gPad->SetGridy();
	h3->Draw();
	gee2->Draw("LP same"); 
	gphi2->Draw("LP same");
	get2->Draw("LP same"); 
	gj12->Draw("LP same");
	gj22->Draw("LP same"); 
	gd02->Draw("LP same");
	gd2->Draw("LP same"); 
	gds2->Draw("LP same");
	glam2->Draw("LP same"); 
	glc2->Draw("LP same");

	TLegend *leg3 = new TLegend(0.12,0.16,0.45,0.45);
	leg3->SetNColumns(2);
/*	TLegend *leg3 = new TLegend(0.12,0.66,0.5,0.9);
	leg3->SetNColumns(3);*/
	leg3->AddEntry("glam2","#Lambda","lp");
	leg3->AddEntry("glc2","#Lambda_{c}","lp");
	leg3->AddEntry("gphi2","#phi","lp");
	leg3->AddEntry("gj12","J/#psi(2e)","lp");
	leg3->AddEntry("gj22","J/#psi(2#mu)","lp");
	leg3->AddEntry("get2","#eta_{c}","lp");
	leg3->AddEntry("gd02","D^{0}","lp");
	leg3->AddEntry("gd2","D^{#pm}","lp");
	leg3->AddEntry("gds2","D_{s}","lp");
	leg3->AddEntry("gee2","e+e-","lp");
	leg3->Draw();
	c3->SaveAs("figtoy_v2/Efficiency_sum_opt_supr1000.gif");
	c3->SaveAs("figtoy_v2/Efficiency_sum_opt_supr1000.pdf");
	
	// ********** 
	// ********** Efficiencies supr 1000
	// ********** 
	
	c4->cd();
	gPad->SetGridx();
	gPad->SetGridy();
	h4->Draw();
	gee3->Draw("LP same"); 
	gphi3->Draw("LP same");
	get3->Draw("LP same"); 
	gj13->Draw("LP same");
	gj23->Draw("LP same"); 
	gd03->Draw("LP same");
	gd3->Draw("LP same"); 
	gds3->Draw("LP same");
	glam3->Draw("LP same"); 
	glc3->Draw("LP same");

	TLegend *leg4 = new TLegend(0.12,0.16,0.45,0.45);
	leg4->SetNColumns(2);
/*	TLegend *leg3 = new TLegend(0.12,0.66,0.5,0.9);
	leg3->SetNColumns(3);*/
	leg4->AddEntry("glam3","#Lambda","lp");
	leg4->AddEntry("glc3","#Lambda_{c}","lp");
	leg4->AddEntry("gphi3","#phi","lp");
	leg4->AddEntry("gj13","J/#psi(2e)","lp");
	leg4->AddEntry("gj23","J/#psi(2#mu)","lp");
	leg4->AddEntry("get3","#eta_{c}","lp");
	leg4->AddEntry("gd03","D^{0}","lp");
	leg4->AddEntry("gd3","D^{#pm}","lp");
	leg4->AddEntry("gds3","D_{s}","lp");
	leg4->AddEntry("gee3","e+e-","lp");
	leg4->Draw();
	c4->SaveAs("figtoy_v2/Efficiency_sum_opt_eff90.gif");
	c4->SaveAs("figtoy_v2/Efficiency_sum_opt_eff90.pdf");
}







