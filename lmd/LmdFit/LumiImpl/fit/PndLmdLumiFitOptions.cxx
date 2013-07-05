/*
 * PndLmdLumiFitOptions.cxx
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#include "PndLmdLumiFitOptions.h"
#include "PndLmdDPMAngModel1D.h"
#include "PndLmdDPMModelParametrization.h"

#include <iostream>

ClassImp(PndLmdLumiFitOptions)

PndLmdLumiFitOptions::PndLmdLumiFitOptions(unsigned long bit_flag_options,
		int smearing_type_, int acc_intpol_type_) :
		binary_options(bit_flag_options), smearing_type(smearing_type_), acc_intpol_type(
				acc_intpol_type_) {
}

PndLmdLumiFitOptions::PndLmdLumiFitOptions(unsigned long bit_flag_options,
		unsigned int free_parameters_code_, int smearing_type_,
		int acc_intpol_type_, double plab, double theta_fit_range_low_,
		double theta_fit_range_high_, double phi_fit_range_low_,
		double phi_fit_range_high_) :
		binary_options(bit_flag_options), free_parameters_code(
				free_parameters_code_), smearing_type(smearing_type_), acc_intpol_type(
				acc_intpol_type_) {
	theta_fit_range_low = theta_fit_range_low_;
	theta_fit_range_high = theta_fit_range_high_;
	phi_fit_range_low = phi_fit_range_low_;
	phi_fit_range_high = phi_fit_range_high_;

	// calculate t range
	PndLmdDPMAngModel1D model("dpm_angular_1d");
	shared_ptr<Parametrization> para(
			new PndLmdDPMModelParametrization(model.getModelParameterSet()));
	model.getModelParameterHandler().registerParametrizations(
			model.getModelParameterSet(), para);
	model.getModelParameterSet().setModelParameterValue("p_lab", plab);
	((Model1D*) &model)->init();
	t_fit_range_low = -model.getMomentumTransferFromTheta(theta_fit_range_low);
	t_fit_range_high = -model.getMomentumTransferFromTheta(theta_fit_range_high);
}

PndLmdLumiFitOptions::PndLmdLumiFitOptions() :
		binary_options(0), smearing_type(0), acc_intpol_type(1) {
}

int PndLmdLumiFitOptions::getSmearingModelType() const {
	return smearing_type;
}

std::string PndLmdLumiFitOptions::getSmearingModelName() const {
	if (0 == smearing_type)
		return std::string("gaussian");
	else if (1 == smearing_type)
		return std::string("double gaussian");
	return std::string("unknown");
}

bool PndLmdLumiFitOptions::getFitDimension() const {
	return binary_options & (1 << (2));
}

bool PndLmdLumiFitOptions::getFitterType() const {
	return binary_options & (1 << (4));
}

bool PndLmdLumiFitOptions::isSmearingOn() const {
	return binary_options & 1;
}

bool PndLmdLumiFitOptions::isAcceptanceCorrOn() const {
	return binary_options & (1 << (1));
}

bool PndLmdLumiFitOptions::isFitRaw() const {
	return binary_options & (1 << (3));
}

int PndLmdLumiFitOptions::getAcceptanceInterpolationType() const {
	return acc_intpol_type;
}

unsigned long PndLmdLumiFitOptions::getBinaryOptions() const {
	return binary_options;
}

unsigned int PndLmdLumiFitOptions::getFreeParametersCode() const {
	return free_parameters_code;
}

double PndLmdLumiFitOptions::getThetaFitRangeLow() const {
	return theta_fit_range_low;
}
double PndLmdLumiFitOptions::getThetaFitRangeHigh() const {
	return theta_fit_range_high;
}
double PndLmdLumiFitOptions::getPhiFitRangeLow() const {
	return phi_fit_range_low;
}
double PndLmdLumiFitOptions::getPhiFitRangeHigh() const {
	return phi_fit_range_high;
}

double PndLmdLumiFitOptions::getTFitRangeLow() const {
	return t_fit_range_low;
}
double PndLmdLumiFitOptions::getTFitRangeHigh() const {
	return t_fit_range_high;
}

void PndLmdLumiFitOptions::setAcceptanceInterpolationType(
		int acc_intpol_type_) {
	acc_intpol_type = acc_intpol_type_;
}

void PndLmdLumiFitOptions::setSmearingModelType(int smearing_type_) {
	smearing_type = smearing_type_;
}

void PndLmdLumiFitOptions::setFitAsRaw(bool use_raw_) {
	if (use_raw_)
		binary_options |= 1 << 4;
	else
		binary_options &= ~(1 << 4);
}

void PndLmdLumiFitOptions::setFitDimension(bool fit_dimension_) {
	if (fit_dimension_)
		binary_options |= 1 << 3;
	else
		binary_options &= ~(1 << 3);
}

void PndLmdLumiFitOptions::setFitterType(bool fitter_type_) {
	if (fitter_type_)
		binary_options |= 1 << 2;
	else
		binary_options &= ~(1 << 2);
}

void PndLmdLumiFitOptions::setSmearingMode(bool with_smearing_) {
	if (with_smearing_)
		binary_options |= 1;
	else
		binary_options &= ~1;
}

void PndLmdLumiFitOptions::setAcceptanceCorrMode(bool with_acceptance_corr_) {
	if (with_acceptance_corr_)
		binary_options |= 1 << 1;
	else
		binary_options &= ~(1 << 1);
}

void PndLmdLumiFitOptions::setFreeParametersOfModel(
		unsigned int free_parameters_code_) {
	free_parameters_code = free_parameters_code_;
}

void PndLmdLumiFitOptions::setThetaFitRange(const double fit_range_low_,
		const double fit_range_high_) {
	theta_fit_range_low = fit_range_low_;
	theta_fit_range_high = fit_range_high_;
}

bool PndLmdLumiFitOptions::operator==(
		const PndLmdLumiFitOptions &fit_options) const {
	if (binary_options != fit_options.getBinaryOptions())
		return false;
	if (theta_fit_range_low != fit_options.getThetaFitRangeLow())
		return false;
	if (theta_fit_range_high != fit_options.getThetaFitRangeHigh())
		return false;
	if (phi_fit_range_low != fit_options.getPhiFitRangeLow())
		return false;
	if (phi_fit_range_high != fit_options.getPhiFitRangeHigh())
		return false;

	return true;
}

bool PndLmdLumiFitOptions::operator!=(
		const PndLmdLumiFitOptions &fit_options) const {
	return !(*this == fit_options);
}

std::ostream& operator<<(std::ostream& os,
		const PndLmdLumiFitOptions& fit_options) {

	os << "************************************************************"
			<< std::endl;
	os << "fit dimension: " << fit_options.getFitDimension() << std::endl;
	if (fit_options.getFitterType() == 0)
		os << "fit type: ROOT" << std::endl;
	else
		os << "fit type: ROOFIT" << std::endl;
	if (fit_options.isAcceptanceCorrOn())
		os << "acceptance correction: on" << std::endl;
	else
		os << "acceptance correction: off" << std::endl;
	if (fit_options.isSmearingOn()) {
		os << "resolution smearing: on" << std::endl;
		os << "using smearing model: " << fit_options.getSmearingModelName()
				<< std::endl;
	} else {
		os << "resolution smearing: off" << std::endl;
	}
	if (fit_options.isFitRaw())
		os << "raw fit (mom. trans.): on" << std::endl;
	else
		os << "raw fit (mom. trans.): off" << std::endl;

	os << "lower theta fit range: " << fit_options.getThetaFitRangeLow()
			<< std::endl;
	os << "upper theta fit range: " << fit_options.getThetaFitRangeHigh()
			<< std::endl;
	os << "lower phi fit range: " << fit_options.getPhiFitRangeLow() << std::endl;
	os << "upper phi fit range: " << fit_options.getPhiFitRangeHigh()
			<< std::endl;
	os << "************************************************************"
			<< std::endl;
	return os;
}

TString PndLmdLumiFitOptions::makeName() const {
	char tempc[30];
	sprintf(tempc, "%iD", getFitDimension());
	TString name = tempc;

	if (getFitterType() == 0)
		name = name + "_ROOT";
	else
		name = name + "_ROOFIT";
	if (isAcceptanceCorrOn())
		name = name + "_acc-corr";
	if (isSmearingOn())
		name = name + "_res-smeared";
	if (isFitRaw())
		name = name + "_raw-fit";

	sprintf(tempc, "_th=%f-%f", getThetaFitRangeLow(), getThetaFitRangeHigh());
	name = name + tempc;
	sprintf(tempc, "_phi=%f-%f", getPhiFitRangeLow(), getPhiFitRangeHigh());
	name = name + tempc;

	return name;
}
