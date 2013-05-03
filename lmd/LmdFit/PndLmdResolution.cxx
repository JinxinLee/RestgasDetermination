/*
 * PndLmdResolution.cxx
 *
 *  Created on: Apr 17, 2013
 *      Author: steve
 */

#include "PndLmdResolution.h"

#include <iostream>

#include "TH2D.h"

PndLmdResolution::PndLmdResolution(double plab, int theta_res_bins_,
    double theta_res_range_low_, double theta_res_range_high_,
    int theta_mc_bins_, double theta_mc_range_low_, double theta_mc_range_high_) :
    PndLmdDataInterface(plab) {
  theta_res_bins = theta_res_bins_;
  theta_mc_bins = theta_mc_bins_;
  // TODO Auto-generated constructor stub
  theta_res_range_low = theta_res_range_low_;
  theta_res_range_high = theta_res_range_high_;

  theta_mc_range_low = theta_mc_range_low_;
  theta_mc_range_high = theta_mc_range_high_;

  theta_res_vs_theta_mc = new TH2D("theta_res_vs_theta_mc", "",
      theta_res_bins, theta_res_range_low, theta_res_range_high, theta_mc_bins,
      theta_mc_range_low, theta_mc_range_high);
}

PndLmdResolution::~PndLmdResolution() {
  // TODO Auto-generated destructor stub
}

void PndLmdResolution::fillHistograms(
    std::vector<std::pair<lmd_values, lmd_values> > &event_data) {
  if (1 == event_data.size()) {
    std::pair<lmd_values, lmd_values> data = event_data[0];
    if (data.first.reconstructed && data.second.reconstructed) {
      theta_res_vs_theta_mc->Fill(data.second.theta - data.first.theta,
          data.first.theta);
    }
  } else {
    std::cout
        << "Warning: Current event contains more than 1 track pair! This is not allowed! Skipping event!"
        << std::endl;
  }
}
