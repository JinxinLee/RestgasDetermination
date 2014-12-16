/*
 * PndLmdFitFacade.cxx
 *
 *  Created on: Aug 30, 2013
 *      Author: steve
 */

#include "PndLmdFitFacade.h"
#include "data/PndLmdAngularData.h"
#include "data/PndLmdVertexData.h"
#include "fit/estimatorImpl/Chi2Estimator.h"
#include "fit/estimatorImpl/LogLikelihoodEstimator.h"
#include "fit/minimizerImpl/ROOT/ROOTMinimizer.h"
#include "fit/data/Data.h"
#include "PndLmdLumiHelper.h"

#include <iostream>
#include <algorithm>

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

void PndLmdFitFacade::addAcceptencesToPool(
		const std::vector<PndLmdAcceptance> &lmd_acc) {
	acceptance_pool.insert(lmd_acc.begin(), lmd_acc.end());
}
void PndLmdFitFacade::addReferenceAcceptancesToPool(
		const std::vector<PndLmdAcceptance> &lmd_acc) {
	reference_acceptance_pool.insert(lmd_acc.begin(), lmd_acc.end());
}
/*void PndLmdFitFacade::addResolutionGraphsToPool(
 const std::vector<PndLmdLumiHelper::lmd_graph*> &lmd_res_graphs) {
 resolution_graph_pool.insert(lmd_res_graphs.begin(), lmd_res_graphs.end());
 }
 void PndLmdFitFacade::addReferenceResolutionGraphsToPool(
 const std::vector<PndLmdLumiHelper::lmd_graph*> &lmd_res_graphs) {
 referenc_resolution_graph_pool.insert(lmd_res_graphs.begin(),
 lmd_res_graphs.end());
 }

 void PndLmdFitFacade::clearPools() {
 acceptance_pool.clear();
 reference_acceptance_pool.clear();
 resolution_graph_pool.clear();
 referenc_resolution_graph_pool.clear();
 }*/

void PndLmdFitFacade::saveFittedObjectsToFile() {
	std::map<PndLmdAngularData, PndLmdAngularData>::iterator iter;
	for (iter = fitted_angular_data.begin(); iter != fitted_angular_data.end();
			iter++) {
		iter->second.saveToRootFile();
	}
}

std::vector<DataStructs::DimensionRange> PndLmdFitFacade::calcRange(
		const PndLmdAbstractData &lmd_abs_data,
		const EstimatorOptions &est_options) {
	DataStructs::DimensionRange temp_range;
	std::vector<DataStructs::DimensionRange> ranges;
	temp_range.range_low =
			lmd_abs_data.getPrimaryDimension().dimension_range.getRangeLow();
	temp_range.range_high =
			lmd_abs_data.getPrimaryDimension().dimension_range.getRangeHigh();
	if (est_options.getFitRangeX().is_active) {
		if (est_options.getFitRangeX().range_low
				> lmd_abs_data.getPrimaryDimension().dimension_range.getRangeLow())
			temp_range.range_low = est_options.getFitRangeX().range_low;
		if (est_options.getFitRangeX().range_high
				< lmd_abs_data.getPrimaryDimension().dimension_range.getRangeHigh())
			temp_range.range_high = est_options.getFitRangeX().range_high;
	}
	ranges.push_back(temp_range);

	if (lmd_abs_data.getSecondaryDimension().is_active) {
		temp_range.range_low =
				lmd_abs_data.getSecondaryDimension().dimension_range.getRangeLow();
		temp_range.range_high =
				lmd_abs_data.getSecondaryDimension().dimension_range.getRangeHigh();

		if (est_options.getFitRangeY().is_active) {
			if (est_options.getFitRangeY().range_low
					> lmd_abs_data.getSecondaryDimension().dimension_range.getRangeLow())
				temp_range.range_low = est_options.getFitRangeY().range_low;
			if (est_options.getFitRangeY().range_high
					< lmd_abs_data.getSecondaryDimension().dimension_range.getRangeHigh())
				temp_range.range_high = est_options.getFitRangeY().range_high;

			std::cout << "fit range was active and changed the range to "
					<< temp_range.range_low << "-" << temp_range.range_high << std::endl;
		}

		ranges.push_back(temp_range);
	}

	return ranges;
}

