/*
 * PndLmdModelFactory.cxx
 *
 *  Created on: Dec 18, 2012
 *      Author: steve
 */

#include "PndLmdModelFactory.h"
#include "LumiFitStructs.h"
#include "fit/ModelFitResult.h"
#include "fit/PndLmdLumiFitResult.h"
#include "data/PndLmdAcceptance.h"
#include "PndLmdDPMMTModel1D.h"
#include "PndLmdDPMAngModel1D.h"
#include "PndLmdDPMAngModel2D.h"
#include "PndLmdFastDPMAngModel2D.h"
#include "PndLmdROOTDataModel1D.h"
#include "PndLmdROOTDataModel2D.h"
#include "models1d/GaussianModel1D.h"
#include "models2d/GaussianModel2D.h"
#include "models2d/DataModel2D.h"
#include "models1d/DoubleGaussianModel1D.h"
#include "AsymmetricGaussianModel1D.h"

#include "operators2d/integration/SimpleIntegralStrategy2D.h"
#include "operators1d/ProductModel1D.h"
#include "operators2d/ProductModel2D.h"
#include "operators1d/convolution/NumericConvolutionModel1D.h"
#include "operators1d/convolution/SmearingConvolutionModel1D.h"
#include "model/PndLmdSmearingConvolutionModel2D.h"
#include "model/PndLmdDivergenceSmearingModel2D.h"
#include "PndLmdDPMModelParametrization.h"

#include "data/PndLmdDataFacade.h"
#include "data/PndLmdAngularData.h"
#include "PndLmdLumiHelper.h"
#include "PndLmdResolutionModelParameterizationHelper.h"

#include <iostream>

#include "TEfficiency.h"
#include "TH2.h"
#include "TCanvas.h"

PndLmdModelFactory::PndLmdModelFactory() {

}

PndLmdModelFactory::~PndLmdModelFactory() {

}

shared_ptr<Model1D> PndLmdModelFactory::generate1DVertexModel(
		const LumiFit::PndLmdFitModelOptions& model_options) const {
	shared_ptr<Model1D> vertex_model;

	if (model_options.vertex_model == LumiFit::UNIFORM) {
		vertex_model.reset(new DoubleGaussianModel1D("vertex_double_gaussian_1d"));
	} else if (model_options.vertex_model == LumiFit::GAUSSIAN) {
		vertex_model.reset(new GaussianModel1D("vertex_gaussian_1d"));
	} else {
		std::cout << "Unknown vertex model!" << std::endl;
	}

	return vertex_model;
}

shared_ptr<Model1D> PndLmdModelFactory::generate1DResolutionModel(
		const LumiFit::PndLmdFitModelOptions& model_options) const {
	shared_ptr<Model1D> resolution_model;

	std::vector<std::string> parameter_names_with_parameterization_models;

	if (model_options.smearing_model == LumiFit::DOUBLE_GAUSSIAN) {
		resolution_model.reset(
				new DoubleGaussianModel1D("smearing_double_gaussian_1d"));
		// ok that part is a little different the following class will just do all
		// of the parametrization stuff for the double gaussian, instead of writing
		// it all down in here...
		//PndLmdSmearingDoubleGaussianModelParametrization1D gauss_parametrization(
		//		resolution_model);
		parameter_names_with_parameterization_models.push_back("gauss_mean_narrow");
		parameter_names_with_parameterization_models.push_back("gauss_mean_wide");
		parameter_names_with_parameterization_models.push_back(
				"gauss_sigma_narrow");
		parameter_names_with_parameterization_models.push_back(
				"gauss_sigma_ratio_narrow_wide");
		parameter_names_with_parameterization_models.push_back(
				"gauss_ratio_narrow_wide");
	} else if (model_options.smearing_model == LumiFit::GAUSSIAN) {
		resolution_model.reset(new GaussianModel1D("smearing_gaussian_1d"));
		//PndLmdSmearingGaussianModelParametrization1D gauss_parametrization(
		//		resolution_model, model_options);
		parameter_names_with_parameterization_models.push_back("gauss_mean");
		parameter_names_with_parameterization_models.push_back("gauss_sigma");
	} else if (model_options.smearing_model == LumiFit::ASYMMETRIC_GAUSSIAN) {
		resolution_model.reset(
				new AsymmetricGaussianModel1D("smearing_asymm_gaussian_1d"));
		//PndLmdSmearingAsymmetricGaussianModelParameterization1D gauss_parametrization(
		//		resolution_model, model_options);
		parameter_names_with_parameterization_models.push_back(
				"asymm_gauss_sigma_left");
		parameter_names_with_parameterization_models.push_back(
				"asymm_gauss_sigma_right");
		parameter_names_with_parameterization_models.push_back("asymm_gauss_mean");
	} else {
		std::cout << "Unknown smearing model!" << std::endl;
	}

	PndLmdResolutionModelParameterizationHelper res_model_helper(resolution_model,
			model_options);
	res_model_helper.createAndInitializeParametrizationModelsForParameters(
			parameter_names_with_parameterization_models);

	return resolution_model;
}

