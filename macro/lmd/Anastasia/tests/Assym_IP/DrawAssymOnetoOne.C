void DrawAssymOnetoOne(TString path = "/panda/myResults/AssymIP/DPM/mom_1_5/", TString file_name="/HitsAssym_x_y_1_5GeV.root", int x_sh = -5, int y_sh = -5,int angx_sh = -5, int angy_sh = -5){
  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/tests/Assym_IP/rootlogon.C");
 const int Nmod = 20;
  //read Ideal parameters ---------------------------------------------------
  TString file_ideal = path;
  file_ideal +="x_0mm_y_0mm_Xtlit_0mrad_Ytlit_0mrad";
  file_ideal +=file_name;
  TFile *finIDEAL = new TFile(file_ideal,"READ");
//N
  TGraphErrors *modN_ideal = (TGraphErrors*)finIDEAL->Get("modN");
  modN_ideal->SetMarkerSize(1.0);
  Double_t *N_ideal = new Double_t[Nmod]; Double_t *errN_ideal = new Double_t[Nmod];
  N_ideal = modN_ideal->GetY();
  errN_ideal = modN_ideal->GetEY();
  // const int Nmod = (modN_ideal->GetN());


  //X0
  TGraphErrors *modX0_ideal = (TGraphErrors*)finIDEAL->Get("modX0");
  modX0_ideal->SetMarkerSize(1.0);
  double *modulesID = modX0_ideal->GetX();
  Double_t *X0_ideal; Double_t *errX0_ideal;
  X0_ideal = modX0_ideal->GetY();
  errX0_ideal = modX0_ideal->GetEY();
//Y0
TGraphErrors *modY0_ideal = (TGraphErrors*)finIDEAL->Get("modY0");
 modY0_ideal->SetMarkerSize(1.0);
 Double_t *Y0_ideal; Double_t *errY0_ideal;
  Y0_ideal = modY0_ideal->GetY();
  errY0_ideal = modY0_ideal->GetEY();
  //Sl X
 TGraphErrors *modSLX_ideal = (TGraphErrors*)finIDEAL->Get("modSLX");
 modSLX_ideal->SetMarkerSize(1.0);
  Double_t *SLX_ideal; Double_t *errSLX_ideal;
  SLX_ideal = modSLX_ideal->GetY();
  errSLX_ideal = modSLX_ideal->GetEY();
 //Sl Y
 TGraphErrors *modSLY_ideal = (TGraphErrors*)finIDEAL->Get("modSLY");
 modSLY_ideal->SetMarkerSize(1.0);
  Double_t *SLY_ideal; Double_t *errSLY_ideal;
  SLY_ideal = modSLY_ideal->GetY();
  errSLY_ideal = modSLY_ideal->GetEY();

  //[END] read Ideal parameters ------------------------------------------

  // read Shifted parameters -----------------------------------------------
  TString this_name = "x_";
  this_name +=x_sh;
  this_name +="mm_y_";
  this_name +=y_sh;
  this_name +="mm_Xtlit_";
  this_name +=angx_sh;
  this_name +="mrad_Ytlit_";
  this_name +=angy_sh;
  this_name +="mrad";


TString this_name2 = "x_";
 if(x_sh<0)  this_name2 +="mn";
 else this_name2 +="pl";
 this_name2 +=fabs(x_sh);
  this_name2 +="mm_y_";
if(y_sh<0)  this_name2 +="mn";
 else this_name2 +="pl";
 this_name2 +=fabs(y_sh);
 this_name2 +="mm_Xtlit_";
 if(angx_sh<0) this_name2 +="mn";
 else this_name2 +="pl";
 this_name2 +=fabs(angx_sh);
  this_name2 +="mrad_Ytlit_";
  if(angy_sh<0) this_name2 +="mn";
  else this_name2 +="pl";
  this_name2 +=fabs(angy_sh);
  this_name2 +="mrad";

  TString file_shifted = path;
  file_shifted +=this_name;
  file_shifted +=file_name;
  TFile *fin_SHIFT = new TFile(file_shifted,"READ");

  TGraphErrors *modN_SHIFT = (TGraphErrors*)fin_SHIFT->Get("modN");
  modN_SHIFT->SetMarkerColor(4);
  modN_SHIFT->SetMarkerSize(1.0);
  Double_t *N_SHIFT = new Double_t[Nmod]; Double_t *errN_SHIFT = new Double_t[Nmod];
  N_SHIFT = modN_SHIFT->GetY();
  errN_SHIFT = modN_SHIFT->GetEY();
  double diffN_SHIFT[Nmod];   double errdiffN_SHIFT[Nmod];
  double N_ideal_norm = N_ideal[0];
  double N_shifted_norm = N_SHIFT[0];
  cout<<"N_ideal_norm = "<<N_ideal_norm<<" N_shifted_norm = "<<N_shifted_norm<<endl;
  for(int i=0;i<Nmod;i++){
    N_ideal[i] *= 1./N_ideal_norm;
    N_SHIFT[i] *= 1./N_shifted_norm;
    diffN_SHIFT[i] = 100.*(N_SHIFT[i] - N_ideal[i])/N_ideal[i];
    //   errdiffN_SHIFT[i] = 0.5*sqrt(errN_ideal[i]*errN_ideal[i]+errN_SHIFT[i]*errN_SHIFT[i]);
    errdiffN_SHIFT[i] = (1/N_ideal[i])*sqrt(TMath::Power(N_SHIFT[i]*errN_ideal[i]/N_ideal[i],2)+TMath::Power(errN_SHIFT[i],2));
    //    cout<<"N_ideal["<<i<<"]="<<N_ideal[i]<<endl;
    cout<<"N_SHIFT["<<i<<"]="<<N_SHIFT[i]<<endl;
  }
  TGraphErrors *gr_diffN_SHIFT = new TGraphErrors(Nmod,modulesID,diffN_SHIFT,0,errdiffN_SHIFT);
  gr_diffN_SHIFT->SetMarkerStyle(20);
  gr_diffN_SHIFT->SetMarkerColor(4);
  gr_diffN_SHIFT->SetLineColor(4);
  gr_diffN_SHIFT->SetLineWidth(1.5);
  gr_diffN_SHIFT->SetMarkerSize(1.0);

  TGraphErrors *modX0_SHIFT = (TGraphErrors*)fin_SHIFT->Get("modX0");
  modX0_SHIFT->SetMarkerColor(4);
  modX0_SHIFT->SetMarkerSize(1.0);
  Double_t *X0_SHIFT; Double_t *errX0_SHIFT;
  X0_SHIFT = modX0_SHIFT->GetY();
  errX0_SHIFT = modX0_SHIFT->GetEY();
  double diffX0_SHIFT[Nmod];   double errdiffX0_SHIFT[Nmod];
  for(int i=0;i<modX0_ideal->GetN();i++){
    diffX0_SHIFT[i] = X0_SHIFT[i] - X0_ideal[i];
    errdiffX0_SHIFT[i] = 0.5*sqrt(errX0_ideal[i]*errX0_ideal[i]+errX0_SHIFT[i]*errX0_SHIFT[i]);
  }
  TGraphErrors *gr_diffX0_SHIFT = new TGraphErrors(Nmod,modulesID,diffX0_SHIFT,0,errdiffX0_SHIFT);
  gr_diffX0_SHIFT->SetMarkerStyle(20);
  gr_diffX0_SHIFT->SetMarkerColor(4);
  gr_diffX0_SHIFT->SetLineColor(4);
  gr_diffX0_SHIFT->SetLineWidth(1.5);
  gr_diffX0_SHIFT->SetMarkerSize(1.0);
  TGraphErrors *modY0_SHIFT = (TGraphErrors*)fin_SHIFT->Get("modY0");
  modY0_SHIFT->SetMarkerColor(4);
  modY0_SHIFT->SetMarkerSize(1.0);
  Double_t *Y0_SHIFT; Double_t *errY0_SHIFT;
  Y0_SHIFT = modY0_SHIFT->GetY();
  errY0_SHIFT = modY0_SHIFT->GetEY();
  double diffY0_SHIFT[20];   double errdiffY0_SHIFT[20];
  for(int i=0;i<modY0_ideal->GetN();i++){
    diffY0_SHIFT[i] = Y0_SHIFT[i] - Y0_ideal[i];
    errdiffY0_SHIFT[i] = 0.5*sqrt(errY0_ideal[i]*errY0_ideal[i]+errY0_SHIFT[i]*errY0_SHIFT[i]);
  }
  TGraphErrors *gr_diffY0_SHIFT = new TGraphErrors(Nmod,modulesID,diffY0_SHIFT,0,errdiffY0_SHIFT);
  gr_diffY0_SHIFT->SetMarkerStyle(20);
  gr_diffY0_SHIFT->SetMarkerColor(4);
  gr_diffY0_SHIFT->SetLineColor(4);
  gr_diffY0_SHIFT->SetLineWidth(1.5);
  gr_diffY0_SHIFT->SetMarkerSize(1.0);
  TGraphErrors *modSLX_SHIFT = (TGraphErrors*)fin_SHIFT->Get("modSLX");
  modSLX_SHIFT->SetMarkerColor(4);
  modSLX_SHIFT->SetMarkerSize(1.0);
  Double_t *SLX_SHIFT; Double_t *errSLX_SHIFT;
  SLX_SHIFT = modSLX_SHIFT->GetY();
  errSLX_SHIFT = modSLX_SHIFT->GetEY();

  double diffSLX_SHIFT[Nmod];   double errdiffSLX_SHIFT[Nmod];
  for(int i=0;i<Nmod;i++){
    diffSLX_SHIFT[i] = SLX_SHIFT[i] - SLX_ideal[i];
    errdiffSLX_SHIFT[i] = 0.5*sqrt(errSLX_ideal[i]*errSLX_ideal[i]+errSLX_SHIFT[i]*errSLX_SHIFT[i]);
  }
  TGraphErrors *gr_diffSLX_SHIFT = new TGraphErrors(Nmod,modulesID,diffSLX_SHIFT,0,errdiffSLX_SHIFT);
  gr_diffSLX_SHIFT->SetMarkerStyle(20);
  gr_diffSLX_SHIFT->SetMarkerColor(4);
  gr_diffSLX_SHIFT->SetLineColor(4);
  gr_diffSLX_SHIFT->SetLineWidth(1.5);
  gr_diffSLX_SHIFT->SetMarkerSize(1.0);
  TGraphErrors *modSLY_SHIFT = (TGraphErrors*)fin_SHIFT->Get("modSLY");
  modSLY_SHIFT->SetMarkerColor(4);
  modSLY_SHIFT->SetMarkerSize(1.0);
  Double_t *SLY_SHIFT; Double_t *errSLY_SHIFT;
  SLY_SHIFT = modSLY_SHIFT->GetY();
  errSLY_SHIFT = modSLY_SHIFT->GetEY();
  double diffSLY_SHIFT[Nmod];   double errdiffSLY_SHIFT[Nmod];
  for(int i=0;i<modSLY_ideal->GetN();i++){
    diffSLY_SHIFT[i] = SLY_SHIFT[i] - SLY_ideal[i];
    errdiffSLY_SHIFT[i] = 0.5*sqrt(errSLY_ideal[i]*errSLY_ideal[i]+errSLY_SHIFT[i]*errSLY_SHIFT[i]);
  }
  TGraphErrors *gr_diffSLY_SHIFT = new TGraphErrors(Nmod,modulesID,diffSLY_SHIFT,0,errdiffSLY_SHIFT);
  gr_diffSLY_SHIFT->SetMarkerStyle(20);
  gr_diffSLY_SHIFT->SetMarkerColor(4);
  gr_diffSLY_SHIFT->SetLineColor(4);
  gr_diffSLY_SHIFT->SetLineWidth(1.5);
  gr_diffSLY_SHIFT->SetMarkerSize(1.0);
  //[END] read Shifted parameters ---------------------------------------
  

  TLegend *leg = new TLegend(0.34,0.65,0.7,0.98);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.055);
  leg->SetHeader("(X,Y)_{POS} [mm]; (X,Y)_{TILT} [mrad]");
 TLegend *leg2 = new TLegend(0.64,0.75,0.98,0.98);
  leg2->SetFillColor(0);
  leg2->SetTextFont(42);
  leg2->SetTextSize(0.055);
  leg2->SetHeader("(X,Y)_{POS} [mm]; (X,Y)_{TILT} [mrad]");
  TCanvas *c1 = new TCanvas("Assym", "Assym", 700, 900);
  c1->Divide(2,5);
  c1->cd(1);
  TMultiGraph *mgr_X0 = new TMultiGraph();
  mgr_X0->Add(modX0_ideal);
  mgr_X0->Add(modX0_SHIFT);
  TString text_sh = "(";
  text_sh +=x_sh;
  text_sh +=",";
  text_sh +=y_sh;
  text_sh += "); (";
  text_sh += angx_sh;
  text_sh +=",";
  text_sh += angy_sh;
  text_sh += ")";
  leg->AddEntry(modX0_ideal,"(0,0); (0,0)","p");	  
  leg->AddEntry(modX0_SHIFT,text_sh,"p");	  
  leg2->AddEntry(gr_diffX0_SHIFT,text_sh,"p");
  mgr_X0->Draw("AP");
  leg->Draw();
  mgr_X0->GetXaxis()->SetTitle("sector");
  mgr_X0->GetYaxis()->SetTitle("X0, cm");
  c1->cd(2);
  gr_diffX0_SHIFT->SetTitle("");
  TString diffX0_name = "diffX0_";
  diffX0_name +=this_name2;
  gr_diffX0_SHIFT->SetName(diffX0_name);
  gr_diffX0_SHIFT->GetXaxis()->SetTitle("sector");
  gr_diffX0_SHIFT->GetYaxis()->SetTitle("#Delta X0, cm");
  gr_diffX0_SHIFT->GetYaxis()->SetRangeUser(-1.,1.);
  gr_diffX0_SHIFT->Draw("APC");
  c1->cd(3);
  TMultiGraph *mgr_Y0 = new TMultiGraph();
  mgr_Y0->Add(modY0_ideal);
  mgr_Y0->Add(modY0_SHIFT);
  mgr_Y0->Draw("AP");
  mgr_Y0->GetXaxis()->SetTitle("sector");
  mgr_Y0->GetYaxis()->SetTitle("Y0, cm");
  c1->cd(4);
  TString diffY0_name = "diffY0_";
  diffY0_name +=this_name2;
  gr_diffY0_SHIFT->SetName(diffY0_name);
  gr_diffY0_SHIFT->GetXaxis()->SetTitle("sector");
  gr_diffY0_SHIFT->GetYaxis()->SetTitle("#Delta Y0, cm");
  gr_diffY0_SHIFT->GetYaxis()->SetRangeUser(-1.,1.);
  gr_diffY0_SHIFT->Draw("APC");
 c1->cd(5);
  TMultiGraph *mgr_SLX = new TMultiGraph();
  mgr_SLX->Add(modSLX_ideal);
  mgr_SLX->Add(modSLX_SHIFT);
  mgr_SLX->Draw("AP");
  mgr_SLX->GetXaxis()->SetTitle("sector");
  mgr_SLX->GetYaxis()->SetTitle("slope X");
  c1->cd(6);
  TString diffSLX_name = "diffSLX_";
  diffSLX_name +=this_name2;
  gr_diffSLX_SHIFT->SetName(diffSLX_name);
  gr_diffSLX_SHIFT->GetXaxis()->SetTitle("sector");
  gr_diffSLX_SHIFT->GetYaxis()->SetTitle("#Delta slope X, cm");
  gr_diffSLX_SHIFT->GetYaxis()->SetRangeUser(-0.1.,0.1);
  gr_diffSLX_SHIFT->Draw("APC");
 c1->cd(7);
  TMultiGraph *mgr_SLY = new TMultiGraph();
  mgr_SLY->Add(modSLY_ideal);
  mgr_SLY->Add(modSLY_SHIFT);
  mgr_SLY->Draw("AP");
  mgr_SLY->GetXaxis()->SetTitle("sector");
  mgr_SLY->GetYaxis()->SetTitle("slope Y");
  c1->cd(8);
  TString diffSLY_name = "diffSLY_";
  diffSLY_name +=this_name2;
  gr_diffSLY_SHIFT->SetName(diffSLY_name);
  gr_diffSLY_SHIFT->GetXaxis()->SetTitle("sector");
  gr_diffSLY_SHIFT->GetYaxis()->SetTitle("#Delta slope Y, cm");
  gr_diffSLY_SHIFT->GetYaxis()->SetRangeUser(-0.1.,0.1);
  gr_diffSLY_SHIFT->Draw("APC");
 c1->cd(9);
 TMultiGraph *mgr_N = new TMultiGraph();
 modN_ideal->SetMarkerStyle(20);
 modN_ideal->SetMarkerColor(2);
 modN_SHIFT->SetMarkerStyle(20);
 mgr_N->Add(modN_ideal);
 mgr_N->Add(modN_SHIFT);
  mgr_N->Draw("AP");
  mgr_N->GetXaxis()->SetTitle("sector");
  mgr_N->GetYaxis()->SetTitle("N/N_{0}");
  mgr_N->GetXaxis()->SetRangeUser(0,19);
  if(angx_sh<0) mgr_N->GetYaxis()->SetRangeUser(-1.,8.5);
  else mgr_N->GetYaxis()->SetRangeUser(-1.,4.5);
  leg2->Draw();
  c1->cd(10);
  TString diffN_name = "diffN_";
  diffN_name +=this_name2;
  gr_diffN_SHIFT->SetName(diffN_name);
  gr_diffN_SHIFT->SetTitle("");
  gr_diffN_SHIFT->GetXaxis()->SetTitle("sector");
  gr_diffN_SHIFT->GetYaxis()->SetTitle("#Delta (N/N_{0}), %");
  gr_diffN_SHIFT->Draw("APC");
  leg2->Draw();
 TString fout_name = path;
 fout_name +="/AssymRes_";
 fout_name += this_name;
 TString fout_name_pdf = fout_name;
 fout_name_pdf  +=".pdf";
TString fout_name_root = fout_name;
 fout_name_root  +=".root";
 c1->SaveAs(fout_name_pdf);
 TFile *finOUT = new TFile(fout_name_root,"RECREATE");
 // mgr_X0->Write();
 modX0_SHIFT->Write();
 gr_diffX0_SHIFT->Write();
 modY0_SHIFT->Write();
 gr_diffY0_SHIFT->Write();
 modSLX_SHIFT->Write();
 gr_diffSLX_SHIFT->Write();
 modSLY_SHIFT->Write();
 gr_diffSLY_SHIFT->Write();
 modN_SHIFT->Write();
 gr_diffN_SHIFT->Write();
 leg->Write();
 finOUT->Write();
}
