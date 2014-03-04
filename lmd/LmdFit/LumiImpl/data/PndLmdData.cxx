/*
 * PndLmdAngularData.h
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */
#include "PndLmdData.h"

ClassImp(PndLmdData)

PndLmdData::PndLmdData() :
		hist_1d(0), hist_2d(0) {
	reference_luminosity_per_event = 1.0;
}

PndLmdData::~PndLmdData() {
}

void PndLmdData::init1DData() {
	// 1d histograms
	hist_1d = new TH1D("hist1d", "", primary_dimension.bins,
			primary_dimension.dimension_range.getRangeLow(),
			primary_dimension.dimension_range.getRangeHigh());

	hist_1d->Sumw2();
}

void PndLmdData::init2DData() {
	hist_2d = new TH2D("hist2d", "", primary_dimension.bins,
			primary_dimension.dimension_range.getRangeLow(),
			primary_dimension.dimension_range.getRangeHigh(),
			secondary_dimension.bins,
			secondary_dimension.dimension_range.getRangeLow(),
			secondary_dimension.dimension_range.getRangeHigh());

	hist_2d->Sumw2();
}

void PndLmdData::cloneData(const PndLmdAbstractData &lmd_abs_data) {
	const PndLmdData * lmd_data = dynamic_cast<const PndLmdData*>(&lmd_abs_data);
	if (lmd_data) {
		hist_1d = new TH1D(*lmd_data->get1DHistogram());
		if (getSecondaryDimension().is_active) {
			hist_2d = new TH2D(*lmd_data->get2DHistogram());
		}
	}
}

void PndLmdData::add(const PndLmdAbstractData &lmd_abs_data_addition) {
	const PndLmdData * lmd_data_addition =
			dynamic_cast<const PndLmdData*>(&lmd_abs_data_addition);
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

double PndLmdData::getReferenceLuminosity() const {
	return reference_luminosity_per_event * getNumEvents();
}

double PndLmdData::getReferenceLuminosityPerEvent() const {
	return reference_luminosity_per_event;
}

void PndLmdData::setReferenceLuminosityPerEvent(
		double reference_luminosity_per_event_) {
	reference_luminosity_per_event = reference_luminosity_per_event_;
}

void PndLmdData::addData(double primary_value, double secondary_value) {
	hist_1d->Fill(primary_value);
	if (secondary_dimension.is_active) {
		hist_2d->Fill(primary_value, secondary_value);
	}
}

TH1D* PndLmdData::get1DHistogram() const {
	return hist_1d;
}

TH2D* PndLmdData::get2DHistogram() const {
	return hist_2d;
}

bool PndLmdData::operator<(const PndLmdData &lmd_data) const {
	if (reference_luminosity_per_event
			< lmd_data.getReferenceLuminosityPerEvent())
		return true;
	else if (reference_luminosity_per_event
			> lmd_data.getReferenceLuminosityPerEvent())
		return false;
	return PndLmdAbstractData::operator<(lmd_data);
}
bool PndLmdData::operator>(const PndLmdData &lmd_data) const {
	return (lmd_data < *this);
}
bool PndLmdData::operator==(const PndLmdData &lmd_data) const {
	if (reference_luminosity_per_event
			!= lmd_data.getReferenceLuminosityPerEvent())
		return false;
	return PndLmdAbstractData::operator==(lmd_data);
}
bool PndLmdData::operator!=(const PndLmdData &lmd_data) const {
	return !(*this == lmd_data);
}
