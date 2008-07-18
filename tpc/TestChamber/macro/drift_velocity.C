{

gROOT->Macro("christian_style.C");
gStyle->SetPadRightMargin(0.05);

TCanvas *drift_canvas = new TCanvas("drift_canvas","Drift Velocity",10,10,1200,900);
//TPad::Pad()->SetGridy();
//TPad::Pad()->SetLogy();
  

//===================================
//Eingabewerte
//===================================


//Zahl der Messpunkte
Int_t n = 40;

//Mittlere Zeitpunkt des Runs [h ab 02.02.07]
double time[n] = {12.5, 111.5, 122.5, 132.5, //H1
		  158, 168, 181, 190, 214, 238, 253.5, //H2
		  262, 277.5, 287, 301.5, 311, //H3
		  327.5, 336, 349.5, 359, 381, 405, 421.5, 430, 444.5, //H4
		  454, //H5
		  469, 478, 493.5, 503, 517.5, 574, 589.5, 598, 613.5, 624, 637.5, 649, //H6
		  661.5, 671}; //H7
  
//Fehler dazu
double d_time[n] = {0,0,0,0, //H1
		    0,0,0,0,0,0,0 //H2
		    0,0,0,0,0 //H3
		    0,0,0,0,0,0,0,0,0, //H4
		    0, //H5
		    0,0,0,0,0,0,0,0,0,0,0,0, //H6
		    0,0}; //H7

//Drift Geschwindigkeit [cm/us]
double velo[n] = {0.556, 0.567, 0.570, 0.570, //H1
		  0.573, 0.570, 0.572, 0.571, 0.573, 0.571, 0.576, //H2
		  0.573, 0.568, 0.559, 0.571, 0.574, //H3
		  0.562, 0.560, 0.565, 0.565, 0.567, 0.571, 0.572, 0.566, 0.572, //H4
		  0.566, //H5
		  0.569, 0.567, 0.570, 0.570, 0.573, 0.569, 0.571, 0.566, 0.565, 0.565, 0.570, 0.567, //H6
		  0.574, 0.573}; //H7

//Fehler dazu
double d_velo[n] = {0.006, 0.006, 0.008, 0.007, //H1
		    0.006, 0.006, 0.007, 0.005, 0.006, 0.007, 0.005, //H2
		    0.006, 0.006, 0.005, 0.006, 0.005, //H3
		    0.005, 0.006, 0.007, 0.006, 0.006, 0.005, 0.006, 0.006, 0.006, //H4
		    0.007, //H5
		    0.006, 0.007, 0.006, 0.007, 0.006, 0.007, 0.006, 0.007, 0.006, 0.007, 0.007, 0.007, //H6
		    0.005, 0.006}; //H7
		    
//===================================
//Graph plotten
//===================================


TGraphErrors *graph1 = new TGraphErrors(n, time, velo, 
					  d_time, d_velo);
graph1->SetTitle("");
graph1->SetMarkerStyle(21);
graph1->SetMarkerColor(2);
graph1->SetMarkerSize(2.0);
graph1->SetLineWidth(2);

TH2F *hr = new TH2F("hr","hr",100,0,700,100,0.53,0.6);
hr->SetStats(kFALSE);
//hr->SetTitle("Gain Measurement");
hr->SetTitle("");
hr->SetYTitle("Drift Velocity [cm/#mus]");
hr->SetTitleOffset(1.4,"y");
hr->SetXTitle("Time [h since 02.02.07]");
//hr->GetYaxis()->CenterTitle();
//hr->GetYaxis()->SetTitleOffset(1.5);
//hr->GetYaxis()->SetTitleSize(0.04);
//hr->GetXaxis()->CenterTitle();
//hr->GetXaxis()->SetTitleOffset(1.4);
//hr->GetXaxis()->SetTitleSize(0.04);
//hr->SetLabelOffset(0.01, "X");
//hr->SetLabelOffset(0.01, "Y");
//hr->SetLabelSize(0.04, "X");
//hr->SetLabelSize(0.04, "Y");

hr->Draw();
graph1->Draw("p");

TLine *line1 = new TLine(145.35,0.53,145.25,0.6);
TLine *line2 = new TLine(257.75,0.53,257.75,0.6);
TLine *line3 = new TLine(319.25,0.53,319.25,0.6);
TLine *line4 = new TLine(449.25,0.53,449.25,0.6);
TLine *line5 = new TLine(461.50,0.53,461.50,0.6);
TLine *line6 = new TLine(655.25,0.53,655.25,0.6);

line1->SetLineWidth(2);
line2->SetLineWidth(2);
line3->SetLineWidth(2);
line4->SetLineWidth(2);
line5->SetLineWidth(2);
line6->SetLineWidth(2);

line1->SetLineColor(4);
line2->SetLineColor(4);
line3->SetLineColor(4);
line4->SetLineColor(4);
line5->SetLineColor(4);
line6->SetLineColor(4);

line1->Draw();
line2->Draw();
line3->Draw();
line4->Draw();
line5->Draw();
line6->Draw();

TText *t1 = new TText(50,0.54,"H1");
t1->SetTextFont(102);
t1->SetTextColor(4);

TText *t2 = new TText(185,0.54,"H2");
t2->SetTextFont(102);
t2->SetTextColor(4);

TText *t3 = new TText(270,0.54,"H3");
t3->SetTextFont(102);
t3->SetTextColor(4);

TText *t4 = new TText(370,0.54,"H4");
t4->SetTextFont(102);
t4->SetTextColor(4);

TText *t5 = new TText(437,0.54,"H5");
t5->SetTextFont(102);
t5->SetTextColor(4);

TText *t6 = new TText(537,0.54,"H6");
t6->SetTextFont(102);
t6->SetTextColor(4);

TText *t7 = new TText(660,0.54,"H7");
t7->SetTextFont(102);
t7->SetTextColor(4);

t1->Draw();
t2->Draw();
t3->Draw();
t4->Draw();
t5->Draw();
t6->Draw();
t7->Draw();

//gain_canvas->Modified();
//gain_canvas->Update();

}