shared_ptr<Data> PndLmdFitFacade::createData1D(
		const PndLmdHistogramData &lmd_hist_data) const {
	shared_ptr<Data> data(new Data(1));
	data_helper.fillBinnedData(data, lmd_hist_data.get1DHistogram());
	return data;
}
shared_ptr<Data> PndLmdFitFacade::createData2D(
		const PndLmdHistogramData &lmd_hist_data) const {
	shared_ptr<Data> data(new Data(2));
	data_helper.fillBinnedData(data, lmd_hist_data.get2DHistogram());
	return data;
}

std::vector<PndLmdLumiFitOptions> PndLmdFitFacade::createFitOptions(
		PndLmdHistogramData &lmd_data) {
	std::vector<PndLmdLumiFitOptions> return_vector;
	PndLmdLumiFitOptions fit_opts;

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

		fit_opts = fit_options_template;

		// set estimator options back to user setting (is only required in case of t recalculation)
		fit_options_template.est_opt = current_est_opt;

	} else if (lmd_data.getPrimaryDimension().dimension_options.dimension_type
			== LumiFit::THETA
			|| lmd_data.getPrimaryDimension().dimension_options.dimension_type
					== LumiFit::THETA_X) {
		fit_options_template.model_opt.momentum_transfer_active = false;
		if (LumiFit::MC_ACC
				== lmd_data.getPrimaryDimension().dimension_options.track_type)
			fit_options_template.model_opt.acceptance_correction_active = true;
		if (LumiFit::RECO
				== lmd_data.getPrimaryDimension().dimension_options.track_type) {
			fit_options_template.model_opt.resolution_smearing_active = true;
			fit_options_template.model_opt.acceptance_correction_active = true;
		}
		fit_opts = fit_options_template;
	} else {
		fit_opts = fit_options_template;
	}

	// if divergence smearing is turned on set the data object reference
	if (fit_opts.model_opt.divergence_smearing_active) {
		fit_opts.model_opt.data_primary_dimension = lmd_data.getPrimaryDimension();
		fit_opts.model_opt.data_secondary_dimension =
				lmd_data.getSecondaryDimension();
	}

// if user wants an acceptance or/and resolution smearing check if
// the corresponding info is available
// and create multiple fit options based on their multiplicy
	if (fit_opts.model_opt.acceptance_correction_active) {
		if (fit_opts.model_opt.resolution_smearing_active) {
			fit_opts.model_opt.selections = lmd_data.getSelectorSet();
		}

		// find all acceptances with the same selections like the data
		// but ignore secondary track filter as well as in acceptance as in the data selector set
		findBySelectionDimensionsIgnoringSecondaryTrackFilters condition(
				fit_opts.model_opt.selections);

		std::set<PndLmdAcceptance>::iterator acc_selection_start = std::find_if(
				acceptance_pool.begin(), acceptance_pool.end(), condition);
		while (acc_selection_start != acceptance_pool.end()) {
			fit_opts.model_opt.acceptance = &*acc_selection_start;
			return_vector.push_back(fit_opts); // clone the fit options
			acc_selection_start = std::find_if(++acc_selection_start,
					acceptance_pool.end(), condition);
		}
		acc_selection_start = std::find_if(reference_acceptance_pool.begin(),
				reference_acceptance_pool.end(), condition);
		while (acc_selection_start != reference_acceptance_pool.end()) {
			fit_opts.model_opt.acceptance = &*acc_selection_start;
			return_vector.push_back(fit_opts);
			acc_selection_start = std::find_if(++acc_selection_start,
					reference_acceptance_pool.end(), condition);
		}
	} else { // if we are not using acceptance correction we have no multiplicity so just add current
		return_vector.push_back(fit_opts);
	}

// set model options back to user settings
	fit_options_template.model_opt = current_mod_opt;

	return return_vector;
}

