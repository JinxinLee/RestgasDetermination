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

ModelFitResult::ModelFitResult(const ModelFitResult &fit_result) {
	setFitStatus(fit_result.getFitStatus());
	setChiSquare(fit_result.getChiSquare());
	setNDF(fit_result.getNDF());
	for (std::set<ModelStructs::minimization_parameter>::const_iterator fit_param =
			fit_result.getFitParameters().begin();
			fit_param != fit_result.getFitParameters().end(); fit_param++) {
		addFitParameter(fit_param->name, fit_param->value, fit_param->error);
	}
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

void ModelFitResult::addFitParameter(std::pair<std::string, std::string> name_
		, double value_,
		double error_) {
	ModelStructs::minimization_parameter fp(name_, value_, error_);
	fit_parameters.insert(fp);
}

const ModelStructs::minimization_parameter ModelFitResult::getFitParameter(
		std::string name_) const {
	for (std::set<ModelStructs::minimization_parameter>::const_iterator min_param =
			fit_parameters.begin(); min_param != fit_parameters.end(); min_param++) {
		if (min_param->name.second.compare(name_) == 0) {
			return *min_param;
		}
	}
	std::cout << "ERROR: requesting value of superior parameter " << name_
			<< " which is unknown!" << std::endl;
	return *fit_parameters.end();
}

const ModelStructs::minimization_parameter& ModelFitResult::getFitParameter(
		std::pair<std::string, std::string> name_) const {
	ModelStructs::minimization_parameter fp(name_);
	if (fit_parameters.find(fp) == fit_parameters.end()) {
		std::cout << "ERROR: requesting value of parameter " << name_.first << ":"
				<< name_.second << " which is unknown!" << std::endl;
	}
	return *fit_parameters.find(fp);
}

const std::set<ModelStructs::minimization_parameter>& ModelFitResult::getFitParameters() const {
	return fit_parameters;
}

int ModelFitResult::getFitStatus() const {
	return fit_status;
}

void ModelFitResult::setFitStatus(int fit_status_) {
	fit_status = fit_status_;
}

