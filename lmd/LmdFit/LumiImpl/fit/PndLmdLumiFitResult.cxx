/*
 * PndLmdLumiFitResult.cxx
 *
 *  Created on: Jun 28, 2012
 *      Author: steve
 */

#include "PndLmdLumiFitResult.h"
#include "PndLmdLumiFitOptions.h"
#include "ModelFitResult.h"


ClassImp(PndLmdLumiFitResult)

PndLmdLumiFitResult::PndLmdLumiFitResult(PndLmdLumiFitOptions *fit_options_) :
		fit_options(fit_options_) {
	model_fit_result = new ModelFitResult();
}

PndLmdLumiFitResult::PndLmdLumiFitResult() :
		fit_options(0) {
}

PndLmdLumiFitResult::~PndLmdLumiFitResult() {
}

double PndLmdLumiFitResult::getLuminosity() const {
	return model_fit_result->getFitParameter("luminosity").value;
}
double PndLmdLumiFitResult::getLuminositySysError() const {
	return luminosity_sys_err;
}
double PndLmdLumiFitResult::getLuminosityStatError() const {
	return model_fit_result->getFitParameter("luminosity").error;
}
double PndLmdLumiFitResult::getLuminosityError() const {
	return luminosity_sys_err
			+ model_fit_result->getFitParameter("luminosity").error;
}

ModelFitResult* PndLmdLumiFitResult::getModelFitResult() {
	return model_fit_result;
}

double PndLmdLumiFitResult::getRedChiSquare() const {
	return model_fit_result->getChiSquare() / model_fit_result->getNDF();
}

void PndLmdLumiFitResult::setFitStatus(int fit_status_) {
	model_fit_result->setFitStatus(fit_status_);
}
void PndLmdLumiFitResult::setChiSquare(double chi_square_) {
	model_fit_result->setChiSquare(chi_square_);
}
void PndLmdLumiFitResult::addFitParameter(std::string name_, double value_,
		double error_) {
	model_fit_result->addFitParameter(name_, value_, error_);
}
void PndLmdLumiFitResult::setNDF(double ndf_) {
	model_fit_result->setNDF(ndf_);
}

void PndLmdLumiFitResult::setLuminositySysError(double luminosity_sys_err_) {
	luminosity_sys_err = luminosity_sys_err_;
}

bool PndLmdLumiFitResult::checkFitOptions(
		PndLmdLumiFitOptions *fit_options_) const {
	return (*fit_options == *fit_options_);
}

PndLmdLumiFitOptions* PndLmdLumiFitResult::getLumiFitOptions() const {
	return fit_options;
}
