/*
 * PndLmdFitFacade.cxx
 *
 *  Created on: Aug 30, 2013
 *      Author: steve
 */

#include "PndLmdFitFacade.h"
#include "PndLmdLumiFitOptions.h"
#include "LumiFitStructs.h"
#include "PndLmdResolution.h"
#include "PndLmdAngularData.h"
#include "PndLmdVertexData.h"
#include "PndLmdLumiHelper.h"
#include "ROOTDataHelper.h"
#include "Chi2Estimator.h"
#include "LogLikelihoodEstimator.h"
#include "PndLmdLumiHelper.h"
#include "ROOTMinimizer.h"
#include "Data.h"

#include <iostream>

#include "TFile.h"
#include "TH1D.h"

using std::cout;
using std::endl;
using std::vector;
using std::pair;

PndLmdFitFacade::PndLmdFitFacade() {
	// TODO Auto-generated constructor stub

}

PndLmdFitFacade::~PndLmdFitFacade() {
	// TODO Auto-generated destructor stub
}

PndLmdLumiFitOptions& PndLmdFitFacade::getFitOptionTemplate() {
	return fit_options_template;
}

void PndLmdFitFacade::setModelFitOptions(
		LumiFit::PndLmdFitModelOptions& model_opt_) {
	fit_options_template.model_opt = model_opt_;
}
void PndLmdFitFacade::setEstimatorOptions(EstimatorOptions& est_opt_) {
	fit_options_template.est_opt = est_opt_;
}
void PndLmdFitFacade::addFreeFitParameter(std::string name) {
	fit_options_template.free_parameter_names.insert(name);
}

ModelFitFacade& PndLmdFitFacade::getModelFitFacade() {
	return model_fit_facade;
}

pair<double, double> PndLmdFitFacade::calcRange(
		const PndLmdAbstractData &lmd_abs_data,
		const PndLmdLumiFitOptions *fit_options) {
	double range_low, range_high;
	range_low = lmd_abs_data.getPrimaryDimension().dimension_range.getRangeLow();
	range_high =
			lmd_abs_data.getPrimaryDimension().dimension_range.getRangeHigh();
	if (fit_options->getEstimatorOptions().getFitRangeX().range_low
			> lmd_abs_data.getPrimaryDimension().dimension_range.getRangeLow())
		range_low = fit_options->getEstimatorOptions().getFitRangeX().range_low;
	if (fit_options->getEstimatorOptions().getFitRangeX().range_high
			< lmd_abs_data.getPrimaryDimension().dimension_range.getRangeHigh())
		range_high = fit_options->getEstimatorOptions().getFitRangeX().range_high;
	return std::make_pair(range_low, range_high);
}

shared_ptr<Data> PndLmdFitFacade::createData1D(
		const PndLmdHistogramData &lmd_hist_data) const {
	shared_ptr<Data> data(new Data(1));
	data_helper.fillBinnedData(data, lmd_hist_data.get1DHistogram());
	return data;
}

PndLmdLumiFitOptions* PndLmdFitFacade::cloneFitOptions() const {
	PndLmdLumiFitOptions* fit_opts = new PndLmdLumiFitOptions(
			fit_options_template);
	return fit_opts;
}

PndLmdLumiFitOptions* PndLmdFitFacade::createFitOptions(
		PndLmdHistogramData &lmd_data) {
	PndLmdLumiFitOptions* fit_opts;

	// get current settings
	EstimatorOptions current_est_opt(fit_options_template.getEstimatorOptions());
	LumiFit::PndLmdFitModelOptions current_mod_opt(
			fit_options_template.getFitModelOptions());

	// switch range to momentum transfer as user usually specifies that in mrad
	if (LumiFit::T
			== lmd_data.getPrimaryDimension().dimension_options.dimension_type) {
		fit_options_template.model_opt.momentum_transfer_active = true;
		fit_options_template.model_opt.resolution_smearing_active = false;
		fit_options_template.model_opt.acceptance_correction_active = false;

		PndLmdLumiHelper lumi_helper;

		// recalcuated ranges to momentum transfer
		DataStructs::DimensionRange temp_range;
		temp_range.range_low = lumi_helper.getMomentumTransferFromTheta(
				lmd_data.getLabMomentum(), current_est_opt.getFitRangeX().range_low);
		temp_range.range_high = lumi_helper.getMomentumTransferFromTheta(
				lmd_data.getLabMomentum(), current_est_opt.getFitRangeX().range_high);

		fit_options_template.est_opt.setFitRangeX(temp_range);

		fit_opts = new PndLmdLumiFitOptions(fit_options_template);

		// set estimator options back to user setting (is only required in case of t recalculation)
		fit_options_template.est_opt = current_est_opt;

	} else {
		fit_options_template.model_opt.momentum_transfer_active = false;
		if (LumiFit::MC_ACC
				== lmd_data.getPrimaryDimension().dimension_options.track_type)
			fit_options_template.model_opt.acceptance_correction_active = true;
		if (LumiFit::RECO
				== lmd_data.getPrimaryDimension().dimension_options.track_type) {
			fit_options_template.model_opt.resolution_smearing_active = true;
			fit_options_template.model_opt.acceptance_correction_active = true;
		}
		fit_opts = new PndLmdLumiFitOptions(fit_options_template);
	}

	// set model options back to user settings
	fit_options_template.model_opt = current_mod_opt;

	return fit_opts;
}

