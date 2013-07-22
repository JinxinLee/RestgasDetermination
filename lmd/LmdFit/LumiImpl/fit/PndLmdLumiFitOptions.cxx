/*
 * PndLmdLumiFitOptions.cxx
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#include "PndLmdLumiFitOptions.h"
#include "PndLmdDPMAngModel1D.h"
#include "PndLmdDPMModelParametrization.h"
#include "PndLmdLumiHelper.h"

#include <iostream>

#include "TFile.h"

ClassImp(PndLmdLumiFitOptions)

PndLmdLumiFitOptions::PndLmdLumiFitOptions(
		LumiFit::LmdBinaryFitOptions bit_flag_options, int smearing_type_,
		int acc_intpol_type_) :
		model_binary_options(bit_flag_options), data_binary_options(
				bit_flag_options), smearing_type(smearing_type_), acc_intpol_type(
				acc_intpol_type_) {
}

PndLmdLumiFitOptions::PndLmdLumiFitOptions(
		LumiFit::LmdBinaryFitOptions bit_flag_options,
		unsigned int free_parameters_code_, int smearing_type_,
		int acc_intpol_type_, double plab, double theta_fit_range_low_,
		double theta_fit_range_high_, double phi_fit_range_low_,
		double phi_fit_range_high_) :
		model_binary_options(bit_flag_options), data_binary_options(
				bit_flag_options), free_parameters_code(free_parameters_code_), smearing_type(
				smearing_type_), acc_intpol_type(acc_intpol_type_) {
	theta_fit_range_low = theta_fit_range_low_;
	theta_fit_range_high = theta_fit_range_high_;
	phi_fit_range_low = phi_fit_range_low_;
	phi_fit_range_high = phi_fit_range_high_;

	// calculate t range
	t_fit_range_low = PndLmdLumiHelper::getMomentumTransferFromTheta(plab,
			theta_fit_range_low);
	t_fit_range_high = PndLmdLumiHelper::getMomentumTransferFromTheta(plab,
			theta_fit_range_high);
}

PndLmdLumiFitOptions::PndLmdLumiFitOptions() :
		model_binary_options(0), data_binary_options(0), smearing_type(0), acc_intpol_type(
				1) {
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

int PndLmdLumiFitOptions::getAcceptanceInterpolationType() const {
	return acc_intpol_type;
}

const LumiFit::LmdBinaryFitOptions& PndLmdLumiFitOptions::getModelBinaryOptions() const {
	return model_binary_options;
}

const LumiFit::LmdBinaryFitOptions& PndLmdLumiFitOptions::getDataBinaryOptions() const {
	return data_binary_options;
}

LumiFit::LmdBinaryFitOptions& PndLmdLumiFitOptions::getDataBinaryOptions() {
	return data_binary_options;
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

void PndLmdLumiFitOptions::setFreeParametersOfModel(
		unsigned int free_parameters_code_) {
	free_parameters_code = free_parameters_code_;
}

TString PndLmdLumiFitOptions::getResolutionParametrizationFileUrl() const {

	return resolution_parametrization_file_url;
}

void PndLmdLumiFitOptions::setResolutionParametrizationFileUrl(
		TString resolution_parametrization_file_url_) {
	resolution_parametrization_file_url = resolution_parametrization_file_url_;
}

void PndLmdLumiFitOptions::setThetaFitRange(const double fit_range_low_,
		const double fit_range_high_) {
	theta_fit_range_low = fit_range_low_;
	theta_fit_range_high = fit_range_high_;
}

bool PndLmdLumiFitOptions::operator==(
		const PndLmdLumiFitOptions &fit_options) const {
	if (model_binary_options.getBinaryOptions()
			!= fit_options.getModelBinaryOptions().getBinaryOptions())
		return false;
	if (data_binary_options.getBinaryOptions()
			!= fit_options.getDataBinaryOptions().getBinaryOptions())
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
	os << "------------------------------------------------------------"
			<< std::endl;
	os << " Data options: " << std::endl;
	os << "------------------------------------------------------------"
			<< std::endl;
	os << "fit dimension: "
			<< fit_options.getDataBinaryOptions().getFitDimension() << std::endl;
	if (fit_options.getDataBinaryOptions().getFitterType() == 0)
		os << "fit type: ROOT" << std::endl;
	else
		os << "fit type: ROOFIT" << std::endl;
	if (fit_options.getDataBinaryOptions().isAcceptanceCorrOn())
		os << "acceptance correction: on" << std::endl;
	else
		os << "acceptance correction: off" << std::endl;
	if (fit_options.getDataBinaryOptions().isSmearingOn()) {
		os << "resolution smearing: on" << std::endl;
		os << "using smearing model: " << fit_options.getSmearingModelName()
				<< std::endl;
	} else {
		os << "resolution smearing: off" << std::endl;
	}
	if (fit_options.getDataBinaryOptions().isFitRaw())
		os << "raw fit (mom. trans.): on" << std::endl;
	else
		os << "raw fit (mom. trans.): off" << std::endl;

	os << "------------------------------------------------------------"
			<< std::endl;
	os << " Model options: " << std::endl;
	os << "------------------------------------------------------------"
			<< std::endl;
	os << "fit dimension: "
			<< fit_options.getModelBinaryOptions().getFitDimension() << std::endl;
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
	sprintf(tempc, "%iD", getModelBinaryOptions().getFitDimension());
	TString name = tempc;

	if (getModelBinaryOptions().getFitterType() == 0)
		name = name + "_ROOT";
	else
		name = name + "_ROOFIT";
	if (getModelBinaryOptions().isAcceptanceCorrOn())
		name = name + "_acc-corr";
	if (getModelBinaryOptions().isSmearingOn())
		name = name + "_res-smeared";
	if (getModelBinaryOptions().isFitRaw())
		name = name + "_raw-fit";

	sprintf(tempc, "_th=%f-%f", getThetaFitRangeLow(), getThetaFitRangeHigh());
	name = name + tempc;
	sprintf(tempc, "_phi=%f-%f", getPhiFitRangeLow(), getPhiFitRangeHigh());
	name = name + tempc;

	return name;
}
