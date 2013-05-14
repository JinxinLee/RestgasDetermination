////////////////////////////////////////////////////////////
//
// PndTrkLegendreTransform
// 
// Class to perform the Legendre Transformation
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//                                   
////////////////////////////////////////////////////////////

#include "PndTrkLegendreTransform.h"

#include <iostream>

using namespace std;


// -----   Default constructor   -------------------------------------------
PndTrkLegendreTransform::PndTrkLegendreTransform() {
  fThetaNofBin = 180; 
  fThetaMin    = 0;    
  fThetaMax    = 180;  
  fRNofBin     = 1000; 
  fRMin        = -0.07; 
  fRMax        = 0.07;  


  fThetaMinZoom    = 0;    
  fThetaMaxZoom    = 180;  
  fRMinZoom        = -0.07; 
  fRMaxZoom        = 0.07;  

  fhLegendre = NULL;
  fhLegendreZoom = NULL;
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkLegendreTransform::~PndTrkLegendreTransform() { 

}
// -------------------------------------------------------------------------

// ------ HISTOGRAM --------------------------------------------------------
void PndTrkLegendreTransform::SetUpLegendreHisto() {
 fhLegendre = new TH2F("fhLegendre", "", fThetaNofBin, fThetaMin, fThetaMax, fRNofBin, fRMin, fRMax);
}

void PndTrkLegendreTransform::SetUpLegendreHisto(double thetaNofBin, double thetaMin, double thetaMax, double rNofBin, double rMin, double rMax) {
  fThetaNofBin = thetaNofBin;
  fThetaMin    = thetaMin;
  fThetaMax    = thetaMax;
  fRNofBin     = rNofBin;
  fRMin        = rMin;
  fRMax        = rMax;
  SetUpLegendreHisto();
}

void PndTrkLegendreTransform::ResetLegendreHisto() {
  fhLegendre->Reset();
  fhLegendre->SetStats(kFALSE);
}


void PndTrkLegendreTransform::FillHisto(TH2F *histo, double thetamin, double thetamax, double x, double y, double radius) {
  Double_t  thetaRad = thetamin * TMath::DegToRad();
  double deltaThetaRad = TMath::DegToRad() * (thetamax - thetamin)/fThetaNofBin;
 
  while(thetaRad < thetamax * TMath::DegToRad()) {
    double thetaDeg = thetaRad * TMath::RadToDeg();
	
    // STT
    if(radius > 0.) {
      double r1 = x * TMath::Cos(thetaRad) + y * TMath::Sin(thetaRad) + radius;
      double r2 = x * TMath::Cos(thetaRad) + y * TMath::Sin(thetaRad) - radius;
      histo->Fill(thetaDeg, r1);
      histo->Fill(thetaDeg, r2);
    }
    else { // MVD, SCITIL, GEM
      double r = x * TMath::Cos(thetaRad) + y * TMath::Sin(thetaRad);
      histo->Fill(thetaDeg, r);
    } 
    
    thetaRad += deltaThetaRad;
    
  }
}

void PndTrkLegendreTransform::FillLegendreHisto(double x, double y, double radius) {
  FillHisto(fhLegendre, fThetaMin, fThetaMax, x, y, radius);
}

// void PndTrkLegendreTransform::ApplyThresholdLegendreHisto(double threshold) {
//   int max = fhLegendre->GetMaximum();
//   fhLegendre->GetZaxis()->SetRangeUser(max * threshold, max);
// }

int PndTrkLegendreTransform::ExtractLegendreMaximum(double &theta_max, double &r_max) {
  ExtractMaximumFromHisto(fhLegendre, theta_max, r_max);
}

void PndTrkLegendreTransform::ExtractLegendreSingleLineParameters(double &slope, double &intercept) {
  double theta, r;
  ExtractLegendreMaximum(theta, r);
  ExtractLineParameters(theta, r, slope, intercept);
}

// ------ ZOOM -----------------------------------------------------------------
void PndTrkLegendreTransform::SetUpZoomHisto() {
  fhLegendreZoom = new TH2F("fhLegendreZoom", "", fThetaNofBin, fThetaMinZoom, fThetaMaxZoom, fRNofBin, fRMin, fRMax);
}

void PndTrkLegendreTransform::SetUpZoomHisto(double theta, double r, double deltatheta, double deltar) {
  if(fhLegendreZoom) {
    ResetZoomHisto();
  }

  fThetaMinZoom = theta - deltatheta;
  fThetaMaxZoom = theta + deltatheta;
  fRMinZoom = r - deltar;
  fRMaxZoom = r + deltar;
  
  fhLegendreZoom->GetXaxis()->SetLimits(fThetaMinZoom, fThetaMaxZoom);
  fhLegendreZoom->GetYaxis()->SetLimits(fRMinZoom, fRMaxZoom);

}

void PndTrkLegendreTransform::ResetZoomHisto() {
  fhLegendreZoom->Reset();
  fhLegendreZoom->SetStats(kFALSE);
}

void PndTrkLegendreTransform::FillZoomHisto(double x, double y, double radius) {
  FillHisto(fhLegendreZoom, fThetaMinZoom, fThetaMaxZoom, x, y, radius);
}

int PndTrkLegendreTransform::ExtractZoomMaximum(double &theta_max, double &r_max) {
  ExtractMaximumFromHisto(fhLegendreZoom, theta_max, r_max);
}

void PndTrkLegendreTransform::ExtractZoomSingleLineParameters(double &slope, double &intercept) {
  double theta, r;
  ExtractZoomMaximum(theta, r);
  ExtractLineParameters(theta, r, slope, intercept);
}

// GET A SINGLE LINE --> THE MAXIMUM -------------------------------------------
int PndTrkLegendreTransform::ExtractMaximumFromHisto(TH2F *histo, double &theta_max, double &r_max) {
  int bin = histo->GetMaximumBin();
  int binx, biny, binz;
  histo->GetBinXYZ(bin, binx, biny, binz);
  theta_max = histo->GetXaxis()->GetBinCenter(binx);
  r_max = histo->GetYaxis()->GetBinCenter(biny);
  return histo->GetMaximum();
  //    cout << "== THETA, R max ======== " << theta_max << " " << r_max << endl;
}

void PndTrkLegendreTransform::ExtractLineParameters(double theta, double r, double &slope, double &intercept) {
  // r = x cost + y sint --> y = r/sint - x/tant

  if(theta == 0.) theta = 1.e-6; // CHECK

  slope = - 1./TMath::Tan(TMath::DegToRad() * theta);
  intercept = r/TMath::Sin(TMath::DegToRad() * theta);

}

// -----------------------------------------------------------------------------
// DELETE A PEAK
void PndTrkLegendreTransform::DeleteZoneAroundXYLegendre(double x, double y) {
  DeleteZoneAroundXY(fhLegendre, x, y);
}
 
void PndTrkLegendreTransform::DeleteZoneAroundXYZoom(double x, double y) {
 DeleteZoneAroundXY(fhLegendreZoom, x, y);
}

  
void PndTrkLegendreTransform::DeleteZoneAroundXY(TH2F *histo, double x, double y) {

  //  int bin = histo->GetMaximumBin();
  //  int bin1, bin2, bin3;
  //  histo->GetBinXYZ(bin, bin1, bin2, bin3);
  // cout << "MAX BIN BEFORE " << bin << " " << bin1 << " " << bin2 << " " << bin3 << endl;

  int binx = (int) ((x -  histo->GetXaxis()->GetXmin())/histo->GetXaxis()->GetBinWidth(1)) + 1;
  int biny = (int) ((y -  histo->GetYaxis()->GetXmin())/histo->GetYaxis()->GetBinWidth(1)) + 1;
  
  // delete the bin and a line and a row around it
  histo->SetBinContent(binx,     biny,     0);
  histo->SetBinContent(binx + 1, biny,     0);
  histo->SetBinContent(binx - 1, biny,     0);
  histo->SetBinContent(binx + 1, biny + 1, 0);
  histo->SetBinContent(binx,     biny + 1, 0);
  histo->SetBinContent(binx - 1, biny + 1, 0);
  histo->SetBinContent(binx + 1, biny - 1, 0);
  histo->SetBinContent(binx,     biny - 1, 0);
  histo->SetBinContent(binx - 1, biny - 1, 0);
   
  // cout << "deleting binx " << binx << " " << biny << endl;
  //  bin = histo->GetMaximumBin();
  //  histo->GetBinXYZ(bin, bin1, bin2, bin3);
  //  cout << "MAX BIN AFTER " << bin << " " << bin1 << " " << bin2 << " " << bin3 << endl;
}


// -----------------------------------------------------------------------------



void PndTrkLegendreTransform::Draw() {
  fhLegendre->Draw("colz");
}

void PndTrkLegendreTransform::DrawZoom() {
  fhLegendreZoom->Draw("colz");
}

ClassImp(PndTrkLegendreTransform)

