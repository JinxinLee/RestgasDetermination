/*
 * ModelFitResult.cxx
 *
 *  Created on: May 23, 2013
 *      Author: steve
 */

#include "ModelFitResult.h"

#include <iostream>

ModelFitResult::ModelFitResult() {
	fit_status = -1;
}

ModelFitResult::~ModelFitResult() {
	// TODO Auto-generated destructor stub
}

double ModelFitResult::getChiSquare() const {
	return chi2;
}

unsigned int ModelFitResult::getNDF() const {
	return ndf;
}

void ModelFitResult::setChiSquare(double chi2_) {
	this->chi2 = chi2_;
}

void ModelFitResult::setNDF(unsigned int ndf_) {
	this->ndf = ndf_;
}

void ModelFitResult::addFitParameter(std::string name_, double value_,
		double error_) {
	fit_parameter fp(name_, value_, error_);
	fit_parameters.insert(fp);
}

const ModelFitResult::fit_parameter& ModelFitResult::getFitParameter(
		std::string name_) const {
	fit_parameter fp(name_);
	if (fit_parameters.find(fp) == fit_parameters.end()) {
		std::cout << "ERROR: requesting value of parameter " << name_
				<< " which is unknown!" << std::endl;
	}
	return *fit_parameters.find(fp);
}

int ModelFitResult::getFitStatus() const
{
    return fit_status;
}

void ModelFitResult::setFitStatus(int fit_status_)
{
    fit_status = fit_status_;
}


