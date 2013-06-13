/*
 * ModelEstimator.cxx
 *
 *  Created on: Jun 5, 2013
 *      Author: steve
 */

#include "ModelEstimator.h"
#include "Model.h"
#include "ModelPar.h"

ModelEstimator::ModelEstimator() {
	// TODO Auto-generated constructor stub
	binning_factor = 1.0;
}

ModelEstimator::~ModelEstimator() {
	// TODO Auto-generated destructor stub
}

unsigned int ModelEstimator::getNumberOfDataPoints() const {
	return data_points.size();
}

const shared_ptr<Model> ModelEstimator::getModel() const {
	return fit_model;
}

void ModelEstimator::setModel(shared_ptr<Model> new_model) {
	fit_model = new_model;

	// get list of all free parameters
	getParameterList().clear();
	free_parameters.clear();
	free_parameters = fit_model->getModelParameterSet().getFreeModelParameters();
	insertParameters();
}

void ModelEstimator::setBinningFactor(double binning_factor_) {
	binning_factor = binning_factor_;
}

void ModelEstimator::clearData() {
	data_points.clear();
}

void ModelEstimator::insertData(
		std::vector<ModelStructs::data_point> &data_points_) {
	for (unsigned int i = 0; i < data_points_.size(); i++) {
		insertData(data_points_[i]);
	}
}
void ModelEstimator::insertData(ModelStructs::data_point &data_point_) {
	data_points.push_back(data_point_);
}

void ModelEstimator::insertParameters() {
	for (std::vector<shared_ptr<ModelPar> >::iterator it =
			free_parameters.begin(); it != free_parameters.end(); it++) {
		getParameterList().push_back(
				ModelStructs::minimization_parameter((*it)->getName(),
						(*it)->getValue(), 0.0));
	}
}

void ModelEstimator::updateFreeModelParameters(const double *new_values) {
	int counter = 0;
	// first overwrite the corresponding parameter values
	// loop over the parameter set and update all the free parameters with these values
	for (std::vector<shared_ptr<ModelPar> >::iterator it =
			free_parameters.begin(); it != free_parameters.end(); it++) {
		(*it)->setValue(new_values[counter]);
		counter++;
	}
}

double ModelEstimator::evaluate(const double *par) {
	/* This point is crucial: because this method is called for every iteration
	 * of the fitter, the "newly changed" parameters have to be updated in the
	 * model so that the changes are actually registered.
	 */
	updateFreeModelParameters(par);

	return eval();
}