void PndLmdFitFacade::initializeResolutionModelOptionsFromParametrizationFile(
		LumiFit::PndLmdFitModelOptions& model_opt, std::string res_file_url) {
// get model options from the resolution parametrization file
	std::cout << "Opening file " << res_file_url
			<< " to get resolution parametrization metadata" << std::endl;
	TFile f(res_file_url.c_str(), "READ");
	PndLmdLumiHelper lmd_helper;
	std::vector<PndLmdLumiHelper::lmd_graph*> allgraphs =
			lmd_helper.getResolutionModelResultsFromFile(&f);
	if (allgraphs.size() > 0) {
		model_opt.smearing_model =
				allgraphs[0]->fit_options->getFitModelOptions().smearing_model;
		model_opt.resolution_parametrization_file_url = res_file_url;
		model_opt.use_resolution_parameter_interpolation =
				allgraphs[0]->fit_options->getFitModelOptions().use_resolution_parameter_interpolation;
		std::cout
				<< "Initialized resolution model options from parametrization file!"
				<< std::endl;
	}
}

void PndLmdFitFacade::doLmdLumiFits(
		std::vector<PndLmdAngularData> &lmd_data_vec) {

	// these are all standard lumi fits, so dataset with appropriate model
	// like accepted mc data with acceptance corrected model etc

	fitMultipleElasticPPbar(lmd_data_vec);
}

void PndLmdFitFacade::fitSmearingModelToResolutions(
		vector<PndLmdResolution> &lmd_resolutions) {

	PndLmdLumiFitOptions *fit_options = cloneFitOptions();

	for (unsigned int index_resolution = 0;
			index_resolution < lmd_resolutions.size(); index_resolution++) {
		fitResolutionForSlice(lmd_resolutions[index_resolution], fit_options);
	}
}

PndLmdLumiFitResult* PndLmdFitFacade::doFit(PndLmdHistogramData &lmd_hist_data,
		const PndLmdLumiFitOptions *fit_options) {

	cout << "Attempting to perform fit with following fit options:" << endl;
	cout << *fit_options << endl;

	//first check if this model with the fit options have already been fitted
	PndLmdLumiFitResult *fit_result = lmd_hist_data.getFitResult(*fit_options);
	if (0 != fit_result) {
		cout << "Fit was already performed! Skipping..." << endl;
		return fit_result;
	}

	model_fit_facade.setEstimatorOptions(fit_options->getEstimatorOptions());

	ModelFitResult temp_fit_result = model_fit_facade.Fit();

	// store fit results
	cout << "Adding fit result to storage..." << endl;

	// in case we have a likelihood we have to create a new chi2 estimator...

	fit_result = new PndLmdLumiFitResult();
	fit_result->setModelFitResult(temp_fit_result);
	lmd_hist_data.addFitResult(fit_options, fit_result);
	return fit_result;
}

