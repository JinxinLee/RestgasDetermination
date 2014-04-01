/*
 * PndLmdModelFactory.cxx
 *
 *  Created on: Dec 18, 2012
 *      Author: steve
 */

#include "PndLmdModelFactory.h"
#include "LumiImpl/LumiFitStructs.h"
#include "ModelFitResult.h"
#include "PndLmdLumiFitResult.h"
#include "PndLmdAcceptance.h"
#include "PndLmdDPMMTModel1D.h"
#include "PndLmdDPMAngModel1D.h"
#include "PndLmdROOTDataModel1D.h"
#include "ProductModel1D.h"
#include "GaussianModel1D.h"
#include "DoubleGaussianModel1D.h"
#include "AsymmetricGaussianModel1D.h"

#include "NumericConvolutionModel1D.h"
#include "SmearingConvolutionModel1D.h"
#include "PndLmdSmearingGaussianModelParametrization1D.h"
#include "PndLmdSmearingDoubleGaussianModelParametrization1D.h"
#include "PndLmdSmearingAsymmetricGaussianModelParameterization1D.h"
#include "PndLmdDPMModelParametrization.h"

#include "PndLmdLumiHelper.h"

#include <iostream>

#include "TFile.h"
#include "TEfficiency.h"
#include "TCanvas.h"

PndLmdModelFactory::PndLmdModelFactory() {

}

PndLmdModelFactory::~PndLmdModelFactory() {

}

shared_ptr<Model1D> PndLmdModelFactory::generate1DVertexModel(
		const LumiFit::PndLmdFitModelOptions& model_options) {
	shared_ptr<Model1D> vertex_model;

	if (model_options.vertex_model == LumiFit::UNIFORM) {
		vertex_model.reset(new DoubleGaussianModel1D("vertex_double_gaussian_1d"));
	} else if (model_options.vertex_model == LumiFit::GAUSSIAN) {
		vertex_model.reset(new GaussianModel1D("vertex_double_gaussian_1d"));
	} else {
		std::cout << "Unknown vertex model!" << std::endl;
	}

	return vertex_model;
}

shared_ptr<Model1D> PndLmdModelFactory::generate1DResolutionModel(
		const LumiFit::PndLmdFitModelOptions& model_options) {
	shared_ptr<Model1D> resolution_model;

	if (model_options.smearing_model == LumiFit::DOUBLE_GAUSSIAN) {
		resolution_model.reset(
				new DoubleGaussianModel1D("smearing_double_gaussian_1d"));
		// ok that part is a little different the following class will just do all
		// of the parametrization stuff for the double gaussian, instead of writing
		// it all down in here...
		PndLmdSmearingDoubleGaussianModelParametrization1D gauss_parametrization(
				resolution_model);
	} else if (model_options.smearing_model == LumiFit::GAUSSIAN) {
		resolution_model.reset(new GaussianModel1D("smearing_gaussian_1d"));
		PndLmdSmearingGaussianModelParametrization1D gauss_parametrization(
				resolution_model);
	} else if (model_options.smearing_model == LumiFit::ASYMMETRIC_GAUSSIAN) {
		resolution_model.reset(
				new AsymmetricGaussianModel1D("smearing_asymm_gaussian_1d"));
		PndLmdSmearingAsymmetricGaussianModelParameterization1D gauss_parametrization(
				resolution_model, model_options);
	} else {
		std::cout << "Unknown smearing model!" << std::endl;
	}

// init values with parametrization if possible
	if (!model_options.use_resolution_parameter_interpolation) {
		TFile f(model_options.resolution_parametrization_file_url.c_str(), "READ");
		PndLmdLumiHelper lmd_helper;
		lmd_helper.initResolutionParametrizationFromFile(&f, resolution_model);
	}

	return resolution_model;
}

shared_ptr<Model1D> PndLmdModelFactory::generate1DModel(
		const LumiFit::PndLmdFitModelOptions& model_options, double plab) {
	shared_ptr<Model1D> current_model;

	if (model_options.momentum_transfer_active) {
		current_model.reset(
				new PndLmdDPMMTModel1D("dpm_mt_1d", model_options.dpm_elastic_parts));
		// set free parameters
	} else {
		current_model.reset(
				new PndLmdDPMAngModel1D("dpm_angular_1d",
						model_options.dpm_elastic_parts));
	}

	shared_ptr<Parametrization> dpm_parametrization(
			new PndLmdDPMModelParametrization(current_model->getModelParameterSet()));
	current_model->getModelParameterHandler().registerParametrizations(
			current_model->getModelParameterSet(), dpm_parametrization);

	if (model_options.acceptance_correction_active) { // with acceptance corr
		if (model_options.acceptance) {
			// translate acceptance interpolation option
			PndLmdROOTDataModel1D *data_model = new PndLmdROOTDataModel1D(
					"acceptance_1d");

			TCanvas can;
			model_options.acceptance->getAcceptance1D()->Draw();
			can.Update();

			data_model->setGraph(
					model_options.acceptance->getAcceptance1D()->GetPaintedGraph());
			data_model->setIntpolType(model_options.acceptance_interpolation);
			data_model->setDataDimension(
					model_options.acceptance->getPrimaryDimension().dimension_range);
			shared_ptr<Model1D> acc(data_model);

			current_model.reset(
					new ProductModel1D("acceptance_corrected_1d", current_model, acc));
		} else {
			std::cout
					<< "ERROR: requesting an acceptance corrected model without an acceptance object!"
					<< std::endl;
		}
	}

	if (model_options.resolution_smearing_active) { // with resolution smearing
		// ok since we have smearing on, generate smearing model
		current_model.reset(
				new SmearingConvolutionModel1D("smeared_acceptance_corrected_1d",
						current_model, generate1DResolutionModel(model_options)));
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

/*Model2D& PndLmdModelFactory::generate2DModel(
 PndLmdLumiFitOptions *fit_options) {
 // TODO

 }*/
