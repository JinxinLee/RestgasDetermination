/*
 * ModelFitInterface.cxx
 *
 *  Created on: May 2, 2013
 *      Author: steve
 */

#include "ModelFitInterface.h"
#include "Model.h"
#include "ModelPar.h"

#include <iostream>

ModelFitInterface::ModelFitInterface() {
	// TODO Auto-generated constructor stub
	binning_factor = 1.0;
}

ModelFitInterface::~ModelFitInterface() {
	// TODO Auto-generated destructor stub
}

void ModelFitInterface::setModel(shared_ptr<Model> new_model) {
	model = new_model;
}

void ModelFitInterface::setBinningFactor(double binning_factor_) {
	binning_factor = binning_factor_;
}

void ModelFitInterface::clearData() {
	data_points.clear();
}
void ModelFitInterface::insertData(std::vector<data_point> &data_points_) {
	for (unsigned int i = 0; i < data_points_.size(); i++) {
		insertData(data_points_[i]);
	}
}
void ModelFitInterface::insertData(data_point &data_point_) {
	data_points.push_back(data_point_);
}

unsigned int ModelFitInterface::getNumberOfDataPoints() const {
	return data_points.size();
}

std::vector<shared_ptr<ModelPar> >& ModelFitInterface::getFreeParameterList() {
	return free_parameters;
}

void ModelFitInterface::updateFreeModelParameters(const double *new_values) {
	int counter = 0;
	// first overwrite the corresponding parameter values
	// loop over the parameter set and update all the free parameters with these values
	for (std::vector<shared_ptr<ModelPar> >::iterator it =
			free_parameters.begin(); it != free_parameters.end(); it++) {
		// if parameter is free
		if (!(*it)->isParameterFixed()) {
			(*it)->setValue(new_values[counter]);
			counter++;
		}
	}
}

double ModelFitInterface::chi2(const double *par) {
	/* This point is crucial: because this method is called for every iteration
	 * of the fitter, the "newly changed" parameters have to be updated in the
	 * model so that the changes are actually registered.
	 */
	updateFreeModelParameters(par);

	//calculate chisquare
	double chisq = 0.0;
	double delta;

	// loop over data
	for (unsigned int i = 0; i < data_points.size(); i++) {
		delta = (data_points[i].z
				- binning_factor * model->evaluate(data_points[i].x))
				/ data_points[i].z_error;
		chisq += delta * delta;
	}
	//std::cout << "chi2: " << chisq << std::endl;
	return chisq;
}

int ModelFitInterface::doFit() {
	// get list of all free parameters
	free_parameters.clear();
	free_parameters =
			model->getModelParameterSet().getFreeModelParameters();

	// then apply minimization procedure
	if (free_parameters.size() > 0) {
		return minimize();
	} else {
		return 2;
	}
}

