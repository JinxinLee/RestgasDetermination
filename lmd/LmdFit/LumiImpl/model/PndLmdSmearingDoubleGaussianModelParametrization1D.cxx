/*
 * PndLmdSmearingDoubleGaussianModelParametrization1D.cxx
 *
 *  Created on: Mar 28, 2013
 *      Author: steve
 */

#include "PndLmdSmearingDoubleGaussianModelParametrization1D.h"
#include "Model1D.h"
#include "ModelPar.h"
#include "ExponentialModel1D.h"
#include "PolynomialModel1D.h"
#include "AdditionModel1D.h"

PndLmdSmearingDoubleGaussianModelParametrization1D::PndLmdSmearingDoubleGaussianModelParametrization1D(
		shared_ptr<Model1D> model) {
	p_lab = model->getModelParameterSet().addModelParameter("p_lab");
	p_lab->setSuperior(true);

	shared_ptr<Model1D> poly_model(
			new PolynomialModel1D("gauss_sigma_1_poly_model_1d", 1));
	poly_model->getModelParameterSet().setModelParameterValue(
			"poly_poly_factor_0", 0.6486);
	poly_model->getModelParameterSet().setModelParameterValue(
			"poly_poly_factor_1", -0.006743);
	shared_ptr<ParametrizationModel> pm(new ParametrizationModel(poly_model));
	model->getModelParameterHandler().registerParametrizationModel(
			model->getModelParameterSet().getModelParameter("gauss_sigma_1"), pm);

	shared_ptr<Model1D> poly_model2(
			new PolynomialModel1D("gauss_sigma_2_poly_model_1d", 1));
	poly_model2->getModelParameterSet().setModelParameterValue(
			"poly_poly_factor_0", 1.044);
	poly_model2->getModelParameterSet().setModelParameterValue(
			"poly_poly_factor_1", -0.007297);
	shared_ptr<ParametrizationModel> pm2(new ParametrizationModel(poly_model2));
	model->getModelParameterHandler().registerParametrizationModel(
			model->getModelParameterSet().getModelParameter("gauss_sigma_2"), pm2);

	shared_ptr<Model1D> poly_model3(
			new PolynomialModel1D("gauss_ratio_12_poly_model_1d", 0));
	poly_model3->getModelParameterSet().setModelParameterValue(
			"poly_poly_factor_0", 0.3045);
	shared_ptr<ParametrizationModel> pm3(new ParametrizationModel(poly_model3));
	model->getModelParameterHandler().registerParametrizationModel(
			model->getModelParameterSet().getModelParameter("gauss_ratio_12"), pm3);

	shared_ptr<Model1D> poly_model4(
			new PolynomialModel1D("gauss_mean_poly_model_1d", 0));
	poly_model4->getModelParameterSet().setModelParameterValue(
			"poly_poly_factor_0", 0.03449);
	shared_ptr<Model1D> exp_model(
			new ExponentialModel1D("gauss_mean_exp_model_1d"));
	exp_model->getModelParameterSet().setModelParameterValue("exp_amplitude",
			0.4413);
	exp_model->getModelParameterSet().setModelParameterValue("exp_exp_factor",
			-0.5409);
	shared_ptr<Model1D> add_model(
			new AdditionModel1D("gauss_mean_poly_model_plus_exp_model_1d", exp_model,
					poly_model4));
	shared_ptr<ParametrizationModel> pm4(new ParametrizationModel(add_model));
	model->getModelParameterHandler().registerParametrizationModel(
			model->getModelParameterSet().getModelParameter("gauss_mean"), pm4);

	/*gauss_sigma_1->setValue(0.6486 - 0.006743 * x[0]);
	 gauss_sigma_2->setValue(1.044 - 0.007297 * x[0]);
	 gauss_ratio_12->setValue(0.3045);
	 gauss_mean->setValue(0.03449 + 0.4413 * exp(-0.5409 * x[0]));*/
}

PndLmdSmearingDoubleGaussianModelParametrization1D::~PndLmdSmearingDoubleGaussianModelParametrization1D() {
	// TODO Auto-generated destructor stub
}
