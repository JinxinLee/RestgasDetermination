

#include <string>
#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2D.h"
#include "TMath.h"
#include "TPaletteAxis.h"
#include "TStyle.h"
#include <iostream>



#include <stdio.h>
#include <vector>

using namespace std;

TCanvas * c1 = new TCanvas("c1", "c1", 1100, 1100);
TH1D* histo(TH2* h2, const char* name = "histo", double min=0, double max=-1);
TH2D* plot(TH2D* h2, TH2D* h2S, TCanvas* canvas,
           int min=0, double fact=-1,
           const char* Xlabel="", const char* Ylabel="", const char* Title="",
           bool Xlog=false, bool Ylog=false, bool Zlog=false,
           double rmin=-1, double rmax=-1,
           const char* filename="",
           TH2D* hmass = NULL, TH2D* hmass_stat = NULL);
TH2D*  divider(TH2D* H3, TH2D* H3S, Int_t t3,
               TH2D* H4, TH2D* H4S, Int_t t4,
               TCanvas* canvas,
               const char* Xlabel="", const char* Ylabel="", const char* Title="",
               bool Xlog=false, bool Ylog=false, bool Zlog=false,
               double rmin=-1, double rmax=-1,
               const char* filename="");
TH2D* Cut(TH2D* H4, TH2D* H4R, const char* name, double thrs, bool larger=true);
TH1D* distro(TH2D* histo, double min = 0, double max = 0);
void Quater(std::vector<TH2D*>& HV);


TH2D* plot(TH2D* h2, TH2D* h2S, TCanvas* canvas,
           int min, double fact,
           const char* Xlabel, const char* Ylabel, const char* Title,
           bool Xlog, bool Ylog, bool Zlog,
           double rmin, double rmax,
           const char* filename,
           TH2D* hmass, TH2D* hmass_stat){

  bool masscorrect = false;
  TH2D* massratio = NULL;
  if((hmass != NULL) && (hmass_stat != NULL)){
    masscorrect = true;
    massratio = (TH2D*) hmass->Clone("massratio");
    massratio->Divide(hmass_stat);
  }
  
  cout << h2->GetName() << endl;
  canvas->cd();

  char cc[64];
  sprintf(cc, "%s_temp", h2->GetTitle());
  TH2D* h2t = (TH2D*)h2->Clone(cc);
  h2t->SetNameTitle(cc, cc);
  h2t->Divide(h2S);

  TH1D* h1 = distro(h2t, -20);
  int minb = h1->GetMinimumBin();
  int maxb = h1->GetMaximumBin();
  std::cout << "Min: " << minb << ' ' << h1->GetBinCenter(minb) << std::endl;
  std::cout << "Max: " << maxb << ' ' << h1->GetBinCenter(maxb) << std::endl;

  double max = TMath::Power(10, h1->GetBinCenter(maxb));

  
  if(Xlog) canvas->SetLogx(1);
  else canvas->SetLogx(0);
  if(Ylog) canvas->SetLogy(1);
  else canvas->SetLogy(0);
  if(Zlog) canvas->SetLogz(1);
  else canvas->SetLogz(0);
  
  cout << h2->GetMinimum(0) << ' ' << h2->GetMaximum() << endl;
  sprintf(cc, "%s_R", h2->GetTitle());
  TH2D* h2c = new TH2D(cc, cc,
                       h2->GetNbinsX(), h2->GetXaxis()->GetBinLowEdge(1),
                       h2->GetXaxis()->GetBinUpEdge(h2->GetNbinsX()),
                       h2->GetNbinsY(), h2->GetYaxis()->GetBinLowEdge(1),
                       h2->GetYaxis()->GetBinUpEdge(h2->GetNbinsY()));



  bool massok = true;
  for(int x = 1; x <= h2S->GetXaxis()->GetNbins(); x++){

    for(int y = 1; y <= h2S->GetYaxis()->GetNbins(); y++){

      //cout<<x<<" "<<y<<" "<<h2S->GetBinContent(x, y)<<endl;
      int content = h2S->GetBinContent(x, y);
      
      if(min < content ) { 

	massok = true;

	if(masscorrect)

	  if(massratio->GetBinContent(x, y) > 1000) massok = false;

	if(massok)

	  h2c->SetBinContent(x, y, h2->GetBinContent(x, y));
      }
    }
  }


  
  h2c->Scale(fact);
  if((rmin > 0) || (rmax > 0))
    h2c->GetZaxis()->SetRangeUser(rmin, rmax);
  else
    h2c->GetZaxis()->SetRangeUser(h2c->GetMinimum(0), h2c->GetMaximum());
  h2c->SetContour(99);


  h2c->SetXTitle(Xlabel);
  h2c->SetYTitle(Ylabel);
  h2c->SetTitle(Title);


  TPaletteAxis *palette = new TPaletteAxis(h2c->GetXaxis()->GetBinCenter(h2->GetNbinsX())*1.025, 
                                           h2c->GetYaxis()->GetBinCenter(1),
                                           h2c->GetXaxis()->GetBinCenter(h2->GetNbinsX())*1.0375, 
                                           h2c->GetYaxis()->GetBinCenter(h2->GetNbinsY()),
                                           h2c);



  palette->SetLabelColor(1);
  palette->SetLabelFont(42);
  palette->SetLabelOffset(0.005);
  palette->SetLabelSize(0.035);
  palette->SetTitleOffset(1);
  palette->SetTitleSize(0.035);
  palette->SetFillColor(100);
  palette->SetFillStyle(1001);
  h2c->GetListOfFunctions()->Add(palette,"br");


  //cout<<5<<endl;

  
  //h2c->Draw("colz");
  canvas->Print(filename, "pdf");

  //cout<<6<<endl;
  
  return h2c;
}

