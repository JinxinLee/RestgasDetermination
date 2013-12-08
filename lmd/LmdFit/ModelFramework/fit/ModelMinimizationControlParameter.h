/*
 * ModelMinimizationControlParameter.h
 *
 *  Created on: Jun 6, 2013
 *      Author: steve
 */

#ifndef MODELMINIMIZATIONCONTROLPARAMETER_H_
#define MODELMINIMIZATIONCONTROLPARAMETER_H_

#include "ModelControlParameter.h"

#ifdef HAS_SHAREDPOINTER
#include <memory>
using std::shared_ptr;
#else
#include <tr1/memory>
using std::tr1::shared_ptr;
#endif

class Model;


class ModelMinimizationControlParameter: public ModelControlParameter {
	private:
	shared_ptr<Model> model;
public:
	ModelMinimizationControlParameter();
	virtual ~ModelMinimizationControlParameter();

  double evaluate(const double *pars);
};

#endif /* MODELMINIMIZATIONCONTROLPARAMETER_H_ */