void PndLmdFitFacade::fitMultipleElasticPPbar(
		std::vector<PndLmdAngularData> &lmd_data) {
	for (unsigned int i = 0; i < lmd_data.size(); i++) {
		PndLmdLumiFitOptions *fit_options = createFitOptions(lmd_data[i]);

		// create chi2 estimator
		//shared_ptr<Chi2Estimator> chi2_est(new Chi2Estimator());
		//model_fit_facade.setEstimator(chi2_est);

		shared_ptr<LogLikelihoodEstimator> loglikelihood_est(
				new LogLikelihoodEstimator());
		model_fit_facade.setEstimator(loglikelihood_est);

		// create a new model via the factory
		shared_ptr<Model1D> model1d = model_factory.generate1DModel(
				fit_options->getFitModelOptions(), lmd_data[i].getLabMomentum());

		//always free luminosity
		std::string model_name("dpm_angular_1d");

		if (fit_options->getFitModelOptions().momentum_transfer_active) {
			model_name = "dpm_mt_1d";
		}

		model1d->getModelParameterSet().freeModelParameter(
				std::make_pair(model_name, "luminosity"));

		//loop over all other free parameter names
		std::set<std::string, ModelStructs::string_comp> free_parameter_names =
				fit_options->getFreeParameterSet();

		for (std::set<std::string, ModelStructs::string_comp>::iterator it =
				free_parameter_names.begin(); it != free_parameter_names.end(); it++) {
			model1d->getModelParameterSet().freeModelParameter(
					std::make_pair(model_name, *it));
		}

		// set model
		model_fit_facade.setModel1d(model1d);

		// create and set data
		model_fit_facade.setData(createData1D(lmd_data[i]));

		// now set better starting amplitude value
		PndLmdLumiHelper lmd_helper;
		pair<double, double> range = calcRange(lmd_data[i], fit_options);
		double integral_data = lmd_helper.calcHistIntegral(
				lmd_data[i].get1DHistogram(), range);
		vector<pair<double, double> > temp_vec_range;
		temp_vec_range.push_back(range);
		double integral_func = model1d->Integral(temp_vec_range, 1e-3);
		double lumi_start = integral_data / integral_func
				/ lmd_data[i].getBinningFactor();
		cout << "binning factor: " << lmd_data[i].getBinningFactor() << endl;
		cout << integral_data << " / "
				<< integral_func * lmd_data[i].getBinningFactor() << endl;
		cout << "(1D) Using start luminosity: " << lumi_start << endl;
		model1d->getModelParameterSet().setModelParameterValue("luminosity",
				lumi_start);

		// create minimizer instance with control parameter
		shared_ptr<ROOTMinimizer> minuit_minimizer(new ROOTMinimizer());

		model_fit_facade.setMinimizer(minuit_minimizer);

		doFit(lmd_data[i], fit_options);
	}
}

void PndLmdFitFacade::fitVertexData(std::vector<PndLmdVertexData> &lmd_data) {
	for (unsigned int i = 0; i < lmd_data.size(); i++) {
		cout << "Fitting resolution " << lmd_data[i].getName() << endl;

		PndLmdLumiFitOptions *fit_options = createFitOptions(lmd_data[i]);

		// create chi2 estimator
		shared_ptr<Chi2Estimator> chi2_est(new Chi2Estimator());
		model_fit_facade.setEstimator(chi2_est);

		// get histogram
		const TH1D* hist = lmd_data[i].get1DHistogram();
		if (hist->Integral() < 400) {
			return;
		}
		model_fit_facade.setData(createData1D(lmd_data[i]));

		// generate the model
		shared_ptr<Model1D> vertex_model = model_factory.generate1DVertexModel(
				fit_options->getFitModelOptions());

		// now we have to set good starting values and free parameters
		if (fit_options->getFitModelOptions().vertex_model == LumiFit::GAUSSIAN) { // simple gaussian
			// amplitude of gauss is equal to number of events in the histogram
			vertex_model->getModelParameterSet().getModelParameter("gauss_amplitude")->setValue(
					hist->GetEntries());
			vertex_model->getModelParameterSet().getModelParameter("gauss_amplitude")->setParameterFixed(
					false);
			vertex_model->getModelParameterSet().getModelParameter("gauss_mean")->setValue(
					hist->GetMean(1));
			vertex_model->getModelParameterSet().getModelParameter("gauss_mean")->setParameterFixed(
					false);
			vertex_model->getModelParameterSet().getModelParameter("gauss_sigma")->setValue(
					hist->GetRMS(1));
			vertex_model->getModelParameterSet().getModelParameter("gauss_sigma")->setParameterFixed(
					false);
		}

		// set model
		model_fit_facade.setModel1d(vertex_model);

		// create minimizer instance with control parameter
		shared_ptr<ROOTMinimizer> minuit_minimizer(new ROOTMinimizer());

		model_fit_facade.setMinimizer(minuit_minimizer);

		doFit(lmd_data[i], fit_options);
	}
}