// TH1D* histo(TH2* h2, const char* name){
TH1D* histo(TH2* h2, const char* name, double min, double max){

  double dynamics = TMath::Ceil(h2->GetMaximum());
  cout << dynamics << endl;
  double Min = min;
  double Max = max;
  if(min < 0) Min = 1;
  if(max < 0) Max = dynamics;
  TH1D* histo = new TH1D(name, name, 10*dynamics, Min, Max);
  for(int x = 1; x <= h2->GetXaxis()->GetNbins(); x++)
    for(int y = 1; y <= h2->GetXaxis()->GetNbins(); y++)
      histo->Fill(h2->GetBinContent(x, y));
  return histo;
}




int main0(const char* filename="/lustre/nyx/panda/carsten/fairsoft_nov15/Data/Geant4.15GeV/RadMap_Out_Final.root"){
  gStyle->SetOptStat(0);


  TFile* _file_g3 = new TFile(filename);
  TH2D* signal;
  TH2D* normal;
  std::vector<TH2D*> H;

  signal   = (TH2D*) _file_g3->Get("EmcFluenceXY_Histo_XY_0");
  normal   = (TH2D*) _file_g3->Get("EmcFluenceXY_StatHisto_XY_0");//200
  //1000 files was added together, in each files the quantities are normalized per evenrs
  H.push_back(plot(signal  ,normal  , c1 , 0, 1.61e14/1000.,
                   "Z [cm]", "X [cm]",
                   "Full neutron fluence (20cm wide XY slice (Z[223cm, 223cm])) []",
                   false, false, true,
                   -1, -1,
                   "figures.pdf("));
  delete signal;
  delete normal;

  Quater(H);
  
  signal   = (TH2D*) _file_g3->Get("EmcEdepXY_Histo_XY_0");
  normal   = (TH2D*) _file_g3->Get("EmcEdepXY_StatHisto_XY_0");//200
  TH2D* masssignal   = (TH2D*) _file_g3->Get("EmcMassXY_Histo_XY_0");
  TH2D* massnormal   = (TH2D*) _file_g3->Get("EmcMassXY_StatHisto_XY_0");//200
  //1000 files was added together, in each files the quantities are normalized per evenrs
  H.push_back(plot(signal  ,normal  , c1 , 0, 1.61e14/1000.,
                   "Z [cm]", "X [cm]",
                   "Full Edep (20cm wide XY slice (Z[223cm, 223cm])) [J]",
                   false, false, true,
                   -1, -1,
                   "figures.pdf(",
                   masssignal, massnormal));
  delete signal;
  delete normal;
  Quater(H);

  /*
  signal   = (TH2D*) _file_g3->Get("EmcDensityXY_Histo_XY_0");
  normal   = (TH2D*) _file_g3->Get("EmcDensityXY_StatHisto_XY_0");//200
  H.push_back(plot(signal  ,normal  , c1 , 0, 1./1000.,//it is not time-dependent
                   "Z [cm]", "X [cm]",
                   "Density (20cm wide XY slice (Z[223cm, 223cm])) []",
                   false, false, true,
                   -1, -1,
                   "figures.pdf("));
  delete signal;
  delete normal;

  signal   = (TH2D*) _file_g3->Get("EmcTwoXY_Histo_XY_0");
  normal   = (TH2D*) _file_g3->Get("EmcTwoXY_StatHisto_XY_0");//200
  H.push_back(plot(signal  ,normal  , c1 , 0, 1./1000.,
                   "Z [cm]", "X [cm]",
                   "Two (20cm wide XY slice (Z[223cm, 223cm])) []",
                   false, false, true,
                   -1, -1,
                   "figures.pdf)"));
  delete signal;
  delete normal;
  */


  return 0;
}


