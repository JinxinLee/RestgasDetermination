/*
 * PndLmdAbstractData.cxx
 *
 *  Created on: Apr 17, 2013
 *      Author: steve
 */

#include "PndLmdAbstractData.h"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;

ClassImp(PndLmdAbstractData)

PndLmdAbstractData::PndLmdAbstractData() :
		p_lab(0.0) {
}

PndLmdAbstractData::~PndLmdAbstractData() {
	// TODO Auto-generated destructor stub
}

int PndLmdAbstractData::getNumEvents() const {
	return num_events;
}

double PndLmdAbstractData::getLabMomentum() const {
	return p_lab;
}

const TString& PndLmdAbstractData::getName() const {
	return name;
}

const LumiFit::LmdDimension& PndLmdAbstractData::getPrimaryDimension() const {
	return primary_dimension;
}

const LumiFit::LmdDimension& PndLmdAbstractData::getSecondaryDimension() const {
	return secondary_dimension;
}

double PndLmdAbstractData::getBinningFactor() const {
	double bin_factor = getPrimaryDimension().bin_size;
	if (getSecondaryDimension().is_active)
		bin_factor *= getSecondaryDimension().bin_size;
	return bin_factor;
}

void PndLmdAbstractData::setNumEvents(int num_events_) {
	num_events = num_events_;
}

void PndLmdAbstractData::setLabMomentum(double p_lab_) {
	p_lab = p_lab_;
}

void PndLmdAbstractData::setName(TString name_) {
	name = name_;
}

void PndLmdAbstractData::setPrimaryDimension(
		LumiFit::LmdDimension primary_dimension_) {
	if (primary_dimension.is_active)
		cout
				<< "Warning: the primary dimension is already set. This function call will have no effect."
				<< endl;
	else {
		primary_dimension = primary_dimension_;
		init1DData();
	}
}

void PndLmdAbstractData::setSecondaryDimension(
		LumiFit::LmdDimension secondary_dimension_) {
	if (secondary_dimension.is_active)
		cout
				<< "Warning: the primary dimension is already set. This function call will have no effect."
				<< endl;
	else {
		secondary_dimension = secondary_dimension_;
		init2DData();
	}
}

int PndLmdAbstractData::addFileToList(TString filepath) {
	return filepath_list.insert(filepath).second;
}

void PndLmdAbstractData::saveToRootFile() {
	cout << "Saving " << getName() << " to file..." << endl;
	this->Write(getName());
}

bool PndLmdAbstractData::operator<(
		const PndLmdAbstractData &lmd_data_int) const {
	// leave out num_events cuz that is a bit special
	if (getLabMomentum() < lmd_data_int.getLabMomentum())
		return true;
	else if (getLabMomentum() > lmd_data_int.getLabMomentum())
		return false;
	if (primary_dimension < lmd_data_int.primary_dimension)
		return true;
	else if (primary_dimension > lmd_data_int.primary_dimension)
		return false;
	if (secondary_dimension.is_active) {
		if (secondary_dimension < lmd_data_int.secondary_dimension)
			return true;
	}

	return false;
}

bool PndLmdAbstractData::operator>(
		const PndLmdAbstractData &lmd_data_int) const {
	return (lmd_data_int < *this);
}

bool PndLmdAbstractData::operator==(
		const PndLmdAbstractData &lmd_data_int) const {

	if (getNumEvents() != lmd_data_int.getNumEvents())
		return false;
	if (getLabMomentum() != lmd_data_int.getLabMomentum())
		return false;
	if (primary_dimension != lmd_data_int.primary_dimension)
		return false;
	if (secondary_dimension.is_active) {
		if (secondary_dimension != lmd_data_int.secondary_dimension)
			return false;
	}
	return true;
}

bool PndLmdAbstractData::operator!=(
		const PndLmdAbstractData &lmd_data_int) const {
	return !(*this == lmd_data_int);
}
