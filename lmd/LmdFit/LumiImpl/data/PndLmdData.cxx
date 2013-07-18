/*
 * PndLmdData.h
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */
#include "PndLmdData.h"
#include "PndLmdAcceptance.h"
#include "ROOTMinimizer.h"
#include "PndLmdLumiFitResult.h"
#include "PndLmdLumiFitOptions.h"
#include "Chi2Estimator.h"
#include "ROOTDataHelper.h"
#include "Data.h"
#include "EstimatorOptions.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"

ClassImp(PndLmdData)

PndLmdData::PndLmdData(TFile *f_, int num_events_, double plab_,
		PndLmdFit::lmd_dimension th_dimension_,
		PndLmdFit::lmd_dimension phi_dimension_,
		double generated_luminosity_per_event_) :
		PndLmdDataBase(f_, num_events_, plab_, th_dimension_, phi_dimension_) {
	//fit_map =
	//		new std::map<PndLmdLumiModel*, std::vector<PndLmdLumiFitResult*> >();

	luminosity_ref = generated_luminosity_per_event_ * num_events_;
	makeDir();
}
PndLmdData::PndLmdData() {
}

PndLmdData::~PndLmdData() {
}

void PndLmdData::saveToRootFile() {
	std::cout << "Saving " << getName() << " to file..." << std::endl;

	makeDir(); // this will just change current directory to the one created by this data set

	this->Write("lmddata");
}

double PndLmdData::getReferenceLuminosity() const {
	return luminosity_ref;
}

void PndLmdData::setReferenceLuminosity(double luminosity_ref_) {
	luminosity_ref = luminosity_ref_;
}

double PndLmdData::getBinningFactor(const PndLmdLumiFitOptions *fit_opt) const {
	if (fit_opt->getModelBinaryOptions().isFitRaw())
		return t_dimension.bin_size;
	double bin_factor = th_dimension.bin_size;
	if (fit_opt->getModelBinaryOptions().getFitDimension())
		bin_factor *= phi_dimension.bin_size;
	return bin_factor;
}

std::pair<double, double> PndLmdData::calcRange(
		const PndLmdLumiFitOptions *fit_options) {
	double range_low, range_high;
	if (fit_options->getModelBinaryOptions().isFitRaw()) {
		range_low = t_dimension.range_low;
		range_high = t_dimension.range_high;
		if (fit_options->getTFitRangeLow() > t_dimension.range_low)
			range_low = fit_options->getTFitRangeLow();
		if (fit_options->getTFitRangeHigh() < t_dimension.range_high)
			range_high = fit_options->getTFitRangeHigh();
	} else {
		range_low = th_dimension.range_low;
		range_high = th_dimension.range_high;
		if (fit_options->getThetaFitRangeLow() > th_dimension.range_low)
			range_low = fit_options->getThetaFitRangeLow();
		if (fit_options->getThetaFitRangeHigh() < th_dimension.range_high)
			range_high = fit_options->getThetaFitRangeHigh();
	}
	return std::make_pair(range_low, range_high);
}

PndLmdLumiFitResult* PndLmdData::Fit(PndLmdAcceptance *lmd_acc,
	const PndLmdLumiFitOptions *fit_options) {

	std::cout << "Attempting to perform fit with following fit options:"
			<< std::endl;
	std::cout << *fit_options << std::endl;

	//first check if this model with the fit options have already been fitted
	std::vector<PndLmdLumiFitResult*> &model_fit_res = fit_map[lmd_acc];
	for (unsigned int i = 0; i < model_fit_res.size(); i++) {
		if (model_fit_res[i]->checkFitOptions(fit_options)) {
			std::cout << "Fit was already performed! Skipping..." << std::endl;
			return model_fit_res[i];
		}
	}

	std::cout << "Initiating Fitter..." << std::endl;

	PndLmdLumiFitResult *fit_result;

	if (fit_options->getModelBinaryOptions().getFitterType() == 0) { // if user wants to use ROOT/Minuit
		// create a new model via the factory
		shared_ptr<Model1D> model1d = signal_model_fac.generate1DModel(fit_options,
				getLabMomentum(), lmd_acc);

		// create chi2 estimator
		Chi2Estimator chi2_est;
		// set model
		chi2_est.setModel(model1d);

		// create and set data
		ROOTDataHelper data_helper;
		chi2_est.setData(
				data_helper.createBinnedData(getMeasuredHist1D(fit_options)));

		// create estimator options
		EstimatorOptions est_opt;
		std::pair<double, double> fit_range = std::make_pair(
				fit_options->getThetaFitRangeLow(),
				fit_options->getThetaFitRangeHigh());
		if (fit_options->getModelBinaryOptions().isFitRaw())
			fit_range = std::make_pair(fit_options->getTFitRangeLow(),
					fit_options->getTFitRangeHigh());

		est_opt.setFitRangeX(fit_range);
		est_opt.setWithIntegralScaling(true);

		// apply estimator options
		chi2_est.applyEstimatorOptions(est_opt);

		// now set better starting lumi value
		std::pair<double, double> range = calcRange(fit_options);
		double integral_data = getMeasuredHist1D(fit_options)->Integral("width");
		std::vector<std::pair<double, double> > temp_vec_range;
		temp_vec_range.push_back(range);
		double integral_func = model1d->Integral(temp_vec_range, 10e-3);
		double lumi_start = integral_data / integral_func
				/ getBinningFactor(fit_options);
		std::cout << integral_data << " / "
				<< integral_func * getBinningFactor(fit_options) << std::endl;
		std::cout << "(1D) Using start luminosity: " << lumi_start << std::endl;
		model1d->getModelParameterSet().setModelParameterValue("luminosity",
				lumi_start);

		// create minimizer instance with control parameter
		ROOTMinimizer fitter(chi2_est);

		int fit_status = fitter.doMinimization();
		// call minimization procedure
		if (fit_status) {
			std::cout
					<< "ERROR: Problem while performing fit. Returning NULL pointer!"
					<< std::endl;
			return fit_result;
		}

		// store fit results
		std::cout << "Adding fit result to storage..." << std::endl;

		ModelFitResult temp_fit_result = fitter.createModelFitResult();
		temp_fit_result.setFitStatus(fit_status);
		// in case we have a likelihood we have to create a new chi2 estimator...
		temp_fit_result.setChiSquare(
				chi2_est.evaluate(fitter.getROOTMinimizer()->X()));
		temp_fit_result.setNDF(
				chi2_est.getData()->getNumberOfDataPoints()
						- fitter.getROOTMinimizer()->NFree());
		fit_result = new PndLmdLumiFitResult(fit_options);
		fit_result->setModelFitResult(temp_fit_result);

	} else { // user wants to use ROOFIT
		// ok do roofit stuff here
	}

	model_fit_res.push_back(fit_result);
	return fit_result;
}