void PndLmdFitFacade::initializeResolutionModelOptionsFromParametrizationFile(
		LumiFit::PndLmdFitModelOptions& model_opt, std::string res_file_url) {
// get model options from the resolution parametrization file
	std::cout << "Opening file " << res_file_url
			<< " to get resolution parametrization metadata" << std::endl;
	PndLmdLumiHelper lmd_helper;
	std::vector<PndLmdLumiHelper::lmd_graph*> allgraphs =
			lmd_helper.getResolutionModelResultsFromFile(res_file_url);
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
	for (unsigned int i = 0; i < lmd_data_vec.size(); i++) {
		std::vector<PndLmdLumiFitOptions> fit_options_vec = createFitOptions(
				lmd_data_vec[i]);
		fitMultipleElasticPPbar(lmd_data_vec[i], fit_options_vec);
	}
}

void PndLmdFitFacade::fitSmearingModelToResolutions(
		vector<PndLmdHistogramData> &lmd_resolutions) {

	PndLmdLumiFitOptions fit_options(fit_options_template);

	for (unsigned int index_resolution = 0;
			index_resolution < lmd_resolutions.size(); index_resolution++) {
		if (lmd_resolutions[index_resolution].getSecondaryDimension().is_active
				&& 2 == fit_options.model_opt.fit_dimension) {
			fit2DResolutionForSlice(lmd_resolutions[index_resolution], fit_options);
		} else {
			fit1DResolutionForSlice(lmd_resolutions[index_resolution], fit_options);
		}
	}
}

PndLmdLumiFitResult PndLmdFitFacade::doFit(PndLmdHistogramData &lmd_hist_data,
		const PndLmdLumiFitOptions &fit_options) {

	cout << "Attempting to perform fit with following fit options:" << endl;
	cout << fit_options << endl;

//first check if this model with the fit options have already been fitted
	PndLmdLumiFitResult fit_result = lmd_hist_data.getFitResult(fit_options);
	if (fit_result.getModelFitResult().getFitParameters().size() > 0) {
		cout << "Fit was already performed! Skipping..." << endl;
		return fit_result;
	}

	model_fit_facade.setEstimatorOptions(fit_options.getEstimatorOptions());

	ModelFitResult temp_fit_result = model_fit_facade.Fit();

// store fit results
	cout << "Adding fit result to storage..." << endl;

	fit_result.setModelFitResult(temp_fit_result);
	lmd_hist_data.addFitResult(fit_options, fit_result);
	cout << "fit storage now contains " << lmd_hist_data.getFitResults().size()
			<< " entries!" << endl;
	return fit_result;
}

void PndLmdFitFacade::fitMultipleElasticPPbar(PndLmdAngularData &lmd_data,
		const std::vector<PndLmdLumiFitOptions> &fit_opt_vec) {
	for (unsigned int fit_option_index = 0; fit_option_index < fit_opt_vec.size();
			fit_option_index++) {
		PndLmdLumiFitOptions fit_options = fit_opt_vec[fit_option_index];
		// create chi2 estimator
		//shared_ptr<Chi2Estimator> chi2_est(new Chi2Estimator());
		//model_fit_facade.setEstimator(chi2_est);

		shared_ptr<LogLikelihoodEstimator> loglikelihood_est(
				new LogLikelihoodEstimator());
		model_fit_facade.setEstimator(loglikelihood_est);

		// create a new model via the factory
		shared_ptr<Model> model = model_factory.generateModel(
				fit_options.getFitModelOptions(), lmd_data.getLabMomentum());

		// free parameters
		if (model.get()) {
			//always free luminosity
			std::cout << "freeing model parameters..." << std::endl;
			model->getModelParameterSet().freeModelParameter("luminosity");

			//loop over all other free parameter names
			std::set<std::string, ModelStructs::string_comp> free_parameter_names =
					fit_options.getFreeParameterSet();

			for (std::set<std::string, ModelStructs::string_comp>::iterator it =
					free_parameter_names.begin(); it != free_parameter_names.end();
					it++) {
				model->getModelParameterSet().freeModelParameter(*it);
			}
		}

		// set model
		model_fit_facade.setModel(model);

		// create and set data
		if (fit_options.getFitModelOptions().fit_dimension == 2) {
			std::cout << "creating 2D data..." << std::endl;
			model_fit_facade.setData(createData2D(lmd_data));
		} else {
			std::cout << "creating 1D data..." << std::endl;
			model_fit_facade.setData(createData1D(lmd_data));
		}

		// now set better starting amplitude value
		PndLmdLumiHelper lmd_helper;
		std::vector<DataStructs::DimensionRange> range = calcRange(lmd_data,
				fit_options.getEstimatorOptions());
		double integral_data = 0.0;
		std::cout << "calculating data integral..." << std::endl;
		if (fit_options.getFitModelOptions().fit_dimension == 2)
			integral_data = lmd_helper.calcHistIntegral(lmd_data.get2DHistogram(),
					range);
		else
			integral_data = lmd_helper.calcHistIntegral(lmd_data.get1DHistogram(),
					range);
		std::cout << "calculating model integral..." << std::endl;
		double integral_func = model->Integral(range, 1e-1);
		double binning_factor = lmd_data.getBinningFactor(
				fit_options.getFitModelOptions().fit_dimension);
		double lumi_start = integral_data / integral_func / binning_factor;
		cout << "binning factor: " << binning_factor << endl;
		cout << integral_data << " / " << integral_func * binning_factor << endl;
		cout << "Using start luminosity: " << lumi_start << endl;
		model->getModelParameterSet().setModelParameterValue("luminosity",
				lumi_start);

		// create minimizer instance with control parameter
		shared_ptr<ROOTMinimizer> minuit_minimizer(new ROOTMinimizer());

		model_fit_facade.setMinimizer(minuit_minimizer);

		PndLmdLumiFitResult fit_res = doFit(lmd_data, fit_options);

		if (fitted_angular_data.find(lmd_data) != fitted_angular_data.end()) {
			fitted_angular_data.find(lmd_data)->second.addFitResult(fit_options,
					fit_res);
		} else {
			fitted_angular_data[lmd_data] = lmd_data;
		}
	}
}

