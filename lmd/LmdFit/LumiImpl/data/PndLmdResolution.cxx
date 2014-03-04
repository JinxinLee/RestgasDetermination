/*
 * PndLmdResolution.cxx
 *
 *  Created on: Aug 25, 2013
 *      Author: steve
 */

#include "PndLmdResolution.h"

PndLmdResolution::PndLmdResolution() {
}

PndLmdResolution::~PndLmdResolution() {
}

const LumiFit::LmdDimension& PndLmdResolution::getPrimarySelectionDimension() const {
	return primary_select_dimension;
}

void PndLmdResolution::setPrimarySelectionDimension(
		LumiFit::LmdDimension primary_select_dimension_) {
	primary_select_dimension = primary_select_dimension_;
}

const LumiFit::LmdDimension& PndLmdResolution::getSecondarySelectionDimension() const {
	return secondary_select_dimension;
}

void PndLmdResolution::setSecondarySelectionDimension(
		LumiFit::LmdDimension secondary_select_dimension_) {
	secondary_select_dimension = secondary_select_dimension_;
}

bool PndLmdResolution::operator<(const PndLmdResolution &lmd_res) const {
	if (primary_select_dimension < lmd_res.primary_select_dimension)
		return true;
	else if (primary_select_dimension > lmd_res.primary_select_dimension)
		return false;
	if (secondary_select_dimension.is_active) {
		if (secondary_select_dimension < lmd_res.secondary_select_dimension)
			return true;
		else if (secondary_select_dimension > lmd_res.secondary_select_dimension)
			return false;
	}

	return PndLmdAbstractData::operator<(lmd_res);
}
bool PndLmdResolution::operator>(const PndLmdResolution &lmd_res) const {
	return (lmd_res < *this);
}
bool PndLmdResolution::operator==(const PndLmdResolution &lmd_res) const {
	if (primary_select_dimension != lmd_res.primary_select_dimension)
		return false;
	if (secondary_select_dimension.is_active) {
		if (secondary_select_dimension != lmd_res.secondary_select_dimension)
			return false;
	}

	return PndLmdAbstractData::operator==(lmd_res);
}
bool PndLmdResolution::operator!=(const PndLmdResolution &lmd_res) const {
	return !(*this == lmd_res);
}
