/*
 * PndLmdModelFactory.cxx
 *
 *  Created on: Dec 18, 2012
 *      Author: steve
 */

#include "PndLmdModelFactory.h"
#include "PndLmdLumiFitOptions.h"
#include "ModelFitResult.h"
#include "PndLmdLumiFitResult.h"
#include "PndLmdAcceptance.h"
#include "PndLmdDPMMTModel1D.h"
#include "PndLmdDPMAngModel1D.h"
#include "PndLmdROOTDataModel1D.h"
#include "ProductModel1D.h"
#include "GaussianModel1D.h"
#include "DoubleGaussianModel1D.h"

#include "NumericConvolutionModel1D.h"
#include "PndLmdSmearingGaussianModelParametrization1D.h"
#include "PndLmdSmearingDoubleGaussianModelParametrization1D.h"
#include "PndLmdDPMModelParametrization.h"

PndLmdModelFactory::PndLmdModelFactory() {

}

PndLmdModelFactory::~PndLmdModelFactory() {

}

shared_ptr<Model1D> PndLmdModelFactory::generate1DResolutionModel(
		const PndLmdLumiFitOptions *fit_options) {
	if (1 == fit_options->getSmearingModelType()) {
		shared_ptr<Model1D> double_gauss(
				new DoubleGaussianModel1D("smearing_double_gaussian_1d"));
		// ok that part is a little different the following class will just do all
		// of the parametrization stuff for the double gaussian, instead of writing
		// it all down in here...
		PndLmdSmearingDoubleGaussianModelParametrization1D gauss_parametrization(
				double_gauss);
		return double_gauss;
	} else //(0 == fit_options->getSmearingModelType())
	{
		shared_ptr<Model1D> gauss(new GaussianModel1D("smearing_gaussian_1d"));
		shared_ptr<Parametrization> smear_parametrization(
				new PndLmdSmearingGaussianModelParametrization1D(
						gauss->getModelParameterSet()));
		gauss->getModelParameterHandler().registerParametrizations(
				gauss->getModelParameterSet(), smear_parametrization);
		return gauss;
	}
}

shared_ptr<Model1D> PndLmdModelFactory::generate1DModel(
		const PndLmdLumiFitOptions *fit_options, double plab,
		const PndLmdAcceptance *acceptance) {
	shared_ptr<Model1D> current_model;
	if (fit_options->isFitRaw()) {
		current_model.reset(new PndLmdDPMMTModel1D("dpm_mt_1d"));
		// set free parameters
		current_model->getModelParameterSet().freeModelParameter(
				std::make_pair("dpm_mt_1d", "luminosity"));
		if (fit_options->getFreeParametersCode() & (1 << (0)))
			current_model->getModelParameterSet().freeModelParameter(
					std::make_pair("dpm_mt_1d", "sigma_tot"));
		if (fit_options->getFreeParametersCode() & (1 << (1)))
			current_model->getModelParameterSet().freeModelParameter(
					std::make_pair("dpm_mt_1d", "rho"));
		if (fit_options->getFreeParametersCode() & (1 << (2)))
			current_model->getModelParameterSet().freeModelParameter(
					std::make_pair("dpm_mt_1d", "b"));
	} else {
		current_model.reset(new PndLmdDPMAngModel1D("dpm_angular_1d"));
		// finally set all parameters free according to the fit options
		// set free parameters
		current_model->getModelParameterSet().freeModelParameter(
				std::make_pair("dpm_angular_1d", "luminosity"));
		if (fit_options->getFreeParametersCode() & (1 << (0)))
			current_model->getModelParameterSet().freeModelParameter(
					std::make_pair("dpm_angular_1d", "sigma_tot"));
		if (fit_options->getFreeParametersCode() & (1 << (1)))
			current_model->getModelParameterSet().freeModelParameter(
					std::make_pair("dpm_angular_1d", "rho"));
		if (fit_options->getFreeParametersCode() & (1 << (2)))
			current_model->getModelParameterSet().freeModelParameter(
					std::make_pair("dpm_angular_1d", "b"));
	}

	shared_ptr<Parametrization> dpm_parametrization(
			new PndLmdDPMModelParametrization(current_model->getModelParameterSet()));
	current_model->getModelParameterHandler().registerParametrizations(
			current_model->getModelParameterSet(), dpm_parametrization);

	if (fit_options->isAcceptanceCorrOn()) { // with acceptance corr
		if (acceptance) {
			// translate acceptance interpolation option
			PndLmdROOTDataModel1D::interpolation_type intpol_type =
					PndLmdROOTDataModel1D::LINEAR;
			if (fit_options->getAcceptanceInterpolationType() == 0) {
				intpol_type = PndLmdROOTDataModel1D::CONSTANT;
			} else if (fit_options->getAcceptanceInterpolationType() == 2) {
				intpol_type = PndLmdROOTDataModel1D::SPLINE;
			}
			shared_ptr<Model1D> acc(
					new PndLmdROOTDataModel1D("acceptance_1d",
							acceptance->getAcceptance1D(fit_options->isFitRaw()),
							intpol_type));

			current_model.reset(
					new ProductModel1D("acceptance_corrected_1d", current_model, acc));
		} else {
			std::cout
					<< "ERROR: requesting an acceptance corrected model without an acceptance object!"
					<< std::endl;
		}
	}

	if (fit_options->isSmearingOn()) { // with resolution smearing
		// ok since we have smearing on, generate smearing model
		current_model.reset(
				new NumericConvolutionModel1D("smeared_acceptance_corrected_1d",
						current_model, generate1DResolutionModel(fit_options)));
	}

	// every model has superior parameters which have to be set by the user
	// if they are kept fixed
	// in this case its the lab momentum plab
	current_model->getModelParameterSet().setModelParameterValue("p_lab", plab);
	// the luminosity (we set it to 1.0 as a default value)
	current_model->getModelParameterSet().setModelParameterValue("luminosity",
			1.0);
	if (current_model->init()) {
		std::cout
				<< "ERROR: Not all parameters of the model were successfully initialized!"
				<< std::endl;
		current_model->getModelParameterSet().printInfo();
	}

	return current_model;
}

void PndLmdModelFactory::initializeModelFromFitResult(shared_ptr<Model1D> model,
		PndLmdLumiFitResult *fit_result) {
	std::set<ModelStructs::minimization_parameter> &fit_params =
			fit_result->getModelFitResult()->getFitParameters();
	for (std::set<ModelStructs::minimization_parameter>::iterator it =
			fit_params.begin(); it != fit_params.end(); it++) {
		if (model->getModelParameterSet().modelParameterExists(it->name)) {
			shared_ptr<ModelPar> model_par =
					model->getModelParameterSet().getModelParameter(it->name);
			bool was_fixed = false;
			if (model_par->isParameterFixed()) {
				was_fixed = true;
				model_par->setParameterFixed(false);
			}
			model_par->setValue(it->value);
			if (was_fixed)
				model_par->setParameterFixed(true);
		}
	}
}

/*Model2D& PndLmdModelFactory::generate2DModel(
 PndLmdLumiFitOptions *fit_options) {
 // TODO

 }*/