TH1D* distro(TH2D* histo, double min, double max){
  char C[64];
  if(min == max){
    min = TMath::Log10(histo->GetMinimum()/2);
    max = TMath::Log10(histo->GetMaximum()*2);
    // min = (histo->GetMinimum()/2);
    // max = (histo->GetMaximum()*2);
  }
  int dx = (int)(ceil(max)-floor(min))*100;
  // std::cout << dx << ' ' << min << ' ' << max << std::endl;
  sprintf(C, "%s_distro", histo->GetName());
  TH1D* h1 = new TH1D(C, C, dx, min, max);
  for(int x = 1; x < histo->GetNbinsX(); x++){
    for(int y = 1; y < histo->GetNbinsY(); y++){
      h1->Fill(TMath::Log10(histo->GetBinContent(x, y)));
      // h1->Fill((histo->GetBinContent(x, y)));
    }
  }
  return h1;
}


TH2D*  divider(TH2D* H3, TH2D* H3S, Int_t t3,
               TH2D* H4, TH2D* H4S, Int_t t4,
               TCanvas* canvas,
               const char* Xlabel, const char* Ylabel, const char* Title,
               bool Xlog, bool Ylog, bool Zlog,
               double rmin, double rmax,
               const char* filename){

  canvas->cd();

  if(Xlog) canvas->SetLogx(1);
  else canvas->SetLogx(0);
  if(Ylog) canvas->SetLogy(1);
  else canvas->SetLogy(0);
  if(Zlog) canvas->SetLogz(1);
  else canvas->SetLogz(0);

  
  char cc[64];
  sprintf(cc, "%s_R", H3->GetTitle());
  TH2D* hresult = new TH2D(cc, cc,
                           H3->GetNbinsX(),
                           H3->GetXaxis()->GetBinLowEdge(1),
                           H3->GetXaxis()->GetBinUpEdge(H3->GetNbinsX()),
                           H3->GetNbinsY(),
                           H3->GetYaxis()->GetBinLowEdge(1),
                           H3->GetYaxis()->GetBinUpEdge(H3->GetNbinsY()));

  for(int x = 1; x <= H3S->GetXaxis()->GetNbins(); x++){
    for(int y = 1; y <= H3S->GetXaxis()->GetNbins(); y++){

      if((H3S->GetBinContent(x, y) > t3) &&
         (H4S->GetBinContent(x, y) > t4))
        {
          double ratio = H4->GetBinContent(x, y)/H3->GetBinContent(x, y);
          hresult->SetBinContent(x, y, ratio);
        }
    }
  }
  if((rmin > 0) || (rmax > 0))
    hresult->GetZaxis()->SetRangeUser(rmin, rmax);
  else
    hresult->GetZaxis()->SetRangeUser(hresult->GetMinimum(0),
                                      hresult->GetMaximum());
  hresult->SetContour(99);
  hresult->SetXTitle(Xlabel);
  hresult->SetYTitle(Ylabel);
  hresult->SetTitle(Title);

  TPaletteAxis *palette = new TPaletteAxis(hresult->GetXaxis()->GetBinCenter(H3->GetNbinsX())*1.025, 
                                           hresult->GetYaxis()->GetBinCenter(1),
                                           hresult->GetXaxis()->GetBinCenter(H3->GetNbinsX())*1.0375, 
                                           hresult->GetYaxis()->GetBinCenter(H3->GetNbinsY()),
                                           hresult);
  palette->SetLabelColor(1);
  palette->SetLabelFont(42);
  palette->SetLabelOffset(0.005);
  palette->SetLabelSize(0.035);
  palette->SetTitleOffset(1);
  palette->SetTitleSize(0.035);
  palette->SetFillColor(100);
  palette->SetFillStyle(1001);
  hresult->GetListOfFunctions()->Add(palette,"br");
  
  hresult->Draw("colz");
  canvas->Print(filename, "pdf");

  return hresult;

}

