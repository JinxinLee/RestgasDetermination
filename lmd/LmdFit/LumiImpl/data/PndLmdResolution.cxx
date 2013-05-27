/*
 * PndLmdResolution.cxx
 *
 *  Created on: Apr 17, 2013
 *      Author: steve
 */

#include "PndLmdResolution.h"

#include <iostream>

#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"

ClassImp(PndLmdResolution)

PndLmdResolution::PndLmdResolution(TFile* f_, int num_events_, double plab,
		double th_slice_range_low_, double th_slice_range_high_,
		double phi_slice_range_low_, double phi_slice_range_high_,
		PndLmdFit::lmd_dimension th_dimension_,
		PndLmdFit::lmd_dimension phi_dimension_) :
		PndLmdDataInterface(plab, th_dimension_, phi_dimension_) {
	setNumEvents(num_events_);

	f = f_;
	th_slice_range_low = th_slice_range_low_;
	th_slice_range_high = th_slice_range_high_;
	phi_slice_range_low = phi_slice_range_low_;
	phi_slice_range_high = phi_slice_range_high_;

	makeName();

	theta_res = new TH1D("theta_res", "", th_dimension.bins,
			th_dimension.range_low, th_dimension.range_high);
	theta_res_vs_phi_res = new TH2D("theta_res_vs_phi_res", "", th_dimension.bins,
			th_dimension.range_low, th_dimension.range_high, phi_dimension.bins,
			phi_dimension.range_low, phi_dimension.range_high);
}
PndLmdResolution::PndLmdResolution() {
	// empty constructor needed by ROOT IO
}

PndLmdResolution::~PndLmdResolution() {
	// TODO Auto-generated destructor stub
}

double PndLmdResolution::getThetaSliceMean() const {
	return (th_slice_range_high + th_slice_range_low) / 2;
}
double PndLmdResolution::getPhiSliceMean() const {
	return (phi_slice_range_high + phi_slice_range_low) / 2;
}

TH1D* PndLmdResolution::getResolutionHistogram1D() {
	return theta_res;
}
TH2D* PndLmdResolution::getResolutionHistogram2D() {
	return theta_res_vs_phi_res;
}

void PndLmdResolution::makeName() {
	char cname[100];
	sprintf(cname, "%i_%i_%f_%f_%i_%f_%f_%f_%f", getNumEvents(),
			th_dimension.bins, th_dimension.range_low, th_dimension.range_high,
			phi_dimension.bins, phi_dimension.range_low, phi_dimension.range_high,
			getThetaSliceMean(), getPhiSliceMean());

	getName() += cname;
}

void PndLmdResolution::saveToRootFile() {
	std::cout << "Saving " << getName() << " to file..." << std::endl;

	makeDir(); // this will just change current directory to the one created by this data set

	this->Write("lmdresolution");
}

void PndLmdResolution::fillHistograms(
		std::vector<std::pair<PndLmdFit::lmd_values, PndLmdFit::lmd_values> > &event_data) {
	if (1 == event_data.size()) {
		std::pair<PndLmdFit::lmd_values, PndLmdFit::lmd_values> data = event_data[0];
		if (data.first.reconstructed && data.second.reconstructed) {
			if (th_slice_range_low < data.first.theta
					&& th_slice_range_high > data.first.theta) {
				theta_res->Fill(1000. * (data.second.theta - data.first.theta));
				if (phi_slice_range_low < data.first.phi
						&& phi_slice_range_high > data.first.phi) {
					theta_res_vs_phi_res->Fill(
							1000. * (data.second.theta - data.first.theta),
							data.second.phi - data.first.phi);
				}
			}
		}
	} else {
		std::cout
				<< "Warning: Current event contains more than 1 track pair! This is not allowed! Skipping event!"
				<< std::endl;
	}
}
