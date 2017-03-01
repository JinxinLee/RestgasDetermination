
// ############### TOY #################

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

// ############### FULL #################

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

int overall_perf_full()
{
	setStyle();
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,700,500);
	TCanvas *c2=new TCanvas("c2","c2",900,10,700,500);
	TCanvas *c3=new TCanvas("c3","c3",900,500,700,500);
	
	TH1F *h=new TH1F("h","Full MC - Background fraction",100,2.0, 6.0);
	h->SetMaximum(1);
	h->SetMinimum(5*1e-4);
	h->SetStats(0);
	h->SetXTitle("#sqrt{s} [GeV]");
	
	TH1F *h2=new TH1F("h","Full MC - Efficiency - mass cut only",100,2.0, 6.0);
	h2->SetMaximum(0.9);
	h2->SetMinimum(0.0);
	h2->SetStats(0);
	h2->SetXTitle("#sqrt{s} [GeV]");
	
	TH1F *h3=new TH1F("h","Full MC - Efficiency - all cuts (high suppression)",100,2.0, 6.0);
	h3->SetMaximum(0.9);
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
//        & e^+e^-   & \phi     & \eta_c   & J/\psi(e)&J/\psi(mu)& D^0      & D^\pm    & D_s      & \Lambda  & \Lambda_c& DPM     \\\hline
//   2.4  &    50.81 &    35.57 &      -   &      -   &      -   &      -   &      -   &      -   &    19.69 &      -   &     3.16\\
//  3.77  &    43.30 &    43.81 &    42.95 &    43.35 &    56.35 &    49.56 &    40.88 &      -   &    21.06 &      -   &    16.06\\
//   4.5  &    45.48 &    43.43 &    52.69 &    42.90 &    55.17 &    54.86 &    50.39 &    53.07 &    21.73 &      -   &    26.48\\
//   5.5  &    36.53 &    41.36 &    58.02 &    47.62 &    58.14 &    57.50 &    55.08 &    58.10 &    23.05 &    60.63 &    40.11\\

	double ee1[4] =   { 50.81 , 43.30 , 45.48 , 36.53 };
	double phi1[4] =  { 35.57 , 43.81 , 43.43 , 41.36 };
	double etac1[3] = { 42.95 , 52.69 , 58.02 };
	double j11[3] =   { 43.35 , 42.90 , 47.62 };
	double j21[3] =   { 56.35 , 55.17 , 58.14 };
	double d01[3] =   { 49.56 , 54.86 , 57.50 };
	double d1[3] =    { 40.88 , 50.39 , 55.08 };
	double ds1[2] =   { 53.07 , 58.10 };
	double lam1[4] =  { 19.69 , 21.06 , 21.73 , 23.05 };
	double lamc1[1] = { 60.63 };
	double dpm1[4] =  {  3.16 , 16.06 , 26.48 , 40.11 };
		
	// ****
	// *** values for high efficiency (90%)
	// ****
/*        & e^+e^-   & \phi     & \eta_c   & J/\psi(e)&J/\psi(mu)& D^0      & D^\pm    & D_s      & \Lambda  & \Lambda_c& DPM     \\\hline
  2.4  &    50.55 &    31.03 &      -   &      -   &      -   &      -   &      -   &      -   &    17.07 &      -   &     0.24\\
 3.77  &    40.86 &    36.92 &    21.51 &    38.90 &    53.15 &    40.75 &    27.80 &      -   &    17.81 &      -   &     0.80\\
  4.5  &    44.05 &    31.70 &    35.23 &    36.23 &    50.22 &    45.91 &    37.08 &    39.15 &    17.93 &      -   &     1.97\\
  5.5  &    33.17 &    32.93 &    39.68 &    33.57 &    48.87 &    48.32 &    40.79 &    45.34 &    17.94 &    43.40 &    14.08\\*/
