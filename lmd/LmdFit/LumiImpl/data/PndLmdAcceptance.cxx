/*
 * PndLmdAcceptance.cxx
 *
 *  Created on: Jul 4, 2012
 *      Author: steve
 */

#include "PndLmdAcceptance.h"
#include "PndLmdLumiFitOptions.h"

#include <cstdio>

#include "TCanvas.h"
#include "TGraphAsymmErrors.h"

ClassImp(PndLmdAcceptance)

PndLmdAcceptance::PndLmdAcceptance(TFile *f_, int num_events_, double plab_,
		PndLmdFit::lmd_dimension th_dimension_,
		PndLmdFit::lmd_dimension phi_dimension_, double acceptance_threshold_) :
		PndLmdDataBase(f_, num_events_, plab_, th_dimension_, phi_dimension_), acceptance_threshold(
				acceptance_threshold_) {
	acceptance_1d = NULL;
	acceptance_2d = NULL;

	t_acceptance_1d = NULL;
}

PndLmdAcceptance::PndLmdAcceptance() {
	f = NULL;
	acceptance_threshold = 0.0;

	acceptance_1d = NULL;
	acceptance_2d = NULL;

	t_acceptance_1d = NULL;
}

PndLmdAcceptance::~PndLmdAcceptance() {
}

void PndLmdAcceptance::makeName() {
	getName() = "acc_corr";

	char suffix[100];
	sprintf(suffix, "_%f_%i_%f_%f_%i_%f_%f", acceptance_threshold,
			th_dimension.bins, th_dimension.range_low, th_dimension.range_high,
			phi_dimension.bins, phi_dimension.range_low, phi_dimension.range_high);

	getName() += suffix;
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

	// create array
	data = new double*[th_dimension.bins * phi_dimension.bins];
	for (int i = 0; i < th_dimension.bins * phi_dimension.bins; i++) {
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

double** PndLmdAcceptance::getData(PndLmdLumiFitOptions *fit_options) {
	// ok check if we need raw or angular data and fill array...
	if (fit_options->isFitRaw()) {
		for (int i = 0; i < th_dimension.bins; i++) {
			data[i][0] = t_dimension.range_low + (i + 0.5) * t_dimension.bin_size;
			data[i][1] = 0.0;
			data[i][2] = t_acceptance_1d->GetEfficiency(i);
		}
	} else {
		for (int i = 0; i < th_dimension.bins; i++) {
			for (int j = 0; j < phi_dimension.bins; j++) {
				data[i][0] = th_dimension.range_low + (i + 0.5) * th_dimension.bin_size;
				data[i][1] = phi_dimension.range_low
						+ (j + 0.5) * phi_dimension.bin_size;
				data[i][2] = acceptance_2d->GetEfficiency(
						acceptance_2d->GetGlobalBin(i, j));
			}
		}
	}
	return data;
}
