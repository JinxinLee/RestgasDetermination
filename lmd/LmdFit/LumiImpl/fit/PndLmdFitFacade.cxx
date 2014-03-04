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
#include "ROOTDataHelper.h"
#include "Chi2Estimator.h"
#include "LogLikelihoodEstimator.h"
#include "PndLmdLumiHelper.h"
#include "ROOTMinimizer.h"
#include "Data.h"

#include <iostream>

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

ModelFitFacade& PndLmdFitFacade::getModelFitFacade() {
	return model_fit_facade;
}

pair<double, double> PndLmdFitFacade::calcRange(
		const PndLmdAbstractData &lmd_data,
		const PndLmdLumiFitOptions *fit_options) {
	double range_low, range_high;
	range_low = lmd_data.getPrimaryDimension().dimension_range.getRangeLow();
	range_high = lmd_data.getPrimaryDimension().dimension_range.getRangeHigh();
	if (fit_options->getPrimaryDimensionFitRange().getRangeLow()
			> lmd_data.getPrimaryDimension().dimension_range.getRangeLow())
		range_low = fit_options->getPrimaryDimensionFitRange().getRangeLow();
	if (fit_options->getPrimaryDimensionFitRange().getRangeHigh()
			< lmd_data.getPrimaryDimension().dimension_range.getRangeHigh())
		range_high = fit_options->getPrimaryDimensionFitRange().getRangeHigh();
	return std::make_pair(range_low, range_high);
}

shared_ptr<Data> PndLmdFitFacade::createData1D(
		const PndLmdData &lmd_data) const {
	shared_ptr<Data> data(new Data(1));
	data_helper.fillBinnedData(data, lmd_data.get1DHistogram());
	return data;
}

PndLmdLumiFitOptions* PndLmdFitFacade::cloneFitOptions() const {
	PndLmdLumiFitOptions* fit_opts = new PndLmdLumiFitOptions(
			fit_options_template);
	return fit_opts;
}

PndLmdLumiFitOptions* PndLmdFitFacade::createFitOptions(PndLmdData &lmd_data) {
	PndLmdLumiFitOptions* fit_opts;

	LumiFit::LmdBinaryFitOptions bit_fit_opt(0);
	fit_options_template.setResolutionParametrizationFileUrlActive(false);

	// switch range to momentum transfer as user usually specifies that in mrad
	if (LumiFit::T
			== lmd_data.getPrimaryDimension().dimension_options.dimension_type) {
		bit_fit_opt.setFitAsRaw(true);
		fit_options_template.setModelBinaryOptions(bit_fit_opt);

		PndLmdLumiHelper lumi_helper;

		// get current dimension settings
		LumiFit::LmdDimensionRange old_range =
				fit_options_template.getPrimaryDimensionFitRange();

		// recalcuated ranges to momentum transfer
		LumiFit::LmdDimensionRange temp_range =
				fit_options_template.getPrimaryDimensionFitRange();
		temp_range.setRangeLow(
				lumi_helper.getMomentumTransferFromTheta(lmd_data.getLabMomentum(),
						temp_range.getRangeLow()));
		temp_range.setRangeHigh(
				lumi_helper.getMomentumTransferFromTheta(lmd_data.getLabMomentum(),
						temp_range.getRangeHigh()));
		temp_range.setUnitPrefix(DataStructs::NONE);
		// overwrite values in template
		fit_options_template.setPrimaryDimensionFitRange(temp_range);

		fit_opts = new PndLmdLumiFitOptions(fit_options_template);

		//reset to the old settings
		fit_options_template.setPrimaryDimensionFitRange(old_range);

	} else {
		if (LumiFit::MC_ACC
				== lmd_data.getPrimaryDimension().dimension_options.track_type)
			bit_fit_opt.setAcceptanceCorrMode(true);
		if (LumiFit::RECO
				== lmd_data.getPrimaryDimension().dimension_options.track_type) {
			fit_options_template.setResolutionParametrizationFileUrlActive(true);
			bit_fit_opt.setAcceptanceCorrMode(true);
			bit_fit_opt.setSmearingMode(true);
		}
		fit_options_template.setModelBinaryOptions(bit_fit_opt);
		fit_opts = new PndLmdLumiFitOptions(fit_options_template);
	}
	return fit_opts;
}

