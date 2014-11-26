void DrawAssym(){
  TFile *finIDEAL = new TFile("/panda/myResults/AssymIP/DPM/mom_1_5/x_0mm_y_0mm/HitsAssym_x_y_1_5GeV.root","READ");
  //N
  TGraphErrors *modN_ideal = (TGraphErrors*)finIDEAL->Get("modN");
  modN_ideal->SetMarkerSize(1.0);
  Double_t *N_ideal; Double_t *errN_ideal;
  N_ideal = modN_ideal->GetY();
  errN_ideal = modN_ideal->GetEY();

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

  //x = 5, y = 0
  TFile *fin_xpl5mm_y0mm = new TFile("/panda/myResults/AssymIP/DPM/mom_1_5/x_5mm_y_0mm/HitsAssym_x_y_1_5GeV.root","READ");
TGraphErrors *modN_xpl5mm_y0mm = (TGraphErrors*)fin_xpl5mm_y0mm->Get("modN");
  modN_xpl5mm_y0mm->SetMarkerColor(4);
  modN_xpl5mm_y0mm->SetMarkerSize(1.0);
  Double_t *N_xpl5mm_y0mm; Double_t *errN_xpl5mm_y0mm;
  N_xpl5mm_y0mm = modN_xpl5mm_y0mm->GetY();
  errN_xpl5mm_y0mm = modN_xpl5mm_y0mm->GetEY();
  double diffN_xpl5mm_y0mm[20];   double errdiffN_xpl5mm_y0mm[20];
  for(int i=0;i<modN_ideal->GetN();i++){
    diffN_xpl5mm_y0mm[i] = 100.*(N_xpl5mm_y0mm[i] - N_ideal[i])/N_ideal[i];
    //   errdiffN_xpl5mm_y0mm[i] = 0.5*sqrt(errN_ideal[i]*errN_ideal[i]+errN_xpl5mm_y0mm[i]*errN_xpl5mm_y0mm[i]);
    errdiffN_xpl5mm_y0mm[i] = (1/N_ideal[i])*sqrt(TMath::Power(N_xpl5mm_y0mm[i]*errN_ideal[i]/N_ideal[i],2)+TMath::Power(errN_xpl5mm_y0mm[i],2));
  }
  TGraphErrors *gr_diffN_xpl5mm_y0mm = new TGraphErrors(10,modulesID,diffN_xpl5mm_y0mm,0,errdiffN_xpl5mm_y0mm);
  gr_diffN_xpl5mm_y0mm->SetMarkerStyle(20);
  gr_diffN_xpl5mm_y0mm->SetMarkerColor(4);
  gr_diffN_xpl5mm_y0mm->SetLineColor(4);
  gr_diffN_xpl5mm_y0mm->SetLineWidth(1.5);
  gr_diffN_xpl5mm_y0mm->SetMarkerSize(1.0);


  TGraphErrors *modX0_xpl5mm_y0mm = (TGraphErrors*)fin_xpl5mm_y0mm->Get("modX0");
  modX0_xpl5mm_y0mm->SetMarkerColor(4);
  modX0_xpl5mm_y0mm->SetMarkerSize(1.0);
  Double_t *X0_xpl5mm_y0mm; Double_t *errX0_xpl5mm_y0mm;
  X0_xpl5mm_y0mm = modX0_xpl5mm_y0mm->GetY();
  errX0_xpl5mm_y0mm = modX0_xpl5mm_y0mm->GetEY();
  double diffX0_xpl5mm_y0mm[20];   double errdiffX0_xpl5mm_y0mm[20];
  for(int i=0;i<modX0_ideal->GetN();i++){
    diffX0_xpl5mm_y0mm[i] = X0_xpl5mm_y0mm[i] - X0_ideal[i];
    errdiffX0_xpl5mm_y0mm[i] = 0.5*sqrt(errX0_ideal[i]*errX0_ideal[i]+errX0_xpl5mm_y0mm[i]*errX0_xpl5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffX0_xpl5mm_y0mm = new TGraphErrors(10,modulesID,diffX0_xpl5mm_y0mm,0,errdiffX0_xpl5mm_y0mm);
  gr_diffX0_xpl5mm_y0mm->SetMarkerStyle(20);
  gr_diffX0_xpl5mm_y0mm->SetMarkerColor(4);
  gr_diffX0_xpl5mm_y0mm->SetLineColor(4);
  gr_diffX0_xpl5mm_y0mm->SetLineWidth(1.5);
  gr_diffX0_xpl5mm_y0mm->SetMarkerSize(1.0);
  TGraphErrors *modY0_xpl5mm_y0mm = (TGraphErrors*)fin_xpl5mm_y0mm->Get("modY0");
  modY0_xpl5mm_y0mm->SetMarkerColor(4);
  modY0_xpl5mm_y0mm->SetMarkerSize(1.0);
  Double_t *Y0_xpl5mm_y0mm; Double_t *errY0_xpl5mm_y0mm;
  Y0_xpl5mm_y0mm = modY0_xpl5mm_y0mm->GetY();
  errY0_xpl5mm_y0mm = modY0_xpl5mm_y0mm->GetEY();
  double diffY0_xpl5mm_y0mm[20];   double errdiffY0_xpl5mm_y0mm[20];
  for(int i=0;i<modY0_ideal->GetN();i++){
    diffY0_xpl5mm_y0mm[i] = Y0_xpl5mm_y0mm[i] - Y0_ideal[i];
    errdiffY0_xpl5mm_y0mm[i] = 0.5*sqrt(errY0_ideal[i]*errY0_ideal[i]+errY0_xpl5mm_y0mm[i]*errY0_xpl5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffY0_xpl5mm_y0mm = new TGraphErrors(10,modulesID,diffY0_xpl5mm_y0mm,0,errdiffY0_xpl5mm_y0mm);
  gr_diffY0_xpl5mm_y0mm->SetMarkerStyle(20);
  gr_diffY0_xpl5mm_y0mm->SetMarkerColor(4);
  gr_diffY0_xpl5mm_y0mm->SetLineColor(4);
  gr_diffY0_xpl5mm_y0mm->SetLineWidth(1.5);
  gr_diffY0_xpl5mm_y0mm->SetMarkerSize(1.0);
  TGraphErrors *modSLX_xpl5mm_y0mm = (TGraphErrors*)fin_xpl5mm_y0mm->Get("modSLX");
  modSLX_xpl5mm_y0mm->SetMarkerColor(4);
  modSLX_xpl5mm_y0mm->SetMarkerSize(1.0);
  Double_t *SLX_xpl5mm_y0mm; Double_t *errSLX_xpl5mm_y0mm;
  SLX_xpl5mm_y0mm = modSLX_xpl5mm_y0mm->GetY();
  errSLX_xpl5mm_y0mm = modSLX_xpl5mm_y0mm->GetEY();

  double diffSLX_xpl5mm_y0mm[20];   double errdiffSLX_xpl5mm_y0mm[20];
  for(int i=0;i<modSLX_ideal->GetN();i++){
    diffSLX_xpl5mm_y0mm[i] = SLX_xpl5mm_y0mm[i] - SLX_ideal[i];
    errdiffSLX_xpl5mm_y0mm[i] = 0.5*sqrt(errSLX_ideal[i]*errSLX_ideal[i]+errSLX_xpl5mm_y0mm[i]*errSLX_xpl5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffSLX_xpl5mm_y0mm = new TGraphErrors(10,modulesID,diffSLX_xpl5mm_y0mm,0,errdiffSLX_xpl5mm_y0mm);
  gr_diffSLX_xpl5mm_y0mm->SetMarkerStyle(20);
  gr_diffSLX_xpl5mm_y0mm->SetMarkerColor(4);
  gr_diffSLX_xpl5mm_y0mm->SetLineColor(4);
  gr_diffSLX_xpl5mm_y0mm->SetLineWidth(1.5);
  gr_diffSLX_xpl5mm_y0mm->SetMarkerSize(1.0);
  TGraphErrors *modSLY_xpl5mm_y0mm = (TGraphErrors*)fin_xpl5mm_y0mm->Get("modSLY");
  modSLY_xpl5mm_y0mm->SetMarkerColor(4);
  modSLY_xpl5mm_y0mm->SetMarkerSize(1.0);
  Double_t *SLY_xpl5mm_y0mm; Double_t *errSLY_xpl5mm_y0mm;
  SLY_xpl5mm_y0mm = modSLY_xpl5mm_y0mm->GetY();
  errSLY_xpl5mm_y0mm = modSLY_xpl5mm_y0mm->GetEY();
  double diffSLY_xpl5mm_y0mm[20];   double errdiffSLY_xpl5mm_y0mm[20];
  for(int i=0;i<modSLY_ideal->GetN();i++){
    diffSLY_xpl5mm_y0mm[i] = SLY_xpl5mm_y0mm[i] - SLY_ideal[i];
    errdiffSLY_xpl5mm_y0mm[i] = 0.5*sqrt(errSLY_ideal[i]*errSLY_ideal[i]+errSLY_xpl5mm_y0mm[i]*errSLY_xpl5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffSLY_xpl5mm_y0mm = new TGraphErrors(10,modulesID,diffSLY_xpl5mm_y0mm,0,errdiffSLY_xpl5mm_y0mm);
  gr_diffSLY_xpl5mm_y0mm->SetMarkerStyle(20);
  gr_diffSLY_xpl5mm_y0mm->SetMarkerColor(4);
  gr_diffSLY_xpl5mm_y0mm->SetLineColor(4);
  gr_diffSLY_xpl5mm_y0mm->SetLineWidth(1.5);
  gr_diffSLY_xpl5mm_y0mm->SetMarkerSize(1.0);


  //x=5, y=5
TFile *fin_xpl5mm_y5mm = new TFile("/panda/myResults/AssymIP/DPM/mom_1_5/x_0mm_y_5mm/HitsAssym_x_y_1_5GeV.root","READ");
TGraphErrors *modN_xpl5mm_y5mm = (TGraphErrors*)fin_xpl5mm_y5mm->Get("modN");
  modN_xpl5mm_y5mm->SetMarkerColor(28);
  modN_xpl5mm_y5mm->SetMarkerSize(1.0);
  Double_t *N_xpl5mm_y5mm; Double_t *errN_xpl5mm_y5mm;
  N_xpl5mm_y5mm = modN_xpl5mm_y5mm->GetY();
  errN_xpl5mm_y5mm = modN_xpl5mm_y5mm->GetEY();
  double diffN_xpl5mm_y5mm[20];   double errdiffN_xpl5mm_y5mm[20];
  for(int i=0;i<modN_ideal->GetN();i++){
    // diffN_xpl5mm_y5mm[i] = N_xpl5mm_y5mm[i] - N_ideal[i];
    // errdiffN_xpl5mm_y5mm[i] = 0.5*sqrt(errN_ideal[i]*errN_ideal[i]+errN_xpl5mm_y5mm[i]*errN_xpl5mm_y5mm[i]);
    diffN_xpl5mm_y5mm[i] = 100.*(N_xpl5mm_y5mm[i] - N_ideal[i])/N_ideal[i];
    errdiffN_xpl5mm_y5mm[i] = (1/N_ideal[i])*sqrt(TMath::Power(N_xpl5mm_y5mm[i]*errN_ideal[i]/N_ideal[i],2)+TMath::Power(errN_xpl5mm_y5mm[i],2));
  }
  TGraphErrors *gr_diffN_xpl5mm_y5mm = new TGraphErrors(10,modulesID,diffN_xpl5mm_y5mm,0,errdiffN_xpl5mm_y5mm);
  gr_diffN_xpl5mm_y5mm->SetMarkerStyle(20);
  gr_diffN_xpl5mm_y5mm->SetMarkerColor(28);
  gr_diffN_xpl5mm_y5mm->SetLineColor(28);
  gr_diffN_xpl5mm_y5mm->SetLineWidth(1.5);
  gr_diffN_xpl5mm_y5mm->SetMarkerSize(1.0);

  TGraphErrors *modX0_xpl5mm_y5mm = (TGraphErrors*)fin_xpl5mm_y5mm->Get("modX0");
  modX0_xpl5mm_y5mm->SetMarkerColor(28);
  modX0_xpl5mm_y5mm->SetMarkerSize(1.0);
  Double_t *X0_xpl5mm_y5mm; Double_t *errX0_xpl5mm_y5mm;
  X0_xpl5mm_y5mm = modX0_xpl5mm_y5mm->GetY();
  errX0_xpl5mm_y5mm = modX0_xpl5mm_y5mm->GetEY();
  double diffX0_xpl5mm_y5mm[20];   double errdiffX0_xpl5mm_y5mm[20];
  for(int i=0;i<modX0_ideal->GetN();i++){
    diffX0_xpl5mm_y5mm[i] = X0_xpl5mm_y5mm[i] - X0_ideal[i];
    errdiffX0_xpl5mm_y5mm[i] = 0.5*sqrt(errX0_ideal[i]*errX0_ideal[i]+errX0_xpl5mm_y5mm[i]*errX0_xpl5mm_y5mm[i]);
  }
  TGraphErrors *gr_diffX0_xpl5mm_y5mm = new TGraphErrors(10,modulesID,diffX0_xpl5mm_y5mm,0,errdiffX0_xpl5mm_y5mm);
  gr_diffX0_xpl5mm_y5mm->SetMarkerStyle(20);
  gr_diffX0_xpl5mm_y5mm->SetMarkerColor(28);
  gr_diffX0_xpl5mm_y5mm->SetLineColor(28);
  gr_diffX0_xpl5mm_y5mm->SetLineWidth(1.5);
  gr_diffX0_xpl5mm_y5mm->SetMarkerSize(1.0);
  TGraphErrors *modY0_xpl5mm_y5mm = (TGraphErrors*)fin_xpl5mm_y5mm->Get("modY0");
  modY0_xpl5mm_y5mm->SetMarkerColor(28);
  modY0_xpl5mm_y5mm->SetMarkerSize(1.0);
  Double_t *Y0_xpl5mm_y5mm; Double_t *errY0_xpl5mm_y5mm;
  Y0_xpl5mm_y5mm = modY0_xpl5mm_y5mm->GetY();
  errY0_xpl5mm_y5mm = modY0_xpl5mm_y5mm->GetEY();
  double diffY0_xpl5mm_y5mm[20];   double errdiffY0_xpl5mm_y5mm[20];
  for(int i=0;i<modY0_ideal->GetN();i++){
    diffY0_xpl5mm_y5mm[i] = Y0_xpl5mm_y5mm[i] - Y0_ideal[i];
    errdiffY0_xpl5mm_y5mm[i] = 0.5*sqrt(errY0_ideal[i]*errY0_ideal[i]+errY0_xpl5mm_y5mm[i]*errY0_xpl5mm_y5mm[i]);
  }
  TGraphErrors *gr_diffY0_xpl5mm_y5mm = new TGraphErrors(10,modulesID,diffY0_xpl5mm_y5mm,0,errdiffY0_xpl5mm_y5mm);
  gr_diffY0_xpl5mm_y5mm->SetMarkerStyle(20);
  gr_diffY0_xpl5mm_y5mm->SetMarkerColor(28);
  gr_diffY0_xpl5mm_y5mm->SetLineColor(28);
  gr_diffY0_xpl5mm_y5mm->SetLineWidth(1.5);
  gr_diffY0_xpl5mm_y5mm->SetMarkerSize(1.0);
  TGraphErrors *modSLX_xpl5mm_y5mm = (TGraphErrors*)fin_xpl5mm_y5mm->Get("modSLX");
  modSLX_xpl5mm_y5mm->SetMarkerColor(28);
  modSLX_xpl5mm_y5mm->SetMarkerSize(1.0);
  Double_t *SLX_xpl5mm_y5mm; Double_t *errSLX_xpl5mm_y5mm;
  SLX_xpl5mm_y5mm = modSLX_xpl5mm_y5mm->GetY();
  errSLX_xpl5mm_y5mm = modSLX_xpl5mm_y5mm->GetEY();

  double diffSLX_xpl5mm_y5mm[20];   double errdiffSLX_xpl5mm_y5mm[20];
  for(int i=0;i<modSLX_ideal->GetN();i++){
    diffSLX_xpl5mm_y5mm[i] = SLX_xpl5mm_y5mm[i] - SLX_ideal[i];
    errdiffSLX_xpl5mm_y5mm[i] = 0.5*sqrt(errSLX_ideal[i]*errSLX_ideal[i]+errSLX_xpl5mm_y5mm[i]*errSLX_xpl5mm_y5mm[i]);
  }
  TGraphErrors *gr_diffSLX_xpl5mm_y5mm = new TGraphErrors(10,modulesID,diffSLX_xpl5mm_y5mm,0,errdiffSLX_xpl5mm_y5mm);
  gr_diffSLX_xpl5mm_y5mm->SetMarkerStyle(20);
  gr_diffSLX_xpl5mm_y5mm->SetMarkerColor(28);
  gr_diffSLX_xpl5mm_y5mm->SetLineColor(28);
  gr_diffSLX_xpl5mm_y5mm->SetLineWidth(1.5);
  gr_diffSLX_xpl5mm_y5mm->SetMarkerSize(1.0);
  TGraphErrors *modSLY_xpl5mm_y5mm = (TGraphErrors*)fin_xpl5mm_y5mm->Get("modSLY");
  modSLY_xpl5mm_y5mm->SetMarkerColor(28);
  modSLY_xpl5mm_y5mm->SetMarkerSize(1.0);
  Double_t *SLY_xpl5mm_y5mm; Double_t *errSLY_xpl5mm_y5mm;
  SLY_xpl5mm_y5mm = modSLY_xpl5mm_y5mm->GetY();
  errSLY_xpl5mm_y5mm = modSLY_xpl5mm_y5mm->GetEY();
  double diffSLY_xpl5mm_y5mm[20];   double errdiffSLY_xpl5mm_y5mm[20];
  for(int i=0;i<modSLY_ideal->GetN();i++){
    diffSLY_xpl5mm_y5mm[i] = SLY_xpl5mm_y5mm[i] - SLY_ideal[i];
    errdiffSLY_xpl5mm_y5mm[i] = 0.5*sqrt(errSLY_ideal[i]*errSLY_ideal[i]+errSLY_xpl5mm_y5mm[i]*errSLY_xpl5mm_y5mm[i]);
  }
  TGraphErrors *gr_diffSLY_xpl5mm_y5mm = new TGraphErrors(10,modulesID,diffSLY_xpl5mm_y5mm,0,errdiffSLY_xpl5mm_y5mm);
  gr_diffSLY_xpl5mm_y5mm->SetMarkerStyle(20);
  gr_diffSLY_xpl5mm_y5mm->SetMarkerColor(28);
  gr_diffSLY_xpl5mm_y5mm->SetLineColor(28);
  gr_diffSLY_xpl5mm_y5mm->SetLineWidth(1.5);
  gr_diffSLY_xpl5mm_y5mm->SetMarkerSize(1.0);

  //x=0,y=5
TFile *fin_x0mm_y5mm = new TFile("/panda/myResults/AssymIP/DPM/mom_1_5/x_5mm_y_5mm/HitsAssym_x_y_1_5GeV.root","READ");
TGraphErrors *modN_x0mm_y5mm = (TGraphErrors*)fin_x0mm_y5mm->Get("modN");
  modN_x0mm_y5mm->SetMarkerColor(6);
  modN_x0mm_y5mm->SetMarkerSize(1.0);
  Double_t *N_x0mm_y5mm; Double_t *errN_x0mm_y5mm;
  N_x0mm_y5mm = modN_x0mm_y5mm->GetY();
  errN_x0mm_y5mm = modN_x0mm_y5mm->GetEY();
  double diffN_x0mm_y5mm[20];   double errdiffN_x0mm_y5mm[20];
  for(int i=0;i<modN_ideal->GetN();i++){
    // diffN_x0mm_y5mm[i] = N_x0mm_y5mm[i] - N_ideal[i];
    // errdiffN_x0mm_y5mm[i] = 0.5*sqrt(errN_ideal[i]*errN_ideal[i]+errN_x0mm_y5mm[i]*errN_x0mm_y5mm[i]);
    diffN_x0mm_y5mm[i] = 100.*(N_x0mm_y5mm[i] - N_ideal[i])/N_ideal[i];
    errdiffN_x0mm_y5mm[i] = (1/N_ideal[i])*sqrt(TMath::Power(N_x0mm_y5mm[i]*errN_ideal[i]/N_ideal[i],2)+TMath::Power(errN_x0mm_y5mm[i],2));
  }
  TGraphErrors *gr_diffN_x0mm_y5mm = new TGraphErrors(10,modulesID,diffN_x0mm_y5mm,0,errdiffN_x0mm_y5mm);
  gr_diffN_x0mm_y5mm->SetMarkerStyle(20);
  gr_diffN_x0mm_y5mm->SetMarkerColor(6);
  gr_diffN_x0mm_y5mm->SetLineColor(6);
  gr_diffN_x0mm_y5mm->SetLineWidth(1.5);
  gr_diffN_x0mm_y5mm->SetMarkerSize(1.0);

  TGraphErrors *modX0_x0mm_y5mm = (TGraphErrors*)fin_x0mm_y5mm->Get("modX0");
  modX0_x0mm_y5mm->SetMarkerColor(6);
  modX0_x0mm_y5mm->SetMarkerSize(1.0);
  Double_t *X0_x0mm_y5mm; Double_t *errX0_x0mm_y5mm;
  X0_x0mm_y5mm = modX0_x0mm_y5mm->GetY();
  errX0_x0mm_y5mm = modX0_x0mm_y5mm->GetEY();
  double diffX0_x0mm_y5mm[20];   double errdiffX0_x0mm_y5mm[20];
  for(int i=0;i<modX0_ideal->GetN();i++){
    diffX0_x0mm_y5mm[i] = X0_x0mm_y5mm[i] - X0_ideal[i];
    errdiffX0_x0mm_y5mm[i] = 0.5*sqrt(errX0_ideal[i]*errX0_ideal[i]+errX0_x0mm_y5mm[i]*errX0_x0mm_y5mm[i]);
  }
  TGraphErrors *gr_diffX0_x0mm_y5mm = new TGraphErrors(10,modulesID,diffX0_x0mm_y5mm,0,errdiffX0_x0mm_y5mm);
  gr_diffX0_x0mm_y5mm->SetMarkerStyle(20);
  gr_diffX0_x0mm_y5mm->SetMarkerColor(6);
  gr_diffX0_x0mm_y5mm->SetLineColor(6);
  gr_diffX0_x0mm_y5mm->SetLineWidth(1.5);
  gr_diffX0_x0mm_y5mm->SetMarkerSize(1.0);

  TGraphErrors *modY0_x0mm_y5mm = (TGraphErrors*)fin_x0mm_y5mm->Get("modY0");
  modY0_x0mm_y5mm->SetMarkerColor(6);
  modY0_x0mm_y5mm->SetMarkerSize(1.0);
  Double_t *Y0_x0mm_y5mm; Double_t *errY0_x0mm_y5mm;
  Y0_x0mm_y5mm = modY0_x0mm_y5mm->GetY();
  errY0_x0mm_y5mm = modY0_x0mm_y5mm->GetEY();
  double diffY0_x0mm_y5mm[20];   double errdiffY0_x0mm_y5mm[20];
  for(int i=0;i<modY0_ideal->GetN();i++){
    diffY0_x0mm_y5mm[i] = Y0_x0mm_y5mm[i] - Y0_ideal[i];
    errdiffY0_x0mm_y5mm[i] = 0.5*sqrt(errY0_ideal[i]*errY0_ideal[i]+errY0_x0mm_y5mm[i]*errY0_x0mm_y5mm[i]);
  }
  TGraphErrors *gr_diffY0_x0mm_y5mm = new TGraphErrors(10,modulesID,diffY0_x0mm_y5mm,0,errdiffY0_x0mm_y5mm);
  gr_diffY0_x0mm_y5mm->SetMarkerStyle(20);
  gr_diffY0_x0mm_y5mm->SetMarkerColor(6);
  gr_diffY0_x0mm_y5mm->SetLineColor(6);
  gr_diffY0_x0mm_y5mm->SetLineWidth(1.5);
  gr_diffY0_x0mm_y5mm->SetMarkerSize(1.0);
  TGraphErrors *modSLX_x0mm_y5mm = (TGraphErrors*)fin_x0mm_y5mm->Get("modSLX");
  modSLX_x0mm_y5mm->SetMarkerColor(6);
  modSLX_x0mm_y5mm->SetMarkerSize(1.0);
  Double_t *SLX_x0mm_y5mm; Double_t *errSLX_x0mm_y5mm;
  SLX_x0mm_y5mm = modSLX_x0mm_y5mm->GetY();
  errSLX_x0mm_y5mm = modSLX_x0mm_y5mm->GetEY();

  double diffSLX_x0mm_y5mm[20];   double errdiffSLX_x0mm_y5mm[20];
  for(int i=0;i<modSLX_ideal->GetN();i++){
    diffSLX_x0mm_y5mm[i] = SLX_x0mm_y5mm[i] - SLX_ideal[i];
    errdiffSLX_x0mm_y5mm[i] = 0.5*sqrt(errSLX_ideal[i]*errSLX_ideal[i]+errSLX_x0mm_y5mm[i]*errSLX_x0mm_y5mm[i]);
  }
  TGraphErrors *gr_diffSLX_x0mm_y5mm = new TGraphErrors(10,modulesID,diffSLX_x0mm_y5mm,0,errdiffSLX_x0mm_y5mm);
  gr_diffSLX_x0mm_y5mm->SetMarkerStyle(20);
  gr_diffSLX_x0mm_y5mm->SetMarkerColor(6);
  gr_diffSLX_x0mm_y5mm->SetLineColor(6);
  gr_diffSLX_x0mm_y5mm->SetLineWidth(1.5);
  gr_diffSLX_x0mm_y5mm->SetMarkerSize(1.0);
  TGraphErrors *modSLY_x0mm_y5mm = (TGraphErrors*)fin_x0mm_y5mm->Get("modSLY");
  modSLY_x0mm_y5mm->SetMarkerColor(6);
  modSLY_x0mm_y5mm->SetMarkerSize(1.0);
  Double_t *SLY_x0mm_y5mm; Double_t *errSLY_x0mm_y5mm;
  SLY_x0mm_y5mm = modSLY_x0mm_y5mm->GetY();
  errSLY_x0mm_y5mm = modSLY_x0mm_y5mm->GetEY();
  double diffSLY_x0mm_y5mm[20];   double errdiffSLY_x0mm_y5mm[20];
  for(int i=0;i<modSLY_ideal->GetN();i++){
    diffSLY_x0mm_y5mm[i] = SLY_x0mm_y5mm[i] - SLY_ideal[i];
    errdiffSLY_x0mm_y5mm[i] = 0.5*sqrt(errSLY_ideal[i]*errSLY_ideal[i]+errSLY_x0mm_y5mm[i]*errSLY_x0mm_y5mm[i]);
  }
  TGraphErrors *gr_diffSLY_x0mm_y5mm = new TGraphErrors(10,modulesID,diffSLY_x0mm_y5mm,0,errdiffSLY_x0mm_y5mm);
  gr_diffSLY_x0mm_y5mm->SetMarkerStyle(20);
  gr_diffSLY_x0mm_y5mm->SetMarkerColor(6);
  gr_diffSLY_x0mm_y5mm->SetLineColor(6);
  gr_diffSLY_x0mm_y5mm->SetLineWidth(1.5);
  gr_diffSLY_x0mm_y5mm->SetMarkerSize(1.0);


  //x=-5,y=0
 TFile *fin_xmn5mm_y0mm = new TFile("/panda/myResults/AssymIP/DPM/mom_1_5/x_-5mm_y_0mm/HitsAssym_x_y_1_5GeV.root","READ");
 TGraphErrors *modN_xmn5mm_y0mm = (TGraphErrors*)fin_xmn5mm_y0mm->Get("modN");
  modN_xmn5mm_y0mm->SetMarkerColor(7);
  modN_xmn5mm_y0mm->SetMarkerSize(1.0);
  Double_t *N_xmn5mm_y0mm; Double_t *errN_xmn5mm_y0mm;
  N_xmn5mm_y0mm = modN_xmn5mm_y0mm->GetY();
  errN_xmn5mm_y0mm = modN_xmn5mm_y0mm->GetEY();
  double diffN_xmn5mm_y0mm[20];   double errdiffN_xmn5mm_y0mm[20];
  for(int i=0;i<modN_ideal->GetN();i++){
    // diffN_xmn5mm_y0mm[i] = N_xmn5mm_y0mm[i] - N_ideal[i];
    // errdiffN_xmn5mm_y0mm[i] = 0.5*sqrt(errN_ideal[i]*errN_ideal[i]+errN_xmn5mm_y0mm[i]*errN_xmn5mm_y0mm[i]);
    diffN_xmn5mm_y0mm[i] = 100.*(N_xmn5mm_y0mm[i] - N_ideal[i])/N_ideal[i];
    errdiffN_xmn5mm_y0mm[i] = (1/N_ideal[i])*sqrt(TMath::Power(N_xmn5mm_y0mm[i]*errN_ideal[i]/N_ideal[i],2)+TMath::Power(errN_xmn5mm_y0mm[i],2));
  }
  TGraphErrors *gr_diffN_xmn5mm_y0mm = new TGraphErrors(10,modulesID,diffN_xmn5mm_y0mm,0,errdiffN_xmn5mm_y0mm);
  gr_diffN_xmn5mm_y0mm->SetMarkerStyle(20);
  gr_diffN_xmn5mm_y0mm->SetMarkerColor(7);
  gr_diffN_xmn5mm_y0mm->SetLineColor(7);
  gr_diffN_xmn5mm_y0mm->SetLineWidth(1.5);
  gr_diffN_xmn5mm_y0mm->SetMarkerSize(1.0);

  TGraphErrors *modX0_xmn5mm_y0mm = (TGraphErrors*)fin_xmn5mm_y0mm->Get("modX0");
  modX0_xmn5mm_y0mm->SetMarkerColor(7);
  modX0_xmn5mm_y0mm->SetMarkerSize(1.0);
  Double_t *X0_xmn5mm_y0mm; Double_t *errX0_xmn5mm_y0mm;
  X0_xmn5mm_y0mm = modX0_xmn5mm_y0mm->GetY();
  errX0_xmn5mm_y0mm = modX0_xmn5mm_y0mm->GetEY();
  double diffX0_xmn5mm_y0mm[20];   double errdiffX0_xmn5mm_y0mm[20];
  for(int i=0;i<modX0_ideal->GetN();i++){
    diffX0_xmn5mm_y0mm[i] = X0_xmn5mm_y0mm[i] - X0_ideal[i];
    errdiffX0_xmn5mm_y0mm[i] = 0.5*sqrt(errX0_ideal[i]*errX0_ideal[i]+errX0_xmn5mm_y0mm[i]*errX0_xmn5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffX0_xmn5mm_y0mm = new TGraphErrors(10,modulesID,diffX0_xmn5mm_y0mm,0,errdiffX0_xmn5mm_y0mm);
  gr_diffX0_xmn5mm_y0mm->SetMarkerStyle(20);
  gr_diffX0_xmn5mm_y0mm->SetMarkerColor(7);
  gr_diffX0_xmn5mm_y0mm->SetLineColor(7);
  gr_diffX0_xmn5mm_y0mm->SetLineWidth(1.5);
  gr_diffX0_xmn5mm_y0mm->SetMarkerSize(1.0);
  TGraphErrors *modY0_xmn5mm_y0mm = (TGraphErrors*)fin_xmn5mm_y0mm->Get("modY0");
  modY0_xmn5mm_y0mm->SetMarkerColor(7);
  modY0_xmn5mm_y0mm->SetMarkerSize(1.0);
  Double_t *Y0_xmn5mm_y0mm; Double_t *errY0_xmn5mm_y0mm;
  Y0_xmn5mm_y0mm = modY0_xmn5mm_y0mm->GetY();
  errY0_xmn5mm_y0mm = modY0_xmn5mm_y0mm->GetEY();
  double diffY0_xmn5mm_y0mm[20];   double errdiffY0_xmn5mm_y0mm[20];
  for(int i=0;i<modY0_ideal->GetN();i++){
    diffY0_xmn5mm_y0mm[i] = Y0_xmn5mm_y0mm[i] - Y0_ideal[i];
    errdiffY0_xmn5mm_y0mm[i] = 0.5*sqrt(errY0_ideal[i]*errY0_ideal[i]+errY0_xmn5mm_y0mm[i]*errY0_xmn5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffY0_xmn5mm_y0mm = new TGraphErrors(10,modulesID,diffY0_xmn5mm_y0mm,0,errdiffY0_xmn5mm_y0mm);
  gr_diffY0_xmn5mm_y0mm->SetMarkerStyle(20);
  gr_diffY0_xmn5mm_y0mm->SetMarkerColor(7);
  gr_diffY0_xmn5mm_y0mm->SetLineColor(7);
  gr_diffY0_xmn5mm_y0mm->SetLineWidth(1.5);
  gr_diffY0_xmn5mm_y0mm->SetMarkerSize(1.0);
  TGraphErrors *modSLX_xmn5mm_y0mm = (TGraphErrors*)fin_xmn5mm_y0mm->Get("modSLX");
  modSLX_xmn5mm_y0mm->SetMarkerColor(7);
  modSLX_xmn5mm_y0mm->SetMarkerSize(1.0);
  Double_t *SLX_xmn5mm_y0mm; Double_t *errSLX_xmn5mm_y0mm;
  SLX_xmn5mm_y0mm = modSLX_xmn5mm_y0mm->GetY();
  errSLX_xmn5mm_y0mm = modSLX_xmn5mm_y0mm->GetEY();

  double diffSLX_xmn5mm_y0mm[20];   double errdiffSLX_xmn5mm_y0mm[20];
  for(int i=0;i<modSLX_ideal->GetN();i++){
    diffSLX_xmn5mm_y0mm[i] = SLX_xmn5mm_y0mm[i] - SLX_ideal[i];
    errdiffSLX_xmn5mm_y0mm[i] = 0.5*sqrt(errSLX_ideal[i]*errSLX_ideal[i]+errSLX_xmn5mm_y0mm[i]*errSLX_xmn5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffSLX_xmn5mm_y0mm = new TGraphErrors(10,modulesID,diffSLX_xmn5mm_y0mm,0,errdiffSLX_xmn5mm_y0mm);
  gr_diffSLX_xmn5mm_y0mm->SetMarkerStyle(20);
  gr_diffSLX_xmn5mm_y0mm->SetMarkerColor(7);
  gr_diffSLX_xmn5mm_y0mm->SetLineColor(7);
  gr_diffSLX_xmn5mm_y0mm->SetLineWidth(1.5);
  gr_diffSLX_xmn5mm_y0mm->SetMarkerSize(1.0);
  TGraphErrors *modSLY_xmn5mm_y0mm = (TGraphErrors*)fin_xmn5mm_y0mm->Get("modSLY");
  modSLY_xmn5mm_y0mm->SetMarkerColor(7);
  modSLY_xmn5mm_y0mm->SetMarkerSize(1.0);
  Double_t *SLY_xmn5mm_y0mm; Double_t *errSLY_xmn5mm_y0mm;
  SLY_xmn5mm_y0mm = modSLY_xmn5mm_y0mm->GetY();
  errSLY_xmn5mm_y0mm = modSLY_xmn5mm_y0mm->GetEY();
  double diffSLY_xmn5mm_y0mm[20];   double errdiffSLY_xmn5mm_y0mm[20];
  for(int i=0;i<modSLY_ideal->GetN();i++){
    diffSLY_xmn5mm_y0mm[i] = SLY_xmn5mm_y0mm[i] - SLY_ideal[i];
    errdiffSLY_xmn5mm_y0mm[i] = 0.5*sqrt(errSLY_ideal[i]*errSLY_ideal[i]+errSLY_xmn5mm_y0mm[i]*errSLY_xmn5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffSLY_xmn5mm_y0mm = new TGraphErrors(10,modulesID,diffSLY_xmn5mm_y0mm,0,errdiffSLY_xmn5mm_y0mm);
  gr_diffSLY_xmn5mm_y0mm->SetMarkerStyle(20);
  gr_diffSLY_xmn5mm_y0mm->SetMarkerColor(7);
  gr_diffSLY_xmn5mm_y0mm->SetLineColor(7);
  gr_diffSLY_xmn5mm_y0mm->SetLineWidth(1.5);
  gr_diffSLY_xmn5mm_y0mm->SetMarkerSize(1.0);

  //x=0,y=-5
TFile *fin_x0mm_ymn5mm = new TFile("/panda/myResults/AssymIP/DPM/mom_1_5/x_-5mm_y_0mm/HitsAssym_x_y_1_5GeV.root","READ");
 TGraphErrors *modN_x0mm_ymn5mm = (TGraphErrors*)fin_x0mm_ymn5mm->Get("modN");
  modN_x0mm_ymn5mm->SetMarkerColor(8);
  modN_x0mm_ymn5mm->SetMarkerSize(1.0);
  Double_t *N_x0mm_ymn5mm; Double_t *errN_x0mm_ymn5mm;
  N_x0mm_ymn5mm = modN_x0mm_ymn5mm->GetY();
  errN_x0mm_ymn5mm = modN_x0mm_ymn5mm->GetEY();
  double diffN_x0mm_ymn5mm[20];   double errdiffN_x0mm_ymn5mm[20];
  for(int i=0;i<modN_ideal->GetN();i++){
    // diffN_x0mm_ymn5mm[i] = N_x0mm_ymn5mm[i] - N_ideal[i];
    // errdiffN_x0mm_ymn5mm[i] = 0.5*sqrt(errN_ideal[i]*errN_ideal[i]+errN_x0mm_ymn5mm[i]*errN_x0mm_ymn5mm[i]);
    diffN_x0mm_ymn5mm[i] = 100.*(N_x0mm_ymn5mm[i] - N_ideal[i])/N_ideal[i];
    errdiffN_x0mm_ymn5mm[i] = (1/N_ideal[i])*sqrt(TMath::Power(N_x0mm_ymn5mm[i]*errN_ideal[i]/N_ideal[i],2)+TMath::Power(errN_x0mm_ymn5mm[i],2));
  }
  TGraphErrors *gr_diffN_x0mm_ymn5mm = new TGraphErrors(10,modulesID,diffN_x0mm_ymn5mm,0,errdiffN_x0mm_ymn5mm);
  gr_diffN_x0mm_ymn5mm->SetMarkerStyle(20);
  gr_diffN_x0mm_ymn5mm->SetMarkerColor(8);
  gr_diffN_x0mm_ymn5mm->SetLineColor(8);
  gr_diffN_x0mm_ymn5mm->SetLineWidth(1.5);
  gr_diffN_x0mm_ymn5mm->SetMarkerSize(1.0);

  TGraphErrors *modX0_x0mm_ymn5mm = (TGraphErrors*)fin_x0mm_ymn5mm->Get("modX0");
  modX0_x0mm_ymn5mm->SetMarkerColor(8);
  modX0_x0mm_ymn5mm->SetMarkerSize(1.0);
  Double_t *X0_x0mm_ymn5mm; Double_t *errX0_x0mm_ymn5mm;
  X0_x0mm_ymn5mm = modX0_x0mm_ymn5mm->GetY();
  errX0_x0mm_ymn5mm = modX0_x0mm_ymn5mm->GetEY();
  double diffX0_x0mm_ymn5mm[20];   double errdiffX0_x0mm_ymn5mm[20];
  for(int i=0;i<modX0_ideal->GetN();i++){
    diffX0_x0mm_ymn5mm[i] = X0_x0mm_ymn5mm[i] - X0_ideal[i];
    errdiffX0_x0mm_ymn5mm[i] = 0.5*sqrt(errX0_ideal[i]*errX0_ideal[i]+errX0_x0mm_ymn5mm[i]*errX0_x0mm_ymn5mm[i]);
  }
  TGraphErrors *gr_diffX0_x0mm_ymn5mm = new TGraphErrors(10,modulesID,diffX0_x0mm_ymn5mm,0,errdiffX0_x0mm_ymn5mm);
  gr_diffX0_x0mm_ymn5mm->SetMarkerStyle(20);
  gr_diffX0_x0mm_ymn5mm->SetMarkerColor(8);
  gr_diffX0_x0mm_ymn5mm->SetLineColor(8);
  gr_diffX0_x0mm_ymn5mm->SetLineWidth(1.5);
  gr_diffX0_x0mm_ymn5mm->SetMarkerSize(1.0);
  TGraphErrors *modY0_x0mm_ymn5mm = (TGraphErrors*)fin_x0mm_ymn5mm->Get("modY0");
  modY0_x0mm_ymn5mm->SetMarkerColor(8);
  modY0_x0mm_ymn5mm->SetMarkerSize(1.0);
  Double_t *Y0_x0mm_ymn5mm; Double_t *errY0_x0mm_ymn5mm;
  Y0_x0mm_ymn5mm = modY0_x0mm_ymn5mm->GetY();
  errY0_x0mm_ymn5mm = modY0_x0mm_ymn5mm->GetEY();
  double diffY0_x0mm_ymn5mm[20];   double errdiffY0_x0mm_ymn5mm[20];
  for(int i=0;i<modY0_ideal->GetN();i++){
    diffY0_x0mm_ymn5mm[i] = Y0_x0mm_ymn5mm[i] - Y0_ideal[i];
    errdiffY0_x0mm_ymn5mm[i] = 0.5*sqrt(errY0_ideal[i]*errY0_ideal[i]+errY0_x0mm_ymn5mm[i]*errY0_x0mm_ymn5mm[i]);
  }
  TGraphErrors *gr_diffY0_x0mm_ymn5mm = new TGraphErrors(10,modulesID,diffY0_x0mm_ymn5mm,0,errdiffY0_x0mm_ymn5mm);
  gr_diffY0_x0mm_ymn5mm->SetMarkerStyle(20);
  gr_diffY0_x0mm_ymn5mm->SetMarkerColor(8);
  gr_diffY0_x0mm_ymn5mm->SetLineColor(8);
  gr_diffY0_x0mm_ymn5mm->SetLineWidth(1.5);
  gr_diffY0_x0mm_ymn5mm->SetMarkerSize(1.0);
  TGraphErrors *modSLX_x0mm_ymn5mm = (TGraphErrors*)fin_x0mm_ymn5mm->Get("modSLX");
  modSLX_x0mm_ymn5mm->SetMarkerColor(8);
  modSLX_x0mm_ymn5mm->SetMarkerSize(1.0);
  Double_t *SLX_x0mm_ymn5mm; Double_t *errSLX_x0mm_ymn5mm;
  SLX_x0mm_ymn5mm = modSLX_x0mm_ymn5mm->GetY();
  errSLX_x0mm_ymn5mm = modSLX_x0mm_ymn5mm->GetEY();

  double diffSLX_x0mm_ymn5mm[20];   double errdiffSLX_x0mm_ymn5mm[20];
  for(int i=0;i<modSLX_ideal->GetN();i++){
    diffSLX_x0mm_ymn5mm[i] = SLX_x0mm_ymn5mm[i] - SLX_ideal[i];
    errdiffSLX_x0mm_ymn5mm[i] = 0.5*sqrt(errSLX_ideal[i]*errSLX_ideal[i]+errSLX_x0mm_ymn5mm[i]*errSLX_x0mm_ymn5mm[i]);
  }
  TGraphErrors *gr_diffSLX_x0mm_ymn5mm = new TGraphErrors(10,modulesID,diffSLX_x0mm_ymn5mm,0,errdiffSLX_x0mm_ymn5mm);
  gr_diffSLX_x0mm_ymn5mm->SetMarkerStyle(20);
  gr_diffSLX_x0mm_ymn5mm->SetMarkerColor(8);
  gr_diffSLX_x0mm_ymn5mm->SetLineColor(8);
  gr_diffSLX_x0mm_ymn5mm->SetLineWidth(1.5);
  gr_diffSLX_x0mm_ymn5mm->SetMarkerSize(1.0);
  TGraphErrors *modSLY_x0mm_ymn5mm = (TGraphErrors*)fin_x0mm_ymn5mm->Get("modSLY");
  modSLY_x0mm_ymn5mm->SetMarkerColor(8);
  modSLY_x0mm_ymn5mm->SetMarkerSize(1.0);
  Double_t *SLY_x0mm_ymn5mm; Double_t *errSLY_x0mm_ymn5mm;
  SLY_x0mm_ymn5mm = modSLY_x0mm_ymn5mm->GetY();
  errSLY_x0mm_ymn5mm = modSLY_x0mm_ymn5mm->GetEY();
  double diffSLY_x0mm_ymn5mm[20];   double errdiffSLY_x0mm_ymn5mm[20];
  for(int i=0;i<modSLY_ideal->GetN();i++){
    diffSLY_x0mm_ymn5mm[i] = SLY_x0mm_ymn5mm[i] - SLY_ideal[i];
    errdiffSLY_x0mm_ymn5mm[i] = 0.5*sqrt(errSLY_ideal[i]*errSLY_ideal[i]+errSLY_x0mm_ymn5mm[i]*errSLY_x0mm_ymn5mm[i]);
  }
  TGraphErrors *gr_diffSLY_x0mm_ymn5mm = new TGraphErrors(10,modulesID,diffSLY_x0mm_ymn5mm,0,errdiffSLY_x0mm_ymn5mm);
  gr_diffSLY_x0mm_ymn5mm->SetMarkerStyle(20);
  gr_diffSLY_x0mm_ymn5mm->SetMarkerColor(8);
  gr_diffSLY_x0mm_ymn5mm->SetLineColor(8);
  gr_diffSLY_x0mm_ymn5mm->SetLineWidth(1.5);
  gr_diffSLY_x0mm_ymn5mm->SetMarkerSize(1.0);

  //x=-5,y=-5
TFile *fin_xmn5mm_ymn5mm = new TFile("/panda/myResults/AssymIP/DPM/mom_1_5/x_-5mm_y_-5mm/HitsAssym_x_y_1_5GeV.root","READ");
TGraphErrors *modN_xmn5mm_ymn5mm = (TGraphErrors*)fin_xmn5mm_ymn5mm->Get("modN");
  modN_xmn5mm_ymn5mm->SetMarkerColor(9);
  modN_xmn5mm_ymn5mm->SetMarkerSize(1.0);
  Double_t *N_xmn5mm_ymn5mm; Double_t *errN_xmn5mm_ymn5mm;
  N_xmn5mm_ymn5mm = modN_xmn5mm_ymn5mm->GetY();
  errN_xmn5mm_ymn5mm = modN_xmn5mm_ymn5mm->GetEY();
  double diffN_xmn5mm_ymn5mm[20];   double errdiffN_xmn5mm_ymn5mm[20];
  for(int i=0;i<modN_ideal->GetN();i++){
    // diffN_xmn5mm_ymn5mm[i] = N_xmn5mm_ymn5mm[i] - N_ideal[i];
    // errdiffN_xmn5mm_ymn5mm[i] = 0.5*sqrt(errN_ideal[i]*errN_ideal[i]+errN_xmn5mm_ymn5mm[i]*errN_xmn5mm_ymn5mm[i]);
    diffN_xmn5mm_ymn5mm[i] = 100.*(N_xmn5mm_ymn5mm[i] - N_ideal[i])/N_ideal[i];
    errdiffN_xmn5mm_ymn5mm[i] = (1/N_ideal[i])*sqrt(TMath::Power(N_xmn5mm_ymn5mm[i]*errN_ideal[i]/N_ideal[i],2)+TMath::Power(errN_xmn5mm_ymn5mm[i],2));
  }
  TGraphErrors *gr_diffN_xmn5mm_ymn5mm = new TGraphErrors(10,modulesID,diffN_xmn5mm_ymn5mm,0,errdiffN_xmn5mm_ymn5mm);
  gr_diffN_xmn5mm_ymn5mm->SetMarkerStyle(20);
  gr_diffN_xmn5mm_ymn5mm->SetMarkerColor(9);
  gr_diffN_xmn5mm_ymn5mm->SetLineColor(9);
  gr_diffN_xmn5mm_ymn5mm->SetLineWidth(1.5);
  gr_diffN_xmn5mm_ymn5mm->SetMarkerSize(1.0);

  TGraphErrors *modX0_xmn5mm_ymn5mm = (TGraphErrors*)fin_xmn5mm_ymn5mm->Get("modX0");
  modX0_xmn5mm_ymn5mm->SetMarkerColor(9);
  modX0_xmn5mm_ymn5mm->SetMarkerSize(1.0);
  Double_t *X0_xmn5mm_ymn5mm; Double_t *errX0_xmn5mm_ymn5mm;
  X0_xmn5mm_ymn5mm = modX0_xmn5mm_ymn5mm->GetY();
  errX0_xmn5mm_ymn5mm = modX0_xmn5mm_ymn5mm->GetEY();
  double diffX0_xmn5mm_ymn5mm[20];   double errdiffX0_xmn5mm_ymn5mm[20];
  for(int i=0;i<modX0_ideal->GetN();i++){
    diffX0_xmn5mm_ymn5mm[i] = X0_xmn5mm_ymn5mm[i] - X0_ideal[i];
    errdiffX0_xmn5mm_ymn5mm[i] = 0.5*sqrt(errX0_ideal[i]*errX0_ideal[i]+errX0_xmn5mm_ymn5mm[i]*errX0_xmn5mm_ymn5mm[i]);
  }
  TGraphErrors *gr_diffX0_xmn5mm_ymn5mm = new TGraphErrors(10,modulesID,diffX0_xmn5mm_ymn5mm,0,errdiffX0_xmn5mm_ymn5mm);
  gr_diffX0_xmn5mm_ymn5mm->SetMarkerStyle(20);
  gr_diffX0_xmn5mm_ymn5mm->SetMarkerColor(9);
  gr_diffX0_xmn5mm_ymn5mm->SetLineColor(9);
  gr_diffX0_xmn5mm_ymn5mm->SetLineWidth(1.5);
  gr_diffX0_xmn5mm_ymn5mm->SetMarkerSize(1.0);
  TGraphErrors *modY0_xmn5mm_ymn5mm = (TGraphErrors*)fin_xmn5mm_ymn5mm->Get("modY0");
  modY0_xmn5mm_ymn5mm->SetMarkerColor(9);
  modY0_xmn5mm_ymn5mm->SetMarkerSize(1.0);
  Double_t *Y0_xmn5mm_ymn5mm; Double_t *errY0_xmn5mm_ymn5mm;
  Y0_xmn5mm_ymn5mm = modY0_xmn5mm_ymn5mm->GetY();
  errY0_xmn5mm_ymn5mm = modY0_xmn5mm_ymn5mm->GetEY();
  double diffY0_xmn5mm_ymn5mm[20];   double errdiffY0_xmn5mm_ymn5mm[20];
  for(int i=0;i<modY0_ideal->GetN();i++){
    diffY0_xmn5mm_ymn5mm[i] = Y0_xmn5mm_ymn5mm[i] - Y0_ideal[i];
    errdiffY0_xmn5mm_ymn5mm[i] = 0.5*sqrt(errY0_ideal[i]*errY0_ideal[i]+errY0_xmn5mm_ymn5mm[i]*errY0_xmn5mm_ymn5mm[i]);
  }
  TGraphErrors *gr_diffY0_xmn5mm_ymn5mm = new TGraphErrors(10,modulesID,diffY0_xmn5mm_ymn5mm,0,errdiffY0_xmn5mm_ymn5mm);
  gr_diffY0_xmn5mm_ymn5mm->SetMarkerStyle(20);
  gr_diffY0_xmn5mm_ymn5mm->SetMarkerColor(9);
  gr_diffY0_xmn5mm_ymn5mm->SetLineColor(9);
  gr_diffY0_xmn5mm_ymn5mm->SetLineWidth(1.5);
  gr_diffY0_xmn5mm_ymn5mm->SetMarkerSize(1.0);
  TGraphErrors *modSLX_xmn5mm_ymn5mm = (TGraphErrors*)fin_xmn5mm_ymn5mm->Get("modSLX");
  modSLX_xmn5mm_ymn5mm->SetMarkerColor(9);
  modSLX_xmn5mm_ymn5mm->SetMarkerSize(1.0);
  Double_t *SLX_xmn5mm_ymn5mm; Double_t *errSLX_xmn5mm_ymn5mm;
  SLX_xmn5mm_ymn5mm = modSLX_xmn5mm_ymn5mm->GetY();
  errSLX_xmn5mm_ymn5mm = modSLX_xmn5mm_ymn5mm->GetEY();

  double diffSLX_xmn5mm_ymn5mm[20];   double errdiffSLX_xmn5mm_ymn5mm[20];
  for(int i=0;i<modSLX_ideal->GetN();i++){
    diffSLX_xmn5mm_ymn5mm[i] = SLX_xmn5mm_ymn5mm[i] - SLX_ideal[i];
    errdiffSLX_xmn5mm_ymn5mm[i] = 0.5*sqrt(errSLX_ideal[i]*errSLX_ideal[i]+errSLX_xmn5mm_ymn5mm[i]*errSLX_xmn5mm_ymn5mm[i]);
  }
  TGraphErrors *gr_diffSLX_xmn5mm_ymn5mm = new TGraphErrors(10,modulesID,diffSLX_xmn5mm_ymn5mm,0,errdiffSLX_xmn5mm_ymn5mm);
  gr_diffSLX_xmn5mm_ymn5mm->SetMarkerStyle(20);
  gr_diffSLX_xmn5mm_ymn5mm->SetMarkerColor(9);
  gr_diffSLX_xmn5mm_ymn5mm->SetLineColor(9);
  gr_diffSLX_xmn5mm_ymn5mm->SetLineWidth(1.5);
  gr_diffSLX_xmn5mm_ymn5mm->SetMarkerSize(1.0);
  TGraphErrors *modSLY_xmn5mm_ymn5mm = (TGraphErrors*)fin_xmn5mm_ymn5mm->Get("modSLY");
  modSLY_xmn5mm_ymn5mm->SetMarkerColor(9);
  modSLY_xmn5mm_ymn5mm->SetMarkerSize(1.0);
  Double_t *SLY_xmn5mm_ymn5mm; Double_t *errSLY_xmn5mm_ymn5mm;
  SLY_xmn5mm_ymn5mm = modSLY_xmn5mm_ymn5mm->GetY();
  errSLY_xmn5mm_ymn5mm = modSLY_xmn5mm_ymn5mm->GetEY();
  double diffSLY_xmn5mm_ymn5mm[20];   double errdiffSLY_xmn5mm_ymn5mm[20];
  for(int i=0;i<modSLY_ideal->GetN();i++){
    diffSLY_xmn5mm_ymn5mm[i] = SLY_xmn5mm_ymn5mm[i] - SLY_ideal[i];
    errdiffSLY_xmn5mm_ymn5mm[i] = 0.5*sqrt(errSLY_ideal[i]*errSLY_ideal[i]+errSLY_xmn5mm_ymn5mm[i]*errSLY_xmn5mm_ymn5mm[i]);
  }
  TGraphErrors *gr_diffSLY_xmn5mm_ymn5mm = new TGraphErrors(10,modulesID,diffSLY_xmn5mm_ymn5mm,0,errdiffSLY_xmn5mm_ymn5mm);
  gr_diffSLY_xmn5mm_ymn5mm->SetMarkerStyle(20);
  gr_diffSLY_xmn5mm_ymn5mm->SetMarkerColor(9);
  gr_diffSLY_xmn5mm_ymn5mm->SetLineColor(9);
  gr_diffSLY_xmn5mm_ymn5mm->SetLineWidth(1.5);
  gr_diffSLY_xmn5mm_ymn5mm->SetMarkerSize(1.0);


  TLegend *leg = new TLegend(0.44,0.33,0.61,0.88);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.055);
  leg->SetHeader("(X,Y)_{IP} [mm]");
  TCanvas *c1 = new TCanvas("Assym", "Assym", 700, 800);
  c1->Divide(2,5);
  c1->cd(1);
  TMultiGraph *mgr_X0 = new TMultiGraph();
  mgr_X0->Add(modX0_ideal);
  mgr_X0->Add(modX0_xpl5mm_y0mm);
  mgr_X0->Add(modX0_xpl5mm_y5mm);
  mgr_X0->Add(modX0_x0mm_y5mm);
  mgr_X0->Add(modX0_xmn5mm_y0mm);
  mgr_X0->Add(modX0_x0mm_ymn5mm);
  mgr_X0->Add(modX0_xmn5mm_ymn5mm);
  leg->AddEntry(modX0_ideal,"(0,0)","p");	  
  leg->AddEntry(modX0_xpl5mm_y0mm,"(5,0)","p");	  
  leg->AddEntry(modX0_x0mm_y5mm,"(0,5)","p");	  
  leg->AddEntry(modX0_xpl5mm_y5mm,"(5,5)","p");	  
  leg->AddEntry(modX0_xmn5mm_y0mm,"(-5,0)","p");	  
  leg->AddEntry(modX0_x0mm_ymn5mm,"(0,-5)","p");	  
  leg->AddEntry(modX0_xmn5mm_ymn5mm,"(-5,-5)","p");	  
  mgr_X0->Draw("AP");
  leg->Draw();
  mgr_X0->GetXaxis()->SetTitle("sector");
  mgr_X0->GetYaxis()->SetTitle("X0, cm");
  c1->cd(2);
 TMultiGraph *mgr_diffX0 = new TMultiGraph();
 mgr_diffX0->Add(gr_diffX0_xpl5mm_y0mm);
 mgr_diffX0->Add(gr_diffX0_xpl5mm_y5mm);
 mgr_diffX0->Add(gr_diffX0_x0mm_y5mm);
 mgr_diffX0->Add(gr_diffX0_xmn5mm_y0mm);
 mgr_diffX0->Add(gr_diffX0_x0mm_ymn5mm);
 mgr_diffX0->Add(gr_diffX0_xmn5mm_ymn5mm);
 mgr_diffX0->Draw("APC");
 mgr_diffX0->GetXaxis()->SetTitle("sector");
 mgr_diffX0->GetYaxis()->SetTitle("#Delta X0, cm");
 mgr_diffX0->GetYaxis()->SetRangeUser(-1.,1.);
 c1->cd(3);
  TMultiGraph *mgr_Y0 = new TMultiGraph();
  mgr_Y0->Add(modY0_ideal);
  mgr_Y0->Add(modY0_xpl5mm_y0mm);
  mgr_Y0->Add(modY0_xpl5mm_y5mm);
  mgr_Y0->Add(modY0_x0mm_y5mm);
  mgr_Y0->Add(modY0_xmn5mm_y0mm);
  mgr_Y0->Add(modY0_x0mm_ymn5mm);
  mgr_Y0->Add(modY0_xmn5mm_ymn5mm);
  mgr_Y0->Draw("AP");
  mgr_Y0->GetXaxis()->SetTitle("sector");
  mgr_Y0->GetYaxis()->SetTitle("Y0, cm");
  c1->cd(4);
 TMultiGraph *mgr_diffY0 = new TMultiGraph();
 mgr_diffY0->Add(gr_diffY0_xpl5mm_y0mm);
 mgr_diffY0->Add(gr_diffY0_xpl5mm_y5mm);
 mgr_diffY0->Add(gr_diffY0_x0mm_y5mm);
 mgr_diffY0->Add(gr_diffY0_xmn5mm_y0mm);
 mgr_diffY0->Add(gr_diffY0_x0mm_ymn5mm);
 mgr_diffY0->Add(gr_diffY0_xmn5mm_ymn5mm);
 mgr_diffY0->Draw("APC");
 mgr_diffY0->GetXaxis()->SetTitle("sector");
 mgr_diffY0->GetYaxis()->SetTitle("#Delta Y0, cm");
 mgr_diffY0->GetYaxis()->SetRangeUser(-1.,1.);
 c1->cd(5);
  TMultiGraph *mgr_SLX = new TMultiGraph();
  mgr_SLX->Add(modSLX_ideal);
  mgr_SLX->Add(modSLX_xpl5mm_y0mm);
  mgr_SLX->Add(modSLX_xpl5mm_y5mm);
  mgr_SLX->Add(modSLX_x0mm_y5mm);
  mgr_SLX->Add(modSLX_xmn5mm_y0mm);
  mgr_SLX->Add(modSLX_x0mm_ymn5mm);
  mgr_SLX->Add(modSLX_xmn5mm_ymn5mm);
  mgr_SLX->Draw("AP");
  mgr_SLX->GetXaxis()->SetTitle("sector");
  mgr_SLX->GetYaxis()->SetTitle("slope X");
  c1->cd(6);
 TMultiGraph *mgr_diffSLX = new TMultiGraph();
 mgr_diffSLX->Add(gr_diffSLX_xpl5mm_y0mm);
 mgr_diffSLX->Add(gr_diffSLX_xpl5mm_y5mm);
 mgr_diffSLX->Add(gr_diffSLX_x0mm_y5mm);
 mgr_diffSLX->Add(gr_diffSLX_xmn5mm_y0mm);
 mgr_diffSLX->Add(gr_diffSLX_x0mm_ymn5mm);
 mgr_diffSLX->Add(gr_diffSLX_xmn5mm_ymn5mm);
 mgr_diffSLX->Draw("APC");
 mgr_diffSLX->GetXaxis()->SetTitle("sector");
 mgr_diffSLX->GetYaxis()->SetTitle("#Delta slope X, cm");
 mgr_diffSLX->GetYaxis()->SetRangeUser(-0.1,0.1);
 c1->cd(7);
  TMultiGraph *mgr_SLY = new TMultiGraph();
  mgr_SLY->Add(modSLY_ideal);
  mgr_SLY->Add(modSLY_xpl5mm_y0mm);
  mgr_SLY->Add(modSLY_xpl5mm_y5mm);
  mgr_SLY->Add(modSLY_x0mm_y5mm);
  mgr_SLY->Add(modSLY_xmn5mm_y0mm);
  mgr_SLY->Add(modSLY_x0mm_ymn5mm);
  mgr_SLY->Add(modSLY_xmn5mm_ymn5mm);
  mgr_SLY->Draw("AP");
  mgr_SLY->GetXaxis()->SetTitle("sector");
  mgr_SLY->GetYaxis()->SetTitle("slope Y");
  c1->cd(8);
 TMultiGraph *mgr_diffSLY = new TMultiGraph();
 mgr_diffSLY->Add(gr_diffSLY_xpl5mm_y0mm);
 mgr_diffSLY->Add(gr_diffSLY_xpl5mm_y5mm);
 mgr_diffSLY->Add(gr_diffSLY_x0mm_y5mm);
 mgr_diffSLY->Add(gr_diffSLY_xmn5mm_y0mm);
 mgr_diffSLY->Add(gr_diffSLY_x0mm_ymn5mm);
 mgr_diffSLY->Add(gr_diffSLY_xmn5mm_ymn5mm);
 mgr_diffSLY->Draw("APC");
 mgr_diffSLY->GetXaxis()->SetTitle("sector");
 mgr_diffSLY->GetYaxis()->SetTitle("#Delta slope Y, cm");
 mgr_diffSLY->GetYaxis()->SetRangeUser(-0.1,0.1);
 c1->cd(9);
 TMultiGraph *mgr_N = new TMultiGraph();
  mgr_N->Add(modN_ideal);
  mgr_N->Add(modN_xpl5mm_y0mm);
  mgr_N->Add(modN_xpl5mm_y5mm);
  mgr_N->Add(modN_x0mm_y5mm);
  mgr_N->Add(modN_xmn5mm_y0mm);
  mgr_N->Add(modN_x0mm_ymn5mm);
  mgr_N->Add(modN_xmn5mm_ymn5mm);  
  mgr_N->Draw("AP");
  mgr_N->GetXaxis()->SetTitle("sector");
  mgr_N->GetYaxis()->SetTitle("N, cm");
  c1->cd(10);
 TMultiGraph *mgr_diffN = new TMultiGraph();
 mgr_diffN->Add(gr_diffN_xpl5mm_y0mm);
 mgr_diffN->Add(gr_diffN_xpl5mm_y5mm);
 mgr_diffN->Add(gr_diffN_x0mm_y5mm);
 mgr_diffN->Add(gr_diffN_xmn5mm_y0mm);
 mgr_diffN->Add(gr_diffN_x0mm_ymn5mm);
 mgr_diffN->Add(gr_diffN_xmn5mm_ymn5mm);
 mgr_diffN->Draw("APC");
 mgr_diffN->GetXaxis()->SetTitle("sector");
 mgr_diffN->GetYaxis()->SetTitle("#Delta N, %");
 // mgr_diffN->GetYaxis()->SetRangeUser(-1.,1.);
 c1->SaveAs("AssymRes.pdf");
 c1->SaveAs("AssymRes.root");
}
