/*
 * PndLmdResolution.h
 *
 *  Created on: Apr 17, 2013
 *      Author: steve
 */

#ifndef PNDLMDRESOLUTION_H_
#define PNDLMDRESOLUTION_H_

#include "PndLmdDataInterface.h"

class TH1D;
class TH2D;
class TFile;

class PndLmdResolution: public PndLmdDataInterface {
private:
	double th_slice_range_low, th_slice_range_high;
	double phi_slice_range_low, phi_slice_range_high;
	TH1D *theta_res;
	TH2D *theta_res_vs_phi_res;

public:
	PndLmdResolution(TFile* f_, int num_events_, double plab, double th_slice_range_low_,
			double th_slice_range_high_, double phi_slice_range_low_,
			double phi_slice_range_high_, PndLmdFit::lmd_dimension th_dimension_,
			PndLmdFit::lmd_dimension phi_dimension_);
	PndLmdResolution();
	virtual ~PndLmdResolution();

	double getThetaSliceMean() const;
	double getPhiSliceMean() const;

	TH1D* getResolutionHistogram1D();
	TH2D* getResolutionHistogram2D();

	void saveToRootFile();
	virtual void makeName();

	void fillHistograms(
			std::vector<std::pair<PndLmdFit::lmd_values, PndLmdFit::lmd_values> > &event_data);

ClassDef(PndLmdResolution,1)
};

#endif /* PNDLMDRESOLUTION_H_ */