void PndLmdFitFacade::fitResolutionForSlice(PndLmdResolution &lmd_resolution,
		const PndLmdLumiFitOptions *fit_options) {

	cout << "Fitting resolution " << lmd_resolution.getName() << endl;

	// create chi2 estimator
	shared_ptr<Chi2Estimator> chi2_est(new Chi2Estimator());
	model_fit_facade.setEstimator(chi2_est);

	// get histogram
	const TH1D* hist = lmd_resolution.get1DHistogram();
	if (hist->Integral() < 400) {
		return;
	}
	model_fit_facade.setData(createData1D(lmd_resolution));

	// generate the model
	shared_ptr<Model1D> resolution_model =
			model_factory.generate1DResolutionModel(
					fit_options->getFitModelOptions());

	// now we have to set good starting values and free parameters
	if (fit_options->getFitModelOptions().smearing_model == LumiFit::GAUSSIAN) { // simple gaussian
		// amplitude of gauss is equal to number of events in the histogram
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_amplitude")->setValue(hist->GetEntries());
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_amplitude")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter("gauss_mean")->setValue(
				hist->GetMean(1));
		resolution_model->getModelParameterSet().getModelParameter("gauss_mean")->setParameterFixed(
				false);
		resolution_model->getModelParameterSet().getModelParameter("gauss_sigma")->setValue(
				hist->GetRMS(1));
		resolution_model->getModelParameterSet().getModelParameter("gauss_sigma")->setParameterFixed(
				false);
	} else if (fit_options->getFitModelOptions().smearing_model
			== LumiFit::DOUBLE_GAUSSIAN) { // double gaussian
		// amplitude of gauss is equal to number of events in the histogram
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_amplitude")->setValue(hist->GetEntries());
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_amplitude")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_narrow")->setValue(0.0);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_narrow")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_wide")->setValue(0.0);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_wide")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_narrow")->setValue(hist->GetRMS() * 0.65);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_narrow")->setParameterFixed(false);

		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_ratio_narrow_wide")->setValue(0.6);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_ratio_narrow_wide")->setParameterFixed(false);

		/*resolution_model->getModelParameterSet().getModelParameter(
		 "gauss_sigma_wide")->setValue(hist->GetRMS() * 1.35);
		 resolution_model->getModelParameterSet().getModelParameter(
		 "gauss_sigma_wide")->setParameterFixed(false);*/

		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_ratio_narrow_wide")->setValue(2.0);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_ratio_narrow_wide")->setParameterFixed(false);
	} else if (fit_options->getFitModelOptions().smearing_model
			== LumiFit::ASYMMETRIC_GAUSSIAN) {
		// amplitude of gauss is equal to number of events in the histogram
		resolution_model->getModelParameterSet().getModelParameter(
				"asymm_gauss_amplitude")->setValue(hist->GetEntries());
		resolution_model->getModelParameterSet().getModelParameter(
				"asymm_gauss_amplitude")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"asymm_gauss_mean")->setValue(hist->GetMean());
		resolution_model->getModelParameterSet().getModelParameter(
				"asymm_gauss_mean")->setParameterFixed(false);

		double startval_left = hist->GetRMS();
		if (hist->GetSkewness() > 0)
			startval_left = startval_left / (1 + hist->GetSkewness());
		double startval_right = hist->GetRMS();
		if (hist->GetSkewness() < 0)
			startval_right = startval_right / (1 - hist->GetSkewness());
		resolution_model->getModelParameterSet().getModelParameter(
				"asymm_gauss_sigma_left")->setValue(startval_left);
		resolution_model->getModelParameterSet().getModelParameter(
				"asymm_gauss_sigma_left")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"asymm_gauss_sigma_right")->setValue(startval_right);
		resolution_model->getModelParameterSet().getModelParameter(
				"asymm_gauss_sigma_right")->setParameterFixed(false);
	}

	// set model
	model_fit_facade.setModel1d(resolution_model);

	// create minimizer instance with control parameter
	shared_ptr<ROOTMinimizer> minuit_minimizer(new ROOTMinimizer());

	model_fit_facade.setMinimizer(minuit_minimizer);

	doFit(lmd_resolution, fit_options);
}
