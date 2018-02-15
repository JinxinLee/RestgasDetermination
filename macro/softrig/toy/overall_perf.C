// *** MASS cut only
//                  2.40    3.77    4.50   5.50
// ---------------------------------------------
// e^+e^-     |    79.41   80.28   80.43   80.88
// \phi       |    88.32   91.16   92.08   91.81
// \eta_c     |     -      87.41   89.24   89.14
// J/\psi(1)  |     -      79.88   80.55   80.84
// J/\psi(2)  |     -      81.49   82.01   82.78
// D^0        |     -      85.16   87.36   87.04
// D^\pm      |     -      81.87   85.55   85.54
// D_s        |     -       -      89.13   90.38
// \Lambda    |    91.12   91.12   90.87   91.05
// \Lambda_c  |     -       -       -      89.06
// DPM        |     3.46   11.70   15.27   21.93
// 

// *** intermediate solution
//                  2.40    3.77    4.50   5.50
// ---------------------------------------------
// e^+e^-     |    79.41   80.27   78.92   79.24
// \phi       |    71.31   79.88   73.34   84.64
// \eta_c     |     -      43.07   41.60   52.76
// J/\psi(1)  |     -      79.22   78.20   77.71
// J/\psi(2)  |     -      80.94   79.92   79.91
// D^0        |     -      50.52   38.94   55.53
// D^\pm      |     -      29.65   28.90   46.91
// D_s        |     -       -      41.90   54.91
// \Lambda    |    45.98   67.65   76.23   72.77
// \Lambda_c  |     -       -       -      39.52
// DPM        |     0.10    0.16    0.18    0.12

// *** FACTOR 1000
//                  2.40    3.77    4.50   5.50
// ---------------------------------------------
// e^+e^-     |    79.41   80.27   78.92   79.24
// \phi       |    71.31   79.81   73.32   84.64
// \eta_c     |     -      29.33   39.63   52.70
// J/\psi(ee) |     -      79.22   78.15   77.71
// J/\psi(\mu\mu) |     -      80.94   79.86   79.91
// D^0        |     -      45.08   21.55   55.50
// D^\pm      |     -      15.59   17.58   46.86
// D_s        |     -       -      24.30   54.83
// \Lambda    |    45.98   67.65   76.23   72.77
// \Lambda_c  |     -       -       -      36.13
// DPM        |     0.10    0.09    0.09    0.09


// *** High EFFICIENCY >90% after combinatorics
//                  2.40    3.77    4.50   5.50
// ---------------------------------------------
// e^+e^-     |    79.41   80.27   78.92   79.24
// \phi       |    80.84   86.55   83.84   85.83
// \eta_c     |     -      69.03   77.04   71.28
// J/\psi(ee) |     -      79.17   78.28   77.87
// J/\psi(\mu\mu) |     -      80.90   79.97   80.02
// D^0        |     -      76.64   79.19   78.52
// D^\pm      |     -      71.74   76.10   73.67
// D_s        |     -       -      74.75   74.42
// \Lambda    |    84.72   83.03   83.77   81.75
// \Lambda_c  |     -       -       -      72.39
// DPM        |     0.08    0.80    1.49    1.11
// 

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

int overall_perf()
{
	setStyle();
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,700,500);
	TCanvas *c2=new TCanvas("c2","c2",900,10,700,500);
	TCanvas *c3=new TCanvas("c3","c3",900,500,700,500);
	
	TH1F *h=new TH1F("h","Background fraction",100,2.0, 6.0);
	h->SetMaximum(0.3);
	h->SetMinimum(0);
	h->SetStats(0);
	h->SetXTitle("#sqrt{s} [GeV]");
	
	TH1F *h2=new TH1F("h","Efficiency - mass cut only",100,2.0, 6.0);
	h2->SetMaximum(1);
	h2->SetMinimum(0.65);
	h2->SetStats(0);
	h2->SetXTitle("#sqrt{s} [GeV]");
	
	TH1F *h3=new TH1F("h","Efficiency - all cuts (high suppression)",100,2.0, 6.0);
	h3->SetMaximum(1);
	h3->SetMinimum(0.0);
	h3->SetStats(0);
	h3->SetXTitle("#sqrt{s} [GeV]");
	
	double E[4]={2.4, 3.77, 4.5, 5.5};
	double E2[3]={ 3.77, 4.5, 5.5};
	double E3[2]={4.5, 5.5};
	double E4[1]={ 5.5};
	
	// ****
	// *** Combinatorics and mass cut only
	// ****
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
	double ee2[4] =  {79.41 ,  80.27  , 78.92  , 79.24};
	double phi2[4] = {71.31 ,  79.81  , 73.32  , 84.64 };
	double etac2[3] = {  29.33  , 39.63  , 52.70 };
	double j12[3] = {79.22  , 78.15  , 77.71 };
	double j22[3] = {80.94 ,  79.86  , 79.91 };
	double d02[3] = {45.08 ,  21.55 ,  55.50 };
	double d2[3] = { 15.59  , 17.58 ,  46.86};
	double ds2[2] = {24.30 , 54.83 };
	double lam2[4] = {45.98 ,  67.65  , 76.23 ,  72.77 };
	double lamc2[1] = {36.13 };
	double dpm2[4] = {0.10 ,   0.09  ,  0.09  , 0.09};

