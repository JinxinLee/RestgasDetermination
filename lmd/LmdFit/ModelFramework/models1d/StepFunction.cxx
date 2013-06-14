/*
 * StepFunction.cxx
 *
 *  Created on: Jun 14, 2013
 *      Author: steve
 */

#include "StepFunction.h"

#include <limits>

StepFunction::StepFunction(bool falling_edge_) :
		falling_edge(falling_edge_) {
	// TODO Auto-generated constructor stub

}

StepFunction::~StepFunction() {
	// TODO Auto-generated destructor stub
}

double StepFunction::eval(const double *x) const {
	if (falling_edge) {
		if (x[0] < edge->getValue())
			return amplitude->getValue();
	} else {
		if (x[0] > edge->getValue())
			return amplitude->getValue();
	}
	return 0.0;
}

void StepFunction::initModelParameters() {
	amplitude = getModelParameterSet().addModelParameter("amplitude");
	amplitude->setValue(1.0);
	amplitude->setParameterFixed(true);
	edge = getModelParameterSet().addModelParameter("edge");
}

void StepFunction::updateDomain() {
	if (falling_edge)
		setDomain(std::numeric_limits<int>::min(), edge->getValue());
	else
		setDomain(edge->getValue(), std::numeric_limits<int>::max());
}
