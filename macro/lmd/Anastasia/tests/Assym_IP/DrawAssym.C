void DrawAssym(){
  TFile *finIDEAL = new TFile("testAssym_x_0mm_y_0mm/DPM/mom_1_5/HitsAssym_x_y_1_5GeV.root","READ");

  //X0
  TGraphErrors *modX0_ideal = (TGraphErrors*)finIDEAL->Get("modX0");
  double *modulesID = modX0_ideal->GetX();
  Double_t *X0_ideal; Double_t *errX0_ideal;
  X0_ideal = modX0_ideal->GetY();
  errX0_ideal = modX0_ideal->GetEY();
//Y0
TGraphErrors *modY0_ideal = (TGraphErrors*)finIDEAL->Get("modY0");
 Double_t *Y0_ideal; Double_t *errY0_ideal;
  Y0_ideal = modY0_ideal->GetY();
  errY0_ideal = modY0_ideal->GetEY();
  //Sl X
 TGraphErrors *modSLX_ideal = (TGraphErrors*)finIDEAL->Get("modSLX");
  Double_t *SLX_ideal; Double_t *errSLX_ideal;
  SLX_ideal = modSLX_ideal->GetY();
  errSLX_ideal = modSLX_ideal->GetEY();
 //Sl Y
 TGraphErrors *modSLY_ideal = (TGraphErrors*)finIDEAL->Get("modSLY");
  Double_t *SLY_ideal; Double_t *errSLY_ideal;
  SLY_ideal = modSLY_ideal->GetY();
  errSLY_ideal = modSLY_ideal->GetEY();

  //x = 5, y = 0
  TFile *fin_xpl5mm_y0mm = new TFile("testAssym_x_5mm_y_0mm/DPM/mom_1_5/HitsAssym_x_y_1_5GeV.root","READ");
  TGraphErrors *modX0_xpl5mm_y0mm = (TGraphErrors*)fin_xpl5mm_y0mm->Get("modX0");
  modX0_xpl5mm_y0mm->SetMarkerColor(4);
  Double_t *X0_xpl5mm_y0mm; Double_t *errX0_xpl5mm_y0mm;
  X0_xpl5mm_y0mm = modX0_xpl5mm_y0mm->GetY();
  errX0_xpl5mm_y0mm = modX0_xpl5mm_y0mm->GetEY();
  double diffX0_xpl5mm_y0mm[10];   double errdiffX0_xpl5mm_y0mm[10];
  for(int i=0;i<modX0_ideal->GetN();i++){
    diffX0_xpl5mm_y0mm[i] = X0_xpl5mm_y0mm[i] - X0_ideal[i];
    errdiffX0_xpl5mm_y0mm[i] = 0.5*sqrt(errX0_ideal[i]*errX0_ideal[i]+errX0_xpl5mm_y0mm[i]*errX0_xpl5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffX0_xpl5mm_y0mm = new TGraphErrors(10,modulesID,diffX0_xpl5mm_y0mm,0,errdiffX0_xpl5mm_y0mm);
  gr_diffX0_xpl5mm_y0mm->SetMarkerStyle(20);
  gr_diffX0_xpl5mm_y0mm->SetMarkerColor(4);
  gr_diffX0_xpl5mm_y0mm->SetMarkerSize(1.5);
  TGraphErrors *modY0_xpl5mm_y0mm = (TGraphErrors*)fin_xpl5mm_y0mm->Get("modY0");
  modY0_xpl5mm_y0mm->SetMarkerColor(4);
  Double_t *Y0_xpl5mm_y0mm; Double_t *errY0_xpl5mm_y0mm;
  Y0_xpl5mm_y0mm = modY0_xpl5mm_y0mm->GetY();
  errY0_xpl5mm_y0mm = modY0_xpl5mm_y0mm->GetEY();
  double diffY0_xpl5mm_y0mm[10];   double errdiffY0_xpl5mm_y0mm[10];
  for(int i=0;i<modY0_ideal->GetN();i++){
    diffY0_xpl5mm_y0mm[i] = Y0_xpl5mm_y0mm[i] - Y0_ideal[i];
    errdiffY0_xpl5mm_y0mm[i] = 0.5*sqrt(errY0_ideal[i]*errY0_ideal[i]+errY0_xpl5mm_y0mm[i]*errY0_xpl5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffY0_xpl5mm_y0mm = new TGraphErrors(10,modulesID,diffY0_xpl5mm_y0mm,0,errdiffY0_xpl5mm_y0mm);
  gr_diffY0_xpl5mm_y0mm->SetMarkerStyle(20);
  gr_diffY0_xpl5mm_y0mm->SetMarkerColor(4);
  gr_diffY0_xpl5mm_y0mm->SetMarkerSize(1.5);
  TGraphErrors *modSLX_xpl5mm_y0mm = (TGraphErrors*)fin_xpl5mm_y0mm->Get("modSLX");
  modSLX_xpl5mm_y0mm->SetMarkerColor(4);
  Double_t *SLX_xpl5mm_y0mm; Double_t *errSLX_xpl5mm_y0mm;
  SLX_xpl5mm_y0mm = modSLX_xpl5mm_y0mm->GetY();
  errSLX_xpl5mm_y0mm = modSLX_xpl5mm_y0mm->GetEY();

  double diffSLX_xpl5mm_y0mm[10];   double errdiffSLX_xpl5mm_y0mm[10];
  for(int i=0;i<modSLX_ideal->GetN();i++){
    diffSLX_xpl5mm_y0mm[i] = SLX_xpl5mm_y0mm[i] - SLX_ideal[i];
    errdiffSLX_xpl5mm_y0mm[i] = 0.5*sqrt(errSLX_ideal[i]*errSLX_ideal[i]+errSLX_xpl5mm_y0mm[i]*errSLX_xpl5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffSLX_xpl5mm_y0mm = new TGraphErrors(10,modulesID,diffSLX_xpl5mm_y0mm,0,errdiffSLX_xpl5mm_y0mm);
  gr_diffSLX_xpl5mm_y0mm->SetMarkerStyle(20);
  gr_diffSLX_xpl5mm_y0mm->SetMarkerColor(4);
  gr_diffSLX_xpl5mm_y0mm->SetMarkerSize(1.5);
  TGraphErrors *modSLY_xpl5mm_y0mm = (TGraphErrors*)fin_xpl5mm_y0mm->Get("modSLY");
  modSLY_xpl5mm_y0mm->SetMarkerColor(4);
  Double_t *SLY_xpl5mm_y0mm; Double_t *errSLY_xpl5mm_y0mm;
  SLY_xpl5mm_y0mm = modSLY_xpl5mm_y0mm->GetY();
  errSLY_xpl5mm_y0mm = modSLY_xpl5mm_y0mm->GetEY();
  double diffSLY_xpl5mm_y0mm[10];   double errdiffSLY_xpl5mm_y0mm[10];
  for(int i=0;i<modSLY_ideal->GetN();i++){
    diffSLY_xpl5mm_y0mm[i] = SLY_xpl5mm_y0mm[i] - SLY_ideal[i];
    errdiffSLY_xpl5mm_y0mm[i] = 0.5*sqrt(errSLY_ideal[i]*errSLY_ideal[i]+errSLY_xpl5mm_y0mm[i]*errSLY_xpl5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffSLY_xpl5mm_y0mm = new TGraphErrors(10,modulesID,diffSLY_xpl5mm_y0mm,0,errdiffSLY_xpl5mm_y0mm);
  gr_diffSLY_xpl5mm_y0mm->SetMarkerStyle(20);
  gr_diffSLY_xpl5mm_y0mm->SetMarkerColor(4);
  gr_diffSLY_xpl5mm_y0mm->SetMarkerSize(1.5);


  //x=5, y=5
TFile *fin_xpl5mm_y5mm = new TFile("testAssym_x_0mm_y_5mm/DPM/mom_1_5/HitsAssym_x_y_1_5GeV.root","READ");
  TGraphErrors *modX0_xpl5mm_y5mm = (TGraphErrors*)fin_xpl5mm_y5mm->Get("modX0");
  modX0_xpl5mm_y5mm->SetMarkerColor(5);
  Double_t *X0_xpl5mm_y5mm; Double_t *errX0_xpl5mm_y5mm;
  X0_xpl5mm_y5mm = modX0_xpl5mm_y5mm->GetY();
  errX0_xpl5mm_y5mm = modX0_xpl5mm_y5mm->GetEY();
  double diffX0_xpl5mm_y5mm[10];   double errdiffX0_xpl5mm_y5mm[10];
  for(int i=0;i<modX0_ideal->GetN();i++){
    diffX0_xpl5mm_y5mm[i] = X0_xpl5mm_y5mm[i] - X0_ideal[i];
    errdiffX0_xpl5mm_y5mm[i] = 0.5*sqrt(errX0_ideal[i]*errX0_ideal[i]+errX0_xpl5mm_y5mm[i]*errX0_xpl5mm_y5mm[i]);
  }
  TGraphErrors *gr_diffX0_xpl5mm_y5mm = new TGraphErrors(10,modulesID,diffX0_xpl5mm_y5mm,0,errdiffX0_xpl5mm_y5mm);
  gr_diffX0_xpl5mm_y5mm->SetMarkerStyle(20);
  gr_diffX0_xpl5mm_y5mm->SetMarkerColor(5);
  gr_diffX0_xpl5mm_y5mm->SetMarkerSize(1.5);
  TGraphErrors *modY0_xpl5mm_y5mm = (TGraphErrors*)fin_xpl5mm_y5mm->Get("modY0");
  modY0_xpl5mm_y5mm->SetMarkerColor(5);
  Double_t *Y0_xpl5mm_y5mm; Double_t *errY0_xpl5mm_y5mm;
  Y0_xpl5mm_y5mm = modY0_xpl5mm_y5mm->GetY();
  errY0_xpl5mm_y5mm = modY0_xpl5mm_y5mm->GetEY();
  double diffY0_xpl5mm_y5mm[10];   double errdiffY0_xpl5mm_y5mm[10];
  for(int i=0;i<modY0_ideal->GetN();i++){
    diffY0_xpl5mm_y5mm[i] = Y0_xpl5mm_y5mm[i] - Y0_ideal[i];
    errdiffY0_xpl5mm_y5mm[i] = 0.5*sqrt(errY0_ideal[i]*errY0_ideal[i]+errY0_xpl5mm_y5mm[i]*errY0_xpl5mm_y5mm[i]);
  }
  TGraphErrors *gr_diffY0_xpl5mm_y5mm = new TGraphErrors(10,modulesID,diffY0_xpl5mm_y5mm,0,errdiffY0_xpl5mm_y5mm);
  gr_diffY0_xpl5mm_y5mm->SetMarkerStyle(20);
  gr_diffY0_xpl5mm_y5mm->SetMarkerColor(5);
  gr_diffY0_xpl5mm_y5mm->SetMarkerSize(1.5);
  TGraphErrors *modSLX_xpl5mm_y5mm = (TGraphErrors*)fin_xpl5mm_y5mm->Get("modSLX");
  modSLX_xpl5mm_y5mm->SetMarkerColor(5);
  Double_t *SLX_xpl5mm_y5mm; Double_t *errSLX_xpl5mm_y5mm;
  SLX_xpl5mm_y5mm = modSLX_xpl5mm_y5mm->GetY();
  errSLX_xpl5mm_y5mm = modSLX_xpl5mm_y5mm->GetEY();

  double diffSLX_xpl5mm_y5mm[10];   double errdiffSLX_xpl5mm_y5mm[10];
  for(int i=0;i<modSLX_ideal->GetN();i++){
    diffSLX_xpl5mm_y5mm[i] = SLX_xpl5mm_y5mm[i] - SLX_ideal[i];
    errdiffSLX_xpl5mm_y5mm[i] = 0.5*sqrt(errSLX_ideal[i]*errSLX_ideal[i]+errSLX_xpl5mm_y5mm[i]*errSLX_xpl5mm_y5mm[i]);
  }
  TGraphErrors *gr_diffSLX_xpl5mm_y5mm = new TGraphErrors(10,modulesID,diffSLX_xpl5mm_y5mm,0,errdiffSLX_xpl5mm_y5mm);
  gr_diffSLX_xpl5mm_y5mm->SetMarkerStyle(20);
  gr_diffSLX_xpl5mm_y5mm->SetMarkerColor(5);
  gr_diffSLX_xpl5mm_y5mm->SetMarkerSize(1.5);
  TGraphErrors *modSLY_xpl5mm_y5mm = (TGraphErrors*)fin_xpl5mm_y5mm->Get("modSLY");
  modSLY_xpl5mm_y5mm->SetMarkerColor(5);
  Double_t *SLY_xpl5mm_y5mm; Double_t *errSLY_xpl5mm_y5mm;
  SLY_xpl5mm_y5mm = modSLY_xpl5mm_y5mm->GetY();
  errSLY_xpl5mm_y5mm = modSLY_xpl5mm_y5mm->GetEY();
  double diffSLY_xpl5mm_y5mm[10];   double errdiffSLY_xpl5mm_y5mm[10];
  for(int i=0;i<modSLY_ideal->GetN();i++){
    diffSLY_xpl5mm_y5mm[i] = SLY_xpl5mm_y5mm[i] - SLY_ideal[i];
    errdiffSLY_xpl5mm_y5mm[i] = 0.5*sqrt(errSLY_ideal[i]*errSLY_ideal[i]+errSLY_xpl5mm_y5mm[i]*errSLY_xpl5mm_y5mm[i]);
  }
  TGraphErrors *gr_diffSLY_xpl5mm_y5mm = new TGraphErrors(10,modulesID,diffSLY_xpl5mm_y5mm,0,errdiffSLY_xpl5mm_y5mm);
  gr_diffSLY_xpl5mm_y5mm->SetMarkerStyle(20);
  gr_diffSLY_xpl5mm_y5mm->SetMarkerColor(5);
  gr_diffSLY_xpl5mm_y5mm->SetMarkerSize(1.5);

  //x=0,y=5
TFile *fin_x0mm_y5mm = new TFile("testAssym_x_5mm_y_5mm/DPM/mom_1_5/HitsAssym_x_y_1_5GeV.root","READ");
  TGraphErrors *modX0_x0mm_y5mm = (TGraphErrors*)fin_x0mm_y5mm->Get("modX0");
  modX0_x0mm_y5mm->SetMarkerColor(6);
  Double_t *X0_x0mm_y5mm; Double_t *errX0_x0mm_y5mm;
  X0_x0mm_y5mm = modX0_x0mm_y5mm->GetY();
  errX0_x0mm_y5mm = modX0_x0mm_y5mm->GetEY();
  double diffX0_x0mm_y5mm[10];   double errdiffX0_x0mm_y5mm[10];
  for(int i=0;i<modX0_ideal->GetN();i++){
    diffX0_x0mm_y5mm[i] = X0_x0mm_y5mm[i] - X0_ideal[i];
    errdiffX0_x0mm_y5mm[i] = 0.5*sqrt(errX0_ideal[i]*errX0_ideal[i]+errX0_x0mm_y5mm[i]*errX0_x0mm_y5mm[i]);
  }
  TGraphErrors *gr_diffX0_x0mm_y5mm = new TGraphErrors(10,modulesID,diffX0_x0mm_y5mm,0,errdiffX0_x0mm_y5mm);
  gr_diffX0_x0mm_y5mm->SetMarkerStyle(20);
  gr_diffX0_x0mm_y5mm->SetMarkerColor(6);
  gr_diffX0_x0mm_y5mm->SetMarkerSize(1.5);
  TGraphErrors *modY0_x0mm_y5mm = (TGraphErrors*)fin_x0mm_y5mm->Get("modY0");
  modY0_x0mm_y5mm->SetMarkerColor(6);
  Double_t *Y0_x0mm_y5mm; Double_t *errY0_x0mm_y5mm;
  Y0_x0mm_y5mm = modY0_x0mm_y5mm->GetY();
  errY0_x0mm_y5mm = modY0_x0mm_y5mm->GetEY();
  double diffY0_x0mm_y5mm[10];   double errdiffY0_x0mm_y5mm[10];
  for(int i=0;i<modY0_ideal->GetN();i++){
    diffY0_x0mm_y5mm[i] = Y0_x0mm_y5mm[i] - Y0_ideal[i];
    errdiffY0_x0mm_y5mm[i] = 0.5*sqrt(errY0_ideal[i]*errY0_ideal[i]+errY0_x0mm_y5mm[i]*errY0_x0mm_y5mm[i]);
  }
  TGraphErrors *gr_diffY0_x0mm_y5mm = new TGraphErrors(10,modulesID,diffY0_x0mm_y5mm,0,errdiffY0_x0mm_y5mm);
  gr_diffY0_x0mm_y5mm->SetMarkerStyle(20);
  gr_diffY0_x0mm_y5mm->SetMarkerColor(6);
  gr_diffY0_x0mm_y5mm->SetMarkerSize(1.5);
  TGraphErrors *modSLX_x0mm_y5mm = (TGraphErrors*)fin_x0mm_y5mm->Get("modSLX");
  modSLX_x0mm_y5mm->SetMarkerColor(6);
  Double_t *SLX_x0mm_y5mm; Double_t *errSLX_x0mm_y5mm;
  SLX_x0mm_y5mm = modSLX_x0mm_y5mm->GetY();
  errSLX_x0mm_y5mm = modSLX_x0mm_y5mm->GetEY();

  double diffSLX_x0mm_y5mm[10];   double errdiffSLX_x0mm_y5mm[10];
  for(int i=0;i<modSLX_ideal->GetN();i++){
    diffSLX_x0mm_y5mm[i] = SLX_x0mm_y5mm[i] - SLX_ideal[i];
    errdiffSLX_x0mm_y5mm[i] = 0.5*sqrt(errSLX_ideal[i]*errSLX_ideal[i]+errSLX_x0mm_y5mm[i]*errSLX_x0mm_y5mm[i]);
  }
  TGraphErrors *gr_diffSLX_x0mm_y5mm = new TGraphErrors(10,modulesID,diffSLX_x0mm_y5mm,0,errdiffSLX_x0mm_y5mm);
  gr_diffSLX_x0mm_y5mm->SetMarkerStyle(20);
  gr_diffSLX_x0mm_y5mm->SetMarkerColor(6);
  gr_diffSLX_x0mm_y5mm->SetMarkerSize(1.5);
  TGraphErrors *modSLY_x0mm_y5mm = (TGraphErrors*)fin_x0mm_y5mm->Get("modSLY");
  modSLY_x0mm_y5mm->SetMarkerColor(6);
  Double_t *SLY_x0mm_y5mm; Double_t *errSLY_x0mm_y5mm;
  SLY_x0mm_y5mm = modSLY_x0mm_y5mm->GetY();
  errSLY_x0mm_y5mm = modSLY_x0mm_y5mm->GetEY();
  double diffSLY_x0mm_y5mm[10];   double errdiffSLY_x0mm_y5mm[10];
  for(int i=0;i<modSLY_ideal->GetN();i++){
    diffSLY_x0mm_y5mm[i] = SLY_x0mm_y5mm[i] - SLY_ideal[i];
    errdiffSLY_x0mm_y5mm[i] = 0.5*sqrt(errSLY_ideal[i]*errSLY_ideal[i]+errSLY_x0mm_y5mm[i]*errSLY_x0mm_y5mm[i]);
  }
  TGraphErrors *gr_diffSLY_x0mm_y5mm = new TGraphErrors(10,modulesID,diffSLY_x0mm_y5mm,0,errdiffSLY_x0mm_y5mm);
  gr_diffSLY_x0mm_y5mm->SetMarkerStyle(20);
  gr_diffSLY_x0mm_y5mm->SetMarkerColor(6);
  gr_diffSLY_x0mm_y5mm->SetMarkerSize(1.5);


  //x=-5,y=0
 TFile *fin_xmn5mm_y0mm = new TFile("testAssym_x_-5mm_y_0mm/DPM/mom_1_5/HitsAssym_x_y_1_5GeV.root","READ");
  TGraphErrors *modX0_xmn5mm_y0mm = (TGraphErrors*)fin_xmn5mm_y0mm->Get("modX0");
  modX0_xmn5mm_y0mm->SetMarkerColor(7);
  Double_t *X0_xmn5mm_y0mm; Double_t *errX0_xmn5mm_y0mm;
  X0_xmn5mm_y0mm = modX0_xmn5mm_y0mm->GetY();
  errX0_xmn5mm_y0mm = modX0_xmn5mm_y0mm->GetEY();
  double diffX0_xmn5mm_y0mm[10];   double errdiffX0_xmn5mm_y0mm[10];
  for(int i=0;i<modX0_ideal->GetN();i++){
    diffX0_xmn5mm_y0mm[i] = X0_xmn5mm_y0mm[i] - X0_ideal[i];
    errdiffX0_xmn5mm_y0mm[i] = 0.5*sqrt(errX0_ideal[i]*errX0_ideal[i]+errX0_xmn5mm_y0mm[i]*errX0_xmn5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffX0_xmn5mm_y0mm = new TGraphErrors(10,modulesID,diffX0_xmn5mm_y0mm,0,errdiffX0_xmn5mm_y0mm);
  gr_diffX0_xmn5mm_y0mm->SetMarkerStyle(20);
  gr_diffX0_xmn5mm_y0mm->SetMarkerColor(7);
  gr_diffX0_xmn5mm_y0mm->SetMarkerSize(1.5);
  TGraphErrors *modY0_xmn5mm_y0mm = (TGraphErrors*)fin_xmn5mm_y0mm->Get("modY0");
  modY0_xmn5mm_y0mm->SetMarkerColor(7);
  Double_t *Y0_xmn5mm_y0mm; Double_t *errY0_xmn5mm_y0mm;
  Y0_xmn5mm_y0mm = modY0_xmn5mm_y0mm->GetY();
  errY0_xmn5mm_y0mm = modY0_xmn5mm_y0mm->GetEY();
  double diffY0_xmn5mm_y0mm[10];   double errdiffY0_xmn5mm_y0mm[10];
  for(int i=0;i<modY0_ideal->GetN();i++){
    diffY0_xmn5mm_y0mm[i] = Y0_xmn5mm_y0mm[i] - Y0_ideal[i];
    errdiffY0_xmn5mm_y0mm[i] = 0.5*sqrt(errY0_ideal[i]*errY0_ideal[i]+errY0_xmn5mm_y0mm[i]*errY0_xmn5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffY0_xmn5mm_y0mm = new TGraphErrors(10,modulesID,diffY0_xmn5mm_y0mm,0,errdiffY0_xmn5mm_y0mm);
  gr_diffY0_xmn5mm_y0mm->SetMarkerStyle(20);
  gr_diffY0_xmn5mm_y0mm->SetMarkerColor(7);
  gr_diffY0_xmn5mm_y0mm->SetMarkerSize(1.5);
  TGraphErrors *modSLX_xmn5mm_y0mm = (TGraphErrors*)fin_xmn5mm_y0mm->Get("modSLX");
  modSLX_xmn5mm_y0mm->SetMarkerColor(7);
  Double_t *SLX_xmn5mm_y0mm; Double_t *errSLX_xmn5mm_y0mm;
  SLX_xmn5mm_y0mm = modSLX_xmn5mm_y0mm->GetY();
  errSLX_xmn5mm_y0mm = modSLX_xmn5mm_y0mm->GetEY();

  double diffSLX_xmn5mm_y0mm[10];   double errdiffSLX_xmn5mm_y0mm[10];
  for(int i=0;i<modSLX_ideal->GetN();i++){
    diffSLX_xmn5mm_y0mm[i] = SLX_xmn5mm_y0mm[i] - SLX_ideal[i];
    errdiffSLX_xmn5mm_y0mm[i] = 0.5*sqrt(errSLX_ideal[i]*errSLX_ideal[i]+errSLX_xmn5mm_y0mm[i]*errSLX_xmn5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffSLX_xmn5mm_y0mm = new TGraphErrors(10,modulesID,diffSLX_xmn5mm_y0mm,0,errdiffSLX_xmn5mm_y0mm);
  gr_diffSLX_xmn5mm_y0mm->SetMarkerStyle(20);
  gr_diffSLX_xmn5mm_y0mm->SetMarkerColor(7);
  gr_diffSLX_xmn5mm_y0mm->SetMarkerSize(1.5);
  TGraphErrors *modSLY_xmn5mm_y0mm = (TGraphErrors*)fin_xmn5mm_y0mm->Get("modSLY");
  modSLY_xmn5mm_y0mm->SetMarkerColor(7);
  Double_t *SLY_xmn5mm_y0mm; Double_t *errSLY_xmn5mm_y0mm;
  SLY_xmn5mm_y0mm = modSLY_xmn5mm_y0mm->GetY();
  errSLY_xmn5mm_y0mm = modSLY_xmn5mm_y0mm->GetEY();
  double diffSLY_xmn5mm_y0mm[10];   double errdiffSLY_xmn5mm_y0mm[10];
  for(int i=0;i<modSLY_ideal->GetN();i++){
    diffSLY_xmn5mm_y0mm[i] = SLY_xmn5mm_y0mm[i] - SLY_ideal[i];
    errdiffSLY_xmn5mm_y0mm[i] = 0.5*sqrt(errSLY_ideal[i]*errSLY_ideal[i]+errSLY_xmn5mm_y0mm[i]*errSLY_xmn5mm_y0mm[i]);
  }
  TGraphErrors *gr_diffSLY_xmn5mm_y0mm = new TGraphErrors(10,modulesID,diffSLY_xmn5mm_y0mm,0,errdiffSLY_xmn5mm_y0mm);
  gr_diffSLY_xmn5mm_y0mm->SetMarkerStyle(20);
  gr_diffSLY_xmn5mm_y0mm->SetMarkerColor(7);
  gr_diffSLY_xmn5mm_y0mm->SetMarkerSize(1.5);

  //x=0,y=-5
TFile *fin_x0mm_ymn5mm = new TFile("testAssym_x_-5mm_y_0mm/DPM/mom_1_5/HitsAssym_x_y_1_5GeV.root","READ");
  TGraphErrors *modX0_x0mm_ymn5mm = (TGraphErrors*)fin_x0mm_ymn5mm->Get("modX0");
  modX0_x0mm_ymn5mm->SetMarkerColor(8);
  Double_t *X0_x0mm_ymn5mm; Double_t *errX0_x0mm_ymn5mm;
  X0_x0mm_ymn5mm = modX0_x0mm_ymn5mm->GetY();
  errX0_x0mm_ymn5mm = modX0_x0mm_ymn5mm->GetEY();
  double diffX0_x0mm_ymn5mm[10];   double errdiffX0_x0mm_ymn5mm[10];
  for(int i=0;i<modX0_ideal->GetN();i++){
    diffX0_x0mm_ymn5mm[i] = X0_x0mm_ymn5mm[i] - X0_ideal[i];
    errdiffX0_x0mm_ymn5mm[i] = 0.5*sqrt(errX0_ideal[i]*errX0_ideal[i]+errX0_x0mm_ymn5mm[i]*errX0_x0mm_ymn5mm[i]);
  }
  TGraphErrors *gr_diffX0_x0mm_ymn5mm = new TGraphErrors(10,modulesID,diffX0_x0mm_ymn5mm,0,errdiffX0_x0mm_ymn5mm);
  gr_diffX0_x0mm_ymn5mm->SetMarkerStyle(20);
  gr_diffX0_x0mm_ymn5mm->SetMarkerColor(8);
  gr_diffX0_x0mm_ymn5mm->SetMarkerSize(1.5);
  TGraphErrors *modY0_x0mm_ymn5mm = (TGraphErrors*)fin_x0mm_ymn5mm->Get("modY0");
  modY0_x0mm_ymn5mm->SetMarkerColor(8);
  Double_t *Y0_x0mm_ymn5mm; Double_t *errY0_x0mm_ymn5mm;
  Y0_x0mm_ymn5mm = modY0_x0mm_ymn5mm->GetY();
  errY0_x0mm_ymn5mm = modY0_x0mm_ymn5mm->GetEY();
  double diffY0_x0mm_ymn5mm[10];   double errdiffY0_x0mm_ymn5mm[10];
  for(int i=0;i<modY0_ideal->GetN();i++){
    diffY0_x0mm_ymn5mm[i] = Y0_x0mm_ymn5mm[i] - Y0_ideal[i];
    errdiffY0_x0mm_ymn5mm[i] = 0.5*sqrt(errY0_ideal[i]*errY0_ideal[i]+errY0_x0mm_ymn5mm[i]*errY0_x0mm_ymn5mm[i]);
  }
  TGraphErrors *gr_diffY0_x0mm_ymn5mm = new TGraphErrors(10,modulesID,diffY0_x0mm_ymn5mm,0,errdiffY0_x0mm_ymn5mm);
  gr_diffY0_x0mm_ymn5mm->SetMarkerStyle(20);
  gr_diffY0_x0mm_ymn5mm->SetMarkerColor(8);
  gr_diffY0_x0mm_ymn5mm->SetMarkerSize(1.5);
  TGraphErrors *modSLX_x0mm_ymn5mm = (TGraphErrors*)fin_x0mm_ymn5mm->Get("modSLX");
  modSLX_x0mm_ymn5mm->SetMarkerColor(8);
  Double_t *SLX_x0mm_ymn5mm; Double_t *errSLX_x0mm_ymn5mm;
  SLX_x0mm_ymn5mm = modSLX_x0mm_ymn5mm->GetY();
  errSLX_x0mm_ymn5mm = modSLX_x0mm_ymn5mm->GetEY();

  double diffSLX_x0mm_ymn5mm[10];   double errdiffSLX_x0mm_ymn5mm[10];
  for(int i=0;i<modSLX_ideal->GetN();i++){
    diffSLX_x0mm_ymn5mm[i] = SLX_x0mm_ymn5mm[i] - SLX_ideal[i];
    errdiffSLX_x0mm_ymn5mm[i] = 0.5*sqrt(errSLX_ideal[i]*errSLX_ideal[i]+errSLX_x0mm_ymn5mm[i]*errSLX_x0mm_ymn5mm[i]);
  }
  TGraphErrors *gr_diffSLX_x0mm_ymn5mm = new TGraphErrors(10,modulesID,diffSLX_x0mm_ymn5mm,0,errdiffSLX_x0mm_ymn5mm);
  gr_diffSLX_x0mm_ymn5mm->SetMarkerStyle(20);
  gr_diffSLX_x0mm_ymn5mm->SetMarkerColor(8);
  gr_diffSLX_x0mm_ymn5mm->SetMarkerSize(1.5);
  TGraphErrors *modSLY_x0mm_ymn5mm = (TGraphErrors*)fin_x0mm_ymn5mm->Get("modSLY");
  modSLY_x0mm_ymn5mm->SetMarkerColor(8);
  Double_t *SLY_x0mm_ymn5mm; Double_t *errSLY_x0mm_ymn5mm;
  SLY_x0mm_ymn5mm = modSLY_x0mm_ymn5mm->GetY();
  errSLY_x0mm_ymn5mm = modSLY_x0mm_ymn5mm->GetEY();
  double diffSLY_x0mm_ymn5mm[10];   double errdiffSLY_x0mm_ymn5mm[10];
  for(int i=0;i<modSLY_ideal->GetN();i++){
    diffSLY_x0mm_ymn5mm[i] = SLY_x0mm_ymn5mm[i] - SLY_ideal[i];
    errdiffSLY_x0mm_ymn5mm[i] = 0.5*sqrt(errSLY_ideal[i]*errSLY_ideal[i]+errSLY_x0mm_ymn5mm[i]*errSLY_x0mm_ymn5mm[i]);
  }
  TGraphErrors *gr_diffSLY_x0mm_ymn5mm = new TGraphErrors(10,modulesID,diffSLY_x0mm_ymn5mm,0,errdiffSLY_x0mm_ymn5mm);
  gr_diffSLY_x0mm_ymn5mm->SetMarkerStyle(20);
  gr_diffSLY_x0mm_ymn5mm->SetMarkerColor(8);
  gr_diffSLY_x0mm_ymn5mm->SetMarkerSize(1.5);


  TLegend *leg = new TLegend(0.4,0.6,0.7,0.99);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.04);

  TCanvas *c1 = new TCanvas("Assym", "Assym", 700, 800);
  c1->Divide(2,4);
  c1->cd(1);
  TMultiGraph *mgr_X0 = new TMultiGraph();
  mgr_X0->Add(modX0_ideal);
  mgr_X0->Add(modX0_xpl5mm_y0mm);
  mgr_X0->Add(modX0_xpl5mm_y5mm);
  mgr_X0->Add(modX0_x0mm_y5mm);
  mgr_X0->Add(modX0_xmn5mm_y0mm);
  mgr_X0->Add(modX0_x0mm_ymn5mm);
  leg->AddEntry(modX0_ideal,"(X_{IP},Y_{IP}) = (0,0) mm","p");	  
  leg->AddEntry(modX0_xpl5mm_y0mm,"(X_{IP},Y_{IP}) = (5,0) mm","p");	  
  leg->AddEntry(modX0_x0mm_y5mm,"(X_{IP},Y_{IP}) = (0,5) mm","p");	  
  leg->AddEntry(modX0_xpl5mm_y5mm,"(X_{IP},Y_{IP}) = (5,5) mm","p");	  
  leg->AddEntry(modX0_xmn5mm_y0mm,"(X_{IP},Y_{IP}) = (-5,0) mm","p");	  
  leg->AddEntry(modX0_x0mm_ymn5mm,"(X_{IP},Y_{IP}) = (0,-5) mm","p");	  
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
 mgr_diffX0->Draw("AP");
 mgr_diffX0->GetXaxis()->SetTitle("sector");
 mgr_diffX0->GetYaxis()->SetTitle("#Delta X0, cm");
 c1->cd(3);
  TMultiGraph *mgr_Y0 = new TMultiGraph();
  mgr_Y0->Add(modY0_ideal);
  mgr_Y0->Add(modY0_xpl5mm_y0mm);
  mgr_Y0->Add(modY0_xpl5mm_y5mm);
  mgr_Y0->Add(modY0_x0mm_y5mm);
  mgr_Y0->Add(modY0_xmn5mm_y0mm);
  mgr_Y0->Add(modY0_x0mm_ymn5mm);
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
 mgr_diffY0->Draw("AP");
 mgr_diffY0->GetXaxis()->SetTitle("sector");
 mgr_diffY0->GetYaxis()->SetTitle("#Delta Y0, cm");
c1->cd(5);
  TMultiGraph *mgr_SLX = new TMultiGraph();
  mgr_SLX->Add(modSLX_ideal);
  mgr_SLX->Add(modSLX_xpl5mm_y0mm);
  mgr_SLX->Add(modSLX_xpl5mm_y5mm);
  mgr_SLX->Add(modSLX_x0mm_y5mm);
  mgr_SLX->Add(modSLX_xmn5mm_y0mm);
  mgr_SLX->Add(modSLX_x0mm_ymn5mm);
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
 mgr_diffSLX->Draw("AP");
 mgr_diffSLX->GetXaxis()->SetTitle("sector");
 mgr_diffSLX->GetYaxis()->SetTitle("#Delta slope X, cm");
 c1->cd(7);
  TMultiGraph *mgr_SLY = new TMultiGraph();
  mgr_SLY->Add(modSLY_ideal);
  mgr_SLY->Add(modSLY_xpl5mm_y0mm);
  mgr_SLY->Add(modSLY_xpl5mm_y5mm);
  mgr_SLY->Add(modSLY_x0mm_y5mm);
  mgr_SLY->Add(modSLY_xmn5mm_y0mm);
  mgr_SLY->Add(modSLY_x0mm_ymn5mm);
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
 mgr_diffSLY->Draw("AP");
 mgr_diffSLY->GetXaxis()->SetTitle("sector");
 mgr_diffSLY->GetYaxis()->SetTitle("#Delta slope Y, cm");
 c1->SaveAs("AssymRes.pdf");
 c1->SaveAs("AssymRes.root");
}
