/*
 * PndLmdDataBaseBase.cxx
 *
 *  Created on: Jul 5, 2012
 *      Author: steve
 */

#include "PndLmdDataBase.h"

#include "PndLmdDPMAngModel1D.h"
#include "PndLmdDPMModelParametrization.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TF2.h"
#include "TFile.h"

ClassImp(PndLmdDataBase)

PndLmdDataBase::PndLmdDataBase() {
}

PndLmdDataBase::PndLmdDataBase(TFile *f_, int num_events_, double plab_,
    int th_bins_, int phi_bins_, double th_range_low_, double th_range_high_,
    double phi_range_low_, double phi_range_high_) :
    PndLmdDataInterface(plab_) {
  f = f_;
  setLabMomentum(plab_);
  setNumEvents(num_events_);

  th_bins = th_bins_;
  phi_bins = phi_bins_;

  if (th_range_low_ < 0.0)
    th_range_low = 0.0;
  else
    th_range_low = th_range_low_;
  th_range_high = th_range_high_;
  phi_range_low = phi_range_low_;
  phi_range_high = phi_range_high_;

  th_bin_size = (th_range_high - th_range_low) / th_bins;
  phi_bin_size = (phi_range_high - phi_range_low) / phi_bins;

  makeName();

  // calculate momentum transfer histogram bounds
  t_bins = th_bins;
  t_range_low = getMomentumTransferFromTheta(th_range_low);
  t_range_high = getMomentumTransferFromTheta(th_range_high);
  std::cout << "calulated t range: " << t_range_low << " - " << t_range_high
      << std::endl;
  t_bin_size = (t_range_high - t_range_low) / t_bins;

  //
  // 1d histograms
  mc_1d = new TH1D("htheta_mc", " #Theta mc truth", th_bins, th_range_low,
      th_range_high);
  mc_acc_1d = new TH1D("htheta_mc_acc", " #Theta mc truth acc", th_bins,
      th_range_low, th_range_high);
  reco_1d = new TH1D("htheta_reco", " #Theta reco", th_bins, th_range_low,
      th_range_high);

  t_mc_1d = new TH1D("ht_mc", " #t mc truth", t_bins, t_range_low,
      t_range_high);
  t_mc_acc_1d = new TH1D("ht_mc_acc", " #t mc truth acc", t_bins, t_range_low,
      t_range_high);
  t_reco_1d = new TH1D("ht_reco", " #t reco", t_bins, t_range_low,
      t_range_high);

  // 2d histograms
  mc_2d = new TH2D("htheta_vs_phi_mc", " #Theta vs #phi mc truth", th_bins,
      th_range_low, th_range_high, phi_bins, phi_range_low, phi_range_high);
  mc_acc_2d = new TH2D("htheta_vs_phi_mc_acc", " #Theta vs #phi mc truth acc",
      th_bins, th_range_low, th_range_high, phi_bins, phi_range_low,
      phi_range_high);
  reco_2d = new TH2D("htheta_vs_phi_reco", " #Theta vs #phi reco", th_bins,
      th_range_low, th_range_high, phi_bins, phi_range_low, phi_range_high);

  mc_1d->Sumw2();
  mc_acc_1d->Sumw2();
  reco_1d->Sumw2();

  t_mc_1d->Sumw2();
  t_mc_acc_1d->Sumw2();
  t_reco_1d->Sumw2();

  mc_2d->Sumw2();
  mc_acc_2d->Sumw2();
  reco_2d->Sumw2();

  reco_unbinned = NULL;
}

PndLmdDataBase::~PndLmdDataBase() {

}

bool PndLmdDataBase::operator==(const PndLmdDataBase &lmd_data_base) const {

  if (getLabMomentum() != lmd_data_base.getLabMomentum())
    return false;
  if (th_bins != lmd_data_base.getThBins())
    return false;
  if (phi_bins != lmd_data_base.getPhiBins())
    return false;
  if (th_range_low != lmd_data_base.getThRangeLow())
    return false;
  if (th_range_high != lmd_data_base.getThRangeHigh())
    return false;
  if (phi_range_low != lmd_data_base.getPhiRangeLow())
    return false;
  if (phi_range_high != lmd_data_base.getPhiRangeHigh())
    return false;

  return true;
}

