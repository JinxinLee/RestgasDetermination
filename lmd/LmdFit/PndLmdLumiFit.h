#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "RooArgSet.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooFFTConvPdf.h"
#include "RooFitResult.h"
#include "RooExtendPdf.h"
#include "RooGenericPdf.h"
#include "RooPolynomial.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooNumIntConfig.h"
#include "RooPlot.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TH1.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TString.h"
#include "TF1.h"
#include "TLatex.h"
#include "TString.h"
#include "TPaveText.h"
#include "TMath.h"
#include "TPad.h"
#include "TFile.h"
#include "TStyle.h"
#include "TROOT.h"
#include "PndLmdcs_th.h"
#include <iostream>
#include <sstream>
#include <iomanip>

class PndLmdLumiFit {

private:
  double momt(double);

  double doppelgaus(Double_t*, Double_t*);

  double mythInt(cs_th&, Double_t, Double_t);


public:
  PndLmdLumiFit(double, double, double, double, double);
  ~PndLmdLumiFit();

  double LumiFit(TH1D*);

protected:

  double Plab;
  double sigtot, rho, b, sigma_th;
  double thmin, thmax, posbox;

};