shared_ptr<Model2D> PndLmdModelFactory::generate2DResolutionModel(
		const LumiFit::PndLmdFitModelOptions& model_options) const {
	shared_ptr<Model2D> resolution_model;

	std::vector<std::string> parameter_names_with_parameterization_models;

	if (model_options.smearing_model == LumiFit::GAUSSIAN) {
		resolution_model.reset(new GaussianModel2D("smearing_double_gaussian_2d"));
		//PndLmdSmearingGaussianModelParametrization2D gauss_parametrization(
		//		resolution_model, model_options);
		parameter_names_with_parameterization_models.push_back("gauss_mean_var1");
		parameter_names_with_parameterization_models.push_back("gauss_sigma_var1");
		parameter_names_with_parameterization_models.push_back("gauss_mean_var2");
		parameter_names_with_parameterization_models.push_back("gauss_sigma_var2");
		parameter_names_with_parameterization_models.push_back("gauss_rho");
	} else {
		std::cout << "Unknown smearing model!" << std::endl;
	}

	// init values with parametrization if possible
	PndLmdResolutionModelParameterizationHelper res_model_helper(resolution_model,
			model_options);
	res_model_helper.createAndInitializeParametrizationModelsForParameters(
			parameter_names_with_parameterization_models);

	return resolution_model;
}

shared_ptr<Model> PndLmdModelFactory::generateModel(
		const LumiFit::PndLmdFitModelOptions& model_options, double plab) const {
	shared_ptr<Model> current_model;

	if (model_options.fit_dimension == 1) {
		current_model = generate1DModel(model_options, plab);
	} else if (model_options.fit_dimension == 2) {
		current_model = generate2DModel(model_options, plab);
	} else {
		std::cout << "WARNING: Requesting a model of dimension "
				<< model_options.fit_dimension
				<< " which is NOT available! Returning NULL pointer." << std::endl;
	}
	return current_model;
}

