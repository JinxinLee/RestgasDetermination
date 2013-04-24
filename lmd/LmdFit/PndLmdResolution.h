/*
 * PndLmdResolution.h
 *
 *  Created on: Apr 17, 2013
 *      Author: steve
 */

#ifndef PNDLMDRESOLUTION_H_
#define PNDLMDRESOLUTION_H_

#include "PndLmdDataInterface.h"

class TH2D;

class PndLmdResolution: public PndLmdDataInterface {
private:
  int theta_res_bins;
  int theta_mc_bins;

  double theta_res_range_low;
  double theta_res_range_high;

  double theta_mc_range_low;
  double theta_mc_range_high;

  TH2D *theta_res_vs_theta_mc;
public:
  PndLmdResolution(double plab, int theta_res_bins_, double theta_res_range_low_,
      double theta_res_range_high_, int theta_mc_bins_, double theta_mc_range_low_,
      double theta_mc_range_high_);
  virtual ~PndLmdResolution();

  void fillHistograms(std::vector<std::pair<lmd_values, lmd_values> > &event_data) =0;
};

#endif /* PNDLMDRESOLUTION_H_ */
