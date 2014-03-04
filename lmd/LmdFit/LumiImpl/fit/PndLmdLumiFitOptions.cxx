/*
 * PndLmdLumiFitOptions.cxx
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#include "PndLmdLumiFitOptions.h"
#include "PndLmdAcceptance.h"

#include <iostream>

ClassImp(PndLmdLumiFitOptions)

PndLmdLumiFitOptions::PndLmdLumiFitOptions() :
		model_binary_options(3), smearing_model_type(0), acc_intpol_type(1), acceptance(
				0) {
	primary_dimension_fit_range_active = false;
	secondary_dimension_fit_range_active = false;
	resolution_parametrization_file_url_active = false;
}

void PndLmdLumiFitOptions::initBinaryOptions(unsigned long bit_flag_options) {
	model_binary_options = LumiFit::LmdBinaryFitOptions(bit_flag_options);
}

const LumiFit::LmdBinaryFitOptions& PndLmdLumiFitOptions::getModelBinaryOptions() const {
	return model_binary_options;
}
void PndLmdLumiFitOptions::setModelBinaryOptions(
		LumiFit::LmdBinaryFitOptions model_binary_options_) {
	model_binary_options = model_binary_options_;
}

PndLmdAcceptance *PndLmdLumiFitOptions::getAcceptance() const {
	return acceptance;
}
void PndLmdLumiFitOptions::setAcceptance(PndLmdAcceptance *acceptance_) {
	acceptance = acceptance_;
}

int PndLmdLumiFitOptions::getSmearingModelType() const {
	return smearing_model_type;
}
void PndLmdLumiFitOptions::setSmearingModelType(int smearing_type_) {
	smearing_model_type = smearing_type_;
}
std::string PndLmdLumiFitOptions::getSmearingModelName() const {
	if (0 == smearing_model_type)
		return std::string("gaussian");
	else if (1 == smearing_model_type)
		return std::string("double gaussian");
	return std::string("unknown");
}

int PndLmdLumiFitOptions::getAcceptanceInterpolationType() const {
	return acc_intpol_type;
}
void PndLmdLumiFitOptions::setAcceptanceInterpolationType(
		int acc_intpol_type_) {
	acc_intpol_type = acc_intpol_type_;
}

unsigned int PndLmdLumiFitOptions::getFreeParametersCode() const {
	return free_parameters_code;
}
void PndLmdLumiFitOptions::setFreeParametersCode(
		unsigned int free_parameters_code_) {
	free_parameters_code = free_parameters_code_;
}

int PndLmdLumiFitOptions::getDpmElasticModelParts() const {
	return dpm_elastic_model_parts;
}
void PndLmdLumiFitOptions::setDpmElasticModelParts(
		int dpm_elastic_model_parts_) {
	dpm_elastic_model_parts = dpm_elastic_model_parts_;
}

bool PndLmdLumiFitOptions::isPrimaryDimensionFitRangeActive() const {
	return primary_dimension_fit_range_active;
}
bool PndLmdLumiFitOptions::isSecondaryDimensionFitRangeActive() const {
	return secondary_dimension_fit_range_active;
}

void PndLmdLumiFitOptions::setPrimaryDimensionFitRangeActive(
		bool primary_dimension_fit_range_active_) {
	primary_dimension_fit_range_active = primary_dimension_fit_range_active_;
}
void PndLmdLumiFitOptions::setSecondaryDimensionFitRangeActive(
		bool secondary_dimension_fit_range_active_) {
	secondary_dimension_fit_range_active =
			secondary_dimension_fit_range_active_;
}

TString PndLmdLumiFitOptions::getResolutionParametrizationFileUrl() const {
	return resolution_parametrization_file_url;
}

void PndLmdLumiFitOptions::setResolutionParametrizationFileUrl(
		TString resolution_parametrization_file_url_) {
	resolution_parametrization_file_url = resolution_parametrization_file_url_;
	resolution_parametrization_file_url_active = true;
}

bool PndLmdLumiFitOptions::isResolutionParametrizationFileUrlActive() const {
	return resolution_parametrization_file_url_active;
}

void PndLmdLumiFitOptions::setResolutionParametrizationFileUrlActive(
		bool resolution_parametrization_file_url_active_) {
	resolution_parametrization_file_url_active =
			resolution_parametrization_file_url_active_;
}

bool PndLmdLumiFitOptions::lessThanNonBinaryOptions(
		const PndLmdLumiFitOptions &rhs) const {
	// then fit ranges
	if (primary_dimension_fit_range_active) {
		if (primary_dimension_fit_range < rhs.getPrimaryDimensionFitRange()) {
			return true;
		} else if (primary_dimension_fit_range
				> rhs.getPrimaryDimensionFitRange()) {
			return false;
		}
	}
	if (secondary_dimension_fit_range_active) {
		if (secondary_dimension_fit_range < rhs.getPrimaryDimensionFitRange()) {
			return true;
		} else if (secondary_dimension_fit_range
				> rhs.getPrimaryDimensionFitRange()) {
			return false;
		}
	}
	// other stuff
	if (acc_intpol_type < rhs.getAcceptanceInterpolationType())
		return true;
	else if (acc_intpol_type > rhs.getAcceptanceInterpolationType())
		return false;
	if (smearing_model_type < rhs.getSmearingModelType())
		return true;
	else if (smearing_model_type > rhs.getSmearingModelType())
		return false;
	if (free_parameters_code < rhs.getFreeParametersCode())
		return true;
	else if (free_parameters_code > rhs.getFreeParametersCode())
		return false;

	return false;
}

bool PndLmdLumiFitOptions::operator<(const PndLmdLumiFitOptions &rhs) const {
	// check binary options first
	if (model_binary_options.getBinaryOptions()
			< rhs.getModelBinaryOptions().getBinaryOptions())
		return true;
	else if (model_binary_options.getBinaryOptions()
			> rhs.getModelBinaryOptions().getBinaryOptions())
		return false;

	return lessThanNonBinaryOptions(rhs);
}

bool PndLmdLumiFitOptions::operator>(const PndLmdLumiFitOptions &rhs) const {
	return (rhs < *this);
}

bool PndLmdLumiFitOptions::operator==(
		const PndLmdLumiFitOptions &fit_options) const {
	return ((*this < fit_options) == (*this > fit_options));
}

bool PndLmdLumiFitOptions::operator!=(
		const PndLmdLumiFitOptions &fit_options) const {
	return !(*this == fit_options);
}

std::ostream& operator<<(std::ostream& os,
		const PndLmdLumiFitOptions& fit_options) {

	os << "************************************************************"
			<< std::endl;
	os << "------------------------------------------------------------"
			<< std::endl;
	os << " Model options: " << std::endl;
	os << "------------------------------------------------------------"
			<< std::endl;
	os << "fit dimension: "
			<< fit_options.getModelBinaryOptions().getFitDimension()
			<< std::endl;
	if (fit_options.getModelBinaryOptions().getFitterType() == 0)
		os << "fit type: ROOT" << std::endl;
	else
		os << "fit type: ROOFIT" << std::endl;
	if (fit_options.getModelBinaryOptions().isAcceptanceCorrOn())
		os << "acceptance correction: on" << std::endl;
	else
		os << "acceptance correction: off" << std::endl;
	if (fit_options.getModelBinaryOptions().isSmearingOn()) {
		os << "resolution smearing: on" << std::endl;
		os << "using smearing model: " << fit_options.getSmearingModelName()
				<< std::endl;
	} else {
		os << "resolution smearing: off" << std::endl;
	}
	if (fit_options.getModelBinaryOptions().isFitRaw())
		os << "raw fit (mom. trans.): on" << std::endl;
	else
		os << "raw fit (mom. trans.): off" << std::endl;

	os << "primary dimension lower fit range: "
			<< fit_options.getPrimaryDimensionFitRange().getRangeLow()
			<< std::endl;
	os << "primary dimension upper fit range: "
			<< fit_options.getPrimaryDimensionFitRange().getRangeHigh()
			<< std::endl;
	os << "secondary dimension lower fit range: "
			<< fit_options.getSecondaryDimensionFitRange().getRangeLow()
			<< std::endl;
	os << "secondary dimension upper fit range: "
			<< fit_options.getSecondaryDimensionFitRange().getRangeHigh()
			<< std::endl;
	os << "************************************************************"
			<< std::endl;
	return os;
}

LumiFit::LmdDimensionRange PndLmdLumiFitOptions::getPrimaryDimensionFitRange() const {
	return primary_dimension_fit_range;
}

LumiFit::LmdDimensionRange PndLmdLumiFitOptions::getSecondaryDimensionFitRange() const {
	return secondary_dimension_fit_range;
}

void PndLmdLumiFitOptions::setPrimaryDimensionFitRange(
		LumiFit::LmdDimensionRange primary_dimension_fit_range_) {
	primary_dimension_fit_range = primary_dimension_fit_range_;
}

void PndLmdLumiFitOptions::setSecondaryDimensionFitRange(
		LumiFit::LmdDimensionRange secondary_dimension_fit_range_) {
	secondary_dimension_fit_range = secondary_dimension_fit_range_;
}

