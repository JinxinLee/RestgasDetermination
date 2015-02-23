/*
 * ParametrizationModel.cxx
 *
 *  Created on: Apr 2, 2013
 *      Author: steve
 */

#include "ParametrizationModel.h"
#include "Model.h"

ParametrizationModel::ParametrizationModel(shared_ptr<Model> model_) :
		model(model_), model_par() {
}

ParametrizationModel::~ParametrizationModel() {
	// TODO Auto-generated destructor stub
}

void ParametrizationModel::parametrize(const double *x) {
	model_par->setValue(model->evaluate(x));
}

void ParametrizationModel::setModelPar(shared_ptr<ModelPar> model_par_) {
	model_par = model_par_;
}

const shared_ptr<ModelPar> ParametrizationModel::getModelPar() const {
	return model_par;
}

shared_ptr<Model> ParametrizationModel::getModel() {
	return model;
}
