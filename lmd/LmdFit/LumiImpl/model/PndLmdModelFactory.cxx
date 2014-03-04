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

shared_ptr<Model1D> PndLmdModelFactory::generate1DResolutionModel(
		const PndLmdLumiFitOptions *fit_options) {
	shared_ptr<Model1D> resolution_model;

	PndLmdLumiFitOptions resolution_fit_options(*fit_options);

	// init values with parametrization if possible
	if (fit_options->isResolutionParametrizationFileUrlActive()) {
		TFile f(fit_options->getResolutionParametrizationFileUrl(), "READ");
		PndLmdLumiHelper lmd_helper;
		std::vector<PndLmdLumiHelper::lmd_graph*> allgraphs =
				lmd_helper.getResolutionModelResultsFromFile(&f);
		if (allgraphs.size() > 0) {
			resolution_fit_options = *allgraphs[0]->fit_options;
			resolution_fit_options.setResolutionParametrizationFileUrl(
					fit_options->getResolutionParametrizationFileUrl());
		}
	}

	if (1 == resolution_fit_options.getSmearingModelType() % 10) {
		resolution_model.reset(
				new DoubleGaussianModel1D("smearing_double_gaussian_1d"));
		// ok that part is a little different the following class will just do all
		// of the parametrization stuff for the double gaussian, instead of writing
		// it all down in here...
		PndLmdSmearingDoubleGaussianModelParametrization1D gauss_parametrization(
				resolution_model);
	} else if (0 == resolution_fit_options.getSmearingModelType() % 10) {
		resolution_model.reset(new GaussianModel1D("smearing_gaussian_1d"));
		PndLmdSmearingGaussianModelParametrization1D gauss_parametrization(
				resolution_model);
	} else if (2 == resolution_fit_options.getSmearingModelType() % 10) {
		resolution_model.reset(
				new AsymmetricGaussianModel1D("smearing_asymm_gaussian_1d"));
		PndLmdSmearingAsymmetricGaussianModelParameterization1D gauss_parametrization(
				resolution_model, &resolution_fit_options);
		std::cout<<"asdfasdf"<<std::endl;
	} else {
		std::cout << "this is not good!" << std::endl;
	}

	// init values with parametrization if possible
	if (resolution_fit_options.getSmearingModelType() < 9
			&& resolution_fit_options.isResolutionParametrizationFileUrlActive()) {
		TFile f(resolution_fit_options.getResolutionParametrizationFileUrl(), "READ");
		PndLmdLumiHelper lmd_helper;
		lmd_helper.initResolutionParametrizationFromFile(&f, resolution_model);
	}

	return resolution_model;
}

shared_ptr<Model1D> PndLmdModelFactory::generate1DModel(
		const PndLmdLumiFitOptions *fit_options, double plab) {
	shared_ptr<Model1D> current_model;

	// get dpm part
	PndLmdDPMMTModel1D::dpm_elastic_parts dpm_elastic_type =
			PndLmdDPMMTModel1D::ALL;
	if (fit_options->getDpmElasticModelParts() == 1)
		dpm_elastic_type = PndLmdDPMMTModel1D::COUL;
	else if (fit_options->getDpmElasticModelParts() == 2)
		dpm_elastic_type = PndLmdDPMMTModel1D::INT;
	else if (fit_options->getDpmElasticModelParts() == 3)
		dpm_elastic_type = PndLmdDPMMTModel1D::HAD;

	if (fit_options->getModelBinaryOptions().isFitRaw()) {
		current_model.reset(new PndLmdDPMMTModel1D("dpm_mt_1d", dpm_elastic_type));
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
		current_model.reset(
				new PndLmdDPMAngModel1D("dpm_angular_1d", dpm_elastic_type));
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

	if (fit_options->getModelBinaryOptions().isAcceptanceCorrOn()) { // with acceptance corr
		if (fit_options->getAcceptance()) {
			// translate acceptance interpolation option
			PndLmdROOTDataModel1D::interpolation_type intpol_type =
					PndLmdROOTDataModel1D::LINEAR;
			if (fit_options->getAcceptanceInterpolationType() == 0) {
				intpol_type = PndLmdROOTDataModel1D::CONSTANT;
			} else if (fit_options->getAcceptanceInterpolationType() == 2) {
				intpol_type = PndLmdROOTDataModel1D::SPLINE;
			}
			PndLmdROOTDataModel1D *data_model = new PndLmdROOTDataModel1D(
					"acceptance_1d");

			TCanvas can;
			fit_options->getAcceptance()->getAcceptance1D()->Draw();
			can.Update();

			data_model->setGraph(
					fit_options->getAcceptance()->getAcceptance1D()->GetPaintedGraph());
			data_model->setIntpolType(intpol_type);
			data_model->setDataDimension(
					fit_options->getAcceptance()->getPrimaryDimension().dimension_range);
			shared_ptr<Model1D> acc(data_model);

			current_model.reset(
					new ProductModel1D("acceptance_corrected_1d", current_model, acc));
		} else {
			std::cout
					<< "ERROR: requesting an acceptance corrected model without an acceptance object!"
					<< std::endl;
		}
	}

	if (fit_options->getModelBinaryOptions().isSmearingOn()) { // with resolution smearing
		// ok since we have smearing on, generate smearing model
		current_model.reset(
				new SmearingConvolutionModel1D("smeared_acceptance_corrected_1d",
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

/*Model2D& PndLmdModelFactory::generate2DModel(
 PndLmdLumiFitOptions *fit_options) {
 // TODO

 }*/
