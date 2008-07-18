// ===========================================================================
// Gain-Messung, 13.01.06 + 20.01.06
// Method: Pulsheight Spectrum + Strommessung, !!! Pulseheight method is wrong
// X-Rays: 10 mA, 15 kV, with filter!
// Window: the one close to the pads
// ===========================================================================

{

  /*
  gStyle->SetFillStyle(0);
  gStyle->SetFillColor(10);
  gStyle->SetCanvasColor(10);
  gStyle->SetDrawBorder(0);
  gStyle->SetPadColor(10);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetPadRightMargin(0.12);
  gStyle->SetPadBottomMargin(0.13);
  */

  gROOT->Macro("christian_style.C");
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetOptFit(0);
  

  TCanvas *gain_canvas = new TCanvas("gain_canvas","Gain Measurement",10,10,1200,900);
  TPad::Pad()->SetGridy();
  TPad::Pad()->SetLogy();
  

  //===================================
  //Allgemeine Eingabewerte
  //===================================


  //Zahl der Messpunkte
  Int_t n = 6;

  //GEM-stack voltage in %
  double voltage[n] = {100,101,102,103,104,105};
  
  //Fehler der GEM-stack voltage in %
  double d_voltage[n] = {0,0,0,0,0,0};


  //===================================
  //Eingabewerte 1
  //===================================

  
  //Pulshoehe vom Fit (Photopeak)
  double pulseheight[n] = {738.8,984.7,1257.4,772.5,1011.9,1239.3};

  //Fehler der Pulshoehe (auch vom Fit)
  double d_pulseheight[n] = {1.8,2.1,2.4,1.6,2.0,2.3};

  //Testpuls-Position (gefittet)
  double testpulse[n] = {871.9,871.9,871.9,1318.6,1318.6,1318.6};

  //Fehler der Testpuls-Position (vom Fit)
  double d_testpulse[n] = {1.4,1.4,1.4,0.8,0.8,0.8};

  //Testpuls Spannung in mV
  double testvoltage[n] = {55,55,55,165,165,165};

  //Testpuls Spannung Fehler (Ablesefehler)
  double d_testvoltage[n] = {1,1,1,3,3,3};


  //===================================
  //Eingabewerte 2
  //===================================


  //X-ray Rate (volle Rate im Plateau) [1/s]
  double xrate = 341;

  //Fehler der X-ray Rate (sqrt(N)) [1/s]
  double d_xrate = 18;

  //Background Rate [1/s]
  double bgrate = 11;

  //Fehler der Bg Rate [1/s]
  double d_bgrate = 3;

  //Strom Messung [pA]
  double current[n] = {130,140,170,200,250,310};

  //Fehler Strom Messung [pA]
  double d_current[n] = {10,10,10,10,20,20};

  //Background Strom Messung [pA]
  double bgcurrent[n] = {60,50,50,30,30,30};

  // Fehler Bg Strom Messung [pA]
  //double d_bgcurrent[n] = {20,10,20,10,10,10};
  double d_bgcurrent[n] = {6,5,5,3,3,3};

  //===================================
  //Rechnung 1
  //===================================


  //Eichung des Spektrums (Annahme: Nullpunktsgerade!)
  double m[n];
  double d_m[n];
  for (int i=0; i<n; i++){
    m[i] = (testvoltage[i])/(testpulse[i]);
    d_m[i]=sqrt((1./testpulse[i])*(1./testpulse[i])*
	       d_testvoltage[i]*d_testvoltage[i] +
	       (testvoltage[i]/(testpulse[i]*testpulse[i]))*
	       (testvoltage[i]/(testpulse[i]*testpulse[i]))*
	       d_testpulse[i]*d_testpulse[i]);
  }

  //Berechnung des Gains
  double gain1[n];
  double d_gain1[n];
  for (int i=0; i<n; i++){
    gain1[i]=(((pulseheight[i]*m[i])*1.0E-3*1.0E-12)/(293*1.6E-19))*(93./50.);
    d_gain1[i]=sqrt(((m[i]*1.0E-3*1.0E-12*93.)/(293*1.6E-19*50))*
		   ((m[i]*1.0E-3*1.0E-12*93.)/(293*1.6E-19*50))*
		   d_pulseheight[i]*d_pulseheight[i] +
		   ((pulseheight[i]*1.0E-3*1.0E-12*93.)/(293*1.6E-19*50))*
		   ((pulseheight[i]*1.0E-3*1.0E-12*93.)/(293*1.6E-19*50))*
		   d_m[i]*d_m[i]);
  }


  //===================================
  //Rechnung 2
  //===================================


  //Korrigierte Rate + Fehler
  double truerate = xrate - bgrate;
  double d_truerate = sqrt((d_xrate*d_xrate) + (d_bgrate*d_bgrate));
  
  //Korrigierter Strom + Fehler
  double truecurrent[n];
  double d_truecurrent[n];
  for (int i=0; i<n; i++){
    truecurrent[i] = current[i] - bgcurrent[i];
    d_truecurrent[i] = sqrt((d_current[i]*d_current[i])
			    +(d_bgcurrent[i]*d_bgcurrent[i]));
  }

  //Berechnung des Gains
  double gain2[n];
  double d_gain2[n];
  for (int i=0; i<n; i++){
    gain2[i]=((truecurrent[i]*1.0E-12)/(281*1.6E-19*truerate));
    d_gain2[i]=sqrt(((1.0E-12)/(281*1.6E-19*truerate))*
		   ((1.0E-12)/(281*1.6E-19*truerate))*
		   d_truecurrent[i]*d_truecurrent[i] +
		   ((truecurrent[i]*1.0E-12)/(281*1.6E-19*truerate*truerate))*
		   ((truecurrent[i]*1.0E-12)/(281*1.6E-19*truerate*truerate))*
		   d_truerate*d_truerate);
  }


  //===================================
  //Graphen plotten und fitten
  //===================================


  TGraphErrors *graph1 = new TGraphErrors(n, voltage, gain1, 
					  d_voltage, d_gain1);
  graph1->SetTitle("");
  graph1->SetMarkerStyle(21);
  graph1->SetMarkerColor(2);
  graph1->SetMarkerSize(0.8);
  graph1->SetLineWidth(2);


  TGraphErrors *graph2 = new TGraphErrors(n, voltage, gain2, d_voltage, d_gain2);
  graph2->SetTitle("");
  graph2->SetMarkerStyle(22);
  graph2->SetMarkerColor(3);
  graph2->SetMarkerSize(2.0);
  graph2->SetLineWidth(2);


  TH2F *hr = new TH2F("hr","hr",100,97,108,100,950,45050);
  hr->SetStats(kFALSE);
  //hr->SetTitle("Gain Measurement");
  hr->SetTitle("");
  hr->SetYTitle("Effective Gain");
  hr->SetXTitle("Detector Voltage [%]");
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
  //graph1->Draw("p");
  graph2->Draw("p");

  TF1 *f1 = new TF1("f1","expo",98,107);
  f1->SetLineWidth(1);  
  //graph1->Fit("f1","R");

  TF1 *f2 = new TF1("f2","expo",98,107);
  f2->SetLineWidth(2);  
  graph2->Fit("f2","R");

  TPave *p = new TPave(100.5, 1300, 107.5, 3200); 
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

  /*
  TPave *p = new TPave(100.5, 1300, 107.5, 3300); 
  p->SetBorderSize(8);
  p->SetLineWidth(2);
  p->SetLineColor(4);
  p->Draw();
  */

  TArrow *a = new TArrow(100.4, 1290, 100.1, 1010);
  a->SetArrowSize(0.02);
  a->SetLineWidth(2);
  a->SetLineColor(4);
  a->Draw();

  /*
  TPaveText *pt = new TPaveText(101.5, 1200, 107.5, 3300);
  pt->SetTextFont(62);
  pt->SetBorderSize(8);
  pt->SetLineWidth(2);
  pt->SetLineColor(4);
  pt->SetTextAlign(11);
  //pt->SetTextSize(0.03);
  pt->AddText("Settings corresponding to 100%:");
  pt->AddText("GEM1: 400V, GEM2: 365V, GEM3: 320V");
  pt->AddText("Transfer-, Collection Field: 3.75kV/cm");
  pt->Draw();
  */

  TText *t4 = new TText(0.15, 0.78, "Drift Field fixed at 250V/cm");
  t4->SetNDC();
  t4->SetTextSize(0.04);
  t4->SetTextFont(102);
  t4->SetTextColor(4);
  t4->Draw();

  /*
  leg = new TLegend(0.85,0.35,0.5,0.2);
  leg->AddEntry(graph2,"Total Current on Pad Plane","p");
  leg->AddEntry(graph1,"Calibrated Pulse Spectra","p");
  leg->SetHeader("Method of Measurement:");
  leg->SetFillColor(10);
  leg->Draw();
  */

  //gain_canvas->ForceUpdate();
  //gain_canvas->PaintModified();
  //TPaveText* p = ((TPaveText*)(gain_canvas->FindObject("title")));
  //p->SetFillColor(10);
  //gain_canvas->Modified();

  gain_canvas->Modified();
  gain_canvas->Update();

}
