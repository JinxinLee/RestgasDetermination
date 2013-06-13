/*
 * ModelMinimizationControlParameter.cxx
 *
 *  Created on: Jun 6, 2013
 *      Author: steve
 */

#include "ModelMinimizationControlParameter.h"
#include "Model.h"

ModelMinimizationControlParameter::ModelMinimizationControlParameter() {
	// TODO Auto-generated constructor stub

}

ModelMinimizationControlParameter::~ModelMinimizationControlParameter() {
	// TODO Auto-generated destructor stub
}

double ModelMinimizationControlParameter::evaluate(const double *pars) {
	return model->eval(pars);
}