// 	double ee2[4] =   { 50.55 , 40.86 , 44.05 , 33.17 };
// 	double phi2[4] =  { 31.03 , 36.92 , 31.70 , 32.93 };
// 	double etac2[3] = { 21.51 , 35.23 , 39.68 };
// 	double j12[3] =   { 38.90 , 36.23 , 33.57 };
// 	double j22[3] =   { 53.15 , 50.22 , 48.87 };
// 	double d02[3] =   { 40.75 , 45.91 , 48.32 };
// 	double d2[3] =    { 27.80 , 37.08 , 40.79 };
// 	double ds2[2] =   { 39.15 , 45.34 };
// 	double lam2[4] =  { 17.07 , 17.81 , 17.93 , 17.94 };
// 	double lamc2[1] = { 43.40 };
// 	double dpm2[4] =  { 0.24 , 0.80 , 1.97 , 14.08 };


	// ****
	// *** values for forced factor 1000 suppression
	// ****
/*           e^+e^-      \phi    \eta_cJ/\psi(ee)J/\psi(\mu\mu)       D^0     D^\pm       D_s   \Lambda \Lambda_c       DPM
2.4  |     50.59     30.43      -         -         -         -         -         -        13.93      -         0.09 
3.77 |     40.04     32.49      7.82     38.67     53.09     34.47     20.49      -        10.95      -         0.08 
4.5  |     44.05     26.09     11.23     36.17     50.20     21.44     13.70     17.78      9.21      -         0.09 
5.5  |     33.37     25.39      6.72     30.70     48.56     22.73     14.81     12.74     10.57      6.99      0.10 */
	double ee2[4] =   { 50.59 , 40.04 , 44.05 , 33.37 };
	double phi2[4] =  { 30.43 , 32.49 , 26.09 , 25.39 };
	double etac2[3] = { 7.82 , 11.23 , 6.72 };
	double j12[3] =   { 38.67 , 36.17 , 30.70 };
	double j22[3] =   { 53.09 , 50.20 , 48.56 };
	double d02[3] =   { 34.47 , 21.44 , 22.73 };
	double d2[3] =    { 20.49 , 13.70 , 14.81 };
	double ds2[2] =   { 17.78 , 12.74 };
	double lam2[4] =  { 13.93 , 10.95 , 9.21 , 10.57 };
	double lamc2[1] = { 6.99 };
	double dpm2[4] =  { 0.09 , 0.08 , 0.09 , 0.10 };

	
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
	
	(c1->cd())->SetLogy(1);
	gPad->SetGridx();
	gPad->SetGridy();
	
	h->Draw();
	gdpm1->Draw("LP same");
	gdpm2->Draw("LP same");
	TLegend *leg1 = new TLegend(0.12,0.75,0.55,0.9);
	//TLegend *leg1 = new TLegend(0.12,0.83,0.58,0.9);
	leg1->AddEntry("gdpm1","mass cut only","lp");
	leg1->AddEntry("gdpm2","all cuts (high efficiency)","lp");
	leg1->Draw();
	
	c1->SaveAs("figfull/Background_sum_supr1000_full.gif");
	c1->SaveAs("figfull/Background_sum_supr1000_full.pdf");

	
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
	TLegend *leg2 = new TLegend(0.12,0.68,0.5,0.9);
//	TLegend *leg2 = new TLegend(0.12,0.16,0.5,0.40);
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
/*	c2->SaveAs("figfull/Efficiency_sum_mass_only_full.gif");
	c2->SaveAs("figfull/Efficiency_sum_mass_only_full.pdf");*/
	
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

	TLegend *leg3 = new TLegend(0.12,0.68,0.5,0.9);
//	TLegend *leg3 = new TLegend(0.12,0.16,0.45,0.45);
	leg3->SetNColumns(3);
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
	c3->SaveAs("figfull/Efficiency_sum_opt_supr1000_full.gif");
	c3->SaveAs("figfull/Efficiency_sum_opt_supr1000_full.pdf");
  return 0;
}







