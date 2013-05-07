/*
 * PndLmdLumiFitResult.cxx
 *
 *  Created on: Jun 28, 2012
 *      Author: steve
 */

#include "PndLmdLumiFitResult.h"
#include "PndLmdLumiFitOptions.h"

#include "TF1.h"
#include "TF2.h"
#include "RooAbsPdf.h"

ClassImp(PndLmdLumiFitResult)

PndLmdLumiFitResult::PndLmdLumiFitResult(PndLmdLumiFitOptions *fit_options_) {
	fit_options = fit_options_;
	init();
}

PndLmdLumiFitResult::PndLmdLumiFitResult() {
	fit_options = NULL;
	init();
}

PndLmdLumiFitResult::~PndLmdLumiFitResult() {
}

void PndLmdLumiFitResult::init() {
	luminosity = 0.0;
	luminosity_sys_err = 0.0;
	luminosity_stat_err = 0.0;
	chi2 = 0.0;
	ndf = 0;
}

double PndLmdLumiFitResult::getLuminosity() const {
	return luminosity;
}
double PndLmdLumiFitResult::getLuminositySysError() const {
	return luminosity_sys_err;
}
double PndLmdLumiFitResult::getLuminosityStatError() const {
	return luminosity_stat_err;
}
double PndLmdLumiFitResult::getLuminosityError() const {
	return luminosity_sys_err + luminosity_stat_err;
}

double PndLmdLumiFitResult::getChiSquare() const {
	return chi2;
}
unsigned int PndLmdLumiFitResult::getNDF() const {
	return ndf;
}

double PndLmdLumiFitResult::getParameterValue(std::string name_) const {
	if (parameters.find(name_) == parameters.end()) {
		std::cout << "ERROR: requesting value of parameter " << name_
				<< " which is unknown!" << std::endl;
		return 0.0;
	}
	return parameters.find(name_)->second.first;
}

void PndLmdLumiFitResult::setChiSquare(double chi2_) {
	chi2 = chi2_;
}
void PndLmdLumiFitResult::setNDF(unsigned int ndf_) {
	ndf = ndf_;
}

void PndLmdLumiFitResult::addParameterValue(std::string name_, double value_,
		double value_error_) {
	parameters[name_] = std::make_pair(value_, value_error_);
}

void PndLmdLumiFitResult::setLuminosity(double luminosity_) {
	luminosity = luminosity_;
}
void PndLmdLumiFitResult::setLuminositySysError(double luminosity_sys_err_) {
	luminosity_sys_err = luminosity_sys_err_;
}
void PndLmdLumiFitResult::setLuminosityStatError(double luminosity_stat_err_) {
	luminosity_stat_err = luminosity_stat_err_;
}

bool PndLmdLumiFitResult::checkFitOptions(
		PndLmdLumiFitOptions *fit_options_) const {
	return (*fit_options == *fit_options_);
}

PndLmdLumiFitOptions* PndLmdLumiFitResult::getLumiFitOptions() const {
	return fit_options;
}