shared_ptr<Model1D> PndLmdModelFactory::generate1DModel(
		const LumiFit::PndLmdFitModelOptions& model_options, double plab) const {
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

			TVirtualPad *current_pad = gPad;
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

			gPad = current_pad; // reset pad to the one before
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

shared_ptr<Model2D> PndLmdModelFactory::generate2DModel(
		const LumiFit::PndLmdFitModelOptions& model_options, double plab) const {
	shared_ptr<Model2D> current_model;

	shared_ptr<PndLmdDPMAngModel1D> dpm_angular_1d(
			new PndLmdDPMAngModel1D("dpm_angular_1d",
					model_options.dpm_elastic_parts));

	shared_ptr<Parametrization> dpm_parametrization(
			new PndLmdDPMModelParametrization(
					dpm_angular_1d->getModelParameterSet()));
	dpm_angular_1d->getModelParameterHandler().registerParametrizations(
			dpm_angular_1d->getModelParameterSet(), dpm_parametrization);

	current_model.reset(
			new PndLmdFastDPMAngModel2D("dpm_angular_2d", dpm_angular_1d));

	current_model->getModelParameterSet().freeModelParameter("tilt_x");
	current_model->getModelParameterSet().freeModelParameter("tilt_y");
	if (model_options.fix_beam_tilts) {
		current_model->getModelParameterSet().getModelParameter("tilt_x")->setValue(
				model_options.beam_tilt_x);
		current_model->getModelParameterSet().getModelParameter("tilt_x")->setParameterFixed(
				true);
		current_model->getModelParameterSet().getModelParameter("tilt_y")->setValue(
				model_options.beam_tilt_y);
		current_model->getModelParameterSet().getModelParameter("tilt_y")->setParameterFixed(
				true);
	}

	if (model_options.divergence_smearing_active
			&& model_options.data_primary_dimension.is_active
			&& model_options.data_secondary_dimension.is_active) {
		// add the divergence smearing on top
		// at this point we introduce "non numerical" discretization from the data
		// thats why the data object is required here

		shared_ptr<GaussianModel2D> divergence_model(
				new GaussianModel2D("divergence_model", 3.0));

		divergence_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_var1")->setValue(0.0005);
		divergence_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_var2")->setValue(0.0005);
		divergence_model->getModelParameterSet().getModelParameter(
				"gauss_mean_var1")->setValue(0.0);
		divergence_model->getModelParameterSet().getModelParameter(
				"gauss_mean_var1")->setParameterFixed(true);
		divergence_model->getModelParameterSet().getModelParameter(
				"gauss_mean_var2")->setValue(0.0);
		divergence_model->getModelParameterSet().getModelParameter(
				"gauss_mean_var2")->setParameterFixed(true);
		divergence_model->getModelParameterSet().getModelParameter("gauss_rho")->setValue(
				0.0);

		current_model.reset(
				new PndLmdDivergenceSmearingModel2D("div_smeared_dpm_angular_2d",
						current_model, divergence_model,
						model_options.data_primary_dimension,
						model_options.data_secondary_dimension));
	}
	if (model_options.acceptance_correction_active) { // with acceptance corr
		if (model_options.acceptance) {
			// translate acceptance interpolation option
			/*PndLmdROOTDataModel2D *data_model = new PndLmdROOTDataModel2D(
			 "acceptance_2d");
			 data_model->setEfficiency(model_options.acceptance->getAcceptance2D());
			 data_model->setIntpolType(model_options.acceptance_interpolation);
			 data_model->setDataDimension(
			 std::make_pair(
			 model_options.acceptance->getPrimaryDimension().dimension_range,
			 model_options.acceptance->getSecondaryDimension().dimension_range));
			 shared_ptr<Model2D> acc(data_model);*/

			shared_ptr<DataModel2D> acc(new DataModel2D("acceptance_2d"));
			TVirtualPad* curpad = gPad;
			TCanvas c;
			model_options.acceptance->getAcceptance2D()->Draw("colz");
			c.Update();

			TH2 *hist =
					model_options.acceptance->getAcceptance2D()->GetPaintedHistogram();
			gPad = curpad;
			std::map<std::pair<double, double>, double> datamap;
			for (unsigned int ix = 0; ix < hist->GetNbinsX(); ix++) {
				for (unsigned int iy = 0; iy < hist->GetNbinsY(); iy++) {
					datamap[std::make_pair(hist->GetXaxis()->GetBinCenter(ix),
							hist->GetYaxis()->GetBinCenter(iy))] = hist->GetBinContent(ix,
							iy);
				}
			}

			acc->setData(datamap);

			current_model.reset(
					new ProductModel2D("acceptance_corrected_2d", current_model, acc));
		} else {
			std::cout
					<< "ERROR: requesting an acceptance corrected model without an acceptance object!"
					<< std::endl;
		}
	}

	if (model_options.resolution_smearing_active) { // with resolution smearing
		// ok since we have smearing on, generate smearing model
		// --- read in box gen data and construct lmd resolution data objects --- //
		PndLmdDataFacade lmd_data_facade;

		TFile *infile = new TFile(
				model_options.resolution_parametrization_file_url.c_str(), "READ");

		std::vector<PndLmdHistogramData> all_lmd_res =
				lmd_data_facade.getDataFromFile<PndLmdHistogramData>(infile);

		// filter the vector for specific options
		LumiFit::LmdDimensionOptions lmd_dim_opt;
		lmd_dim_opt.dimension_type = LumiFit::THETA_X;
		lmd_dim_opt.track_param_type = LumiFit::IP;
		lmd_dim_opt.track_type = LumiFit::DIFF_RECO_MC;

		LumiFit::Comparisons::data_primary_dimension_options_filter filter(
				lmd_dim_opt);

		std::vector<PndLmdHistogramData> lmd_res_vec = lmd_data_facade.filterData<
				PndLmdHistogramData>(all_lmd_res, filter);

		lmd_dim_opt.track_type = LumiFit::RECO;
		filter.setDimensionOptions(lmd_dim_opt);

		TFile *infile2 = new TFile(model_options.elastic_reco_data_file_url.c_str(),
				"READ");
		std::vector<PndLmdAngularData> all_lmd_data =
				lmd_data_facade.getDataFromFile<PndLmdAngularData>(infile2);

		std::vector<PndLmdAngularData> reco_lmd_data_vec =
				lmd_data_facade.filterData<PndLmdAngularData>(all_lmd_data, filter);

		PndLmdLumiHelper lmd_helper;
		shared_ptr<PndLmdSmearingModel2D> detector_smearing_model(
				new PndLmdSmearingModel2D());
		detector_smearing_model->setSmearingParameterization(
				lmd_helper.generate2DResolutionMap(lmd_res_vec, reco_lmd_data_vec[0]));

		current_model.reset(
				new PndLmdSmearingConvolutionModel2D("smeared_acceptance_corrected_2d",
						current_model, detector_smearing_model));
		/*
		 current_model.reset(
		 new SmearingConvolutionModel2D("smeared_acceptance_corrected_2d",
		 current_model, generate2DResolutionModel(model_options)));
		 current_model->setIntegralStrategy(new SimpleIntegralStrategy2D());*/
	}

	// every model has superior parameters which have to be set by the user
	// if they are kept fixed
	// in this case its the lab momentum plab
	dpm_angular_1d->getModelParameterSet().setModelParameterValue("p_lab", plab);
	// the luminosity (we set it to 1.0 as a default value)
	dpm_angular_1d->getModelParameterSet().setModelParameterValue("luminosity",
			1.0);
	if (current_model->init()) {
		std::cout
				<< "ERROR: Not all parameters of the model were successfully initialized!"
				<< std::endl;
		current_model->getModelParameterSet().printInfo();
	}

	return current_model;
}
