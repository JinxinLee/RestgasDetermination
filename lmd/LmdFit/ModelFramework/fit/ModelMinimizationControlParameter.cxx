/*
 * ModelMinimizationControlParameter.cxx
 *
 *  Created on: Jun 6, 2013
 *      Author: steve
 */

#include "ModelMinimizationControlParameter.h"
#include "core/Model.h"

ModelMinimizationControlParameter::ModelMinimizationControlParameter() : model() {
}

ModelMinimizationControlParameter::~ModelMinimizationControlParameter() {
	// TODO Auto-generated destructor stub
}

double ModelMinimizationControlParameter::evaluate(const double *pars) {
	return model->eval(pars);
}
