/*
 * PndLmdDPMModel2D.cxx
 *
 *  Created on: Jan 17, 2013
 *      Author: steve
 */

#include "PndLmdDPMMTModel2D.h"

#include "TMath.h"

PndLmdDPMMTModel2D::PndLmdDPMMTModel2D(std::string name_,
		LumiFit::DPMElasticParts elastic_type) :
		Model2D(name_), dpm_model_1d("dpm_mt_1d", elastic_type) {
	getModelParameterSet().addModelParameters(
			dpm_model_1d.getModelParameterSet());
}

PndLmdDPMMTModel2D::~PndLmdDPMMTModel2D() {
	// TODO Auto-generated destructor stub
}

double PndLmdDPMMTModel2D::eval(double *x) const {
	return dpm_model_1d.eval(x) / (2.0 * TMath::Pi());
}
