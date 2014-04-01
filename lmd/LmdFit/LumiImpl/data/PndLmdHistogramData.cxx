/*
 * PndLmdHistogramData.cxx
 *
 *  Created on: Mar 26, 2014
 *      Author: steve
 */

#include "PndLmdHistogramData.h"

ClassImp(PndLmdHistogramData)

PndLmdHistogramData::PndLmdHistogramData() : hist_1d(0), hist_2d(0) {
}

PndLmdHistogramData::~PndLmdHistogramData() {
	// TODO Auto-generated destructor stub
}

void PndLmdHistogramData::init1DData() {
	// 1d histograms
	hist_1d = new TH1D("hist1d", "", primary_dimension.bins,
			primary_dimension.dimension_range.getRangeLow(),
			primary_dimension.dimension_range.getRangeHigh());

	hist_1d->Sumw2();
}

void PndLmdHistogramData::init2DData() {
	hist_2d = new TH2D("hist2d", "", primary_dimension.bins,
			primary_dimension.dimension_range.getRangeLow(),
			primary_dimension.dimension_range.getRangeHigh(),
			secondary_dimension.bins,
			secondary_dimension.dimension_range.getRangeLow(),
			secondary_dimension.dimension_range.getRangeHigh());

	hist_2d->Sumw2();
}

void PndLmdHistogramData::cloneData(const PndLmdAbstractData &lmd_abs_data) {
	const PndLmdHistogramData * lmd_data = dynamic_cast<const PndLmdHistogramData*>(&lmd_abs_data);
	if (lmd_data) {
		hist_1d = new TH1D(*lmd_data->get1DHistogram());
		if (getSecondaryDimension().is_active) {
			hist_2d = new TH2D(*lmd_data->get2DHistogram());
		}
	}
}

void PndLmdHistogramData::add(const PndLmdAbstractData &lmd_abs_data_addition) {
	const PndLmdHistogramData * lmd_data_addition =
			dynamic_cast<const PndLmdHistogramData*>(&lmd_abs_data_addition);
	if (lmd_data_addition) {
		if (getPrimaryDimension().dimension_range
				== lmd_data_addition->getPrimaryDimension().dimension_range) {
			setNumEvents(getNumEvents() + lmd_data_addition->getNumEvents());
			hist_1d->Add(lmd_data_addition->get1DHistogram());
			if (getSecondaryDimension().is_active) {
				if (getSecondaryDimension().dimension_range
						== lmd_data_addition->getSecondaryDimension().dimension_range) {
					hist_2d->Add(lmd_data_addition->get2DHistogram());
				}
			}
		}
	}
}

void PndLmdHistogramData::addData(double primary_value, double secondary_value) {
	hist_1d->Fill(primary_value);
	if (secondary_dimension.is_active) {
		hist_2d->Fill(primary_value, secondary_value);
	}
}

TH1D* PndLmdHistogramData::get1DHistogram() const {
	return hist_1d;
}

TH2D* PndLmdHistogramData::get2DHistogram() const {
	return hist_2d;
}

const map<PndLmdLumiFitOptions, PndLmdLumiFitResult*>& PndLmdHistogramData::getFitResults() const {
	return fit_storage.getFitResults();
}

PndLmdLumiFitResult* PndLmdHistogramData::getFitResult(
		const PndLmdLumiFitOptions &fit_options) const {
		return fit_storage.getFitResult(fit_options);
}

void PndLmdHistogramData::addFitResult(const PndLmdLumiFitOptions *fit_options,
		PndLmdLumiFitResult* fit_result_) {
	fit_storage.addFitResult(fit_options, fit_result_);
}
