/*
 * ModelMinimizer.cxx
 *
 *  Created on: Jun 5, 2013
 *      Author: steve
 */

#include "ModelMinimizer.h"

ModelMinimizer::ModelMinimizer(ModelControlParameter &control_param_) :
		control_param(control_param_) {
	// TODO Auto-generated constructor stub

}

ModelMinimizer::~ModelMinimizer() {
	// TODO Auto-generated destructor stub
}

int ModelMinimizer::doMinimization() {
	// then apply minimization procedure
	if (control_param.getParameterList().size() > 0) {
		return minimize();
	} else {
		return -1;
	}
}