void PndLmdFitFacade::fitVertexData(std::vector<PndLmdVertexData> &lmd_data) {
	for (unsigned int i = 0; i < lmd_data.size(); i++) {
		cout << "Fitting resolution " << lmd_data[i].getName() << endl;

		// get histogram
		const TH1D* hist = lmd_data[i].get1DHistogram();
		if (hist->Integral() < 400) {
			return;
		}

		DataStructs::DimensionRange old_range =
				fit_options_template.est_opt.getFitRangeX();
		double ideal_sigma;
		if (lmd_data[i].getPrimaryDimension().dimension_options.track_type
				== LumiFit::MC) {
			DataStructs::DimensionRange fit_range;

			if (lmd_data[i].getPrimaryDimension().dimension_options.dimension_type
					== LumiFit::X) {
				ideal_sigma = lmd_data[i].getSimulationIPParameters().offset_x_width;
				fit_range.range_low =
						lmd_data[i].getSimulationIPParameters().offset_x_mean
								- 2.0 * ideal_sigma;
				fit_range.range_high =
						lmd_data[i].getSimulationIPParameters().offset_x_mean
								+ 2.0 * ideal_sigma;
			} else if (lmd_data[i].getPrimaryDimension().dimension_options.dimension_type
					== LumiFit::Y) {
				ideal_sigma = lmd_data[i].getSimulationIPParameters().offset_y_width;
				fit_range.range_low =
						lmd_data[i].getSimulationIPParameters().offset_y_mean
								- 2.0 * ideal_sigma;
				fit_range.range_high =
						lmd_data[i].getSimulationIPParameters().offset_y_mean
								+ 2.0 * ideal_sigma;
			} else if (lmd_data[i].getPrimaryDimension().dimension_options.dimension_type
					== LumiFit::Z) {
				ideal_sigma = lmd_data[i].getSimulationIPParameters().offset_z_width;
				fit_range.range_low =
						lmd_data[i].getSimulationIPParameters().offset_z_mean
								- 2.0 * ideal_sigma;
				fit_range.range_high =
						lmd_data[i].getSimulationIPParameters().offset_z_mean
								+ 2.0 * ideal_sigma;
			}

			fit_range.is_active = true;
			fit_options_template.est_opt.setFitRangeX(fit_range);
		} else if (lmd_data[i].getPrimaryDimension().dimension_options.track_type
				== LumiFit::RECO) {
			DataStructs::DimensionRange fit_range;

			if (lmd_data[i].getPrimaryDimension().dimension_options.dimension_type
					== LumiFit::X) {
				ideal_sigma = hist->GetRMS(1);
				fit_range.range_low =
						lmd_data[i].getSimulationIPParameters().offset_x_mean
								- 2.0 * ideal_sigma;
				fit_range.range_high =
						lmd_data[i].getSimulationIPParameters().offset_x_mean
								+ 2.0 * ideal_sigma;
			} else if (lmd_data[i].getPrimaryDimension().dimension_options.dimension_type
					== LumiFit::Y) {
				ideal_sigma = hist->GetRMS(1);
				fit_range.range_low =
						lmd_data[i].getSimulationIPParameters().offset_y_mean
								- 2.0 * ideal_sigma;
				fit_range.range_high =
						lmd_data[i].getSimulationIPParameters().offset_y_mean
								+ 2.0 * ideal_sigma;
			} else if (lmd_data[i].getPrimaryDimension().dimension_options.dimension_type
					== LumiFit::Z) {
				ideal_sigma = hist->GetRMS(1);
				fit_range.range_low =
						lmd_data[i].getSimulationIPParameters().offset_z_mean
								- 2.0 * ideal_sigma;
				fit_range.range_high =
						lmd_data[i].getSimulationIPParameters().offset_z_mean
								+ 2.0 * ideal_sigma;
			}

			fit_range.is_active = true;
			fit_options_template.est_opt.setFitRangeX(fit_range);
		}

		PndLmdLumiFitOptions fit_options = createFitOptions(lmd_data[i])[0];
		fit_options_template.est_opt.setFitRangeX(old_range);

		// create chi2 estimator
		shared_ptr<Chi2Estimator> chi2_est(new Chi2Estimator());
		model_fit_facade.setEstimator(chi2_est);

		/*shared_ptr<LogLikelihoodEstimator> loglikelihood_est(
		 new LogLikelihoodEstimator());
		 model_fit_facade.setEstimator(loglikelihood_est);*/

		model_fit_facade.setData(createData1D(lmd_data[i]));

		// generate the model
		shared_ptr<Model1D> vertex_model = model_factory.generate1DVertexModel(
				fit_options.getFitModelOptions());

		// now we have to set good starting values and free parameters
		if (fit_options.getFitModelOptions().vertex_model == LumiFit::GAUSSIAN) { // simple gaussian
			// amplitude of gauss is equal to number of events in the histogram
			vertex_model->getModelParameterSet().getModelParameter("gauss_amplitude")->setParameterFixed(
					false);
			vertex_model->getModelParameterSet().getModelParameter("gauss_amplitude")->setValue(
					hist->Integral());
			vertex_model->getModelParameterSet().getModelParameter("gauss_mean")->setValue(
					hist->GetMean(1));
			vertex_model->getModelParameterSet().getModelParameter("gauss_mean")->setParameterFixed(
					false);
			if (lmd_data[i].getPrimaryDimension().dimension_options.track_type
					== LumiFit::MC) {
				vertex_model->getModelParameterSet().getModelParameter("gauss_sigma")->setValue(
						ideal_sigma);
			} else {
				vertex_model->getModelParameterSet().getModelParameter("gauss_sigma")->setValue(
						hist->GetRMS(1));
			}
			vertex_model->getModelParameterSet().getModelParameter("gauss_sigma")->setParameterFixed(
					false);
		}

		// set model
		model_fit_facade.setModel(vertex_model);

		// create minimizer instance with control parameter
		shared_ptr<ROOTMinimizer> minuit_minimizer(new ROOTMinimizer());

		model_fit_facade.setMinimizer(minuit_minimizer);

		doFit(lmd_data[i], fit_options);
	}
}

