/*
 * PndLmdDPMMTModel2D.h
 *
 *  Created on: Jan 17, 2013
 *      Author: steve
 */

#ifndef PNDLMDDPMMODEL2D_H_
#define PNDLMDDPMMODEL2D_H_

#include "core/Model2D.h"
#include "PndLmdDPMMTModel1D.h"

class PndLmdDPMMTModel2D: public Model2D {
	PndLmdDPMMTModel1D dpm_model_1d;
public:
	PndLmdDPMMTModel2D(std::string name_,
			LumiFit::DPMElasticParts elastic_type);
	virtual ~PndLmdDPMMTModel2D();

	double eval(double *x) const;
};

#endif /* PNDLMDDPMMODEL2D_H_ */
