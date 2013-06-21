/*
 * PndLmdSmearingGaussianModelParametrization1D.cxx
 *
 *  Created on: Jan 22, 2013
 *      Author: steve
 */

#include "PndLmdSmearingGaussianModelParametrization1D.h"
#include "Model1D.h"
#include "ModelPar.h"
#include "PolynomialModel1D.h"

PndLmdSmearingGaussianModelParametrization1D::PndLmdSmearingGaussianModelParametrization1D(
		shared_ptr<Model1D> model) {
	p_lab = model->getModelParameterSet().addModelParameter("p_lab");
	p_lab->setSuperior(true);

	shared_ptr<Model1D> poly_model1(
			new PolynomialModel1D("gauss_sigma_poly_model_1d", 1));
	//poly_model1->getModelParameterSet().setModelParameterValue(
	//		"poly_poly_factor_0", 0.03449);
	shared_ptr<ParametrizationModel> pm1(new ParametrizationModel(poly_model1));
	model->getModelParameterHandler().registerParametrizationModel(
			model->getModelParameterSet().getModelParameter("gauss_sigma"),
			pm1);

	shared_ptr<Model1D> poly_model2(
			new PolynomialModel1D("gauss_mean_poly_model_1d", 1));
	shared_ptr<ParametrizationModel> pm2(new ParametrizationModel(poly_model2));
	model->getModelParameterHandler().registerParametrizationModel(
			model->getModelParameterSet().getModelParameter("gauss_mean"),
			pm2);
}

PndLmdSmearingGaussianModelParametrization1D::~PndLmdSmearingGaussianModelParametrization1D() {
	// TODO Auto-generated destructor stub
}