void PndLmdFitFacade::fit1DResolutionForSlice(
		PndLmdHistogramData &lmd_resolution,
		const PndLmdLumiFitOptions &fit_options) {

	cout << "Fitting resolution " << lmd_resolution.getName() << endl;

// create chi2 estimator
	shared_ptr<Chi2Estimator> chi2_est(new Chi2Estimator());
	model_fit_facade.setEstimator(chi2_est);

// get histogram
	const TH1D* hist = lmd_resolution.get1DHistogram();
	if (hist->GetMaximum() < 20 || hist->Integral() < 500) {
		return;
	}
	model_fit_facade.setData(createData1D(lmd_resolution));

// generate the model
	shared_ptr<Model1D> resolution_model =
			model_factory.generate1DResolutionModel(fit_options.getFitModelOptions());

// now we have to set good starting values and free parameters
	if (fit_options.getFitModelOptions().smearing_model == LumiFit::GAUSSIAN) { // simple gaussian
		// amplitude of gauss is equal to number of events in the histogram
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_amplitude")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_amplitude")->setValue(hist->GetEntries());
		resolution_model->getModelParameterSet().getModelParameter("gauss_mean")->setParameterFixed(
				false);
		resolution_model->getModelParameterSet().getModelParameter("gauss_mean")->setValue(
				hist->GetMean(1));
		resolution_model->getModelParameterSet().getModelParameter("gauss_sigma")->setParameterFixed(
				false);
		resolution_model->getModelParameterSet().getModelParameter("gauss_sigma")->setValue(
				hist->GetRMS(1));
	} else if (fit_options.getFitModelOptions().smearing_model
			== LumiFit::DOUBLE_GAUSSIAN) { // double gaussian
		// amplitude of gauss is equal to number of events in the histogram
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_amplitude")->setValue(hist->GetEntries());
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_amplitude")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_narrow")->setValue(hist->GetMean());
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_narrow")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_wide")->setValue(hist->GetMean());
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_wide")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_narrow")->setValue(hist->GetRMS() * 0.9);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_narrow")->setParameterFixed(false);

		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_ratio_narrow_wide")->setValue(0.9);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_ratio_narrow_wide")->setParameterFixed(false);

		/*resolution_model->getModelParameterSet().getModelParameter(
		 "gauss_sigma_wide")->setValue(hist->GetRMS() * 1.35);
		 resolution_model->getModelParameterSet().getModelParameter(
		 "gauss_sigma_wide")->setParameterFixed(false);*/

		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_ratio_narrow_wide")->setValue(1.0);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_ratio_narrow_wide")->setParameterFixed(false);
	} else if (fit_options.getFitModelOptions().smearing_model
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
	model_fit_facade.setModel(resolution_model);

// create minimizer instance with control parameter
	shared_ptr<ROOTMinimizer> minuit_minimizer(new ROOTMinimizer());

	model_fit_facade.setMinimizer(minuit_minimizer);

	doFit(lmd_resolution, fit_options);
}