void PndLmdFitFacade::doLmdLumiFits(std::vector<PndLmdData> &lmd_data_vec) {

	// these are all standard lumi fits, so dataset with appropriate model
	// like accepted mc data with acceptance corrected model etc

	fit_options_template.setPrimaryDimensionFitRangeActive(true);
	fit_options_template.setSecondaryDimensionFitRangeActive(false);

	fitMultipleElasticPPbar(lmd_data_vec);
}

void PndLmdFitFacade::fitSmearingModelToResolutions(
		vector<PndLmdResolution> &lmd_resolutions) {

	LumiFit::LmdBinaryFitOptions bit_fit_opt(0);
	fit_options_template.setModelBinaryOptions(bit_fit_opt);
	PndLmdLumiFitOptions *fit_options = cloneFitOptions();

	for (unsigned int index_resolution = 0;
			index_resolution < lmd_resolutions.size(); index_resolution++) {
		fitResolutionForSlice(lmd_resolutions[index_resolution], fit_options);
	}
}

PndLmdLumiFitResult* PndLmdFitFacade::doFit(PndLmdAbstractData &lmd_abs_data,
		const PndLmdLumiFitOptions *fit_options) {

	cout << "Attempting to perform fit with following fit options:" << endl;
	cout << *fit_options << endl;

	//first check if this model with the fit options have already been fitted
	PndLmdLumiFitResult *fit_result = lmd_abs_data.getFitResult(*fit_options);
	if (0 != fit_result) {
		cout << "Fit was already performed! Skipping..." << endl;
		return fit_result;
	}

	ModelFitResult temp_fit_result = model_fit_facade.Fit();

	// store fit results
	cout << "Adding fit result to storage..." << endl;

	// in case we have a likelihood we have to create a new chi2 estimator...

	fit_result = new PndLmdLumiFitResult();
	fit_result->setModelFitResult(temp_fit_result);
	lmd_abs_data.addFitResult(fit_options, fit_result);
	return fit_result;
}

void PndLmdFitFacade::fitMultipleElasticPPbar(
		std::vector<PndLmdData> &lmd_data) {
	for (unsigned int i = 0; i < lmd_data.size(); i++) {
		PndLmdLumiFitOptions *fit_options = createFitOptions(lmd_data[i]);

		if (fit_options->getModelBinaryOptions().getFitterType() == 0) { // if user wants to use ROOT/Minuit
			// create chi2 estimator
			//shared_ptr<Chi2Estimator> chi2_est(new Chi2Estimator());
			//model_fit_facade.setEstimator(chi2_est);

			shared_ptr<LogLikelihoodEstimator> loglikelihood_est(
					new LogLikelihoodEstimator());
			model_fit_facade.setEstimator(loglikelihood_est);

			// create a new model via the factory
			shared_ptr<Model1D> model1d = model_factory.generate1DModel(fit_options,
					lmd_data[i].getLabMomentum());
			// set model
			model_fit_facade.setModel1d(model1d);

			// create and set data
			model_fit_facade.setData(createData1D(lmd_data[i]));

			pair<double, double> fit_range = std::make_pair(
					fit_options->getPrimaryDimensionFitRange().getRangeLow(),
					fit_options->getPrimaryDimensionFitRange().getRangeHigh());

			model_fit_facade.getEstimatorOptions().setFitRangeX(fit_range);
			model_fit_facade.getEstimatorOptions().setWithIntegralScaling(true);

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

		} else { // user wants to use ROOFIT
			// ok do roofit stuff here
			return;
		}

		doFit(lmd_data[i], fit_options);
	}
}

void PndLmdFitFacade::fitVertexData(std::vector<PndLmdData*> lmd_data) {

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
			model_factory.generate1DResolutionModel(fit_options);

	// now we have to set good starting values and free parameters
	if (0 == fit_options->getSmearingModelType() % 10) { // simple gaussian
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
	} else if (1 == fit_options->getSmearingModelType() % 10) { // double gaussian
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
	} else if (2 == fit_options->getSmearingModelType() % 10) { // asymm gaussian
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

	pair<double, double> fit_range = std::make_pair(
			fit_options->getPrimaryDimensionFitRange().getRangeLow(),
			fit_options->getPrimaryDimensionFitRange().getRangeHigh());

	model_fit_facade.getEstimatorOptions().setFitRangeX(fit_range);
	model_fit_facade.getEstimatorOptions().setWithIntegralScaling(true);

	// create minimizer instance with control parameter
	shared_ptr<ROOTMinimizer> minuit_minimizer(new ROOTMinimizer());

	model_fit_facade.setMinimizer(minuit_minimizer);

	doFit(lmd_resolution, fit_options);
}