TH1D* PndLmdData::getMeasuredHist1D(
		const PndLmdLumiFitOptions *fit_options) const {
	if (fit_options->getDataBinaryOptions().isFitRaw()) {
		if (fit_options->getDataBinaryOptions().isSmearingOn()) {
			if (fit_options->getDataBinaryOptions().isAcceptanceCorrOn()) {
				return t_reco_1d;
			} else {
				std::cout
						<< "WARNING: Requesting data which has flat acceptance and is resolution smeared."
						<< "This cannot exist... the standard reconstruction histogram will be returned."
						<< "Make sure that the fit range is chosen appropriately (in which acceptance is actually flat)."
						<< std::endl;
				return t_reco_1d;
			}
		} else {
			if (fit_options->getDataBinaryOptions().isAcceptanceCorrOn()) {
				return t_mc_acc_1d;
			} else {
				return t_mc_1d;
			}
		}
	} else {
		if (fit_options->getDataBinaryOptions().isSmearingOn()) {
			if (fit_options->getDataBinaryOptions().isAcceptanceCorrOn()) {
				return reco_1d;
			} else {
				std::cout
						<< "WARNING: Requesting data which has flat acceptance and is resolution smeared."
						<< "This cannot exist... the standard reconstruction histogram will be returned."
						<< "Make sure that the fit range is chosen appropriately (in which acceptance is actually flat)."
						<< std::endl;
				return reco_1d;
			}
		} else {
			if (fit_options->getDataBinaryOptions().isAcceptanceCorrOn()) {
				return mc_acc_1d;
			} else {
				return mc_1d;
			}
		}
	}
}

TH2D* PndLmdData::getMeasuredHist2D(
		const PndLmdLumiFitOptions *fit_options) const {
	if (fit_options->getDataBinaryOptions().isSmearingOn()) {
		if (fit_options->getDataBinaryOptions().isAcceptanceCorrOn()) {
			return reco_2d;
		} else {
			std::cout
					<< "WARNING: Requesting data which has flat acceptance and is resolution smeared."
					<< "This cannot exist... the standard reconstruction histogram will be returned."
					<< "Make sure that the fit range is chosen appropriately (in which acceptance is actually flat)."
					<< std::endl;
			return reco_2d;
		}
	} else {
		if (fit_options->getDataBinaryOptions().isAcceptanceCorrOn()) {
			return mc_acc_2d;
		} else {
			return mc_2d;
		}
	}
}

std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >& PndLmdData::getFitMap() {
	return fit_map;
}

std::vector<PndLmdAcceptance*> PndLmdData::getListOfAcceptances() {
	std::vector<PndLmdAcceptance*> return_vector;
	for (std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >::iterator i =
			fit_map.begin(); i != fit_map.end(); i++) {
		return_vector.push_back(i->first);
	}
	return return_vector;
}

PndLmdLumiFitResult* PndLmdData::getFitResult(PndLmdAcceptance* lmd_acc,
		PndLmdLumiFitOptions* fit_options) {
	std::vector<PndLmdLumiFitResult*> &fit_res_vec = fit_map[lmd_acc];
	for (unsigned int i = 0; i < fit_res_vec.size(); i++) {
		if (fit_res_vec[i]->checkFitOptions(fit_options))
			return fit_res_vec[i];
	}
	return NULL; // found nothing
}

std::vector<PndLmdLumiFitResult*> PndLmdData::getFitResults(
		PndLmdAcceptance* lmd_acc) {
	return fit_map[lmd_acc];
}

std::vector<std::pair<PndLmdAcceptance*, PndLmdLumiFitResult*> > PndLmdData::getFitResults(
		PndLmdLumiFitOptions* fit_options) {
	std::vector<std::pair<PndLmdAcceptance*, PndLmdLumiFitResult*> > new_fit_res_vec;
	for (std::map<PndLmdAcceptance*, std::vector<PndLmdLumiFitResult*> >::iterator i =
			fit_map.begin(); i != fit_map.end(); i++) {
		for (unsigned int j = 0; j < i->second.size(); j++) {
			if (i->second[j]->checkFitOptions(fit_options))
				new_fit_res_vec.push_back(std::make_pair(i->first, i->second[j]));
		}
	}
	return new_fit_res_vec;
}