void PndLmdFitFacade::fit2DResolutionForSlice(
		PndLmdHistogramData &lmd_resolution,
		const PndLmdLumiFitOptions &fit_options) {

	cout << "Fitting resolution " << lmd_resolution.getName() << endl;

// create chi2 estimator
	shared_ptr<Chi2Estimator> chi2_est(new Chi2Estimator());
	model_fit_facade.setEstimator(chi2_est);

// get histogram
	const TH2D* hist = lmd_resolution.get2DHistogram();
	if (hist->Integral() < 400) {
		return;
	}
	model_fit_facade.setData(createData2D(lmd_resolution));

// generate the model
	shared_ptr<Model2D> resolution_model =
			model_factory.generate2DResolutionModel(fit_options.getFitModelOptions());

// now we have to set good starting values and free parameters
	if (fit_options.getFitModelOptions().smearing_model == LumiFit::GAUSSIAN) { // simple gaussian
		// amplitude of gauss is equal to number of events in the histogram
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_amplitude")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_amplitude")->setValue(hist->GetEntries());
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_var1")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_var1")->setValue(hist->GetMean(1));
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_var2")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_mean_var2")->setValue(hist->GetMean(2));
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_var1")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_var1")->setValue(hist->GetRMS(1));
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_var2")->setParameterFixed(false);
		resolution_model->getModelParameterSet().getModelParameter(
				"gauss_sigma_var2")->setValue(hist->GetRMS(2));
		resolution_model->getModelParameterSet().getModelParameter("gauss_rho")->setParameterFixed(
				false);
		resolution_model->getModelParameterSet().getModelParameter("gauss_rho")->setValue(
				hist->GetCovariance() / hist->GetRMS(1) / hist->GetRMS(2));

	}

// set model
	model_fit_facade.setModel(resolution_model);

// create minimizer instance with control parameter
	shared_ptr<ROOTMinimizer> minuit_minimizer(new ROOTMinimizer());

	model_fit_facade.setMinimizer(minuit_minimizer);

	doFit(lmd_resolution, fit_options);
}
