/*
 * PndLmdLumiFitOptions.cxx
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#include "PndLmdLumiFitOptions.h"

#include <iostream>

ClassImp(PndLmdLumiFitOptions)

PndLmdLumiFitOptions::PndLmdLumiFitOptions() :
		estimator_type(LumiFit::LOG_LIKELIHOOD) {
}

const LumiFit::PndLmdFitModelOptions& PndLmdLumiFitOptions::getFitModelOptions() const {
	return model_opt;
}

const EstimatorOptions& PndLmdLumiFitOptions::getEstimatorOptions() const {
	return est_opt;
}

const std::set<std::string, ModelStructs::string_comp>& PndLmdLumiFitOptions::getFreeParameterSet() const {
	return free_parameter_names;
}

bool PndLmdLumiFitOptions::operator<(const PndLmdLumiFitOptions &rhs) const {
	// check binary options first
	if (model_opt < rhs.getFitModelOptions())
		return true;
	else if (model_opt > rhs.getFitModelOptions())
		return false;

	if (getEstimatorOptions() < rhs.getEstimatorOptions())
		return true;
	else if (getEstimatorOptions() > rhs.getEstimatorOptions())
		return false;

	if (free_parameter_names < rhs.getFreeParameterSet())
		return true;
	else if (free_parameter_names > rhs.getFreeParameterSet())
		return false;

	return false;
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
	os << "fit dimension: " << fit_options.getFitModelOptions().fit_dimension
			<< std::endl;
	if (fit_options.getFitModelOptions().momentum_transfer_active)
		os << "raw fit (mom. trans.): on" << std::endl;
	else
		os << "raw fit (mom. trans.): off" << std::endl;
	if (fit_options.getFitModelOptions().acceptance_correction_active)
		os << "acceptance correction: on" << std::endl;
	else
		os << "acceptance correction: off" << std::endl;
	if (fit_options.getFitModelOptions().resolution_smearing_active) {
		os << "resolution smearing: on" << std::endl;
	} else {
		os << "resolution smearing: off" << std::endl;
	}

	os << std::endl;
	os << "smearing model: " << fit_options.getFitModelOptions().smearing_model
			<< std::endl;
	if (fit_options.getFitModelOptions().use_resolution_parameter_interpolation) {
		os << "linear smearing model interpolation: on" << std::endl;
	} else
		os << "linear smearing model interpolation: off" << std::endl;

	os << "resolution parametrization file url: "
			<< fit_options.getFitModelOptions().resolution_parametrization_file_url
			<< std::endl;
	os << std::endl;

	os << fit_options.getEstimatorOptions();
	os << "************************************************************"
			<< std::endl;
	return os;
}