TH2D* Cut(TH2D* H4, TH2D* H4R, const char* name, double thrs, bool larger){

  TH2D* hresult = new TH2D(name, name,
                           H4->GetNbinsX(),
                           H4->GetXaxis()->GetBinLowEdge(1),
                           H4->GetXaxis()->GetBinUpEdge(H4->GetNbinsX()),
                           H4->GetNbinsY(),
                           H4->GetYaxis()->GetBinLowEdge(1),
                           H4->GetYaxis()->GetBinUpEdge(H4->GetNbinsY()));

  for(int x = 1; x <= H4R->GetXaxis()->GetNbins(); x++){
    for(int y = 1; y <= H4R->GetXaxis()->GetNbins(); y++){

      // bool true = false;
      if((thrs < H4->GetBinContent(x, y)) && larger)
        hresult->SetBinContent(x, y, H4->GetBinContent(x, y));
      if((H4->GetBinContent(x, y) < thrs) && !larger)
        hresult->SetBinContent(x, y, H4->GetBinContent(x, y));
    }
  }
  return hresult;
}


void Quater(std::vector<TH2D*>& HV){
  char C[64];


  int isize = (HV.size()-1);
  cout<<"ISIZE="<<isize<<endl;
  
  sprintf(C, "%s_F2", HV.at(isize)->GetName());

  TH2D* h2 = new TH2D(C, C,
                      HV.at(isize)->GetNbinsX()/2, 0, HV.at(isize)->GetXaxis()->GetBinUpEdge(HV.at(isize)->GetNbinsX()),
                      HV.at(isize)->GetNbinsY()/2, 0, HV.at(isize)->GetYaxis()->GetBinUpEdge(HV.at(isize)->GetNbinsY()));
  
  for(int i = 1; i < HV.at(isize)->GetNbinsX(); i++){
    double x = HV.at(isize)->GetXaxis()->GetBinCenter(i);
    for(int j = 1; j < HV.at(isize)->GetNbinsY(); j++){
      double y = HV.at(isize)->GetYaxis()->GetBinCenter(j);
      h2->Fill(TMath::Abs(x), TMath::Abs(y), HV.at(isize)->GetBinContent(i, j)/4.);
    }
  }
  HV.push_back(h2);
}