//                  2.40    3.77    4.50   5.50
// ---------------------------------------------
// e^+e^-     |    79.41   80.27   78.92   79.24
// \phi       |    80.84   86.55   83.84   85.83
// \eta_c     |     -      69.03   77.04   71.28
// J/\psi(ee) |     -      79.17   78.28   77.87
// J/\psi(\mu\mu) |     -      80.90   79.97   80.02
// D^0        |     -      76.64   79.19   78.52
// D^\pm      |     -      71.74   76.10   73.67
// D_s        |     -       -      74.75   74.42
// \Lambda    |    84.72   83.03   83.77   81.75
// \Lambda_c  |     -       -       -      72.39
// DPM        |     0.08    0.80    1.49    1.11
	
	// ****
	//*** Values with high efficiency
	// ****
/*	double ee2[4] =  {79.41 ,  80.27  , 78.92  , 79.24};
	double phi2[4] = {  80.84 ,  86.55 ,  83.84  , 85.83};
	double etac2[3] = { 69.03 ,  77.04 ,  71.28  };
	double j12[3] = { 79.17 ,  78.28 ,  77.87 };
	double j22[3] = {80.90  , 79.97  , 80.02};
	double d02[3] = {76.64  , 79.19  , 78.52};
	double d2[3] = {71.74  , 76.10 ,  73.67 };
	double ds2[2] = { 74.75 ,  74.42};
	double lam2[4] = { 84.72  , 83.03  , 83.77 ,  81.75 };
	double lamc2[1] = { 72.39};
	double dpm2[4] = { 0.08   , 0.80  ,  1.49 ,   1.11};*/
	
	renorm(dpm1);renorm(dpm2);renorm(ee1);renorm(ee2);
	renorm(phi1);renorm(phi2);renorm(etac1,3);renorm(etac2,3);
	renorm(j11,3);renorm(j12,3);renorm(j21,3);renorm(j22,3);
	renorm(d01,3);renorm(d02,3);renorm(d1,3);renorm(d2,3);
	renorm(ds1,2);renorm(ds2,2);renorm(lam1);renorm(lam2);
	renorm(lamc1,1);renorm(lamc2,1);
	
	TGraph *gdpm1=new TGraph(4,E,dpm1);
	TGraph *gdpm2=new TGraph(4,E,dpm2);
	
	TGraph *gee1=new TGraph(4,E,ee1);
	TGraph *gee2=new TGraph(4,E,ee2);
	
	TGraph *gphi1=new TGraph(4,E,phi1);
	TGraph *gphi2=new TGraph(4,E,phi2);
	
	TGraph *get1=new TGraph(3,E2,etac1);
	TGraph *get2=new TGraph(3,E2,etac2);
	
	TGraph *gj11=new TGraph(3,E2,j11);
	TGraph *gj12=new TGraph(3,E2,j12);
	
	TGraph *gj21=new TGraph(3,E2,j21);
	TGraph *gj22=new TGraph(3,E2,j22);
	
	TGraph *gd01=new TGraph(3,E2,d01);
	TGraph *gd02=new TGraph(3,E2,d02);
	
	TGraph *gd1=new TGraph(3,E2,d1);
	TGraph *gd2=new TGraph(3,E2,d2);
	
	TGraph *gds1=new TGraph(2,E3,ds1);
	TGraph *gds2=new TGraph(2,E3,ds2);
	
	TGraph *glam1=new TGraph(4,E,lam1);
	TGraph *glam2=new TGraph(4,E,lam2);
	
	TGraph *glc1=new TGraph(1,E4,lamc1);
	TGraph *glc2=new TGraph(1,E4,lamc2);
	
	
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
	
	glc1->SetMarkerSize(2);glc2->SetMarkerSize(2);
	
	(c1->cd())->SetLogy(0);
	gPad->SetGridx();
	gPad->SetGridy();
	
	h->Draw();
	gdpm1->Draw("LP same");
	//gdpm2->Draw("LP same");
	//TLegend *leg1 = new TLegend(0.12,0.75,0.58,0.9);
	TLegend *leg1 = new TLegend(0.12,0.83,0.58,0.9);
	leg1->AddEntry("gdpm1","mass cut only","lp");
	//leg1->AddEntry("gdpm2","all cuts (high suppression)","lp");
	leg1->Draw();
	
	c1->SaveAs("fig/Background_sum.gif");
	c1->SaveAs("fig/Background_sum.pdf");

	
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
	c2->SaveAs("fig/Efficiency_sum_mass_only.gif");
	c2->SaveAs("fig/Efficiency_sum_mass_only.pdf");
	
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
	c3->SaveAs("fig/Efficiency_sum_opt_fac1000.gif");
	c3->SaveAs("fig/Efficiency_sum_opt_fac1000.pdf");
  return 0;
}







