/*
 * PndLmdAcceptance.cxx
 *
 *  Created on: Jul 4, 2012
 *      Author: steve
 */

#include "PndLmdAcceptance.h"
#include "PndLmdLumiFitOptions.h"

#include <cstdio>

#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooArgSet.h"

#include "TCanvas.h"
#include "TGraphAsymmErrors.h"

ClassImp(PndLmdAcceptance)

PndLmdAcceptance::PndLmdAcceptance(TFile *f_, int num_events_, double plab_,
    int th_bins_, int phi_bins_, double th_range_low_, double th_range_high_,
    double phi_range_low_, double phi_range_high_, double acceptance_threshold_) :
    PndLmdDataBase(f_, num_events_, plab_, th_bins_, phi_bins_, th_range_low_,
        th_range_high_, phi_range_low_, phi_range_high_), acceptance_threshold(
        acceptance_threshold_) {
  acceptance_1d = NULL;
  acceptance_2d = NULL;

  t_acceptance_1d = NULL;

  roo_acceptance_1d = NULL;
  roo_acceptance_2d = NULL;
}

PndLmdAcceptance::PndLmdAcceptance() {
  f = NULL;
  th_bins = 0;
  th_range_low = 0.0;
  th_range_high = 0.0;
  phi_range_low = 0.0;
  phi_range_high = 0.0;
  acceptance_threshold = 0.0;

  acceptance_1d = NULL;
  acceptance_2d = NULL;

  t_acceptance_1d = NULL;

  roo_acceptance_1d = NULL;
  roo_acceptance_2d = NULL;
}

PndLmdAcceptance::~PndLmdAcceptance() {
}

void PndLmdAcceptance::makeName() {
  getName() = "acc_corr";

  char suffix[100];
  sprintf(suffix, "_%f_%i_%f_%f_%i_%f_%f", acceptance_threshold, th_bins,
      th_range_low, th_range_high, phi_bins, phi_range_low, phi_range_high);

  getName() += suffix;
}

void PndLmdAcceptance::makeRooFitAcceptances() {
  roo_acceptance_1d = new RooDataHist("roo_acceptance_1d",
      "RooFit Theta-Acceptance", RooArgList(*roo_theta_var));
  roo_acceptance_2d = new RooDataHist("roo_acceptance_2d",
      "RooFit Theta-Phi-Acceptance", RooArgList(*roo_theta_var, *roo_phi_var));

  //loop over tefficiency objects
  for (int x = 0; x < getThBins(); x++) {
    *roo_theta_var = getThRangeLow() + getThBinSize() * (0.5 + x);

    for (int y = 0; y < getPhiBins(); y++) {
      *roo_phi_var = getPhiRangeLow() + getPhiBinSize() * (0.5 + y);
      int bin = acceptance_2d->FindFixBin(roo_theta_var->getVal(),
          roo_phi_var->getVal());
      roo_acceptance_2d->add(RooArgSet(*roo_theta_var, *roo_phi_var),
          acceptance_2d->GetEfficiency(bin));
      //	Double_t	GetEfficiencyErrorLow(Int_t bin) const
      //  Double_t	GetEfficiencyErrorUp(Int_t bin) const
    }

    int bin = acceptance_1d->FindFixBin(roo_theta_var->getVal());
    roo_acceptance_1d->add(RooArgSet(*roo_theta_var),
        acceptance_1d->GetEfficiency(bin));

    //std::cout << roo_theta_var->getVal() << " || "
    //		<< acceptance_1d->GetEfficiency(bin) << std::endl;
  }
}

void PndLmdAcceptance::makeFitterHists() {
  //makeDir(); we dont need to create folders for these objects since they are stored within the fit_map of the PndLmdData object

  acceptance_1d = new TEfficiency(*mc_acc_1d, *mc_1d);
  acceptance_2d = new TEfficiency(*mc_acc_2d, *mc_2d);

  t_acceptance_1d = new TEfficiency(*t_mc_acc_1d, *t_mc_1d);

  /*} else if (acceptance_type == 1) {
   acceptance_1d = new TEfficiency(*reco_1d, *mc_1d);
   acceptance_2d = new TEfficiency(*reco_2d, *mc_2d);

   t_acceptance_1d = new TEfficiency(*t_reco_1d, *t_mc_1d);
   }*/

  makeRooFitAcceptances();

  // create array
  data = new double*[getThBins() * getPhiBins()];
  for (int i = 0; i < getThBins() * getPhiBins(); i++) {
    data[i] = new double[3];
  }
}

void PndLmdAcceptance::saveToRootFile() {
  makeDir();
  if (acceptance_1d)
    acceptance_1d->Write();
  if (acceptance_2d)
    acceptance_2d->Write();
  if (t_acceptance_1d)
    t_acceptance_1d->Write();
}

TH2D* PndLmdAcceptance::getMCHist() {
  return mc_2d;
}
TH2D* PndLmdAcceptance::getRecoHist() {
  return reco_2d;
}

double PndLmdAcceptance::getAcceptanceThreshold() const {
  return acceptance_threshold;
}

TEfficiency* PndLmdAcceptance::getAcceptance1D(bool is_raw) const {
  if (is_raw) // if it is raw fit
    return t_acceptance_1d;
  return acceptance_1d;
}
TEfficiency* PndLmdAcceptance::getAcceptance2D() const {
  return acceptance_2d;
}

RooDataHist* PndLmdAcceptance::getRooFitAcceptance1D() const {
  return roo_acceptance_1d;
}
RooDataHist* PndLmdAcceptance::getRooFitAcceptance2D() const {
  return roo_acceptance_2d;
}

double** PndLmdAcceptance::getData(PndLmdLumiFitOptions *fit_options) {
  // ok check if we need raw or angular data and fill array...
  if (fit_options->isFitRaw()) {
    for (unsigned int i = 0; i < getThBins(); i++) {
      data[i][0] = getTRangeLow() + (i + 0.5) * getTBinSize();
      data[i][1] = 0.0;
      data[i][2] = t_acceptance_1d->GetEfficiency(i);
    }
  } else {
    for (unsigned int i = 0; i < getThBins(); i++) {
      for (unsigned int j = 0; j < getPhiBins(); j++) {
        data[i][0] = getThRangeLow() + (i + 0.5) * getThBinSize();
        data[i][1] = getPhiRangeLow() + (j + 0.5) * getPhiBinSize();
        data[i][2] = acceptance_2d->GetEfficiency(
            acceptance_2d->GetGlobalBin(i, j));
      }
    }
  }
  return data;
}