bool PndLmdDataBase::operator!=(const PndLmdDataBase &lmd_data_base) const {
  return !(*this == lmd_data_base);
}

double PndLmdDataBase::getMomentumTransferFromTheta(double theta) const {
  PndLmdDPMAngModel1D model("dpm_angular_1d");
  shared_ptr<Parametrization> para(
      new PndLmdDPMModelParametrization(model.getModelParameterSet()));
  model.getModelParameterHandler().registerParametrizations(
      model.getModelParameterSet(), para);
  model.getModelParameterSet().setModelParameterValue("p_lab",
      getLabMomentum());
  ((Model1D*) &model)->init();
  return -model.getMomentumTransferFromTheta(theta);
}

void PndLmdDataBase::makeName() {
  char cname[100];
  sprintf(cname, "%i_%i_%f_%f_%i_%f_%f", getNumEvents(), th_bins, th_range_low,
      th_range_high, phi_bins, phi_range_low, phi_range_high);

  getName() += cname;
}

void PndLmdDataBase::makeDir() {
  TDirectory *dir = f->GetDirectory(getName());
  if (dir) {
    dir->cd();
  } else {
    f->cd();
    f->mkdir(getName());
    f->cd(getName());
  }
}

void PndLmdDataBase::saveToRootFile() {
}

int PndLmdDataBase::getThBins() const {
  return th_bins;
}
int PndLmdDataBase::getPhiBins() const {
  return phi_bins;
}
double PndLmdDataBase::getThRangeLow() const {
  return th_range_low;
}
double PndLmdDataBase::getThRangeHigh() const {
  return th_range_high;
}
double PndLmdDataBase::getPhiRangeLow() const {
  return phi_range_low;
}
double PndLmdDataBase::getPhiRangeHigh() const {
  return phi_range_high;
}
double PndLmdDataBase::getThBinSize() const {
  return th_bin_size;
}
double PndLmdDataBase::getPhiBinSize() const {
  return phi_bin_size;
}

double PndLmdDataBase::getTBinSize() const {
  return t_bin_size;
}
double PndLmdDataBase::getTRangeLow() const {
  return t_range_low;
}
double PndLmdDataBase::getTRangeHigh() const {
  return t_range_high;
}

void PndLmdDataBase::setUnbinnedRecoData(TTree* reco_tree) {
  reco_unbinned = reco_tree;
}
TTree* PndLmdDataBase::getUnbinnedRecoData() const {
  return reco_unbinned;
}

void PndLmdDataBase::fillHistograms(
    std::vector<std::pair<lmd_values, lmd_values> > &event_data) {
  /*std::cout << "---- MC info ----" << std::endl;
   std::cout << "t: " << data.first.t << std::endl;
   std::cout << "theta: " << data.first.theta << std::endl;
   std::cout << "phi: " << data.first.phi << std::endl;
   std::cout << "reconstructed: " << data.first.reconstructed << std::endl;
   std::cout << "-----------------" << std::endl;
   std::cout << "--- Reco info ---" << std::endl;
   std::cout << "t: " << data.second.t << std::endl;
   std::cout << "theta: " << data.second.theta << std::endl;
   std::cout << "phi: " << data.second.phi << std::endl;
   std::cout << "reconstructed: " << data.second.reconstructed << std::endl;
   std::cout << "-----------------" << std::endl;*/

  for (unsigned int i = 0; i < event_data.size(); i++) {
    std::pair<lmd_values, lmd_values> data = event_data[i];
    if (data.first.reconstructed) {
      t_mc_1d->Fill(data.first.t);

      mc_1d->Fill(1000. * data.first.theta);
      mc_2d->Fill(1000. * data.first.theta, data.first.phi);

      if (data.second.reconstructed) {
        t_mc_acc_1d->Fill(data.first.t);

        mc_acc_1d->Fill(1000. * data.first.theta);
        mc_acc_2d->Fill(1000. * data.first.theta, data.first.phi);
      }
    }
    if (data.second.reconstructed) {
      t_reco_1d->Fill(data.second.t);

      reco_1d->Fill(1000. * data.second.theta);
      reco_2d->Fill(1000. * data.second.theta, data.second.phi);
    }
  }
}
