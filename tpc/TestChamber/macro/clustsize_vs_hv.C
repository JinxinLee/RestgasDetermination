{

gROOT->Macro("christian_style.C");
gStyle->SetPadRightMargin(0.05);

TCanvas *clust_canvas = new TCanvas("clust_canvas","Cluster Size vs HV",10,10,1200,900);
//TPad::Pad()->SetGridy();
//TPad::Pad()->SetLogy();
  

//===================================
//Eingabewerte
//===================================


//Zahl der Messpunkte
Int_t n = 4;

//GEM-stack voltage in %
double voltage[n] = {98,100,103,106};
  
//Fehler dazu
double d_voltage[n] = {0,0,0,0};

//clustersize
double clustsize[n] = {2.4, 3.2, 3.7, 4.3};
		    
//Fehler dazu
double d_clustsize[n] = {1.3,1.7,2.1,2.6};


//===================================
//Graph plotten
//===================================


TGraphErrors *graph1 = new TGraphErrors(n, voltage, clustsize, 
					  d_voltage, d_clustsize);
graph1->SetTitle("");
graph1->SetMarkerStyle(21);
graph1->SetMarkerColor(2);
graph1->SetMarkerSize(1.5);
graph1->SetLineWidth(1.5);

TH2F *hr = new TH2F("hr","hr",100,97,108,100,0,7.2);
hr->SetStats(kFALSE);
//hr->SetTitle("Gain Measurement");
hr->SetTitle("");
hr->SetYTitle("Cluster Size");
hr->SetXTitle("Detector Voltage [%]");

hr->Draw();
graph1->Draw("p");

TPave *p = new TPave(100.5, 0.2, 107.5, 1.55); 
p->SetBorderSize(8);
p->SetLineWidth(2);
p->SetLineColor(4);
//p->SetFillStyle(1);
//p->SetFillColor(0);
p->Draw();

TText *t1 = new TText(0.4, 0.32, "Settings corresponding to 100%:");
t1->SetNDC();
t1->SetTextSize(0.038);
t1->SetTextFont(102);
t1->Draw();

TText *t2 = new TText(0.4, 0.25, "GEM1: 400V, GEM2: 365V, GEM3: 320V");
t2->SetNDC();
t2->SetTextSize(0.032);
t2->SetTextFont(102);
t2->SetTextColor(4);
t2->Draw();

TText *t3 = new TText(0.4, 0.21, "Transfer-, Collection Field: 3.75kV/cm");
t3->SetNDC();
t3->SetTextSize(0.032);
t3->SetTextFont(102);
t3->SetTextColor(4);
t3->Draw();

TArrow *a = new TArrow(100.4, 0.4, 100.1, 0.05);
a->SetArrowSize(0.02);
a->SetLineWidth(2);
a->SetLineColor(4);
a->Draw();

TText *t4 = new TText(0.15, 0.78, "Drift Field fixed at 250V/cm");
t4->SetNDC();
t4->SetTextSize(0.04);
t4->SetTextFont(102);
t4->SetTextColor(4);
t4->Draw();

clust_canvas->Modified();
clust_canvas->Update();

}
