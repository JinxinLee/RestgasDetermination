// estimate assymetry on opposite modules

void DrawAssymOpp(TString path = "/panda/myResults/AssymIP/DPM/mom_1_5/", TString file_name="/HitsAssym_x_y_1_5GeV.root", int x_sh = -5, int y_sh = -5,int angx_sh = -5, int angy_sh = -5){
  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/tests/Assym_IP/rootlogon.C");
 const int Nmod = 20;
 const int Nmod_2 = 0.5*Nmod;

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
  double N_ideal_norm = N_ideal[0];
 double Nass_ideal[Nmod_2]; double errNass_ideal[Nmod_2];
 double X0ass_ideal[Nmod_2];   double errX0ass_ideal[Nmod_2];
 double Y0ass_ideal[Nmod_2];   double errY0ass_ideal[Nmod_2];
 double SLXass_ideal[Nmod_2];   double errSLXass_ideal[Nmod_2];
 double SLYass_ideal[Nmod_2];   double errSLYass_ideal[Nmod_2];
 // for(int i=0;i<Nmod;i++){
 //   for(int j=0;j<Nmod;j++){
 //     if(i!=j)
 //       if(fabs(N_ideal[i]-N_ideal[j])<1e3) cout<<"N_ideal["<<i<<"] - N_ideal["<<j<<"] = "<<N_ideal[i]-N_ideal[j]<<endl;
 //   }
 //   cout<<" "<<endl;
 // }
     //   cout<<"N_ideal["<<i<<"] = "<<N_ideal[i]<<endl;
  for(int i=0;i<Nmod_2 ;i++){
    // N_ideal[i] *= 1/N_ideal_norm;
    // N_ideal[i+10] *= 1/N_ideal_norm;
    Nass_ideal[i] = (N_ideal[i]-N_ideal[i+10])/(N_ideal[i]+N_ideal[i+10]);
    errNass_ideal[i] = sqrt(TMath::Power(errN_ideal[i]*2*N_ideal[i+10],2)+TMath::Power(errN_ideal[i+10]*2*N_ideal[i],2))/TMath::Power(N_ideal[i]+N_ideal[i+10],2);
    //Nass_ideal[i] = (N_ideal[i]-N_ideal[i+10]);
  //  cout<<"Nass_ideal["<<i<<"] = "<<Nass_ideal[i]<<" N_ideal[i] "<<N_ideal[i]<<" N_ideal[i+10] "<<N_ideal[i+10]<<endl;
    X0_ideal[i] = fabs(X0_ideal[i]);
    Y0_ideal[i] = fabs(Y0_ideal[i]);
    X0_ideal[i+10] = fabs(X0_ideal[i+10]);
    Y0_ideal[i+10] = fabs(Y0_ideal[i+10]);
    SLX_ideal[i] = fabs(SLX_ideal[i]);
    SLY_ideal[i] = fabs(SLY_ideal[i]);
    SLX_ideal[i+10] = fabs(SLX_ideal[i+10]);
    SLY_ideal[i+10] = fabs(SLY_ideal[i+10]);
    X0ass_ideal[i] = (X0_ideal[i]-X0_ideal[i+10])/(X0_ideal[i]+X0_ideal[i+10]);
    errX0ass_ideal[i] = sqrt(TMath::Power(errX0_ideal[i]*2*X0_ideal[i+10],2)+TMath::Power(errX0_ideal[i+10]*2*X0_ideal[i],2))/TMath::Power(X0_ideal[i]+X0_ideal[i+10],2);
    Y0ass_ideal[i] = (Y0_ideal[i]-Y0_ideal[i+10])/(Y0_ideal[i]+Y0_ideal[i+10]);
    errY0ass_ideal[i] = sqrt(TMath::Power(errY0_ideal[i]*2*Y0_ideal[i+10],2) +TMath::Power(errY0_ideal[i+10]*2*Y0_ideal[i],2))/TMath::Power(Y0_ideal[i]+Y0_ideal[i+10],2);
    SLXass_ideal[i] = (SLX_ideal[i]-SLX_ideal[i+10])/(SLX_ideal[i]+SLX_ideal[i+10]);
    errSLXass_ideal[i] = sqrt(TMath::Power(errSLX_ideal[i]*2*SLX_ideal[i+10],2)+TMath::Power(errSLX_ideal[i+10]*2*SLX_ideal[i],2))/TMath::Power(SLX_ideal[i]+SLX_ideal[i+10],2);
    SLYass_ideal[i] = (SLY_ideal[i]-SLY_ideal[i+10])/(SLY_ideal[i]+SLY_ideal[i+10]);
    errSLYass_ideal[i] = sqrt(TMath::Power(errSLY_ideal[i]*2*SLY_ideal[i+10],2) +TMath::Power(errSLY_ideal[i+10]*2*SLY_ideal[i],2))/TMath::Power(SLY_ideal[i]+SLY_ideal[i+10],2);
  }

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

  double N_shifted_norm = N_SHIFT[0];
  
 

 
  TGraphErrors *modX0_SHIFT = (TGraphErrors*)fin_SHIFT->Get("modX0");
  modX0_SHIFT->SetMarkerColor(4);
  modX0_SHIFT->SetMarkerSize(1.0);
  Double_t *X0_SHIFT; Double_t *errX0_SHIFT;
  X0_SHIFT = modX0_SHIFT->GetY();
  errX0_SHIFT = modX0_SHIFT->GetEY();

  TGraphErrors *modY0_SHIFT = (TGraphErrors*)fin_SHIFT->Get("modY0");
  modY0_SHIFT->SetMarkerColor(4);
  modY0_SHIFT->SetMarkerSize(1.0);
  Double_t *Y0_SHIFT; Double_t *errY0_SHIFT;
  Y0_SHIFT = modY0_SHIFT->GetY();
  errY0_SHIFT = modY0_SHIFT->GetEY();
  
  TGraphErrors *modSLX_SHIFT = (TGraphErrors*)fin_SHIFT->Get("modSLX");
  modSLX_SHIFT->SetMarkerColor(4);
  modSLX_SHIFT->SetMarkerSize(1.0);
  Double_t *SLX_SHIFT; Double_t *errSLX_SHIFT;
  SLX_SHIFT = modSLX_SHIFT->GetY();
  errSLX_SHIFT = modSLX_SHIFT->GetEY();

  TGraphErrors *modSLY_SHIFT = (TGraphErrors*)fin_SHIFT->Get("modSLY");
  modSLY_SHIFT->SetMarkerColor(4);
  modSLY_SHIFT->SetMarkerSize(1.0);
  Double_t *SLY_SHIFT; Double_t *errSLY_SHIFT;
  SLY_SHIFT = modSLY_SHIFT->GetY();
  errSLY_SHIFT = modSLY_SHIFT->GetEY();

  double Nass_SHIFT[Nmod_2];
  double errNass_SHIFT[Nmod_2]; 
  double X0ass_SHIFT[Nmod_2];   double errX0ass_SHIFT[Nmod_2];
  double Y0ass_SHIFT[Nmod_2];   double errY0ass_SHIFT[Nmod_2];
  double SLXass_SHIFT[Nmod_2];   double errSLXass_SHIFT[Nmod_2];
  double SLYass_SHIFT[Nmod_2];   double errSLYass_SHIFT[Nmod_2];
  for(int i=0;i<Nmod_2 ;i++){
    // N_SHIFT[i] *= 1./N_shifted_norm;
    // N_SHIFT[i+10] *= 1./N_shifted_norm;
    Nass_SHIFT[i] = (N_SHIFT[i]-N_SHIFT[i+10])/(N_SHIFT[i]+N_SHIFT[i+10]);
    errNass_SHIFT[i] = sqrt(TMath::Power(errN_SHIFT[i]*2*N_SHIFT[i+10],2)+TMath::Power(errN_SHIFT[i+10]*2*N_SHIFT[i],2))/TMath::Power(N_SHIFT[i]+N_SHIFT[i+10],2);
    //    Nass_SHIFT[i] = (N_SHIFT[i]-N_SHIFT[i+10]);

    X0_SHIFT[i] = fabs(X0_SHIFT[i]);
    Y0_SHIFT[i] = fabs(Y0_SHIFT[i]);
    X0_SHIFT[i+10] = fabs(X0_SHIFT[i+10]);
    Y0_SHIFT[i+10] = fabs(Y0_SHIFT[i+10]);
    X0ass_SHIFT[i] = (X0_SHIFT[i]-X0_SHIFT[i+10])/(X0_SHIFT[i]+X0_SHIFT[i+10]);
    errX0ass_SHIFT[i] = sqrt(TMath::Power(errX0_SHIFT[i]*2*X0_SHIFT[i+10],2)+TMath::Power(errX0_SHIFT[i+10]*2*X0_SHIFT[i],2))/TMath::Power(X0_SHIFT[i]+X0_SHIFT[i+10],2);
    cout<<" errX0ass_SHIFT["<<i<<"] = "<<errX0ass_SHIFT[i]<<" errX0_SHIFT["<<i<<"] = "<<errX0_SHIFT[i]<<" "<<TMath::Power(errX0_SHIFT[i]*2*X0_SHIFT[i+10],2)<<" "<<TMath::Power(errX0_SHIFT[i+10]*2*X0_SHIFT[i],2)<<endl;
    Y0ass_SHIFT[i] = (Y0_SHIFT[i]-Y0_SHIFT[i+10])/(Y0_SHIFT[i]+Y0_SHIFT[i+10]);
    errY0ass_SHIFT[i] = sqrt(TMath::Power(errY0_SHIFT[i]*2*Y0_SHIFT[i+10],2) +TMath::Power(errY0_SHIFT[i+10]*2*Y0_SHIFT[i],2))/TMath::Power(Y0_SHIFT[i]+Y0_SHIFT[i+10],2);
    SLX_SHIFT[i] = fabs(SLX_SHIFT[i]);
    SLY_SHIFT[i] = fabs(SLY_SHIFT[i]);
    SLX_SHIFT[i+10] = fabs(SLX_SHIFT[i+10]);
    SLY_SHIFT[i+10] = fabs(SLY_SHIFT[i+10]);
    SLXass_SHIFT[i] = (SLX_SHIFT[i]-SLX_SHIFT[i+10])/(SLX_SHIFT[i]+SLX_SHIFT[i+10]);
    errSLXass_SHIFT[i] = sqrt(TMath::Power(errSLX_SHIFT[i]*2*SLX_SHIFT[i+10],2)+TMath::Power(errSLX_SHIFT[i+10]*2*SLX_SHIFT[i],2))/TMath::Power(SLX_SHIFT[i]+SLX_SHIFT[i+10],2);
    SLYass_SHIFT[i] = (SLY_SHIFT[i]-SLY_SHIFT[i+10])/(SLY_SHIFT[i]+SLY_SHIFT[i+10]);
    errSLYass_SHIFT[i] = sqrt(TMath::Power(errSLY_SHIFT[i]*2*SLY_SHIFT[i+10],2) +TMath::Power(errSLY_SHIFT[i+10]*2*SLY_SHIFT[i],2))/TMath::Power(SLY_SHIFT[i]+SLY_SHIFT[i+10],2);
  }
  //[END] read Shifted parameters ---------------------------------------
TGraphErrors *gr_assN_SHIFT = new TGraphErrors(Nmod_2,modulesID,Nass_SHIFT,0,errNass_SHIFT);
 gr_assN_SHIFT->SetName("Nass_shift");
  gr_assN_SHIFT->SetMarkerStyle(20);
  gr_assN_SHIFT->SetMarkerColor(4);
  gr_assN_SHIFT->SetLineColor(4);
  gr_assN_SHIFT->SetLineWidth(1.5);
  gr_assN_SHIFT->SetMarkerSize(1.0);

  TGraphErrors *gr_assN_ideal = new TGraphErrors(Nmod_2,modulesID,Nass_ideal,0,errNass_ideal);
  gr_assN_ideal->SetName("Nass_ideal");
  gr_assN_ideal->SetMarkerStyle(20);
  gr_assN_ideal->SetMarkerColor(2);
  gr_assN_ideal->SetLineColor(2);
  gr_assN_ideal->SetLineWidth(1.5);
  gr_assN_ideal->SetMarkerSize(1.0);

   TGraphErrors *gr_X0ass_ideal =  new TGraphErrors(Nmod_2,modulesID,X0ass_ideal,0,errX0ass_ideal);
  gr_X0ass_ideal->SetName("X0ass_ideal");
  gr_X0ass_ideal->SetMarkerStyle(20);
  gr_X0ass_ideal->SetMarkerColor(2);
  gr_X0ass_ideal->SetLineColor(2);
  gr_X0ass_ideal->SetLineWidth(1.5);
  gr_X0ass_ideal->SetMarkerSize(1.0);
  TGraphErrors *gr_Y0ass_ideal =  new TGraphErrors(Nmod_2,modulesID,Y0ass_ideal,0,errY0ass_ideal);
  gr_Y0ass_ideal->SetName("Y0ass_ideal");
  gr_Y0ass_ideal->SetMarkerStyle(20);
  gr_Y0ass_ideal->SetMarkerColor(4);
  gr_Y0ass_ideal->SetLineColor(4);
  gr_Y0ass_ideal->SetLineWidth(1.5);
  gr_Y0ass_ideal->SetMarkerSize(1.0);
 TGraphErrors *gr_X0ass_SHIFT =  new TGraphErrors(Nmod_2,modulesID,X0ass_SHIFT,0,errX0ass_SHIFT);
  gr_X0ass_SHIFT->SetName("X0ass_SHIFT");
  gr_X0ass_SHIFT->SetMarkerStyle(20);
  gr_X0ass_SHIFT->SetMarkerColor(4);
  gr_X0ass_SHIFT->SetLineColor(4);
  gr_X0ass_SHIFT->SetLineWidth(1.5);
  gr_X0ass_SHIFT->SetMarkerSize(1.0);
  TGraphErrors *gr_Y0ass_SHIFT =  new TGraphErrors(Nmod_2,modulesID,Y0ass_SHIFT,0,errY0ass_SHIFT);
 gr_Y0ass_SHIFT->SetName("Y0ass_SHIFT");
  gr_Y0ass_SHIFT->SetMarkerStyle(20);
  gr_Y0ass_SHIFT->SetMarkerColor(4);
  gr_Y0ass_SHIFT->SetLineColor(4);
  gr_Y0ass_SHIFT->SetLineWidth(1.5);
  gr_Y0ass_SHIFT->SetMarkerSize(1.0);

 TGraphErrors *gr_SLXass_ideal =  new TGraphErrors(Nmod_2,modulesID,SLXass_ideal,0,errSLXass_ideal);
  gr_SLXass_ideal->SetName("SLXass_ideal");
  gr_SLXass_ideal->SetMarkerStyle(20);
  gr_SLXass_ideal->SetMarkerColor(2);
  gr_SLXass_ideal->SetLineColor(2);
  gr_SLXass_ideal->SetLineWidth(1.5);
  gr_SLXass_ideal->SetMarkerSize(1.0);
  TGraphErrors *gr_SLYass_ideal =  new TGraphErrors(Nmod_2,modulesID,SLYass_ideal,0,errSLYass_ideal);
  gr_SLYass_ideal->SetName("SLYass_ideal");
  gr_SLYass_ideal->SetMarkerStyle(20);
  gr_SLYass_ideal->SetMarkerColor(4);
  gr_SLYass_ideal->SetLineColor(4);
  gr_SLYass_ideal->SetLineWidth(1.5);
  gr_SLYass_ideal->SetMarkerSize(1.0);
 TGraphErrors *gr_SLXass_SHIFT =  new TGraphErrors(Nmod_2,modulesID,SLXass_SHIFT,0,errSLXass_SHIFT);
  gr_SLXass_SHIFT->SetName("SLXass_SHIFT");
  gr_SLXass_SHIFT->SetMarkerStyle(20);
  gr_SLXass_SHIFT->SetMarkerColor(4);
  gr_SLXass_SHIFT->SetLineColor(4);
  gr_SLXass_SHIFT->SetLineWidth(1.5);
  gr_SLXass_SHIFT->SetMarkerSize(1.0);
  TGraphErrors *gr_SLYass_SHIFT =  new TGraphErrors(Nmod_2,modulesID,SLYass_SHIFT,0,errSLYass_SHIFT);
 gr_SLYass_SHIFT->SetName("SLYass_SHIFT");
  gr_SLYass_SHIFT->SetMarkerStyle(20);
  gr_SLYass_SHIFT->SetMarkerColor(4);
  gr_SLYass_SHIFT->SetLineColor(4);
  gr_SLYass_SHIFT->SetLineWidth(1.5);
  gr_SLYass_SHIFT->SetMarkerSize(1.0);


  TLegend *leg = new TLegend(0.64,0.8,0.99,0.98);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.03);
  leg->SetHeader("(X,Y)_{POS} [mm]; (X,Y)_{TILT} [mrad]");
 TLegend *leg2 = new TLegend(0.64,0.8,0.98,0.98);
  leg2->SetFillColor(0);
  leg2->SetTextFont(42);
  leg2->SetTextSize(0.055);
  leg2->SetHeader("(X,Y)_{POS} [mm]; (X,Y)_{TILT} [mrad]");
  TCanvas *c1 = new TCanvas("Assym", "Assym", 700, 900);
  c1->Divide(2,3);
  c1->cd(1);
  TMultiGraph *mgr_X0 = new TMultiGraph();
   mgr_X0->Add(gr_X0ass_ideal);
  mgr_X0->Add(gr_X0ass_SHIFT);
 
  TString text_sh = "(";
  text_sh +=x_sh;
  text_sh +=",";
  text_sh +=y_sh;
  text_sh += "); (";
  text_sh += angx_sh;
  text_sh +=",";
  text_sh += angy_sh;
  text_sh += ")";
  leg->AddEntry(gr_X0ass_ideal,"(0,0); (0,0)","p");	  
  leg->AddEntry(gr_X0ass_SHIFT,text_sh,"p");	  
  leg2->AddEntry(gr_X0ass_SHIFT,text_sh,"p");
  mgr_X0->Draw("AP");
  leg->Draw();
  mgr_X0->GetXaxis()->SetTitle("sector");
  mgr_X0->GetYaxis()->SetTitle("(X0_{1}-X0_{2})/(X0_{1}+X0_{2})");
  c1->cd(2);
   TMultiGraph *mgr_Y0 = new TMultiGraph();
   mgr_Y0->Add(gr_Y0ass_ideal);
   mgr_Y0->Add(gr_Y0ass_SHIFT);
  mgr_Y0->Draw("AP");
  mgr_Y0->GetXaxis()->SetTitle("sector");
  mgr_Y0->GetYaxis()->SetTitle("(Y0_{1}-Y0_{2})/(Y0_{1}+Y0_{2})");
 c1->cd(3);
  TMultiGraph *mgr_SLX = new TMultiGraph();
  mgr_SLX->Add(gr_SLXass_ideal);
  mgr_SLX->Add(gr_SLXass_SHIFT);
  mgr_SLX->Draw("AP");
  mgr_SLX->GetXaxis()->SetTitle("sector");
  mgr_SLX->GetYaxis()->SetTitle("(slope X_{1} - slope X_{2})/(slope X_{1} + slope X_{2})");
  c1->cd(4);
  TMultiGraph *mgr_SLY = new TMultiGraph();
  mgr_SLY->Add(gr_SLYass_ideal);
  mgr_SLY->Add(gr_SLYass_SHIFT);
  mgr_SLY->Draw("AP");
  mgr_SLY->GetXaxis()->SetTitle("sector");
  mgr_SLY->GetYaxis()->SetTitle("(slope Y_{1} - slope Y_{2})/(slope Y_{1} + slope Y_{2})");
  c1->cd(5);
  TMultiGraph *mgr_N = new TMultiGraph();
  mgr_N->Add(gr_assN_ideal);
  mgr_N->Add(gr_assN_SHIFT);
  mgr_N->Draw("AP");
  mgr_N->GetXaxis()->SetTitle("sector");
  mgr_N->GetYaxis()->SetTitle("(N_{i}-N_{j})/(N_{i}+N_{j})");
  mgr_N->GetXaxis()->SetRangeUser(0,19);
  leg->Draw();
  
 TString fout_name = path;
 fout_name +="/AssymOppRes_";
 fout_name += this_name;
 TString fout_name_pdf = fout_name;
 fout_name_pdf  +=".pdf";
TString fout_name_root = fout_name;
 fout_name_root  +=".root";
 c1->SaveAs(fout_name_pdf);
 TFile *finOUT = new TFile(fout_name_root,"RECREATE");
 // mgr_X0->Write();
 // modX0_SHIFT->Write();
 // gr_diffX0_SHIFT->Write();
 // modY0_SHIFT->Write();
 // gr_diffY0_SHIFT->Write();
 // modSLX_SHIFT->Write();
 // gr_diffSLX_SHIFT->Write();
 // modSLY_SHIFT->Write();
 // gr_diffSLY_SHIFT->Write();
 // modN_SHIFT->Write();
 // gr_diffN_SHIFT->Write();
 // gr_assN_ideal->Write();
 gr_assN_SHIFT->Write();
 gr_X0ass_SHIFT->Write();
 gr_Y0ass_SHIFT->Write();
 gr_SLXass_SHIFT->Write();
 gr_SLYass_SHIFT->Write();
 leg->Write();
 finOUT->Write();
}
